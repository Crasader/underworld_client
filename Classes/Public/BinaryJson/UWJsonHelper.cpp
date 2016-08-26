//
//  UWJsonHelper.cpp
//  Underworld_Client
//
//  Created by Mac on 16/8/26.
//
//

#include "cocos2d.h"
#include "UWJsonHelper.h"

const rapidjson::Value& UWJsonHelper::getSubDictionary_json(const rapidjson::Value &root, const char* key)
{
    return root[key];
}

const rapidjson::Value& UWJsonHelper::getSubDictionary_json(const rapidjson::Value &root, const char* key, int idx)
{
    return root[key][idx];
}

const rapidjson::Value& UWJsonHelper::getSubDictionary_json(const rapidjson::Value &root, int idx)
{
    return root[idx];
}

int UWJsonHelper::getIntValue_json(const rapidjson::Value& root, const char* key, int def)
{
    int nRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(!root.HasMember(key));
        CC_BREAK_IF(root[key].IsNull());
        CC_BREAK_IF(!root[key].IsInt());
        nRet = root[key].GetInt();
    } while (0);
    
    return nRet;
}


float UWJsonHelper::getFloatValue_json(const rapidjson::Value& root,const char* key, float def)
{
    float fRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(!root.HasMember(key));
        CC_BREAK_IF(root[key].IsNull());
        CC_BREAK_IF(!root[key].IsDouble());
        fRet = (float)root[key].GetDouble();
    } while (0);
    
    return fRet;
}

bool UWJsonHelper::getBooleanValue_json(const rapidjson::Value& root,const char* key, bool def)
{
    bool bRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(!root.HasMember(key));
        CC_BREAK_IF(root[key].IsNull());
        CC_BREAK_IF(!root[key].IsBool());
        bRet = root[key].GetBool();
    } while (0);
    
    return bRet;
}

const char* UWJsonHelper::getStringValue_json(const rapidjson::Value& root,const char* key, const char *def)
{
    const char* sRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(!root.HasMember(key));
        CC_BREAK_IF(root[key].IsNull());
        CC_BREAK_IF(!root[key].IsString());
        sRet = root[key].GetString();
    } while (0);
    
    return sRet;
}



int UWJsonHelper::getArrayCount_json(const rapidjson::Value& root, const char* key, int def)
{
    int nRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(!root.HasMember(key));
        CC_BREAK_IF(root[key].IsNull());
        CC_BREAK_IF(!root[key].IsArray());
        nRet = (int)(root[key].Size());
    } while (0);
    
    return nRet;
}


int UWJsonHelper::getIntValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, int def)
{
    int nRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(root[arrayKey].IsNull());
        CC_BREAK_IF(root[arrayKey][idx].IsNull());
        CC_BREAK_IF(!root[arrayKey][idx].IsInt());
        nRet = root[arrayKey][idx].GetInt();
    } while (0);
    
    return nRet;
}


float UWJsonHelper::getFloatValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, float def)
{
    float fRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(root[arrayKey].IsNull());
        CC_BREAK_IF(root[arrayKey][idx].IsNull());
        CC_BREAK_IF(!root[arrayKey][idx].IsDouble());
        fRet = (float)root[arrayKey][idx].GetDouble();
    } while (0);
    
    return fRet;
}

bool UWJsonHelper::getBoolValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, bool def)
{
    bool bRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(root[arrayKey].IsNull());
        CC_BREAK_IF(root[arrayKey][idx].IsNull());
        CC_BREAK_IF(!root[arrayKey][idx].IsBool());
        bRet = root[arrayKey][idx].GetBool();
    } while (0);
    
    return bRet;
}

const char* UWJsonHelper::getStringValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, const char *def)
{
    const char *sRet = def;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(root[arrayKey].IsNull());
        CC_BREAK_IF(root[arrayKey][idx].IsNull());
        CC_BREAK_IF(!root[arrayKey][idx].IsString());
        sRet = root[arrayKey][idx].GetString();
    } while (0);
    
    return sRet;
}

const rapidjson::Value& UWJsonHelper::getDictionaryFromArray_json(const rapidjson::Value &root, const char* key,int idx)
{
    return root[key][idx];
}

bool UWJsonHelper::checkObjectExist_json(const rapidjson::Value &root)
{
    bool bRet = false;
    do {
        CC_BREAK_IF(root.IsNull());
        bRet = true;
    } while (0);
    
    return bRet;
}

bool UWJsonHelper::checkObjectExist_json(const rapidjson::Value &root,const char* key)
{
    bool bRet = false;
    do {
        CC_BREAK_IF(root.IsNull());
        bRet = root.HasMember(key);
    } while (0);
    
    return bRet;
}

bool UWJsonHelper::checkObjectExist_json(const rapidjson::Value &root, int index)
{
    bool bRet = false;
    do {
        CC_BREAK_IF(root.IsNull());
        CC_BREAK_IF(!root.IsArray());
        CC_BREAK_IF(index < 0 || root.Size() <= (unsigned int )index);
        bRet = true;
    } while (0);
    
    return bRet;
}