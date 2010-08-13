/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: sndctl.h
 * description: header file for sndctl.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_SNDCTL_H
#define SNDCTL_SNDCTL_H

#include "setup.h"
#include "soundman.h"
#include "status.h"

/*
 * cPluginSndctl
 * plugin main class definition
 */
class cPluginSndctl : public cPlugin {
  private:
    cSetupSndctl  *setup;
    cSoundMan     *soundman;
    cStatusSndctl *statusMonitor;

  public:
    cPluginSndctl( void );
    virtual ~cPluginSndctl();

    virtual const char      *CommandLineHelp( void );
    virtual const char      *Description( void );
    cSoundMan               *GetSoundManager( void );
    cSetupSndctl            *GetSetup( void );
    virtual const char      *MainMenuEntry( void );
    virtual cOsdObject      *MainMenuAction( void );
    virtual cMenuSetupPage  *SetupMenu( void );
    virtual bool             SetupParse( const char*, const char* );
    virtual bool             Start( void );
    virtual void             Stop( void );
    void                     Store( string, string );
    virtual cString          SVDRPCommand( const char*, const char*, int& );
    virtual const char     **SVDRPHelpPages( void );
    virtual const char      *Version( void );

/*    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual void Housekeeping(void);
    virtual void MainThreadHook(void);
    virtual cString Active(void);
    virtual bool Service(const char *Id, void *Data = NULL);
*/
};

#endif //SNDCTL_SNDCTL_H

