//
//  UserDefaultHelper.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef UserDefaultHelper_h
#define UserDefaultHelper_h

#include <iostream>

namespace UserDefaultHelper
{
    int getIntegerForKey(const char* pKey, int defaultValue);
    void setIntegerForKey(const char* pKey, int value);
    std::string getStringForKey(const char* pKey, const std::string & defaultValue);
    void setStringForKey(const char* pKey, const std::string & value);
    std::string getStringForKeyWithAES(const char* pKey, const std::string & defaultValue);
    void setStringForKeyWithAES(const char* pKey, const std::string & value);
    bool getBoolForKey(const char* pKey, bool defaultValue);
    void setBoolForKey(const char* pKey, bool value);
    
    void flush();
    
    void setSoundOn(bool on);
    bool getSoundOn();
    
    void setMusicOn(bool on);
    bool getMusicOn();
};

#endif /* UserDefaultHelper_h */
