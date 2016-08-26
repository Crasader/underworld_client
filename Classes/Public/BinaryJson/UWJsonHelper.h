//
//  UWJsonHelper.hpp
//  Underworld_Client
//
//  Created by Mac on 16/8/26.
//
//

#ifndef UWJsonHelper_h
#define UWJsonHelper_h

#include "json/document.h"

class UWJsonHelper
{
public:
    
    ~UWJsonHelper() {}
    
    static const rapidjson::Value& getSubDictionary_json(const rapidjson::Value &root, const char* key);
    static const rapidjson::Value& getSubDictionary_json(const rapidjson::Value &root, const char* key, int idx);
    static const rapidjson::Value& getSubDictionary_json(const rapidjson::Value &root, int idx);
    
    static int getIntValue_json(const rapidjson::Value& root, const char* key, int def = 0);
    static float getFloatValue_json(const rapidjson::Value& root,const char* key, float def = 0.0f);
    static bool getBooleanValue_json(const rapidjson::Value& root,const char* key, bool def = false);
    static const char* getStringValue_json(const rapidjson::Value& root,const char* key, const char *def = NULL);
    static int getArrayCount_json(const rapidjson::Value& root,const char* key, int def = 0);
    
    static int getIntValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, int def = 0);
    static float getFloatValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, float def = 0.0f);
    static bool getBoolValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, bool def = false);
    static const char* getStringValueFromArray_json(const rapidjson::Value& root,const char* arrayKey,int idx, const char *def = NULL);
    static const rapidjson::Value &getDictionaryFromArray_json(const rapidjson::Value &root, const char* key,int idx);
    static bool checkObjectExist_json(const rapidjson::Value &root);
    static bool checkObjectExist_json(const rapidjson::Value &root, const char* key);
    static bool checkObjectExist_json(const rapidjson::Value &root, int index);
    
private:
    UWJsonHelper() {}
};


#endif /* UWJsonHelper_h */
