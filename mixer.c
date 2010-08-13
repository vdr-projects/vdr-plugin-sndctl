/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: mixer.c
 * description: parent class for all types of mixer devices
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "mixer.h"
#include "soundset.h"

/*********************************************************
 * member functions for class cMixer
 *********************************************************/

/*
 * constructor
 */
cMixer::cMixer(){
}

/*
 * destructor
 */
cMixer::~cMixer(){
  int countControls = vControls.size();
  for(int i = 0 ; i < countControls ; i++) {
    delete vControls[i];
  }
}

int cMixer::CountControls() {
  return vControls.size();
}


/*
 * returns a vector with all controls names
 */
vector<cControlId> cMixer::GetControls( void ){
  vector<cControlId> ctrls;
  int            i;

  // loop over all controls
  for( i = 0; i < vControls.size(); i++ ) {
    ctrls.push_back(cControlId(vControls[i]->soundCardId, vControls[i]->name, vControls[i]->controlNr));
  }

  return ctrls;
}

/*
 * update mixer values
 */
void cMixer::Update( cSoundSet *SoundSet ){
  int               i;
  string::size_type loc;
  string            v;

  dsyslog( "sndctl (cMixer::Update): Updating controls with sound set '%s'.", SoundSet->GetName().c_str());

  // loop over all controls
  for( i = 0; i < vControls.size(); i++ ){
    control* actualControl = vControls[i];
    v = SoundSet->Get(cControlId(actualControl->soundCardId, actualControl->name, actualControl->controlNr ));

    // get operation
    actualControl->op = v.substr( 0, 1 ).c_str()[0];

    // get value
    if(actualControl->op == SNDCTL_MIXER_NONE )
      actualControl->value = 0;
    else
      actualControl->value = atoi( v.substr( 1 ).c_str());

    // check and correct limits
    actualControl->value = actualControl->value < 0 ? 0 : actualControl->value;
    actualControl->value = actualControl->value > 999 ? 999 : actualControl->value;

    // check and set minimum volume
    actualControl->limitL = actualControl->min;
    if(( loc = v.find( ';', 0 )) != string::npos )
      actualControl->limitL = atoi( v.substr( ++loc ).c_str());

    // check and set maximum volume
    actualControl->limitU = actualControl->max;
    if(( loc != string::npos ) && (( loc = v.find( ';', loc )) != string::npos ))
      vControls[i]->limitU = atoi( v.substr( ++loc ).c_str());

    dsyslog( "sndctl (cMixer::Update): '%s' updated with %c%d;%ld;%ld", actualControl->name.c_str(), actualControl->op, actualControl->value, actualControl->limitL, actualControl->limitU );
  }
}

/*
 * sets the volume of this sound set
 */
void cMixer::Volume( int Vol ){
  int  i;
  long vol;

  dsyslog( "sndctl (cMixer::Volume): Volume goes to %d%%.", Vol );

  // loop over all controls
  for( i = 0; i < vControls.size(); i++ ){
    control* actualControl = vControls[i];
    // initial value for 'volume'
    actualControl->volume = -1;

    // calculate volume according to operation mode
    switch( actualControl->op ){
      case SNDCTL_MIXER_STATIC:
        vol = actualControl->value;
        break;

      case SNDCTL_MIXER_PLUS:
        vol = Vol + actualControl->value;
        break;

      case SNDCTL_MIXER_MINUS:
        vol = Vol - actualControl->value;
        break;

      case SNDCTL_MIXER_PERCENT:
        vol = Vol * actualControl->value / 100;
        break;

      case SNDCTL_MIXER_NONE:
      default:
        dsyslog( "sndctl (cMixer::Volume): Control '%s' stays unchanged.", actualControl->name.c_str());
        continue;
        break;
    }

    // Vol = 0 means, no matter, what happend before
    if( Vol == 0 )
      vol = 0;

    // scale volume to real values
    vol = actualControl->min + ( actualControl->max - actualControl->min ) * vol / 100;

    // check for hard limits
    if( Vol > 0 && vol < actualControl->limitL )
      vol = actualControl->limitL;
    if( vol > actualControl->limitU )
      vol = actualControl->limitU;

    dsyslog( "sndctl (cMixer::Volume): Control '%s' goes to '%ld'.", actualControl->name.c_str(), vol );

    // set volume
    actualControl->volume = vol;
  }

  // set volumes to real hardware
  Volumes();
}

cControlId::cControlId(string SoundCardId, string DisplayName, int ControlNr) {
  name = DisplayName;
  soundCardId = SoundCardId;
  controlNr = ControlNr;
}

cControlId::cControlId() : name(), soundCardId(), controlNr(0) {
}

bool cControlId::operator==(cControlId const& b) {
  return controlNr == b.controlNr && soundCardId.compare(b.soundCardId) == 0;
}

string cControlId::getDisplayName() { return name; }
int cControlId::getControlNr() { return controlNr; }
string cControlId::getSoundCardId() { return soundCardId; }
