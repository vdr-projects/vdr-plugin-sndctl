/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: alsa.h
 * description: header file for alsa.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_ALSA_H
#define SNDCTL_ALSA_H

#include <alsa/asoundlib.h>
#include "mixer.h"

/*
 * cAlsa
 * class for handling the Alsa API
 */
class cAlsa : public cMixer {
  private:
	struct alsacontrol : public control {
		string hwName;
		string mixername;
	};
	snd_mixer_t *handle;
    vector<snd_mixer_t *> handles;

    void init();

  protected:
    virtual void Volumes( void );

  public:
    cAlsa();
    virtual ~cAlsa();

    virtual bool IsMixer( string );
};

#endif //SNDCTL_ALSA_H
