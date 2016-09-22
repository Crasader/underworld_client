//
//  JSonUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/5.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "JSonUtils.h"

bool JSonUtils::isExist(const rapidjson::Value& json, const char* key)
{
    return (!json.IsNull() && DICTOOL->checkObjectExist_json(json, key) && !DICTOOL->getSubDictionary_json(json, key).IsNull());
}
