//
//  UserDefaultsDataManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef UserDefaultsDataManager_h
#define UserDefaultsDataManager_h

#include <iostream>
#include "CocosGlobal.h"

class UserDefaultsDataManager
{
public:
    static UserDefaultsDataManager* getInstance();
    static void purge();
    
    int getIntegerForKey(const char* pKey, int defaultValue) const;
    void setIntegerForKey(const char* pKey, int value) const;
    std::string getStringForKey(const char* pKey, const std::string & defaultValue) const;
    void setStringForKey(const char* pKey, const std::string & value) const;
    std::string getStringForKeyWithAES(const char* pKey, const std::string & defaultValue) const;
    void setStringForKeyWithAES(const char* pKey, const std::string & value) const;
    bool getBoolForKey(const char* pKey, bool defaultValue) const;
    void setBoolForKey(const char* pKey, bool value) const;
    
protected:
    UserDefaultsDataManager();
    virtual ~UserDefaultsDataManager();
    M_DISALLOW_COPY_AND_ASSIGN(UserDefaultsDataManager);
    
public:
    void flush() const;
    
    void setFinishTutorialTag(int index) const;
    bool hasFinishedTutorial(int index) const;
    
    void setSoundOn(bool on) const;
    bool getSoundOn() const;
    
    void setMusicOn(bool on) const;
    bool getMusicOn() const;
};

#endif /* UserDefaultsDataManager_h */
