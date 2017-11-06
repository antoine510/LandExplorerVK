#include "sound.h"
#include "utility/xmlTools.h"

static bool loadSounds(Sound* sound);
static bool loadSound(Sound* sound, const std::string& path, unsigned int index);
static bool loadMusic(Sound* sound, const std::string& path, unsigned int index);
static void destroySounds(Sound* sound);

static void playSound(Sound* sound, int soundFlag);
static void playMusic(Sound* sound, int musicID);
static void pauseMusic(Sound* sound, bool pause);

static void changeMusic(Sound* sound, int musicID);
static void setMusicVolume(Sound* sound, float volume);
static void setMasterVolume(Sound* sound, float volume);

SoundStack* soundStack;

void initSoundstack()
{
    soundStack->currentIndex = 0;
    soundStack->masterVolume = 1.0f;
    soundStack->musicVolume = 0.5f;
    soundStack->music = MUSIC_NO_MUSIC;
    soundStack->musicChanged = true;
    soundStack->volumeChanged = true;
}

//! \brief Adds a sound to the stack \param soundFlag The sound flag
void soundstack_addSound(int soundFlag)
{
    SDL_assert(soundStack->currentIndex < MAX_SIMULTANEOUS_SOUNDS);
    soundStack->sounds[soundStack->currentIndex++] = soundFlag;
}

//! \brief Transmits the sounds to play \return The vector of flags
void soundstack_changeMusic(int musicID)
{
    if(musicID == soundStack->music) return;
    soundStack->music = musicID;
    soundStack->musicChanged = true;
}

void soundstack_changeMusicVolume(float volume)
{
    if(soundStack->musicVolume == volume) return;
    soundStack->musicVolume = volume;
    soundStack->volumeChanged = true;
}

void soundstack_changeMasterVolume(float volume)
{
    if(soundStack->masterVolume == volume) return;
    soundStack->masterVolume = volume;
    soundStack->volumeChanged = true;
}

Sound* sound_create()
{
    Sound* sound = (Sound*)calloc(1, sizeof(Sound));
    soundStack = (SoundStack*)calloc(1, sizeof(SoundStack));
    initSoundstack();

    sound->audioAvailable = true;

    FMOD_RESULT result = FMOD_System_Create(&sound->system);
    if(result != FMOD_OK) {fprintf(stderr, "FMOD Error : Couldn't create system\n"); sound->audioAvailable = false;}

    if(sound->audioAvailable)
    {
        result = FMOD_System_Init(sound->system, 8, FMOD_INIT_NORMAL, NULL);
        if(result != FMOD_OK) {fprintf(stderr, "FMOD error : %d while initializing system\n", result); sound->audioAvailable = false;}
    }

    if(sound->audioAvailable && loadSounds(sound) != 0)
    {
        fprintf(stderr, "FMOD error : Sound assets loading failed\n"); sound->audioAvailable = false;
    }

    //Initialization of music channel
    if(sound->audioAvailable)
    {
        playMusic(sound, MUSIC_OVERWORLD);
        pauseMusic(sound, true);
    }
    else fprintf(stderr, "Audio is disabled\n");

    return sound;
}

void sound_playAllSounds(Sound* sound)
{
    if(sound->audioAvailable)
    {
        FMOD_System_Update(sound->system);

        if (soundStack->volumeChanged || soundStack->musicChanged) {
            if(soundStack->musicChanged)
            {
                changeMusic(sound, soundStack->music);
                soundStack->musicChanged = false;
            }
            setMusicVolume(sound, soundStack->musicVolume);
            setMasterVolume(sound, soundStack->masterVolume);
            soundStack->volumeChanged = false;
        }

        unsigned int i;
        for(i = 0; i < soundStack->currentIndex; i++)
        {
            playSound(sound, soundStack->sounds[i]);
        }
    }

    soundStack->currentIndex = 0;
}

void sound_destroy(Sound* sound)
{
    if(sound->audioAvailable) destroySounds(sound);
    free(soundStack);
    free(sound);
}


