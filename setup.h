/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: setup.h
 * description: header file for setup.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_SETUP_H
#define SNDCTL_SETUP_H

#include "sndctl.h"

/*
 * cSetupSndctl
 * setup class definition
 */
class cSetupSndctl {
  private:
    map<string,string> items;
    vector<string>     parameters;

  public:
    cSetupSndctl();

    string Get( string );
    bool   GetBool( string );
    int    GetInt( string );
    bool   Set( string, string, cSoundMan* = NULL );
    bool   Store( cPluginSndctl* );
};

#endif //SNDCTL_SETUP_H
