#pragma once

#include "common.h"
#include <fmod.h>

#define MAX_SIMULTANEOUS_SOUNDS 16
#define MAX_SOUNDS 256
#define MAX_MUSICS 32

#define SOUND_NO_SOUND 0
#define SOUND_DIG 1
#define SOUND_DIG_ALT 2
#define SOUND_DIG_ALT2 3
#define SOUND_TINK 4
#define SOUND_TINK_ALT 5
#define SOUND_TINK_ALT2 6
#define SOUND_MENU_OPEN 7
#define SOUND_MENU_CLOSE 8
#define SOUND_MENU_TICK 9
#define SOUND_GRAB 10
#define SOUND_PLAYER_HIT 11
#define SOUND_PLAYER_HIT_ALT 12
#define SOUND_PLAYER_HIT_ALT2 13
#define SOUND_PLAYER_DEATH 14
#define SOUND_SWING_SWORD 15
#define SOUND_HIT_PASSIVE 16
#define SOUND_HIT_ENEMY 17
#define SOUND_KILL_PASSIVE 18
#define SOUND_KILL_ENEMY 19

#define MUSIC_NO_MUSIC 0
#define MUSIC_OVERWORLD 1
#define MUSIC_OVERWORLD_ALT 2
#define MUSIC_TITLESCREEN 3
#define MUSIC_WIN 4

typedef struct SoundStack {
    int sounds[MAX_SIMULTANEOUS_SOUNDS];
    unsigned int currentIndex;
    int music;
    float musicVolume, masterVolume;
    bool musicChanged, volumeChanged;
} SoundStack;

extern SoundStack* soundStack;

typedef struct Sound {
    FMOD_SYSTEM* system;
    bool audioAvailable;

    FMOD_SOUND* sounds[MAX_SOUNDS];
    FMOD_SOUND* musics[MAX_MUSICS];
    FMOD_CHANNEL* musicChannel;

	int currentMusicID;
} Sound;

void soundstack_addSound(int soundFlag);
void soundstack_changeMusic(int musicID);
void soundstack_changeMusicVolume(float volume);
void soundstack_changeMasterVolume(float volume);

Sound* sound_create();
void sound_playAllSounds(Sound* sound);
void sound_destroy(Sound* sound);

