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

class XMLUtils {
private:
    static const char* getData(tinyxml2::XMLElement* xml, const char* key);
    
    template<class T>
    static T __parse_iml(const char* data, std::true_type)
    {
        if (data) {
            return static_cast<T>(atof(data));
        }
        
        return 0;
    }
    
    template<class T>
    static T __parse_iml(const char* data, std::false_type)
    {
        if (data) {
            return std::string(data);
        }
        
        return "";
    }
    
public:
    template<class T>
    static T parse(tinyxml2::XMLElement* xml, const char* key)
    {
        return __parse_iml<T>(getData(xml, key), std::is_arithmetic<T>());
    }
    
    template<class T>
    static void parse(tinyxml2::XMLElement* xml, const char* key, T& output)
    {
        auto data(getData(xml, key));
        if (data) {
            output = __parse_iml<T>(data, std::is_arithmetic<T>());
        }
    }
    
    static void parseString(tinyxml2::XMLElement* xml, const char* key, const std::string& delimit, const std::function<void(int, const std::string&)>& callback);
};

#endif /* XMLUtils_h */
