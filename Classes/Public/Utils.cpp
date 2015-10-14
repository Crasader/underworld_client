//
//  Utils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "Utils.h"
#include "Global.h"

string Utils::format(const char *format, ...)
{
    static const unsigned int MAX_TEXT_LENGTH = 1024 * 100;
    string ret;
    va_list args;
    va_start(args, format);
    char* buf = (char*)calloc(MAX_TEXT_LENGTH, sizeof(char));
    if (buf != nullptr)
    {
        vsnprintf(buf, MAX_TEXT_LENGTH, format, args);
        ret = buf;
        free(buf);
    }
    va_end(args);
    return ret;
}

void Utils::split(vector<string> &v, const string& src, const string& delimit, const string& null_subst)
{
    if (src.empty() || delimit.empty())
    {
        return;
    }
    
    string strTmp = src;
    string::size_type cutAt = 0;
    v.clear();
    while((cutAt = strTmp.find_first_of(delimit)) != strTmp.npos )
    {
        v.push_back((cutAt > 0) ? strTmp.substr(0, cutAt) : null_subst);
        strTmp = strTmp.substr(cutAt + 1);
    }
    
    v.push_back(strTmp.empty() ? null_subst : strTmp);
}

void Utils::split2int(vector<int> &v, const string& src, const string& delimit, int d)
{
    vector<string> stringVector;
    split(stringVector, src, delimit, "");
    
    v.clear();
    for (int i = 0; i < stringVector.size(); ++i)
    {
        const string& s = stringVector.at(i);
        v.push_back(s.empty() ? d : atoi(s.c_str()));
    }
}

string Utils::convertCoordinateToString(const Coordinate& coordinate)
{
    return format("%d_%d", coordinate.x, coordinate.y);
}

Coordinate Utils::convertStringToCoordinate(const string& text)
{
    vector<string> result;
    split(result, text, "_", "");
    assert(result.size() == 2);
    return Coordinate(atoi(result.at(0).c_str()), atoi(result.at(1).c_str()));
}

int Utils::getIntValue_json(const rapidjson::Value& root, const char* key, int def)
{
    int nRet = def;
    do {
        M_BREAK_IF(root.IsNull());
        M_BREAK_IF(root[key].IsNull());
        nRet = root[key].GetInt();
    } while (0);
    
    return nRet;
}


float Utils::getFloatValue_json(const rapidjson::Value& root,const char* key, float def)
{
    float fRet = def;
    do {
        M_BREAK_IF(root.IsNull());
        M_BREAK_IF(root[key].IsNull());
        fRet = (float)root[key].GetDouble();
    } while (0);
    
    return fRet;
}

bool Utils::getBooleanValue_json(const rapidjson::Value& root,const char* key, bool def)
{
    bool bRet = def;
    do {
        M_BREAK_IF(root.IsNull());
        M_BREAK_IF(root[key].IsNull());
        bRet = root[key].GetBool();
    } while (0);
    
    return bRet;
}

const char* Utils::getStringValue_json(const rapidjson::Value& root,const char* key, const char *def)
{
    const char* sRet = def;
    do {
        M_BREAK_IF(root.IsNull());
        M_BREAK_IF(root[key].IsNull());
        sRet = root[key].GetString();
    } while (0);
    
    return sRet;
}


bool Utils::checkObjectExist_json(const rapidjson::Value &root)
{
    bool bRet = false;
    do {
        M_BREAK_IF(root.IsNull());
        bRet = true;
    } while (0);
    
    return bRet;
}

bool Utils::checkObjectExist_json(const rapidjson::Value &root,const char* key)
{
    bool bRet = false;
    do {
        M_BREAK_IF(root.IsNull());
        bRet = root.HasMember(key);
    } while (0);
    
    return bRet;
}

bool Utils::checkObjectExist_json(const rapidjson::Value &root, int index)
{
    bool bRet = false;
    do {
        M_BREAK_IF(root.IsNull());
        M_BREAK_IF(!root.IsArray());
        M_BREAK_IF(index < 0 || root.Size() <= (unsigned int )index);
        bRet = true;
    } while (0);
    
    return bRet;
}

const rapidjson::Value& Utils::getSubDictionary_json(const rapidjson::Value &root, const char* key)
{
    return root[key];
}

const rapidjson::Value& Utils::getSubDictionary_json(const rapidjson::Value &root, const char* key, int idx)
{
    return root[key][idx];
}

const rapidjson::Value& Utils::getSubDictionary_json(const rapidjson::Value &root, int idx)
{
    return root[idx];
}

int Utils::getArrayCount_json(const rapidjson::Value& root, const char* key, int def)
{
    int nRet = def;
    do {
        M_BREAK_IF(root.IsNull());
        M_BREAK_IF(root[key].IsNull());
        nRet = (int)(root[key].Size());
    } while (0);
    
    return nRet;
}