bool loadSounds(Sound* sound)
{
    xmlDocPtr soundDoc = parseXML("audio/soundAssets.xml");
	xmlNodePtr soundType = xmlDocGetRootElement(soundDoc)->xmlChildrenNode;
	while(soundType->type == XML_TEXT_NODE) soundType = soundType->next;

	xmlNodePtr soundNode = soundType->children;
	while (soundNode)
    {
		if (checkName(soundNode, "Sound"))
		{
			std::string filename("audio/");
			filename += asStringl(soundType, "path");
			filename += asStringl(soundNode, "path");
            if(loadSound(sound, filename, asIntl(soundNode, "id"))) return true;
		}

        do soundNode = soundNode->next; while(soundNode && soundNode->type == XML_TEXT_NODE);
    }

    do soundType = soundType->next; while(soundType && soundType->type == XML_TEXT_NODE);
    xmlNodePtr musicNode = soundType->children;

	while (musicNode)
    {
		if (checkName(musicNode, "Music"))
		{
			std::string filename("audio/");
			filename += asStringl(soundType, "path");
			filename += asStringl(musicNode, "path");
            if(loadMusic(sound, filename, asIntl(musicNode, "id"))) return true;
		}

        do musicNode = musicNode->next; while(musicNode && musicNode->type == XML_TEXT_NODE);
    }

	xmlFreeDoc(soundDoc);

	return false;
}

bool loadSound(Sound* sound, const std::string& path, unsigned int index)
{
    FMOD_RESULT result = FMOD_System_CreateSound(sound->system, path.c_str(), FMOD_CREATESAMPLE, 0, &sound->sounds[index]);
    if(result != FMOD_OK) {std::cerr << "FMOD error : Couldn't load sound:" << path << std::endl; return true;}
    return false;
}

bool loadMusic(Sound* sound, const std::string& path, unsigned int index)
{
    FMOD_RESULT result = FMOD_System_CreateSound(sound->system, path.c_str(), FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &sound->musics[index]);
    if(result != FMOD_OK) { std::cerr << "FMOD error : Couldn't load music:" << path << std::endl; return true;}

    FMOD_Sound_SetLoopCount(sound->musics[index], -1);
    return false;
}

void setMusicVolume(Sound* sound, float volume)
{
    SDL_assert(FMOD_Channel_SetVolume(sound->musicChannel, volume) == FMOD_OK);
}

void setMasterVolume(Sound* sound, float volume)
{
    FMOD_CHANNELGROUP* masterGroup;

    SDL_assert(FMOD_System_GetMasterChannelGroup(sound->system, &masterGroup) == FMOD_OK);
    SDL_assert(FMOD_ChannelGroup_SetVolume(masterGroup, volume) == FMOD_OK);
}

void destroySounds(Sound* sound)
{
    int i;
    for(i = 0; i < MAX_SOUNDS; i++)
    {
        if(sound->sounds[i] != NULL) FMOD_Sound_Release(sound->sounds[i]);
    }
    for(i = 0; i < MAX_MUSICS; i++)
    {
        if(sound->musics[i] != NULL) FMOD_Sound_Release(sound->musics[i]);
    }
	FMOD_System_Close(sound->system);
    FMOD_System_Release(sound->system);
}

void playSound(Sound* sound, int soundFlag)
{
    FMOD_System_PlaySound(sound->system, sound->sounds[soundFlag], NULL, 0, NULL);
}

void playMusic(Sound* sound, int musicID)
{
	FMOD_Channel_Stop(sound->musicChannel);

    FMOD_System_PlaySound(sound->system, sound->musics[musicID], NULL, 0, &sound->musicChannel);
	pauseMusic(sound, false);

	sound->currentMusicID = musicID;
}

void pauseMusic(Sound* sound, bool pause)
{
    FMOD_Channel_SetPaused(sound->musicChannel, pause);
}

void changeMusic(Sound* sound, int musicID)
{
    if(musicID == MUSIC_NO_MUSIC)
    {
        pauseMusic(sound, true);
    }
    else if(musicID != sound->currentMusicID)
    {
        //SDL_assert(FMOD_Channel_SetPosition(sound->musicChannel, 0, FMOD_TIMEUNIT_MS) == FMOD_OK);

        playMusic(sound, musicID);
	}
	else {
		pauseMusic(sound, false);
	}
}
