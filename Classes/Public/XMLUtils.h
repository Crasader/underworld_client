//
//  XMLUtils.h
//  Underworld_Client
//
//  Created by Andy on 16/9/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef XMLUtils_h
#define XMLUtils_h

#include "tinyxml2/tinyxml2.h"
#include <string>
#include <functional>
#include "Utils.h"

class XMLUtils {
private:
    static const char* getData(tinyxml2::XMLElement* xml, const char* key);
    
    template<class T> static typename std::enable_if<std::is_floating_point<T>::value, T>::type
    __parse_iml(const char* data) { return static_cast<T>(data ? Utils::stof(data) : 0); }
    
    template<class T> static typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value, T>::type
    __parse_iml(const char* data) { return static_cast<T>(data ? Utils::stoi(data) : 0); }
    
    template<class T> static typename std::enable_if<std::is_same<T, std::string>::value, T>::type
    __parse_iml(const char* data) { return data ? std::string(data) : ""; }
    
public:
    template<class T>
    static T parse(tinyxml2::XMLElement* xml, const char* key)
    {
        return __parse_iml<T>(getData(xml, key));
    }
    
    template<class T>
    static void parse(tinyxml2::XMLElement* xml, const char* key, T& output)
    {
        auto data(getData(xml, key));
        if (data && strlen(data) > 0) {
            output = __parse_iml<T>(data);
        }
    }
    
    static void parseString(tinyxml2::XMLElement* xml, const char* key, const std::string& delimit, const std::function<void(int, const std::string&)>& callback);
};

#endif /* XMLUtils_h */
