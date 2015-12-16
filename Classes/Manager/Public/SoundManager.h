//
//  SoundManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef SoundManager_h
#define SoundManager_h

#include <iostream>
#include "CocosGlobal.h"

class SoundManager
{
public:
    static SoundManager * getInstance();
    static void purge();
    
    // public
    const std::string& getCurrentMusic();
    void setMusicOn(bool on);
    bool isMusicOn();
    void setSoundOn(bool on);
    bool isSoundOn();
    void playBackgroundMusic(const std::string& url, bool loop = true);
    void stopBackgroundMusic();
    void resumeBackgroundMusic();
    void pauseBackgroundMusic();
    unsigned int playSound(const std::string& url, const bool loop = false);
    void stopSound(const unsigned int);
    
    // special
    void playButtonSound();
    void playButtonCancelSound();
    void playButtonInvalidSound();
    void playMessageHintSound();
    void playButtonSelectUnitSound();
    void playButtonGoOnSound();
    
protected:
    SoundManager();
    virtual ~SoundManager();
    M_DISALLOW_COPY_AND_ASSIGN(SoundManager);
    
private:
    void preloadBackgroundMusic(const std::string& url);
    
    void preloadSound(const std::string& url);
    void stopAllSounds();
    
private:
    bool _isMusicOn;
    bool _isSoundOn;
    std::string _playingMusic;
    bool _isPaused;
};

#endif /* SoundManager_h */
