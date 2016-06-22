//
//  UserDefaultsDataManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "UserDefaultsDataManager.h"
#include "CocosUtils.h"
#include "AESCTREncryptor.h"
#include "MD5Verifier.h"
#include "Utils.h"

USING_NS_CC;
using namespace std;

static const char *SOUND_ON_TAG = "SOUND_ON_TAG";
static const char *MUSIC_ON_TAG = "MUSIC_ON_TAG";

void UserDefaultsDataManager::flush()
{
    UserDefault::getInstance()->flush();
}

void UserDefaultsDataManager::setStringForKey(const char* pKey, const string & value)
{
    UserDefault::getInstance()->setStringForKey(pKey, MD5Verifier::getInstance()->encrypt(value));
}

string UserDefaultsDataManager::getStringForKey(const char* pKey, const string & defaultValue)
{
    string value = UserDefault::getInstance()->getStringForKey(pKey, "");
    if (value.empty()) {
        return defaultValue;
    }
    string realValue;
    if(MD5Verifier::getInstance()->verify(value, realValue)){
        return realValue;
    }
    return defaultValue;
}

void UserDefaultsDataManager::setStringForKeyWithAES(const char* pKey, const string & value)
{
    string enValue = AESCTREncryptor::getInstance()->Encrypt(value);
    UserDefault::getInstance()->setStringForKey(pKey, MD5Verifier::getInstance()->encrypt(CocosUtils::charToHex(enValue)));
}

string UserDefaultsDataManager::getStringForKeyWithAES(const char* pKey, const string & defaultValue)
{
    string value = UserDefault::getInstance()->getStringForKey(pKey, "");
    if (value.empty()) {
        return defaultValue;
    }
    string hexValue;
    if(MD5Verifier::getInstance()->verify(value, hexValue)){
        string enValue = CocosUtils::hexToChar(hexValue);
        return AESCTREncryptor::getInstance()->Decrypt(enValue);
    }
    return defaultValue;
}

void UserDefaultsDataManager::setIntegerForKey(const char *pKey, int value)
{
    setStringForKey(pKey, CocosUtils::itoa(value));
}

int UserDefaultsDataManager::getIntegerForKey(const char *pKey, int defaultValue)
{
    string value = getStringForKey(pKey, CocosUtils::itoa(defaultValue));
    return atoi(value.c_str());
}

void UserDefaultsDataManager::setBoolForKey(const char* pKey, bool value)
{
    UserDefault::getInstance()->setBoolForKey(pKey, value);
}

bool UserDefaultsDataManager::getBoolForKey(const char* pKey, bool defaultValue)
{
    return UserDefault::getInstance()->getBoolForKey(pKey, defaultValue);
}

void UserDefaultsDataManager::setSoundOn(bool on)
{
    setBoolForKey(SOUND_ON_TAG, on);
}

bool UserDefaultsDataManager::getSoundOn()
{
    return getBoolForKey(SOUND_ON_TAG, true);
}

void UserDefaultsDataManager::setMusicOn(bool on)
{
    setBoolForKey(MUSIC_ON_TAG, on);
}

bool UserDefaultsDataManager::getMusicOn()
{
    return getBoolForKey(MUSIC_ON_TAG, true);
}
