/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: soundman.c
 * description: manager for volume change requests
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "soundman.h"
#include <stdio.h>

/*********************************************************
 * member functions for class cSoundMan
 *********************************************************/

/*
 * constructor
 */
cSoundMan::cSoundMan( cPluginSndctl *Plugin ){
  vector<string>           controls;
  vector<string>::iterator it;

  dsyslog( "sndctl (cSoundMan::cSoundMan): cSoundMan created" );

  plugin = Plugin;

  // create mixer device
  mixer = (cMixer*) new cAlsa();

  // initialize variables
  volume = volumeCounter = 0;
  soundflash = false;
}

/*
 * destructor
 */
cSoundMan::~cSoundMan(){
  // remove objects
  delete mixer;
  if (!soundsets.empty()) {
      map<string,cSoundSet*>::iterator it;
      for( it = soundsets.begin(); it != soundsets.end(); it++ ) {
          delete it->second;
      }
  }
//  free( &activeSoundSetID );
}

/*
 * care for sound set switching, when audio track is changed
 */
void cSoundMan::AudioTrack( const char* Track ){
  string sset;

  dsyslog( "sndctl (cSoundMan::AudioTrack): Audio track switched to '%s'.", Track );

  if( plugin->GetSetup()->GetBool( SNDCTL_SETUP_DD_AUTO_SWITCH )){
    if( strstr( Track, "dolby" ) != NULL ||
        strstr( Track, "Dolby" ) != NULL ||
        strstr( Track, "DOLBY" ) != NULL )
      // switch to DD sound set
      sset = plugin->GetSetup()->Get( SNDCTL_SETUP_DD_AUTO_SSET );
    else
      // switch back to 'normal' sound set
      sset = plugin->GetSetup()->Get( SNDCTL_SETUP_DEFAULT_SSET );

    dsyslog( "sndctl (cSoundMan::AudioTrack): Doing DD auto switching to '%s'.", sset.c_str());
    SetSoundSet( sset );
  }
}

/*
 * create a new, empty sound set
 */
string cSoundMan::CreateSoundSet(){
  char   rnd[6];
  string id;

  // initialize randomizer
  srand( time( NULL ));

  // create a new ID
  do{
    // create new random number (in range 1000 ~ 1999)
    sprintf( rnd, "s%u", (int) ( 1000 + ((double) rand()) / RAND_MAX * 999 ));
    id = rnd;
  } while( soundsets.find( id ) != soundsets.end());

  // create a new sound set
  soundsets[id] = new cSoundSet( mixer, id );

  dsyslog( "sndctl (cSoundMan::CreateSoundSet): New sound set with ID '%s' created.", id.c_str());

  return id;
}

/*
 * delete a sound set
 */
bool cSoundMan::DeleteSoundSet( string Id ){
  string                           defSoundSet;
  map<string,cSoundSet*>::iterator it;

  if (soundsets.size() <= 1) {
      esyslog( "sndctl (cSoundMan::DeleteSoundSet): Failed to delete sound set with ID '%s'. Cannot delete last one.", Id.c_str());
      return false;
  }
  // trying to find the requested sound set
  if(( it = soundsets.find( Id )) != soundsets.end()){
    // remove sound set
    free( it->second );
    soundsets.erase( it );

    isyslog( "sndctl (cSoundMan::DeleteSoundSet): Sound set with ID '%s' deleted.", Id.c_str());

    // do something, when deleted sound set was the active one
    if( Id == activeSoundSetID ){
      defSoundSet = plugin->GetSetup()->Get( string( SNDCTL_SETUP_DEFAULT_SSET ));
      if( Id != defSoundSet )
        // currently deleted sound set was NOT the default one
        // -> set active sound set to default
        SetSoundSet( defSoundSet );
      else{
        // currently deleted sound set was the default one
        // -> we need a new default one
        SetSoundSet( defSoundSet = soundsets.begin()->first );
        isyslog( "sndctl (cSoundMan::DeleteSoundSet): Deleted sound set was the default sound set." );
        plugin->GetSetup()->Set( string( SNDCTL_SETUP_DEFAULT_SSET ), defSoundSet );
      }

      isyslog( "sndctl (cSoundMan::DeleteSoundSet): Active sound set to '%s'.", activeSoundSetID.c_str());
    }

    return true;
  }

  esyslog( "sndctl (cSoundMan::DeleteSoundSet): Failed to delete sound set with ID '%s'. Sound set not found.", Id.c_str());
  return false;
}

/*
 * returns the ID of the current active sound set
 */
string cSoundMan::GetCurrentSoundSetID( void ){
  return activeSoundSetID;
}

/*
 * returns a sound set object
 */
cSoundSet *cSoundMan::GetSoundSet( string Id, bool CreateNew ){
  map<string,cSoundSet*>::iterator it;

  // trying to find an already created sound set by its ID
  if( soundsets.find( Id ) != soundsets.end())
    return soundsets[Id];

  // trying to find a sound set by its name
  for( it = soundsets.begin(); it != soundsets.end(); it++ )
    if( it->second->GetName() == Id )
      return it->second;

  // no sound set found, create a new one or return nothing
  return CreateNew ? soundsets[Id] = new cSoundSet( mixer, Id ) : NULL;
}

