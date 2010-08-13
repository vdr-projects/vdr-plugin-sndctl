/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: soundset.c
 * description: representation of a soundset
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "soundset.h"

/*********************************************************
 * member functions for class cSoundSet
 *********************************************************/

/*
 * constructors
 */
cSoundSet::cSoundSet( cMixer *Mixer, string Id ){
  int                          i = 0;
  vector<cControlId>           ctrls;
  vector<cControlId>::iterator it;

  dsyslog( "sndctl (cSoundSet::cSoundSet): New sound set created" );

  // initial value for volume
  volume = 0;

  // save mixer class
  mixer = Mixer;

  // save my ID
  id = Id;

  // set default name
  name = tr( SNDCTL_DEFAULT_SOUNDSET_NAME );

  // initialize control array
  if(( nrOfControls = mixer->CountControls()) > 0 )
    if( controls = new struct control[nrOfControls] ){
      // fill controls array with values
      ctrls = GetControls();
      for( it = ctrls.begin(); it != ctrls.end(); it++ ){
        (controls + i)->control = (*it);
        strncpy((controls + i)->value, "~", SNDCTL_MAX_LEN_CONTROL_VAL );
        i++;
      }
    }else
        esyslog( "sndctl (cSoundSet::cSoundSet): ERROR! Can't allocate memory for '%d' controls.", nrOfControls );
  else
    esyslog( "sndctl (cSoundSet::cSoundSet): WARNING! No controls found. Sound set is not initialized!" );
}

/*
 * destructor
 */
cSoundSet::~cSoundSet(){
  // remove mixers
  //delete &mixers;
  delete [] controls;
}

/*
 * returns the value for a control
 */
string cSoundSet::Get( cControlId Control){
  int    i;
  string result;

  result = string( "~" );

  // search for this control
  for( i = 0; i < nrOfControls; i++ ) {
    if( (controls + i)->control == Control){
      result = string((controls + i)->value );
      break;
    }
  }

  dsyslog( "sndctl (cSoundSet::Get): Value for '%s' requested, returning '%s'", Control.getDisplayName().c_str(), result.c_str());

  // nothing found, return default value
  return result;
}

/*
 * returns a vector with all controls
 */
vector<cControlId> cSoundSet::GetControls( void ){
  return mixer->GetControls();
}

/*
 * returns the ID of this sound set
 */
string cSoundSet::GetID( void ){
  return id;
}

/*
 * returns the name of this sound set
 */
string cSoundSet::GetName( void ){
  return name;
}

/*
 * set a parameter for this soundset
 */
bool cSoundSet::Set(string Name, string Value ){
  int i;

  dsyslog( "sndctl (cSoundSet::Set): Trying to set '%s' to '%s'.", Name.c_str(), Value.c_str());

  // name?
  if( Name == "name" ){
    name = Value;
    dsyslog( "sndctl (cSoundSet::Set): Sound set was renamed to '%s'.", name.c_str());
    return true;
  }

  // a known control?
  int pos = 0;
  if ((pos = Name.find_first_of("@")) != string::npos) {
    string soundCardId = Name.substr(0, pos);
    string name = Name.substr(pos+1);
    int mixerNr = atoi(name.c_str());
    cControlId id(soundCardId, string(""), mixerNr);
    for( i = 0; i < nrOfControls; i++ ){
      if( (controls + i)->control == id){
        // save value
        strncpy((controls + i)->value, Value.c_str(), SNDCTL_MAX_LEN_CONTROL_VAL );
        dsyslog( "sndctl (cSoundSet::Set): Control '%s@%s' in sound set '%s' is now '%s'.",
            (controls + i)->control.getSoundCardId().c_str(),
            (controls + i)->control.getDisplayName().c_str(),
            name.c_str(),
            Get( id ).c_str());

        mixer->Update( this );
        return true;
      }
    };
  }

  // nothing of all
  return false;
}

bool cSoundSet::Set(cControlId ControlId, string Value ){
  int i;

  dsyslog( "sndctl (cSoundSet::Set): Trying to set '%s'@'%s' to '%s'.", ControlId.getSoundCardId().c_str(), ControlId.getDisplayName().c_str(), Value.c_str());
  for( i = 0; i < nrOfControls; i++ ){
    if( (controls + i)->control == ControlId){
      // save value
      strncpy((controls + i)->value, Value.c_str(), SNDCTL_MAX_LEN_CONTROL_VAL );
      dsyslog( "sndctl (cSoundSet::Set): Control '%s@%s' in sound set '%s' is now '%s'.",
          (controls + i)->control.getSoundCardId().c_str(),
          (controls + i)->control.getDisplayName().c_str(),
          name.c_str(),
          Get( ControlId ).c_str());

      mixer->Update( this );
      return true;
    }
  }

  // nothing of all
  return false;
}

/*
 * store this sound sets settings
 */
bool cSoundSet::Store( string Id, cPluginSndctl *Plugin ){
  int i;
  char buf[64];

  // save my name
  Plugin->Store( Id + "_name", name );

  // save control settings
  for( i = 0; i < nrOfControls; i++ ) {
    if( strcmp((controls + i)->value, "~" )) {
      sprintf(buf, "%d", (controls + i)->control.getControlNr());
      Plugin->Store( Id + "_" + (controls + i)->control.getSoundCardId()+"@"+buf, (controls + i)->value );
    }
  }

  return true;
}

/*
 * sets the volume of this sound set
 * Vol range is here 0..100
 */
void cSoundSet::Volume( int Vol ){
  // save last set volume
  volume = Vol < 0 ? volume : Vol;

  isyslog( "sndctl (cSoundSet::Volume): Volume for soundset '%s' goes to %d%%.", name.c_str(), volume );
  mixer->Volume( volume );
}

