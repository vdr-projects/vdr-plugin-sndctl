/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: soundsetmenu.c
 * description: the sound set editor menu
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "soundsetmenu.h"
#include "menuitems.h"
#include "soundset.h"

/*********************************************************
 * member functions for class cSoundsetMenuSndctl
 *********************************************************/

/*
 * constructors
 */
cSoundsetMenuSndctl::cSoundsetMenuSndctl( cSoundSet *SoundSet, string ActiveSoundSet ){
  string title;

  // save parameter
  soundset = SoundSet;
  isActive = soundset->GetID() == ActiveSoundSet;

  // set titel
  title = string( tr(SNDCTL_TXT_0006));
  title.append( " - " );
  title.append( soundset->GetName());
  SetTitle( title.c_str());

  // get initial values
  name = strdup( soundset->GetName().c_str());

  // add content
  Set();
}

/*
 * makes setup menu entries
 */
void cSoundsetMenuSndctl::Set( void ){
  vector<cControlId>           controls;
  vector<cControlId>::iterator it;

  // sound set name
  Add( new cMenuEditStrItem( tr( SNDCTL_TXT_0008 ), name, 20, tr(FileNameChars)));

  // one entry for every control
  controls = soundset->GetControls();
  cControlId *lastId = NULL;
  for( it = controls.begin(); it != controls.end(); it++ ) {
    if (lastId == NULL || lastId->getSoundCardId() != it->getSoundCardId())
      Add (new cOsdItem(it->getSoundCardId().c_str(), osUnknown, false));
    lastId = it.base();
    Add( new cSoundSetControlItem( soundset, *it, isActive ));
  }
}

/*
 * stores changed values
 */
void cSoundsetMenuSndctl::Store( void ){
  // name
  soundset->Set( string( "name" ), string( name ));
}
