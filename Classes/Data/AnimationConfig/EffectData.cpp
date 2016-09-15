//
//  EffectData.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/9/14.
//
//

#include "EffectData.h"
#include "CoreUtils.h"

const std::string EffectData::EFFECT_DATA_PARAMS_SPLITOR(";");

void EffectData::init(const std::string& s) {
    std::vector<std::string> params;
    
    UnderWorld::Core::UnderWorldCoreUtils::split(params, s, EFFECT_DATA_PARAMS_SPLITOR);
    
    if (params.size() > 0 && !params[0].empty()) {
        const size_t found = params[0].find(PILIST_SUFFIX);
        if (found != std::string::npos) {
            _resourceFile = std::string("particle/") + params[0];
        } else {
            _resourceFile = params[0];
        }
    } else {
        _resourceFile.clear();
    }
    
    if (params.size() > 1 && !params[1].empty()) {
        _scale = atof(params[1].c_str());
    } else {
        _scale = 1.f;
    }
    
    if (params.size() > 2 && !params[2].empty()) {
        _frameDelay = 1.f / atoi(params[2].c_str());
    } else {
        _frameDelay = DEFAULT_FRAME_DELAY;
    }
}