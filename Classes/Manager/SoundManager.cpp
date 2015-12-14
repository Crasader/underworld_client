//
//  SoundManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "SoundManager.h"
#include "audio/include/SimpleAudioEngine.h"
#include "UserDefaultsDataManager.h"

using namespace std;

static const string BUTTON_SOUND_OK("sounds/button_ok.mp3");
static const string BUTTON_SOUND_CANCEL("sounds/button_cancel.mp3");
static const string BUTTON_SOUND_INVALID("sounds/button_invalid.mp3");

static SoundManager *s_pSharedInstance = nullptr;

SoundManager* SoundManager::getInstance()
{
    if (!s_pSharedInstance)
    {
        s_pSharedInstance = new (std::nothrow) SoundManager();
        CCASSERT(s_pSharedInstance, "FATAL: Not enough memory");
    }
    
    return s_pSharedInstance;
}

void SoundManager::purge()
{
    if (s_pSharedInstance)
    {
        delete s_pSharedInstance;
        s_pSharedInstance = nullptr;
    }
}

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
    
}

#pragma mark - config
const string& SoundManager::getCurrentMusic()
{
    return _playingMusic;
}

void SoundManager::setMusicOn(bool on)
{
    if (on != _isMusicOn)
    {
        _isMusicOn = on;
        UserDefaultsDataManager::getInstance()->setMusicOn(on);
        
        if (on)
        {
            resumeBackgroundMusic();
        }
        else
        {
            pauseBackgroundMusic();
        }
    }
}

bool SoundManager::isMusicOn()
{
    return _isMusicOn;
}

void SoundManager::setSoundOn(bool on)
{
    if (on != _isSoundOn)
    {
        _isSoundOn = on;
        UserDefaultsDataManager::getInstance()->setSoundOn(on);
        
        if (false == on)
        {
            stopAllSounds();
        }
    }
}

bool SoundManager::isSoundOn()
{
    return _isSoundOn;
}

#pragma mark - music
void SoundManager::playBackgroundMusic(const string& url,bool loop)
{
    if (_isMusicOn)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(url.c_str(), loop);
    }
    
    _isPaused = false;
    // it also needs to resign the url when music is off, in case the music will be resumed
    _playingMusic.assign(url);
}

void SoundManager::resumeBackgroundMusic()
{
    if (_playingMusic.length() > 0)
    {
        if (_isPaused)
        {
            _isPaused = false;
            CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
        }
        else
        {
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_playingMusic.c_str());
        }
    }
}

void SoundManager::pauseBackgroundMusic()
{
    if (_playingMusic.length() > 0)
    {
        _isPaused = true;
        CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    }
}

void SoundManager::preloadBackgroundMusic(const string& url)
{
    if (_isMusicOn)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(url.c_str());
    }
}

void SoundManager::stopBackgroundMusic()
{
    if (_playingMusic.length() > 0)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }
    
    _isPaused = false;
    _playingMusic.assign("");
}

#pragma mark - sound
unsigned int SoundManager::playSound(const string& url, const bool loop)
{
    if (_isSoundOn)
    {
        return CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(url.c_str(), loop);
    }
    else
    {
        return 0;
    }
}

void SoundManager::preloadSound(const string& url)
{
    if (_isSoundOn)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(url.c_str());
    }
}

void SoundManager::stopSound(unsigned int id)
{
    if (_isSoundOn)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(id);
    }
}

void SoundManager::stopAllSounds()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
}

#pragma mark - special
void SoundManager::playButtonSound()
{
    playSound(BUTTON_SOUND_OK);
}

void SoundManager::playButtonCancelSound()
{
    playSound(BUTTON_SOUND_CANCEL);
}

void SoundManager::playButtonInvalidSound()
{
    playSound(BUTTON_SOUND_INVALID);
}

void SoundManager::playMessageHintSound()
{
    playSound("sounds/effect/map_star_01.mp3");
}