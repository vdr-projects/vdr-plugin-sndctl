/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: mainmenu.h
 * description: header file for mainmenu.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_MAINMENU_H
#define SNDCTL_MAINMENU_H

/*
 * cMainMenuSndctl
 * the main menu OSD
 */
class cMainMenuSndctl:public cOsdMenu {
  private:
    cPluginSndctl *plugin;

    void Set( void );
    void SetHelpKeys( void );

  public:
    cMainMenuSndctl( cPluginSndctl* );

    virtual void     Display( void );
    virtual eOSState ProcessKey( eKeys );
};

#endif //SNDCTL_MAINMENU_H
