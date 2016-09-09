//
//  JSonUtils.h
//  Underworld_Client
//
//  Created by Andy on 16/9/5.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef JSonUtils_h
#define JSonUtils_h

#include "json/document.h"
#include "cocostudio/CocoStudio.h"
#include <string>

using namespace cocostudio;

namespace JSonUtils
{
    inline bool isExist(const rapidjson::Value& json, const char* key);
    
    // ========================= sub modules =========================
    
    // ------------------------- bool -------------------------
    template<class T> typename std::enable_if<std::is_same<T, bool>::value, T>::type
    __json_to_value(const rapidjson::Value& json, const char* key)
    {
        return DICTOOL->getBooleanValue_json(json, key);
    }
    
    // ------------------------- string -------------------------
    template<class T> typename std::enable_if<std::is_same<T, std::string>::value, T>::type
    __json_to_value(const rapidjson::Value& json, const char* key)
    {
        return DICTOOL->getStringValue_json(json, key, "");
    }
    
    // ------------------------- integers & enum-------------------------
    template<class T> typename std::enable_if<sizeof(T) == 8, T>::type
    __json_to_value(const rapidjson::Value& json, const char* key)
    {
        if (isExist(json, key)) {
            return static_cast<T>(json[key].GetInt64());
        }
        
        return 0;
    }
    
    template<class T> typename std::enable_if<(std::is_integral<T>::value && !std::is_same<T, bool>::value && sizeof(T) != 8) || std::is_enum<T>::value, T>::type
    __json_to_value(const rapidjson::Value& json, const char* key)
    {
        return static_cast<T>(DICTOOL->getIntValue_json(json, key));
    }
    
    // ------------------------- floats -------------------------
    template<class T> typename std::enable_if<std::is_floating_point<T>::value, T>::type
    __json_to_value(const rapidjson::Value& json, const char* key)
    {
        return static_cast<T>(DICTOOL->getFloatValue_json(json, key));
    }
    
    // ========================= sub modules =========================
    
    // generic
    inline bool isExist(const rapidjson::Value& json, const char* key)
    {
        return (!json.IsNull() && DICTOOL->checkObjectExist_json(json, key) && !DICTOOL->getSubDictionary_json(json, key).IsNull());
    }
    
    template<class T> T parse(const rapidjson::Value& json, const char* key)
    {
        return __json_to_value<T>(json, key);
    }
    
    template<class T> T parse(const rapidjson::Value& json, const char* key, const T& def)
    {
        if (isExist(json, key)) {
            return __json_to_value<T>(json, key);
        }
        
        return def;
    }
    
    template<class T> void parse(T& value, const rapidjson::Value& json, const char* key)
    {
        value = __json_to_value<T>(json, key);
    }
    
    template<class T> void parse(T& value, const rapidjson::Value& json, const char* key, const T& def)
    {
        if (isExist(json, key)) {
            value = __json_to_value<T>(json, key);
        }
        
        value = def;
    }
}

#endif /* JSonUtils_h */
