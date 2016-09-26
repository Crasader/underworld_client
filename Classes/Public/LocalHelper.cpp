//
//  LocalHelper.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "LocalHelper.h"
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#include "AESCTREncryptor.h"
#include "UserDefaultHelper.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "ApiBridge.h"
#endif

USING_NS_CC;
using namespace std;

static const string languageKey("languageKey");
static LocalType _localType = static_cast<LocalType>(-1);

typedef unordered_map<string, string> StringsType;
StringsType _baseStrings;
StringsType _localizedStrings;

// http://www.lingoes.net/en/translator/langcode.htm
static string getLocalKey(LocalType type)
{
    switch (type) {
        case LocalType::ENGLISH:
            return "en";
        case LocalType::FRENCH:
            return "fr";
        case LocalType::GERMAN:
            return "de";
        case LocalType::SPANISH:
            return "es";
        case LocalType::ITALIAN:
            return "it";
        case LocalType::DUTCH:
            return "nl";
        case LocalType::NORWEGIAN:
            return "nb";
        case LocalType::PORTUGUESE:
            return "pt";
        case LocalType::TURKISH:
            return "tr";
        case LocalType::JAPANESE:
            return "ja";
        case LocalType::KOREAN:
            return "ko";
        case LocalType::RUSSIAN:
            return "ru";
        case LocalType::ARABIC:
            return "ar";
        case LocalType::CHINESE:
            return "zh";
        case LocalType::TCHINESE:
            return "zht";
    }
}

