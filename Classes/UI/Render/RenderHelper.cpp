//
//  RenderHelper.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/9/14.
//
//

#include "RenderHelper.h"
#include "DataManager.h"
#include "EffectData.h"
#include "CocosUtils.h"

namespace UnderWorld{ namespace Core{

cocos2d::Node* RenderHelper::buildEffectNode(
    const EffectData& effectData,
    bool loop,
    const std::function<void ()>& callback) {
    
    cocos2d::Node* ret = nullptr;
    
    // create node
    do {
        // check file
        const std::string& rf = effectData.getResourceFile();
        if (rf.empty()) break;
        
        // create foreground node
        function<void(cocos2d::Node*)> func = nullptr;
        if (!loop) {
            func = [callback](cocos2d::Node* sender) {
                if (sender) sender->removeFromParent();
                if (callback) callback();
            };
        }
        ret = CocosUtils::playAnimation(rf, effectData.getFrameDelay(), loop, 0, -1, func);
        if (ret) ret->setScale(effectData.getScale());
    } while (0);

    return ret;
}









}}