/*
 * returns the sound sets map
 */
map<string,cSoundSet*> *cSoundMan::GetSoundSets(){
  return &soundsets;
}

/*
 * checks and correct the default soundset ID
 * (it's only done once at start)
 */
string cSoundMan::DefaultSoundSetID( string RequestedID ){
  string id;

  dsyslog( "sndctl (cSoundMan::DefaultSoundSetID): Trying to set default sound set to ID '%s'.", RequestedID.c_str());

  if( soundsets.find( RequestedID ) != soundsets.end())
    // 'RequestedID' found
    id = RequestedID;

  // if no soundset exists, we have to create a first one
  else if( soundsets.empty())
    id = CreateSoundSet();

  // at least, lets take the first one
  else
    id = soundsets.begin()->first;

  isyslog( "sndctl (cSoundMan::DefaultSoundSetID): Default sound set is '%s'.", id.c_str());

  // now return the default sound set to store this value
  return id;
}

/*
 * returns the current volume value
 */
int cSoundMan::GetVolume( void ){
  return volume;
}

/*
 * sets the active sound set
 */
string cSoundMan::SetSoundSet( string Id ){
  cSoundSet *sset;

  // look for this ID
  if( soundsets.find( Id ) == soundsets.end()){
    // look for the name
    if(( sset = GetSoundSet( Id, false )) == NULL ){
      esyslog( "sndctl (cSoundMan::SetSoundSet): No sound set with ID or name '%s' found.", Id.c_str());
      return string( "~" );
    } else {
      // 'Id' contains a sound set name, but we need an ID
      activeSoundSetID = sset->GetID();
    }
  } else
    activeSoundSetID = Id;

  isyslog( "sndctl (cSoundMan::SetSoundSet): Active sound set is now '%s'.", activeSoundSetID.c_str());

  // update mixer with sound set parameters
  mixer->Update( GetSoundSet( activeSoundSetID ));

  // set volume for new sound set
  SetVolume();

  return activeSoundSetID;
}

/*
 * doing a sound flash
 */
void cSoundMan::SoundFlash( void ){
  int vol;

  isyslog( "sndctl (cSoundMan::SoundFlash): Igniting sound flash." );

  // mark sound flashing in progress
  soundflash = true;

  // save volume
  vol = volume;

  // volume up
  cDevice::PrimaryDevice()->SetVolume( plugin->GetSetup()->GetInt( SNDCTL_SETUP_SOUNDFLASH_VOL ) / 100 * 255, true);

  // wait
  sleep( plugin->GetSetup()->GetInt( SNDCTL_SETUP_SOUNDFLASH_TIME ));

  // change volume back to former value
  cDevice::PrimaryDevice()->SetVolume( vol, true);

  // sound flashing is done
  soundflash = false;
}

/*
 * set the current volume level
 * (and checks the limits of 0..255)
 */
int cSoundMan::SetVolume( int Volume ){
  int initVol;

  // time for a sound flash?
  if( plugin->GetSetup()->GetBool( SNDCTL_SETUP_SOUNDFLASH ) &&
      !soundflash &&
      lastWasUp &&
      Volume < volume &&
      ( time( NULL ) - lastSet ) < plugin->GetSetup()->GetInt( SNDCTL_SETUP_SOUNDFLASH_DELTA )){
    SoundFlash();
    return volume;
  }

  // save time and direction of last volume setting
  time( &lastSet );
  lastWasUp = Volume > volume;

  // save volume
  volume = Volume < 0 ? volume : Volume;

  // care for initial volume?
  if( volumeCounter < 2 && plugin->GetSetup()->GetInt( SNDCTL_SETUP_INIT_VOLUME ) >= 0 ){
    volumeCounter++;
    initVol = plugin->GetSetup()->GetInt( SNDCTL_SETUP_INIT_VOLUME );
    dsyslog( "sndctl (cSoundMan::SetVolume): Using initial volume %d%%", initVol );
    GetSoundSet( activeSoundSetID )->Volume( initVol );
    return initVol / 100 * 255;
  }

  // work with an initial volume?
  if( plugin->GetSetup()->GetInt( SNDCTL_SETUP_INIT_VOLUME ) < 0 )
    // no initial volume, save given value
    volume = Volume < 0 ? volume : Volume;
  else
    // initial volume set, count this event
    volumeCounter++;

  // check limits
  volume = volume < 0 ? 0 : volume;
  volume = volume > 255 ? 255 : volume;

  isyslog( "sndctl (cSoundMan::SetVolume): Volume goes to %d (of 255).", volume );

  // set volume to current soundset
  GetSoundSet( activeSoundSetID )->Volume( 100 * volume / 255 );

  return volume;
}

/*
 * store all soundsets
 */
bool cSoundMan::Store( cPluginSndctl *Plugin ){
  map<string,cSoundSet*>::iterator it;

  // iterate over all soundsets
  for( it = soundsets.begin(); it != soundsets.end(); it++ )
    it->second->Store( it->first, Plugin );

  return true;
}
