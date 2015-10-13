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

using namespace std;
USING_NS_CC;

static const string LOCAL_KEY_BASE = "base";
static LocalType local = kLocalType_Chinese;
static map<string, map<string, string> > stringsMap;

static string getLocalKey(LocalType type)
{
    switch (type) {
        case kLocalType_Chinese:
            return "zh";
        case kLocalType_English:
            return "en";
        default:
            return "";
    }
}

static void parseStrings(string local) {
    string file = StringUtils::format("configs/string-%s.xml", local.c_str());
    if (LocalHelper::isFileExists(file)) {
        if (stringsMap.find(local) == stringsMap.end()) {
            stringsMap.insert(make_pair(local, map<string, string>()));
        }
        map<string, string>& localMap = stringsMap.at(local);
        tinyxml2::XMLDocument* xmlDoc = new tinyxml2::XMLDocument();
        if (xmlDoc) {
            string content = LocalHelper::loadFileContentString(file);
            xmlDoc->Parse(content.c_str());
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement()) {
                string key = item->Attribute("key");
                string value = item->Attribute("value");
                assert(localMap.find(key) == localMap.end());
                localMap.insert(make_pair(key, value));
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

bool LocalHelper::isFileExists(const std::string& file) {
    return FileUtils::getInstance()->isFileExist(file);
}

string LocalHelper::loadFileContentString(const std::string& file) {
    Data data = FileUtils::getInstance()->getDataFromFile(file);
    if(!data.isNull()){
        std::string ret;
        ret.assign((char*)data.getBytes(), data.getSize());
        return AESCTREncryptor::getInstance()->Encrypt(ret);
    }
    return "";
}

void LocalHelper::init() {
    LanguageType lt = Application::getInstance()->getCurrentLanguage();
    
    if (lt == LanguageType::CHINESE) {
        local = kLocalType_Chinese;
    } else {
        local = kLocalType_English;
    }
    
    // test
    local = kLocalType_Chinese;
    
    parseStrings(LOCAL_KEY_BASE);
    parseStrings(getLocalKey(local));
}

string LocalHelper::getLocalizedFilePath(const std::string &filePath) {
    const string key = getLocalKey(local);
    string localPath = key + "/" + filePath;
    if (isFileExists(localPath)) {
        return localPath;
    } else {
        return filePath;
    }
}

LocalType LocalHelper::getLocal() {
    return local;
}

string LocalHelper::getString(const std::string &key) {
    const string localKey = getLocalKey(local);
    if (stringsMap.find(localKey) != stringsMap.end()
        && stringsMap.find(localKey)->second.find(key) != stringsMap.find(localKey)->second.end()) {
        return stringsMap.find(localKey)->second.find(key)->second;
    } else if (stringsMap.find(LOCAL_KEY_BASE) != stringsMap.end()
               && stringsMap.find(LOCAL_KEY_BASE)->second.find(key) != stringsMap.find(LOCAL_KEY_BASE)->second.end()) {
        return stringsMap.find(LOCAL_KEY_BASE)->second.find(key)->second;
    } else {
        // lack value
        if (key.length() > 0) {
            return key;
        }
        
        return "This label lacks key";
    }
}

void LocalHelper::setLocal(LocalType type) {
    local = type;
}