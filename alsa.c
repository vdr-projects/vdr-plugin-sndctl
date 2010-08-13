/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: alsa.c
 * description: handling of the Alsa API
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "alsa.h"

/*********************************************************
 * member functions for class cAlsa
 *********************************************************/

/*
 * constructors
 */
cAlsa::cAlsa(){
  init();
}

/*
 * destructor
 */
cAlsa::~cAlsa(){
}

/*
 * Initialize mapCardNameToHwNumber and mapCardNumberToMixerHandle
 */
static void dumpErrorMessage(string msg, int err)
{
    msg.append(snd_strerror(err));
    dsyslog(msg.c_str());
}

void cAlsa::init( void ) {
  char hwName[64];
  snd_ctl_card_info_t *info;
  snd_ctl_card_info_malloc(&info);
  snd_ctl_t *ctl;
  snd_hctl_t *hctl;
  int err;

  int number = -1;
  for (;;) {
    /* open card for getting the mixers of this card */
    err = snd_card_next(&number);
    if (err < 0) {
      dumpErrorMessage(string("sndctl (cAlsa::init): ERROR: Cannot get next card number because "), err);
      break;
    }
    if (number < 0)
      break;
    sprintf(hwName, "hw:%d", number);
    if ((err = snd_ctl_open(&ctl, hwName, 0)) < 0) {
      dumpErrorMessage(string("sndctl (cAlsa::init): ERROR: Cannot open card number because "), err);
      break;
    }
    if ((err = snd_ctl_card_info(ctl, info)) < 0) {
      dumpErrorMessage(string("sndctl (cAlsa::init): ERROR: Cannot get sound card info because "), err);
      break;
    }
    dsyslog("sndctl (cAlsa::init): Found sound card:%s\n", snd_ctl_card_info_get_name(info));

    if ((err = snd_hctl_open(&hctl, hwName, 0)) < 0) {
      dumpErrorMessage(string("sndctl (cAlsa::init): ERROR: Cannot snd_hctl_open because "), err);
      break;
    }

    /* now iterate over the mixers */
    if ((err = snd_hctl_load(hctl)) < 0) {
      dumpErrorMessage(string("sndctl (cAlsa::init): ERROR: Cannot snd_hctl_load because "), err);
    }

    snd_hctl_elem_t *elem = NULL;
    snd_ctl_elem_info_t *elemInfo;
    snd_ctl_elem_info_alloca(&elemInfo);

    int controlNr = 0;
    for (elem = snd_hctl_first_elem(hctl); elem; elem = snd_hctl_elem_next(elem)) {
      if ((err = snd_hctl_elem_info(elem, elemInfo)) < 0) {
        dumpErrorMessage(string("sndctl (cAlsa::init): ERROR: Cannot snd_hctl_elem_info because "), err);
        break;
      }
      if (snd_ctl_elem_info_is_inactive(elemInfo)) {
        continue;
      }

      snd_ctl_elem_id_t *id;
      snd_ctl_elem_id_alloca(&id);
      snd_hctl_elem_get_id(elem, id);

      if (snd_ctl_elem_info_get_type(elemInfo) == SND_CTL_ELEM_TYPE_INTEGER) {
        alsacontrol *control = new alsacontrol();
        control->min = snd_ctl_elem_info_get_min(elemInfo);
        control->max = snd_ctl_elem_info_get_max(elemInfo);
        control->soundCardId = string(snd_ctl_card_info_get_name(info));
        control->hwName = string(hwName);

        // check for a senseful range
        if( control->min == 0 && control->max > control->min && control->max > 10 ){
          // get name
          control->name = string( snd_ctl_elem_id_get_name(id));
          control->mixername = string( snd_ctl_elem_id_get_name(id));
          control->controlNr = controlNr;
          controlNr++;

          dsyslog( "sndctl (cAlsa::init): '%s' updated with min=%ld and max=%ld", control->name.c_str(), control->min, control->max );
          vControls.push_back(control);
        } else {
          delete control;
        }
      } else {
        dsyslog("sndctl (cAlsa::init): '%s' has no playback volume", snd_ctl_elem_id_get_name(id));
      }
    }
    snd_hctl_close(hctl);
    snd_ctl_close(ctl);
  }
  snd_ctl_card_info_free(info);
  dsyslog("sndctl (cAlsa::init): found %d controls:", vControls.size());
}

/*
 * checks for a mixer with a given name
 */
bool cAlsa::IsMixer( string Name ){
  snd_mixer_elem_t *elem;

  // search in all mixers for 'Name'
  for( elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next( elem )){
    if(( strcmp(snd_mixer_selem_get_name( elem ), Name.c_str()) == 0 ) &&
         snd_mixer_selem_is_active( elem ) &&
         ( snd_mixer_selem_has_playback_volume( elem ) ||
         snd_mixer_selem_has_playback_switch( elem )))
      return true;
  }

  // nothing found
  return false;
}

/*
 * set the volume of all controls
 */
void cAlsa::Volumes( void ){
/*  snd_mixer_elem_t             *elem;
  int                           i;
  snd_mixer_selem_id_t         *sid;
  snd_mixer_selem_id_malloc( &sid );
*/

  // loop over all controls
  for(int i = 0; i < vControls.size(); i++ ){
    alsacontrol *control = static_cast<alsacontrol*>(vControls[i]);
    if( control->volume >= 0 ){
      int err;
      // get ID from the name
      dsyslog("sndctl (cAlsa::Volumes): trying to set control '%s' of card '%s'", control->name.c_str(), control->hwName.c_str());
      snd_hctl_t *hctl;
      if ((err = snd_hctl_open(&hctl, control->hwName.c_str(), 0)) < 0) {
        dumpErrorMessage(string("sndctl (cAlsa::Volumes): snd_hctl_open failed because "), err);
        break;
      }
      if ((err = snd_hctl_load(hctl)) < 0) {
        dumpErrorMessage(string("sndctl (cAlsa::Volumes): snd_hctl_load failed because "), err);
        break;
      }

      snd_ctl_elem_id_t *id;
      snd_ctl_elem_id_alloca(&id);
      snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

      snd_ctl_elem_id_set_name(id, control->mixername.c_str());

      snd_hctl_elem_t *elem = snd_hctl_find_elem(hctl, id);

      snd_ctl_elem_value_t *ctl;
      snd_ctl_elem_value_alloca(&ctl);
      snd_ctl_elem_value_set_id(ctl, id);

      /* left */
      snd_ctl_elem_value_set_integer(ctl, 0, control->volume);
      /* right */
      snd_ctl_elem_value_set_integer(ctl, 1, control->volume);

      if ((err = snd_hctl_elem_write(elem, ctl)) < 0) {
        dumpErrorMessage(string("sndctl (cAlsa::Volumes): snd_hctl_elem_write failed because "), err);
        break;
      }

      snd_hctl_close(hctl);

    }
  }
}
