#include "Music.h"
#include "Core.h"
#include "SDL_mixer.h"

/* ******************************************** */

Music::Music(void) {

	this->currentMusic = mNOTHING;
}

Music::~Music(void) {
	for(unsigned int i = 0; i < vMusic.size(); i++) {
		Mix_FreeMusic(vMusic[i]);
	}

	vMusic.clear();

	for(unsigned int i = 0; i < vChunk.size(); i++) {
		Mix_FreeChunk(vChunk[i]);
	}

	vChunk.clear();
}

/* ******************************************** */

int Music::Init(){
    int audio_rate=44100;
    Uint16 audio_format=MIX_DEFAULT_FORMAT;
    int audio_channels=2;

    int ret=Mix_OpenAudio(audio_rate, audio_format, audio_channels, 2048);
    SDL_Log("Mix_OpenAudio1 ret:%d\n",ret);

    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    SDL_Log("Opened audio at %d Hz %d bit%s %s\n", audio_rate,
            (audio_format&0xFF),
            (SDL_AUDIO_ISFLOAT(audio_format) ? " (float)" : ""),
            (audio_channels > 2) ? "surround" :
            (audio_channels > 1) ? "stereo" : "mono");
/*
    Mix_Music* mux= Mix_LoadMUS("sounds/overworld.mp3");
    SDL_Log("Load snd ret:%p",mux);
    if(mux){
        //Mix_PlayChannel(0, mux, 2);
        Mix_FadeInMusic(mux,5,2000);
    }else {
        SDL_Log("Error:%s", SDL_GetError());
    }


*/
	vMusic.push_back(loadMusic("overworld"));
	vMusic.push_back(loadMusic("overworld-fast"));
	vMusic.push_back(loadMusic("underground"));
	vMusic.push_back(loadMusic("underground-fast"));
	vMusic.push_back(loadMusic("underwater"));
	vMusic.push_back(loadMusic("underwater-fast"));
	vMusic.push_back(loadMusic("castle"));
	vMusic.push_back(loadMusic("castle-fast"));
	vMusic.push_back(loadMusic("lowtime"));
	vMusic.push_back(loadMusic("starmusic"));
	vMusic.push_back(loadMusic("starmusic-fast"));
	vMusic.push_back(loadMusic("scorering"));

	vChunk.push_back(loadChunk("coin"));
	vChunk.push_back(loadChunk("blockbreak"));
	vChunk.push_back(loadChunk("blockhit"));
	vChunk.push_back(loadChunk("boom"));
	vChunk.push_back(loadChunk("bowserfall"));
	vChunk.push_back(loadChunk("bridgebreak"));
	vChunk.push_back(loadChunk("bulletbill"));
	vChunk.push_back(loadChunk("death"));
	vChunk.push_back(loadChunk("fire"));
	vChunk.push_back(loadChunk("fireball"));
	vChunk.push_back(loadChunk("gameover"));
	vChunk.push_back(loadChunk("intermission"));
	vChunk.push_back(loadChunk("jump"));
	vChunk.push_back(loadChunk("jumpbig"));
	vChunk.push_back(loadChunk("levelend"));
	vChunk.push_back(loadChunk("lowtime"));
	vChunk.push_back(loadChunk("mushroomappear"));
	vChunk.push_back(loadChunk("mushroomeat"));
	vChunk.push_back(loadChunk("oneup"));
	vChunk.push_back(loadChunk("pause"));
	vChunk.push_back(loadChunk("shrink"));
	vChunk.push_back(loadChunk("rainboom"));
	vChunk.push_back(loadChunk("shot"));
	vChunk.push_back(loadChunk("shrink"));
	vChunk.push_back(loadChunk("stomp"));
	vChunk.push_back(loadChunk("swim"));
	vChunk.push_back(loadChunk("vine"));
	vChunk.push_back(loadChunk("castleend"));
	vChunk.push_back(loadChunk("princessmusic"));

    setVolume(100);
    return 0;
}

void Music::changeMusic(bool musicByLevel, bool forceChange) {
	eMusic eNew = currentMusic;

	if(musicByLevel) {
		if(CCore::getMap()->getInEvent() && CCore::getMap()->getEvent()->inEvent) {
			eNew = mNOTHING;
			PlayChunk(cINTERMISSION);
		} else {
			switch(CCore::getMap()->getLevelType()) {
				case 0: case 4:
					eNew = CCore::getMap()->getMapTime() > 90 ? mOVERWORLD : mOVERWORLDFAST;
					break;
				case 1:
					eNew = CCore::getMap()->getMapTime() > 90 ? mUNDERWORLD : mUNDERWORLDFAST;
					break;
				case 2:
					eNew = CCore::getMap()->getMapTime() > 90 ? mUNDERWATER : mUNDERWATERFAST;
					break;
				case 3:
					eNew = CCore::getMap()->getMapTime() > 90 ? mCASTLE : mCASTLEFAST;
					break;
				case 100:
					eNew = mNOTHING;
					PlayChunk(cINTERMISSION);
					CCore::getMap()->setLevelType(0);
					break;
				default:
					eNew = mNOTHING;
					break;
			}
		}
	}

	if(currentMusic != eNew || forceChange) {
		StopMusic();
		currentMusic = eNew;
		PlayMusic();
	}
}

void Music::PlayMusic() {
	if(currentMusic != mNOTHING) {
		Mix_PlayMusic(vMusic[currentMusic - 1], -1);
		musicStopped = false;
	} else {
		StopMusic();
	}
}

void Music::PlayMusic(eMusic musicID) {
	if(musicID != mNOTHING) {
		Mix_PlayMusic(vMusic[musicID - 1], -1);
		musicStopped = false;
		currentMusic = musicID;
	} else {
		StopMusic();
		currentMusic = mNOTHING;
	}
}

void Music::StopMusic() {
	if(!musicStopped) {
		Mix_HaltMusic();
		musicStopped = true;
	}
}

void Music::PauseMusic() {
	if(Mix_PausedMusic() == 1) {
		Mix_ResumeMusic();
		musicStopped = false;
	} else {
		Mix_PauseMusic();
		musicStopped = true;
	}
}

/* ******************************************** */

void Music::PlayChunk(eChunk chunkID) {
	Mix_VolumeChunk(vChunk[chunkID], iVolume);
	Mix_PlayChannel(-1, vChunk[chunkID], 0);
}

/* ******************************************** */

Mix_Music* Music::loadMusic(std::string fileName) {
	fileName = "sounds/" + fileName + ".mp3";

    Mix_Music* ret = Mix_LoadMUS(fileName.c_str());
    if(!ret){
        SDL_Log("Error load file: %s\n", fileName.c_str());
    }
	return ret;
}

Mix_Chunk* Music::loadChunk(std::string fileName) {
	fileName = "sounds/" + fileName + ".wav";
    Mix_Chunk* ret= Mix_LoadWAV(fileName.c_str());
    if(!ret){
        SDL_Log("Error load file: %s\n", fileName.c_str());
    }
	return ret;
}

int Music::getVolume() {
	return iVolume;
}

void Music::setVolume(int iVolume) {
	this->iVolume = iVolume;
	Mix_VolumeMusic(iVolume);
}
