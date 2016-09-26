//
//  UserDefaultHelper.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "UserDefaultHelper.h"
#include "Utils.h"
#include "AESCTREncryptor.h"
#include "MD5Verifier.h"
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

static const char *SOUND_ON_TAG = "SOUND_ON_TAG";
static const char *MUSIC_ON_TAG = "MUSIC_ON_TAG";

void UserDefaultHelper::flush()
{
    UserDefault::getInstance()->flush();
}

void UserDefaultHelper::setStringForKey(const char* pKey, const string & value)
{
    UserDefault::getInstance()->setStringForKey(pKey, MD5Verifier::getInstance()->encrypt(value));
}

string UserDefaultHelper::getStringForKey(const char* pKey, const string & defaultValue)
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

void UserDefaultHelper::setStringForKeyWithAES(const char* pKey, const string & value)
{
    string enValue = AESCTREncryptor::getInstance()->Encrypt(value);
    UserDefault::getInstance()->setStringForKey(pKey, MD5Verifier::getInstance()->encrypt(Utils::charToHex(enValue)));
}

string UserDefaultHelper::getStringForKeyWithAES(const char* pKey, const string & defaultValue)
{
    string value = UserDefault::getInstance()->getStringForKey(pKey, "");
    if (value.empty()) {
        return defaultValue;
    }
    string hexValue;
    if(MD5Verifier::getInstance()->verify(value, hexValue)){
        string enValue = Utils::hexToChar(hexValue);
        return AESCTREncryptor::getInstance()->Decrypt(enValue);
    }
    return defaultValue;
}

void UserDefaultHelper::setIntegerForKey(const char *pKey, int value)
{
    setStringForKey(pKey, Utils::toString(value));
}

int UserDefaultHelper::getIntegerForKey(const char *pKey, int defaultValue)
{
    return Utils::stoi(getStringForKey(pKey, Utils::toString(defaultValue)));
}

void UserDefaultHelper::setBoolForKey(const char* pKey, bool value)
{
    UserDefault::getInstance()->setBoolForKey(pKey, value);
}

bool UserDefaultHelper::getBoolForKey(const char* pKey, bool defaultValue)
{
    return UserDefault::getInstance()->getBoolForKey(pKey, defaultValue);
}

void UserDefaultHelper::setSoundOn(bool on)
{
    setBoolForKey(SOUND_ON_TAG, on);
}

bool UserDefaultHelper::getSoundOn()
{
    return getBoolForKey(SOUND_ON_TAG, true);
}

void UserDefaultHelper::setMusicOn(bool on)
{
    setBoolForKey(MUSIC_ON_TAG, on);
}

bool UserDefaultHelper::getMusicOn()
{
    return getBoolForKey(MUSIC_ON_TAG, true);
}
