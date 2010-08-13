/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: sndctl.c
 * description: main plugin file
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "sndctl.h"
#include "mainmenu.h"
#include "setupmenu.h"

static const char *VERSION         = "0.1.5-1";
static const char *DESCRIPTION     = "ALSA mixer control";
static const char *SVDRHelpPages[] = {
  "SSET [ sound set name ]\n"
  "    Set or show the active sound set by name.",
  "SSID [ sound set ID ]\n"
  "    Set or show the active sound set by ID.",
  "LIST [ names | all ]\n"
  "    List all available sound sets.\n"
  "    Specify 'names' to see sound set names instead of ID's.\n"
  "    Specify 'all' to see sound set ID's and names.",
  NULL
};

/*********************************************************
 * member functions for class cPluginSndctl
 *********************************************************/

/*
 * constructor
 */
cPluginSndctl::cPluginSndctl( void ){
  dsyslog( "sndctl (cPluginSndctl::cPluginSndctl): cPluginSndctl created" );

  // create sound manager
  soundman = new cSoundMan( this );

  // create setup manager
  setup = new cSetupSndctl();

  // initialize statusMonitor
  statusMonitor = 0;
}

/*
 * destructor
 */
cPluginSndctl::~cPluginSndctl(){
  // destroy something
  delete statusMonitor;
  delete setup;
  delete soundman;
  statusMonitor = 0;
  setup = 0;
  soundman = 0;
}

/*
 * return this plugins command line help
 */
const char *cPluginSndctl::CommandLineHelp( void ){
// FIXME: do something with this
  return NULL;
}

/*
 * return this plugins description
 */
const char *cPluginSndctl::Description( void ){
  return tr( DESCRIPTION );
}

/*
 * returns the sound manager
 */
cSoundMan *cPluginSndctl::GetSoundManager( void ){
  return soundman;
}

/*
 * returns the setup object
 */
cSetupSndctl *cPluginSndctl::GetSetup( void ){
  return setup;
}

/*
 * return this plugins main menu entry
 */
const char *cPluginSndctl::MainMenuEntry( void ){
  if( setup->GetBool( SNDCTL_SETUP_HIDEMAINMENUENTRY ))
    return NULL;

  return setup->Get( SNDCTL_SETUP_MENUNAME ).c_str();
}

/*
 * returns the OSD object for the main menu entry
 */
cOsdObject *cPluginSndctl::MainMenuAction( void ){
  return new cMainMenuSndctl( this );
}

/*
 * returns the setup page for this plugin
 */
cMenuSetupPage *cPluginSndctl::SetupMenu( void ){
  return new cSetupMenuSndctl( this );
}

/*
 * care for setup parameters
 */
bool cPluginSndctl::SetupParse( const char *Name, const char *Value ){
  return setup->Set( string( Name ), string( Value ), soundman );
}

/*
 * start function
 */
bool cPluginSndctl::Start( void ){
  // register i18n phrases
  RegisterI18n( Phrases );

  // set default sound set
  setup->Set( string( SNDCTL_SETUP_DEFAULT_SSET ), soundman->SetSoundSet( soundman->DefaultSoundSetID( setup->Get( string( SNDCTL_SETUP_DEFAULT_SSET )))));

  // create status monitor
  statusMonitor = new cStatusSndctl( soundman );

  return true;
}

/*
 * stop function
 */
void cPluginSndctl::Stop(void){
  // mute volume, if requested
  if( setup->GetBool( SNDCTL_SETUP_MUTE_ON_END )){
    dsyslog( "sndctl (cPluginSndctl::Stop): Mute on end is enabled." );
    soundman->SetVolume( 0 );
  }

  // save setup
  setup->Store( this );
}

/*
 * a wrapper for VDR's SetupStore function
 */
void cPluginSndctl::Store( string Name, string Value ){
  dsyslog( "sndctl (cPluginSndctl::Store): Store '%s' with value '%s'.", Name.c_str(), Value.c_str());

  if( Value.empty())
    SetupStore( Name.c_str());
  else
    SetupStore( Name.c_str(), Value.c_str());
}

/*
 * implements the SVDR commands
 */
cString cPluginSndctl::SVDRPCommand( const char *Command, const char *Option, int &ReplyCode ){
  map<string,cSoundSet*>::iterator  it;
  string                            list;
  map<string,cSoundSet*>           *ssets;

  isyslog( "sndctl (cPluginSndctl::SVDRPCommand): Receiving '%s %s'", Command, Option );

  // check for SSET
  if( strcasecmp( Command, SNDCTL_SVDR_SSET ) == 0 )
    if( *Option ){
      if( soundman->SetSoundSet( string( Option )) == "~" ){
        ReplyCode = 954;
        return cString::sprintf( "Sound set not found: \"%s\"", Option );
      } else {
        ReplyCode = 950;
        return cString::sprintf( "Switched to sound set: \"%s\"", Option );
      }
    }else {
      ReplyCode = 950;
      return cString::sprintf( "%s", soundman->GetSoundSet( soundman->GetCurrentSoundSetID())->GetName().c_str());
    }

  // check for SSID
  else if( strcasecmp( Command, SNDCTL_SVDR_SSID ) == 0 )
    if( *Option ){
      if( soundman->SetSoundSet( string( Option )) == "~" ){
        ReplyCode = 954;
        return cString::sprintf( "Sound set not found: \"%s\"", Option );
      } else {
        ReplyCode = 950;
        return cString::sprintf( "Switched to sound set: \"%s\"", Option );
      }
    }else {
      ReplyCode = 950;
      return cString::sprintf( "%s", soundman->GetCurrentSoundSetID().c_str());
    }

  // check for LIST
  else if( strcasecmp( Command, SNDCTL_SVDR_LIST ) == 0 ){
    // prepare list
    list = string();
    ssets = soundman->GetSoundSets();
    for( it = ssets->begin(); it != ssets->end(); it++ ){
      // add a newline, when list is not empty
      if( !list.empty())
        list.append( "\n" );

      if( strcasecmp( Option, "names" ) == 0 )
        list.append( it->second->GetName());
      else if( strcasecmp( Option, "all" ) == 0 )
        list.append( it->first + " " + it->second->GetName());
      else
        list.append( it->first );
    }

    ReplyCode = 950;
    return cString::sprintf("%s", list.c_str());
  }

  return NULL;
}

/*
 * returns the SVDR help page(s)
 */
const char **cPluginSndctl::SVDRPHelpPages( void ){
  return SVDRHelpPages;
}

/*
 * return this plugins version
 */
const char *cPluginSndctl::Version( void ){
  return VERSION;
}


/*
bool cPluginSndctl::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginSndctl::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

void cPluginSndctl::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginSndctl::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginSndctl::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

bool cPluginSndctl::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}
*/

VDRPLUGINCREATOR(cPluginSndctl); // Don't touch this! (oops, not in my dreams...!)
