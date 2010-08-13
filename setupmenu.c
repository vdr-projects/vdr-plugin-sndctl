/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: setupmenu.c
 * description: the OSD menu page for this plugin
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "setupmenu.h"
#include "menuitems.h"
#include "sndctl.h"
#include "setup.h"

/*********************************************************
 * member functions for class cSetupMenuSndctl
 *********************************************************/

/*
 * constructors
 */
cSetupMenuSndctl::cSetupMenuSndctl( cPluginSndctl *Plugin ){
  // save plugin
  plugin = Plugin;

  // get current values
  menuname = strdup( plugin->GetSetup()->Get( SNDCTL_SETUP_MENUNAME ).c_str());
  hideMainMenuEntry = plugin->GetSetup()->GetBool( SNDCTL_SETUP_HIDEMAINMENUENTRY ) ? 1 : 0;
  initVolume = plugin->GetSetup()->GetInt( SNDCTL_SETUP_INIT_VOLUME );
  defSoundSet = plugin->GetSetup()->Get( SNDCTL_SETUP_DEFAULT_SSET );
  ddAutoSwitch = plugin->GetSetup()->GetBool( SNDCTL_SETUP_DD_AUTO_SWITCH ) ? 1 : 0;
  ddAutoSoundSet = plugin->GetSetup()->Get( SNDCTL_SETUP_DD_AUTO_SSET );
  soundflash = plugin->GetSetup()->GetBool( SNDCTL_SETUP_SOUNDFLASH ) ? 1 : 0;
  muteOnEnd = plugin->GetSetup()->GetBool( SNDCTL_SETUP_MUTE_ON_END ) ? 1 : 0;

  // add content
  Set();
}

/*
 * makes setup menu entries
 */
void cSetupMenuSndctl::Set( void ){
  // main menu entry
  Add( new cMenuEditBoolItem( tr( SNDCTL_TXT_0002 ), &hideMainMenuEntry ));

  // main menu name
  Add( new cMenuEditStrItem( tr( SNDCTL_TXT_0007 ), menuname, 40, tr(FileNameChars)));

  // default soundset
  Add( new cSoundSetChooserItem( plugin->GetSoundManager(), &defSoundSet, SNDCTL_TXT_0003 ));

  // initial volume
  Add( new cMenuEditIntItem( tr( SNDCTL_TXT_0014 ), &initVolume, -1, 100 ));

  // DD auto switch
  Add( new cMenuEditBoolItem( tr( SNDCTL_TXT_0012 ), &ddAutoSwitch ));

  // DD auto sound set
  Add( new cSoundSetChooserItem( plugin->GetSoundManager(), &ddAutoSoundSet, SNDCTL_TXT_0013 ));

  // sound flash
  Add( new cMenuEditBoolItem( tr( SNDCTL_TXT_0015 ), &soundflash ));

  // sound flash
  Add( new cMenuEditBoolItem( tr( SNDCTL_TXT_0016 ), &muteOnEnd ));
}

/*
 * stores changed values
 */
void cSetupMenuSndctl::Store( void ){
  char vol[3];

  // main menu entry
  plugin->GetSetup()->Set( SNDCTL_SETUP_HIDEMAINMENUENTRY, string( hideMainMenuEntry ? "yes" : "no" ));

  // main menu name
  plugin->GetSetup()->Set( SNDCTL_SETUP_MENUNAME, string( menuname ));

  // default sound set
  plugin->GetSetup()->Set( SNDCTL_SETUP_DEFAULT_SSET, defSoundSet );

  // initial volume
  sprintf( vol, "%d", initVolume );
  plugin->GetSetup()->Set( SNDCTL_SETUP_INIT_VOLUME, string( vol ));

  // DD auto switch
  plugin->GetSetup()->Set( SNDCTL_SETUP_DD_AUTO_SWITCH, string( ddAutoSwitch ? "yes" : "no" ));

  // DD auto switch sound set
  plugin->GetSetup()->Set( SNDCTL_SETUP_DD_AUTO_SSET, ddAutoSoundSet );

  // sound flash
  plugin->GetSetup()->Set( SNDCTL_SETUP_SOUNDFLASH, string( soundflash ? "yes" : "no" ));

  // mute on end
  plugin->GetSetup()->Set( SNDCTL_SETUP_MUTE_ON_END, string( muteOnEnd ? "yes" : "no" ));

  // do a global store now
  plugin->GetSetup()->Store( plugin );
}
