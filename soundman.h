/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: soundman.h
 * description: header file for soundman.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_SOUNDMAN_H
#define SNDCTL_SOUNDMAN_H

#include "alsa.h"
#include "sndctl.h"
#include "soundset.h"

/*
 * cSoundMan
 * sound manager class definition
 */
class cSoundMan {
  private:
    string                  activeSoundSetID;
    time_t                  lastSet;
    bool                    lastWasUp;
    cMixer                 *mixer;
    cPluginSndctl          *plugin;
    bool                    soundflash;
    map<string,cSoundSet*>  soundsets;
    int                     volume;      // this is the VDR volume, 0..255
    int                     volumeCounter;

    void                    SoundFlash( void );

  public:
    cSoundMan( cPluginSndctl* );
    ~cSoundMan();

    void                    AudioTrack( const char* );
    string                  CreateSoundSet( void );
    string                  DefaultSoundSetID( string );
    bool                    DeleteSoundSet( string );
    string                  GetCurrentSoundSetID( void );
    cSoundSet              *GetSoundSet( string, bool = true );
    map<string,cSoundSet*> *GetSoundSets( void );
    int                     GetVolume( void );
    string                  SetSoundSet( string );
    int                     SetVolume( int = -1 );
    bool                    Store( cPluginSndctl* );
};

#endif //SNDCTL_SOUNDMAN_H
