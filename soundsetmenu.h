/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: soundsetmenu.h
 * description: header file for soundsetmenu.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_SOUNDSETMENU_H
#define SNDCTL_SOUNDSETMENU_H

/*
 * cSoundsetMenuSndctl
 * the sound set editor menu
 */
class cSoundsetMenuSndctl:public cMenuSetupPage {
  private:
    bool       isActive;
    char      *name;
    cSoundSet *soundset;

    void Set();

  protected:
    virtual void Store( void );

  public:
    cSoundsetMenuSndctl( cSoundSet*, string );
};

#endif //SNDCTL_SOUNDSETMENU_H
