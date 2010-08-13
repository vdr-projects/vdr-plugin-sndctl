/*
 * sndctl  -  a plugin for the Video Disk Recorder
 *        file: defaults.h
 * description: default definitions
 *
 *      author: Thomas Hildebrandt <toxym@web.de>
 *
 * inspired by and reengineered from 'avolctl'
 * thanks to Martin Prochnow <nordlichtl@martins-kabuff.de>
 */

#ifndef SNDCTL_DEFAULTS_H
#define SNDCTL_DEFAULTS_H

class cMainMenuSndctl;
class cMixer;
class cPluginSndctl;
class cSetupSndctl;
class cSetupMenuSndctl;
class cSoundMan;
class cSoundSet;
class cSoundsetMenuSndctl;
class cStatusSndctl;

#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <vdr/interface.h>
#include <vdr/plugin.h>
#include "i18n.h"
using namespace std;

#define SNDCTL_MAX_LEN_CONTROL_NAME      128
#define SNDCTL_MAX_LEN_CONTROL_VAL       12

#define SNDCTL_DEFAULT_MENUNAME          "Sound control"
#define SNDCTL_DEFAULT_SOUNDSET_NAME     "New sound set"
#define SNDCTL_DEFAULT_HIDEMAINMENUENTRY "no"
#define SNDCTL_DEFAULT_DEFAULT_SSET      ""
#define SNDCTL_DEFAULT_INIT_VOLUME       "-1"
#define SNDCTL_DEFAULT_DD_AUTO_SWITCH    "no"
#define SNDCTL_DEFAULT_DD_AUTO_SSET      ""
#define SNDCTL_DEFAULT_SOUNDFLASH        "no"
#define SNDCTL_DEFAULT_SOUNDFLASH_DELTA  "2"
#define SNDCTL_DEFAULT_SOUNDFLASH_VOL    "100"
#define SNDCTL_DEFAULT_SOUNDFLASH_TIME   "1"
#define SNDCTL_DEFAULT_MUTE_ON_END       "yes"

#define SNDCTL_SETUP_MENUNAME            "MenuName"
#define SNDCTL_SETUP_HIDEMAINMENUENTRY   "HideMainMenuEntry"
#define SNDCTL_SETUP_DEFAULT_SSET        "DefaultSoundSet"
#define SNDCTL_SETUP_INIT_VOLUME         "InitialVolume"
#define SNDCTL_SETUP_DD_AUTO_SWITCH      "DDAutoSwitch"
#define SNDCTL_SETUP_DD_AUTO_SSET        "DDAutoSoundSet"
#define SNDCTL_SETUP_SOUNDFLASH          "SoundFlash"
#define SNDCTL_SETUP_SOUNDFLASH_DELTA    "SoundFlashDelta"
#define SNDCTL_SETUP_SOUNDFLASH_VOL      "SoundFlashVolume"
#define SNDCTL_SETUP_SOUNDFLASH_TIME     "SoundFlashTime"
#define SNDCTL_SETUP_MUTE_ON_END         "MuteOnEnd"

#define SNDCTL_MIXER_NONE                '~'
#define SNDCTL_MIXER_STATIC              '='
#define SNDCTL_MIXER_PERCENT             '%'
#define SNDCTL_MIXER_PLUS                '+'
#define SNDCTL_MIXER_MINUS               '-'

#define SNDCTL_SVDR_SSET                 "SSET"
#define SNDCTL_SVDR_SSID                 "SSID"
#define SNDCTL_SVDR_LIST                 "LIST"

#endif //SNDCTL_DEFAULTS_H
