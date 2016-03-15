//
//  LocalHelper.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef LocalHelper_h
#define LocalHelper_h

#include <string>

enum class LocalType {
    Chinese = 0,
    English,
};

namespace LocalHelper
{    
    bool isFileExists(const std::string& file);
    std::string loadFileContentString(const std::string& file);
    
    void init();
    std::string getLocalizedFilePath(const std::string& filePath);
    std::string getLocalizedConfigFilePath(const std::string& fileName);
    LocalType getLocal();
    std::string getString(const std::string& key);
    
    void setLocal(LocalType type);
};

#endif /* LocalHelper_h */
