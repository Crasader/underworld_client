//
//  LocalHelper.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef LocalHelper_h
#define LocalHelper_h

#include "cocos2d.h"

USING_NS_CC;

enum class LocalType {
    ENGLISH = 0,
    FRENCH,
    GERMAN,
    SPANISH,
    ITALIAN,
    DUTCH,
    NORWEGIAN,
    PORTUGUESE,
    TURKISH,
    JAPANESE,
    KOREAN,
    RUSSIAN,
    ARABIC,
    CHINESE,
    TCHINESE
};

namespace LocalHelper
{
    std::string loadFileContentString(const std::string& file);
    
    void init();
    std::string getLocalizedFilePath(const std::string& filePath);
    std::string getLocalizedConfigFilePath(const std::string& fileName);
    std::string getString(const std::string& key);
    
    LocalType getLocalType();
    void setLocalType(LocalType type);
    const std::string& getLanguageName(LocalType type);
    const std::string& getCurrentLanguageName();
};

#endif /* LocalHelper_h */
