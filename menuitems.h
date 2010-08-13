/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: menuitems.h
 * description: header file for soundsetitem.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_MENUITEMS_H
#define SNDCTL_MENUITEMS_H

#include "mixer.h"

/*
 * cursor position when editing a mixer control value
 */
enum CursorPos {op, val, limitL};

/*
 * cSoundSetItem
 * representation of an OSD menu entry for one sound set
 */
class cSoundSetItem:public cOsdItem {
  private:
    string    id;
    string    soundCardId;
    cSoundMan *soundman;

  public:
    cSoundSetItem( string, cSoundMan* );

    string GetId( void );
};

/*
 * cSoundSetChooserItem
 * representation of a list of all soundsets
 */
class cSoundSetChooserItem:public cMenuEditItem {
  private:
    string    *soundset;
    cSoundMan *soundman;

  protected:
    virtual void Set( void );

  public:
    cSoundSetChooserItem( cSoundMan*, string*, char* );

    virtual eOSState ProcessKey( eKeys );
};

/*
 * cSoundSetControlItem
 * representation of one mixer control value
 */
class cSoundSetControlItem:public cMenuEditStrItem {
  private:
    cControlId control;
    bool       isActive;
    cSoundSet *soundset;
    char      *value;

  public:
    cSoundSetControlItem( cSoundSet*, cControlId Control, bool );
    ~cSoundSetControlItem();

    virtual eOSState ProcessKey( eKeys );
};

#endif //SNDCTL_MENUITEMS_H
