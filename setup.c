/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: setup.c
 * description: management of setup configuration
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "setup.h"
#include "soundset.h"

/*********************************************************
 * member functions for class cSetupSndctl
 *********************************************************/

/*
 * constructor
 */
cSetupSndctl::cSetupSndctl(){
  dsyslog( "sndctl (cSetupSndctl::cSetupSndctl): cSetupSndctl created" );

  // create all possible setup items with default values
  items[SNDCTL_SETUP_MENUNAME] = tr( SNDCTL_DEFAULT_MENUNAME );
  items[SNDCTL_SETUP_HIDEMAINMENUENTRY] = SNDCTL_DEFAULT_HIDEMAINMENUENTRY;
  items[SNDCTL_SETUP_DEFAULT_SSET] = SNDCTL_DEFAULT_DEFAULT_SSET;
  items[SNDCTL_SETUP_INIT_VOLUME] = SNDCTL_DEFAULT_INIT_VOLUME;
  items[SNDCTL_SETUP_DD_AUTO_SWITCH] = SNDCTL_DEFAULT_DD_AUTO_SWITCH;
  items[SNDCTL_SETUP_DD_AUTO_SSET] = SNDCTL_DEFAULT_DD_AUTO_SSET;
  items[SNDCTL_SETUP_SOUNDFLASH] = SNDCTL_DEFAULT_SOUNDFLASH;
  items[SNDCTL_SETUP_SOUNDFLASH_DELTA] = SNDCTL_DEFAULT_SOUNDFLASH_DELTA;
  items[SNDCTL_SETUP_SOUNDFLASH_VOL] = SNDCTL_DEFAULT_SOUNDFLASH_VOL;
  items[SNDCTL_SETUP_SOUNDFLASH_TIME] = SNDCTL_DEFAULT_SOUNDFLASH_TIME;
  items[SNDCTL_SETUP_MUTE_ON_END] = SNDCTL_DEFAULT_MUTE_ON_END;
}

/*
 * returns a setup value
 */
string cSetupSndctl::Get( string Name ){
  if( items.find( Name ) != items.end())
    return items[Name];

  // default return value is NULL
  return NULL;
}

/*
 * returns a setup value as bool
 */
bool cSetupSndctl::GetBool( string Name ){
  return ( Get( Name ) == "yes" ) ? true : false;
}

/*
 * returns a setup value as integer
 */
int cSetupSndctl::GetInt( string Name ){
  return atoi( Get( Name ).c_str());
}

/*
 * set a setup value
 */
bool cSetupSndctl::Set( string Name, string Value, cSoundMan *Soundman ){
  bool         result;
  cSoundSet    *sset;
  unsigned int div;

  dsyslog( "sndctl (cSetupSndctl::Set): Receiving parameter '%s' with value '%s'.", Name.c_str(), Value.c_str() );

  // save this parameter
  parameters.push_back( Name );

  // lets assume the worst case ;-)
  result = false;

  // first check for a general option
  if( result = (items.find( Name ) != items.end())){
    // save this item
    items[Name] = Value;

    // take special care for SNDCTL_DEFAULT_DD_AUTO_SSET
    if( Name == SNDCTL_SETUP_DEFAULT_SSET &&
        items[SNDCTL_SETUP_DD_AUTO_SSET].empty())
      items[SNDCTL_SETUP_DD_AUTO_SSET] = Value;
  }

  // assuming this value belongs to a soundset
  // we need a '_' in 'Name' to deal with this
  else if(( div = Name.find_first_of( '_' )) != string::npos ){
    // get sound set for this ID (it's up to the first '_')
    sset = Soundman->GetSoundSet( Name.substr( 0, div ));

    // set value
    result = sset->Set( Name.substr( div + 1 ), Value );
  }

  // nothing of all, that's bad
  return result;
}

/*
 * store all setup values
 */
bool cSetupSndctl::Store( cPluginSndctl *Plugin ){
  vector<string>::iterator it;

  // first, remove all parameters
  for( it = parameters.begin(); it != parameters.end(); it++ ){
    dsyslog( "sndctl (cSetupSndctl::Store): Removing: %s", (*it).c_str());
    Plugin->Store( *it, string() );
  }

  // store all general settings
  Plugin->Store( string( SNDCTL_SETUP_MENUNAME ), Get( SNDCTL_SETUP_MENUNAME ));
  Plugin->Store( string( SNDCTL_SETUP_HIDEMAINMENUENTRY ), Get( SNDCTL_SETUP_HIDEMAINMENUENTRY ));
  Plugin->Store( string( SNDCTL_SETUP_DEFAULT_SSET ), Get( SNDCTL_SETUP_DEFAULT_SSET ));
  Plugin->Store( string( SNDCTL_SETUP_INIT_VOLUME ), Get( SNDCTL_SETUP_INIT_VOLUME ));
  Plugin->Store( string( SNDCTL_SETUP_DD_AUTO_SWITCH ), Get( SNDCTL_SETUP_DD_AUTO_SWITCH ));
  Plugin->Store( string( SNDCTL_SETUP_DD_AUTO_SSET ), Get( SNDCTL_SETUP_DD_AUTO_SSET ));
  Plugin->Store( string( SNDCTL_SETUP_SOUNDFLASH ), Get( SNDCTL_SETUP_SOUNDFLASH ));
  Plugin->Store( string( SNDCTL_SETUP_SOUNDFLASH_DELTA ), Get( SNDCTL_SETUP_SOUNDFLASH_DELTA ));
  Plugin->Store( string( SNDCTL_SETUP_SOUNDFLASH_VOL ), Get( SNDCTL_SETUP_SOUNDFLASH_VOL ));
  Plugin->Store( string( SNDCTL_SETUP_SOUNDFLASH_TIME ), Get( SNDCTL_SETUP_SOUNDFLASH_TIME ));
  Plugin->Store( string( SNDCTL_SETUP_MUTE_ON_END ), Get( SNDCTL_SETUP_MUTE_ON_END ));

  // store all settings from all soundsets
  Plugin->GetSoundManager()->Store( Plugin );

  return true;
}
