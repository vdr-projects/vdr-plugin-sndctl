/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: mixer.h
 * description: header file for mixer.c
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_MIXER_H
#define SNDCTL_MIXER_H

/*
 * cMixer
 * parent class for all types of mixer devices
 */

class cControlId {
private:
  string name;
  int controlNr;
  string soundCardId;
public:
  cControlId(string SoundCardId, string DisplayName, int controlNr);
  cControlId();
  bool operator==(cControlId const& b);
  string getDisplayName();
  int getControlNr();
  string getSoundCardId();
};

class cMixer {
  protected:
    struct control{
      long   min;
      long   max;
      string name;
      long   limitL;
      long   limitU;
      char   op;
      int    value;
      long   volume;
      string soundCardId;
      int    controlNr;
    };

    vector<control *> vControls;

    virtual void           Volumes( void )        = 0;

  public:

    cMixer();
    virtual ~cMixer();

    vector<cControlId> GetControls( void );
    void           Update( cSoundSet* );
    void           Volume( int );
    int            CountControls( void );

    virtual bool   IsMixer( string ) = 0;
};

#endif //SNDCTL_MIXER_H
