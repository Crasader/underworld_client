//
//  LocalHelper.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef LocalHelper_h
#define LocalHelper_h

#include "Global.h"

typedef enum
{
    kLocalType_Chinese = 0,
    kLocalType_English,
}LocalType;

namespace LocalHelper
{    
    bool isFileExists(const std::string& file);
    std::string loadFileContentString(const std::string& file);
    
    void init();
    std::string getLocalizedFilePath(const std::string& filePath);
    LocalType getLocal();
    std::string getString(const std::string& key);
    
    void setLocal(LocalType type);
};

#endif /* LocalHelper_h */
