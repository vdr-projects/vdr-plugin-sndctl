/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: mainmenu.c
 * description: the main menu OSD
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "mainmenu.h"
#include "soundset.h"
#include "soundsetmenu.h"
#include "menuitems.h"

/*********************************************************
 * member functions for class cMainMenuSndctl
 *********************************************************/

/*
 * constructors
 */
cMainMenuSndctl::cMainMenuSndctl( cPluginSndctl *Plugin )
:cOsdMenu( tr( SNDCTL_TXT_0001 ), 2 ){
  // save plugin
  plugin = Plugin;

  // add content
  Set();

  // add help keys
  SetHelpKeys();

  // show
  Display();
}

/*
 * displays the menu (after refreshing the content)
 */
void cMainMenuSndctl::Display( void ){
  // refresh items
  Set();

  // let do cOsdMenu the rest of the job
  cOsdMenu::Display();

  // update help keys
  SetHelpKeys();
}

/*
 * cares for pressing a key at a sound set entry
 */
eOSState cMainMenuSndctl::ProcessKey( eKeys Key ){
  cSoundSetItem *item;

  // get the current item
  item = (cSoundSetItem *) Get( Current());

  eOSState state = cOsdMenu::ProcessKey( Key );

  if( state == osUnknown )
    switch( Key ){
      case kOk:
      case kRed:
        // select sound set
        plugin->GetSoundManager()->SetSoundSet( item->GetId());
        state = osEnd;
        break;

      case kGreen:
        // edit sound set
        return AddSubMenu( new cSoundsetMenuSndctl( plugin->GetSoundManager()->GetSoundSet( item->GetId()), plugin->GetSoundManager()->GetCurrentSoundSetID()));
        break;

      case kYellow:
        // delete sound set only if there are more than one
        if(plugin->GetSoundManager()->GetSoundSets()->size() <= 1)
          break;
        if( Interface->Confirm( tr( SNDCTL_TXT_0011 ))){
          plugin->GetSoundManager()->DeleteSoundSet( item->GetId());
          cOsdMenu::Del( Current());
          SetHelpKeys();
          Display();
        }

        state = osContinue;
        break;

      case kBlue:
        // create a new sound set and switch to edit screen
        return AddSubMenu( new cSoundsetMenuSndctl( plugin->GetSoundManager()->GetSoundSet( plugin->GetSoundManager()->CreateSoundSet()), plugin->GetSoundManager()->GetCurrentSoundSetID()));
        break;

      default:
        break;
  }

  return state;
}

/*
 * makes one menu entry for every soundset
 */
void cMainMenuSndctl::Set( void ){
  map<string,cSoundSet*>::iterator  it;
  map<string,cSoundSet*>           *soundsets;

  // remove any old item
  Clear();

  // get defined sound sets
  soundsets = plugin->GetSoundManager()->GetSoundSets();

  // iterate over all soundsets
  for( it = (*soundsets).begin(); it != (*soundsets).end(); it++ )
    Add( new cSoundSetItem( it->first, plugin->GetSoundManager()));
}

/*
 * set the colored help keys for this menu
 */
void cMainMenuSndctl::SetHelpKeys( void ){
  if (plugin->GetSoundManager()->GetSoundSets()->size() > 1) {
    SetHelp( tr( SNDCTL_TXT_0004 ), tr( SNDCTL_TXT_0005 ), tr( SNDCTL_TXT_0010 ), tr( SNDCTL_TXT_0009 ));
  } else {
    SetHelp( tr( SNDCTL_TXT_0004 ), tr( SNDCTL_TXT_0005 ), NULL, tr( SNDCTL_TXT_0009 ));
  }
}
