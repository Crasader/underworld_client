//
//  RenderHelper.h
//  Underworld_Client
//
//  Created by wenchengye on 16/9/14.
//
//

#ifndef RenderHelper_h
#define RenderHelper_h

#include <functional>

#include "cocos2d.h"

class EffectData;

namespace UnderWorld{ namespace Core{

class RenderHelper {
public:
    static cocos2d::Node* buildEffectNode(
        const EffectData& effectData,
        bool loop,
        const std::function<void ()>& callback);

    
private:
    RenderHelper() {}
};

}}


#endif /* RenderHelper_h */
