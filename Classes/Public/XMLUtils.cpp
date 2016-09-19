//
//  XMLUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "XMLUtils.h"
#include "Utils.h"
#include <vector>

const char* XMLUtils::getData(tinyxml2::XMLElement* xml, const char* key) {
    if (xml && key && strlen(key) > 0) {
        return xml->Attribute(key);
    }
    
    return nullptr;
}

void XMLUtils::parseString(tinyxml2::XMLElement* xml, const char* key, const std::string& delimit, const std::function<void(int, const std::string&)>& callback)
{
    auto data = parse<std::string>(xml, key);
    std::vector<std::string> result;
    Utils::split(result, data, delimit, "");
    for (int i = 0; i < result.size(); ++i) {
        if (callback) {
            callback(i, result.at(i));
        }
    }
}
