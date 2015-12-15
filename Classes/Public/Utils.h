//
//  Utils.h
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef Utils_h
#define Utils_h

#include <string>
#include <vector>
#include <map>
#include "Coordinate.h"
#include "json/document.h"

namespace Utils
{
    std::string format(const char *format, ...);
    void split(std::vector<std::string> &v, const std::string& src, const std::string& delimit, const std::string& null_subst = "");
    void split2int(std::vector<int> &v, const std::string& src, const std::string& delimit, int d = 0);
    std::string convertCoordinateToString(const UnderWorld::Core::Coordinate& coordinate);
    UnderWorld::Core::Coordinate convertStringToCoordinate(const std::string& text);
    
    /** template operations */
    template<typename _Type> void clearVector(std::vector<_Type> &vec);
    template<typename _Key, typename _Value> void clearMap(std::map<_Key, _Value> &m);
    
    /** for rapidjson */
    int   getIntValue_json(const rapidjson::Value& root, const char* key, int def = 0);
    float getFloatValue_json(const rapidjson::Value& root,const char* key, float def = 0.0f);
    bool  getBooleanValue_json(const rapidjson::Value& root,const char* key, bool def = false);
    const char* getStringValue_json(const rapidjson::Value& root,const char* key, const char *def = NULL);
    bool checkObjectExist_json(const rapidjson::Value &root);
    bool checkObjectExist_json(const rapidjson::Value &root, const char* key);
    bool checkObjectExist_json(const rapidjson::Value &root, int index);
    const rapidjson::Value& getSubDictionary_json(const rapidjson::Value &root, const char* key);
    const rapidjson::Value& getSubDictionary_json(const rapidjson::Value &root, const char* key, int idx);
    const rapidjson::Value& getSubDictionary_json(const rapidjson::Value &root, int idx);
    int getArrayCount_json(const rapidjson::Value& root, const char* key, int def = 0);
};

#pragma mark - template operations
template<typename _Type> void Utils::clearVector(std::vector<_Type> &vec)
{
    for(typename std::vector<_Type>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
    {
        _Type p = *iter;
        if (p) { delete p; }
    }
    vec.clear();
}

template<typename _Key, typename _Value> void Utils::clearMap(std::map<_Key, _Value> &m)
{
    for(typename std::map<_Key, _Value>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        _Value p = iter->second;
        if (p) { delete p; }
    }
    m.clear();
}

#endif /* Utils_h */
