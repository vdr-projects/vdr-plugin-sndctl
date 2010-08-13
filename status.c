/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: status.c
 * description: status monitor to get informations from VDR
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#include "defaults.h"
#include "status.h"

/*********************************************************
 * member functions for class cStatusSndctl
 *********************************************************/

/*
 * constructor
 */
cStatusSndctl::cStatusSndctl( const cSoundMan *Soundman ){
  // save sound manager pointer
  soundman = (cSoundMan*) Soundman;
}

void cStatusSndctl::SetAudioTrack( int Index, const char * const *Tracks ){
  isyslog( "sndctl (cStatusSndctl::SetAudioTrack): Audio track switched to '%s'.", Tracks[Index] );

  // inform sound manager
  soundman->AudioTrack( Tracks[Index] );
}

/*
 * the volume has been set to the given value, either
 * absolutely or relative to the current volume.
 */
void cStatusSndctl::SetVolume( int Volume, bool Absolute ){

  isyslog( "sndctl (cStatusSndctl::SetVolume): Received volume from VDR: '%d'.", Volume );

  // set current volume
  if( Absolute )
    soundman->SetVolume( Volume );
  else
    soundman->SetVolume( soundman->GetVolume() + Volume );
}
