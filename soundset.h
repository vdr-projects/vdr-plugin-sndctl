/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: soundset.h
 * description: header file for soundset.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_SOUNDSET_H
#define SNDCTL_SOUNDSET_H

#include "sndctl.h"
#include "mixer.h"

/*
 * cSoundSet
 * sound set representation class definition
 */
class cSoundSet {
  private:
    struct control{
      cControlId control;
      char value[SNDCTL_MAX_LEN_CONTROL_VAL];
    }                  *controls;
    string              id;
    cMixer             *mixer;
    string              name;
    int                 nrOfControls;
    int                 volume;

  public:
    cSoundSet( cMixer*, string );
    ~cSoundSet();

    string         Get( cControlId ControlId );
    string         GetID( void );
    string         GetName( void );
    bool           Set( string, string );
    bool           Set(cControlId ControlId, string Value );
    bool           Store( string, cPluginSndctl* );
    void           Volume( int = -1 );
    vector<cControlId> GetControls( void );
};

#endif //SNDCTL_SOUNDSET_H
