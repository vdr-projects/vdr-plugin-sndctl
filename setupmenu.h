/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: setupmenu.h
 * description: header file for setupmenu.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_SETUPMENU_H
#define SNDCTL_SETUPMENU_H

/*
 * cSetupMenuSndctl
 * the OSD menu page for this plugin
 */
class cSetupMenuSndctl:public cMenuSetupPage {
  private:
    string         defSoundSet;
    string         ddAutoSoundSet;
    int            ddAutoSwitch;
    int            hideMainMenuEntry;
    int            initVolume;
    int            muteOnEnd;
    int            soundflash;
    char          *menuname;
    cPluginSndctl *plugin;

    void Set();

  protected:
    virtual void Store( void );

  public:
    cSetupMenuSndctl( cPluginSndctl* );
};

#endif //SNDCTL_SETUPMENU_H
