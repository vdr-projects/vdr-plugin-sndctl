/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: status.h
 * description: header file for status.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_STATUS_H
#define SNDCTL_STATUS_H

#include <vdr/status.h>
#include "soundman.h"

/*
 * cStatusSndctl
 * status monitor class definition
 */
class cStatusSndctl : public cStatus {
  private:
    cSoundMan *soundman;

  protected:
    virtual void SetAudioTrack( int, const char* const* );
    virtual void SetVolume( int, bool );

  public:
    cStatusSndctl( const cSoundMan *Soundman );
};

#endif //SNDCTL_STATUS_H
