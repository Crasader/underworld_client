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

using namespace std;

class UserDefaultsDataManager
{
public:
    static UserDefaultsDataManager* getInstance();
    static void purge();
    
    int getIntegerForKey(const char* pKey, int defaultValue) const;
    void setIntegerForKey(const char* pKey, int value) const;
    string getStringForKey(const char* pKey, const std::string & defaultValue) const;
    void setStringForKey(const char* pKey, const std::string & value) const;
    string getStringForKeyWithAES(const char* pKey, const std::string & defaultValue) const;
    void setStringForKeyWithAES(const char* pKey, const std::string & value) const;
    bool getBoolForKey(const char* pKey, bool defaultValue) const;
    void setBoolForKey(const char* pKey, bool value) const;
    
protected:
    UserDefaultsDataManager();
    virtual ~UserDefaultsDataManager();
    UserDefaultsDataManager(UserDefaultsDataManager const&) = delete;
    void operator=(UserDefaultsDataManager const&) = delete;
    
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
