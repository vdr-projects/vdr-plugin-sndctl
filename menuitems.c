/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: menuitems.c
 * description: representation of OSD menu entries
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "menuitems.h"
#include "soundman.h"

/*********************************************************
 * member functions for class cSoundSetItem
 *********************************************************/

/*
 * constructors
 */
cSoundSetItem::cSoundSetItem( string Id, cSoundMan *SoundMan )
:cOsdItem( "" ){
  string text;

  // save Id
  id = Id;

  // save sound manager
  soundman = SoundMan;

  // set text
  text = string();
  if( id == soundman->GetCurrentSoundSetID())
    text.append( ">" );
    
  text.append( "\t" );
  text.append( soundman->GetSoundSet( id )->GetName());

  SetText( text.c_str());
}

/*
 * returns this items sound set ID
 */
string cSoundSetItem::GetId( void ){
  return id;
}

/*********************************************************
 * member functions for class cSoundSetChooserItem
 *********************************************************/

/*
 * constructor
 */
cSoundSetChooserItem::cSoundSetChooserItem( cSoundMan *SoundMan, string *SoundSet, char *Text )
:cMenuEditItem( tr( Text )){
  // save arguments
  soundman = SoundMan;
  soundset = SoundSet;

  // set initial value
  Set();
}

/*
 * cares for pressing a key at a sound set entry
 */
eOSState cSoundSetChooserItem::ProcessKey( eKeys Key ){
  map<string,cSoundSet*>           *soundsets;
  map<string,cSoundSet*>::iterator  sset;

  eOSState state = cMenuEditItem::ProcessKey( Key );

  // get list of sound sets
  soundsets = soundman->GetSoundSets();
  sset = (*soundsets).find( *soundset );

  if( state == osUnknown )
    switch( Key ){
      case kLeft:
        // go to the previous sound set (if possible)
        if( sset-- != (*soundsets).begin())
          (*soundset).assign( sset->first );
        state = osContinue;
        break;

      case kRight:
        // go to the next sound set (if possible)
        if( ++sset != (*soundsets).end())
          (*soundset).assign( sset->first );
        state = osContinue;
        break;

      default:
        break;
  }

  // update display
  Set();

  return state;
}

/*
 * set the current value (means: the name of the current soundset)
 */
void cSoundSetChooserItem::Set( void ){
  SetValue( soundman->GetSoundSet( *soundset )->GetName().c_str());  
}

/*********************************************************
 * member functions for class cSoundSetControlItem
 *********************************************************/


static char* copyMixerControl(string controlString) {
	char *copied = new char[SNDCTL_MAX_LEN_CONTROL_VAL];
	strncpy(copied, controlString.c_str(), SNDCTL_MAX_LEN_CONTROL_VAL-1);
	return copied;
}

/*
 * constructor
 */
cSoundSetControlItem::cSoundSetControlItem( cSoundSet *SoundSet, cControlId Control, bool IsActive )
:cMenuEditStrItem( Control.getDisplayName().c_str(), copyMixerControl( SoundSet->Get( Control )), 11, "~=+-%0123456789;" ){
  // save arguments
  soundset = SoundSet;
  control = Control;
  isActive = IsActive;
}

/*
 * cares for pressing a key when editing a control value
 */
eOSState cSoundSetControlItem::ProcessKey( eKeys Key ){
  if( InEditMode()) {
	/* The ProcessKey call sets the value */
	eOSState state = cMenuEditStrItem::ProcessKey( Key );
    switch( Key ){
      case kOk:
        dsyslog( "sndctl (cSoundSetControlItem::ProcessKey): Control '%s' set to '%s'.", control.getDisplayName().c_str(), value );
        // save changed value
        soundset->Set( control, string( value ));
        // set volume
        if( isActive )
          soundset->Volume();
        break;

      default:
        break;
    }
    return state;
  } else {
	  return cMenuEditStrItem::ProcessKey( Key );
  }

}

cSoundSetControlItem::~cSoundSetControlItem() {
  delete[] value;
}
