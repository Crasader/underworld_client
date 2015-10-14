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
#include "Coordinate.h"
#include "json/document.h"

using namespace std;

namespace Utils
{
    string format(const char *format, ...);
    void split(vector<string> &v, const string& src, const string& delimit, const string& null_subst = "");
    void split2int(vector<int> &v, const string& src, const string& delimit, int d = 0);
    std::string convertCoordinateToString(const Coordinate& coordinate);
    Coordinate convertStringToCoordinate(const string& text);
    
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

#endif /* Utils_h */
