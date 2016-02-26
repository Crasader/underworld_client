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
static UserDefaultsDataManager *s_pSharedInstance = nullptr;

UserDefaultsDataManager* UserDefaultsDataManager::getInstance()
{
    if (!s_pSharedInstance)
    {
        s_pSharedInstance = new (nothrow) UserDefaultsDataManager();
        CCASSERT(s_pSharedInstance, "FATAL: Not enough memory");
    }
    
    return s_pSharedInstance;
}

void UserDefaultsDataManager::purge()
{
    if (s_pSharedInstance)
    {
        delete s_pSharedInstance;
        s_pSharedInstance = nullptr;
    }
}

UserDefaultsDataManager::UserDefaultsDataManager()
{
}

UserDefaultsDataManager::~UserDefaultsDataManager()
{
    
}

void UserDefaultsDataManager::flush() const
{
    UserDefault::getInstance()->flush();
}

void UserDefaultsDataManager::setStringForKey(const char* pKey, const string & value) const
{
    UserDefault::getInstance()->setStringForKey(pKey, MD5Verifier::getInstance()->encrypt(value));
}

string UserDefaultsDataManager::getStringForKey(const char* pKey, const string & defaultValue) const
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

void UserDefaultsDataManager::setStringForKeyWithAES(const char* pKey, const string & value) const
{
    string enValue = AESCTREncryptor::getInstance()->Encrypt(value);
    UserDefault::getInstance()->setStringForKey(pKey, MD5Verifier::getInstance()->encrypt(CocosUtils::charToHex(enValue)));
}

string UserDefaultsDataManager::getStringForKeyWithAES(const char* pKey, const string & defaultValue) const
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

void UserDefaultsDataManager::setIntegerForKey(const char *pKey, int value) const
{
    setStringForKey(pKey, CocosUtils::itoa(value));
}

int UserDefaultsDataManager::getIntegerForKey(const char *pKey, int defaultValue) const
{
    string value = getStringForKey(pKey, CocosUtils::itoa(defaultValue));
    return atoi(value.c_str());
}

void UserDefaultsDataManager::setBoolForKey(const char* pKey, bool value) const
{
    UserDefault::getInstance()->setBoolForKey(pKey, value);
}

bool UserDefaultsDataManager::getBoolForKey(const char* pKey, bool defaultValue) const
{
    return UserDefault::getInstance()->getBoolForKey(pKey, defaultValue);
}

static const char* FinishSkillTutorialKey = "First_Entry_Key";
void UserDefaultsDataManager::setFinishTutorialTag(int index) const
{
    string key = StringUtils::format("%s_%d", FinishSkillTutorialKey, index);
    setBoolForKey(key.c_str(), true);
    flush();
}

bool UserDefaultsDataManager::hasFinishedTutorial(int index) const
{
    string key = StringUtils::format("%s_%d", FinishSkillTutorialKey, index);
    return getBoolForKey(key.c_str(), false);
}

void UserDefaultsDataManager::setSoundOn(bool on) const
{
    setBoolForKey(SOUND_ON_TAG, on);
}

bool UserDefaultsDataManager::getSoundOn() const
{
    
    return getBoolForKey(SOUND_ON_TAG, true);
}

void UserDefaultsDataManager::setMusicOn(bool on) const
{
    setBoolForKey(MUSIC_ON_TAG, on);
}

bool UserDefaultsDataManager::getMusicOn() const
{
    return getBoolForKey(MUSIC_ON_TAG, true);
}

#pragma mark - card
static const char* pickedCardsKey("pickedCardsKey");
void UserDefaultsDataManager::getSelectedCards(set<string>& output) const
{
    output.clear();
    
    const string data = getStringForKey(pickedCardsKey, "");
    if (data.length() > 0) {
        vector<string> results;
        Utils::split(results, data, ",", "");
        for (int i = 0; i < results.size(); ++i) {
            output.insert(results.at(i));
        }
    }
}

void UserDefaultsDataManager::setSelectedCards(const set<string>& output) const
{
    string data;
    for (auto iter = begin(output); iter != end(output); ++iter) {
        data += *iter + ",";
    }
    data.pop_back();
    
    setStringForKey(pickedCardsKey, data);
}