static void parseStrings(const string& file, StringsType& container)
{
    if (FileUtils::getInstance()->isFileExist(file)) {
        auto xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc) {
            auto content = LocalHelper::loadFileContentString(file);
            xmlDoc->Parse(content.c_str());
            for (auto item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement()) {
                auto key = item->Attribute("key");
                auto value = item->Attribute("value");
                assert(container.find(key) == container.end());
                container.insert(make_pair(key, value));
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

static void parseBaseStrings()
{
    parseStrings("configs/string-base.xml", _baseStrings);
}

static void parseLocalizedStrings(LocalType type)
{
    auto file = StringUtils::format("configs/string-%s.xml", getLocalKey(type).c_str());
    parseStrings(file, _localizedStrings);
}

static bool isSimplifiedChinese()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    if (string::npos != iOSApi::getLanguage().find("zh-Hans")) {
        return true;
    }
    
    return false;
#else
    return true;
#endif
}

static LocalType getLocalTypeFromLanguage()
{
    auto language(Application::getInstance()->getCurrentLanguage());
    
    switch (language) {
        case LanguageType::CHINESE: {
            if (isSimplifiedChinese()) {
                return LocalType::CHINESE;
            } else {
                return LocalType::TCHINESE;
            }
        }
        case LanguageType::FRENCH:
            return LocalType::FRENCH;
        case LanguageType::GERMAN:
            return LocalType::GERMAN;
        case LanguageType::SPANISH:
            return LocalType::SPANISH;
        case LanguageType::ITALIAN:
            return LocalType::ITALIAN;
        case LanguageType::DUTCH:
            return LocalType::DUTCH;
        case LanguageType::NORWEGIAN:
            return LocalType::NORWEGIAN;
        case LanguageType::PORTUGUESE:
            return LocalType::PORTUGUESE;
        case LanguageType::TURKISH:
            return LocalType::TURKISH;
        case LanguageType::JAPANESE:
            return LocalType::JAPANESE;
        case LanguageType::KOREAN:
            return LocalType::KOREAN;
        case LanguageType::RUSSIAN:
            return LocalType::RUSSIAN;
        case LanguageType::ARABIC:
            return LocalType::ARABIC;
            
        default:
            return LocalType::ENGLISH;
    }
}

#pragma mark - LocalHelper
string LocalHelper::loadFileContentString(const string& file) {
    auto data = FileUtils::getInstance()->getDataFromFile(file);
    if(!data.isNull()){
        string ret;
        ret.assign((char*)data.getBytes(), data.getSize());
        return AESCTREncryptor::getInstance()->Encrypt(ret);
    }
    
    return "";
}

void LocalHelper::init() {
    parseBaseStrings();
    
    LocalType type;
    int value = UserDefaultHelper::getIntegerForKey(languageKey.c_str(), -1);
    if (value >= 0) {
        type = static_cast<LocalType>(value);
    } else {
        if (/* DISABLES CODE */ (false)) {
            type = getLocalTypeFromLanguage();
        } else {
            // test
            type = LocalType::CHINESE;
        }
    }
    
    setLocalType(type);
}

string LocalHelper::getLocalizedFilePath(const string &filePath) {
    const string key = getLocalKey(_localType);
    string localPath = key + "/" + filePath;
    if (FileUtils::getInstance()->isFileExist(localPath)) {
        return localPath;
    } else {
        return filePath;
    }
}

string LocalHelper::getLocalizedConfigFilePath(const string& fileName) {
    static const string defaultPrefix = "configs/";
#if VERSION_HK
    static const string localizedFolder = "configs_tw/";
#elif VERSION_TAI
    static const string localizedFolder = "configs_tai/";
#else
    static const string localizedFolder = "";
#endif
    
    string path = localizedFolder + fileName;
    auto fileUtils = FileUtils::getInstance();
    if (localizedFolder.length() == 0 || !fileUtils->isFileExist(path)) {
        path = defaultPrefix + fileName;
        if (!fileUtils->isFileExist(path)) {
            return "";
        }
    }
    
    return path;
}

string LocalHelper::getString(const string &key) {
    auto iter(_localizedStrings.find(key));
    if (iter != end(_localizedStrings)) {
        return iter->second;
    } else {
        auto iter(_baseStrings.find(key));
        if (iter != end(_baseStrings)) {
            return iter->second;
        } else {
            // lack value
            if (key.length() > 0) {
                return key;
            }
            
            return "This label lacks key";
        }
    }
}

const vector<LocalType>& LocalHelper::getSupportedLocalTypes() {
    static const vector<LocalType> languages = {
        LocalType::ENGLISH,
        LocalType::FRENCH,
        LocalType::GERMAN,
        LocalType::SPANISH,
        LocalType::ITALIAN,
        LocalType::DUTCH,
        LocalType::NORWEGIAN,
        LocalType::PORTUGUESE,
        LocalType::TURKISH,
        LocalType::CHINESE,
        LocalType::JAPANESE,
        LocalType::KOREAN,
        LocalType::RUSSIAN,
        LocalType::TCHINESE,
        LocalType::ARABIC,
    };
    
    return languages;
}

LocalType LocalHelper::getLocalType() {
    return _localType;
}

void LocalHelper::setLocalType(LocalType type) {
    if (_localType != type) {
        _localType = type;
        UserDefaultHelper::setIntegerForKey(languageKey.c_str(), static_cast<int>(type));
        _localizedStrings.clear();
        parseLocalizedStrings(type);
    }
}

const string& LocalHelper::getLanguageName(LocalType type) {
    static map<LocalType, string> languages = {
        {LocalType::ENGLISH, "English"},
        {LocalType::FRENCH, "Français"},
        {LocalType::GERMAN, "Deutsch"},
        {LocalType::SPANISH, "Español"},
        {LocalType::ITALIAN, "Italiano"},
        {LocalType::DUTCH, "Nederlands"},
        {LocalType::NORWEGIAN, "Norsk"},
        {LocalType::PORTUGUESE, "Português"},
        {LocalType::TURKISH, "Türkçe"},
        {LocalType::CHINESE, "简体中文"},
        {LocalType::JAPANESE, "日本語"},
        {LocalType::KOREAN, "한국어"},
        {LocalType::RUSSIAN, "Pусский"},
        {LocalType::TCHINESE, "繁體中文"},
        {LocalType::ARABIC, "العربية"},
    };
    
    auto iter(languages.find(type));
    if (iter != end(languages)) {
        return iter->second;
    }
    
    static const string empty("");
    return empty;
}

const string& LocalHelper::getCurrentLanguageName()
{
    return getLanguageName(_localType);
}
