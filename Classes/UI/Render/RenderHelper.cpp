//
//  RenderHelper.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/9/14.
//
//

#include "RenderHelper.h"
#include "DataManager.h"
#include "SpellConfigData.h"
#include "CocosUtils.h"

namespace UnderWorld{ namespace Core{

RenderHelper::SpellEffectBundle RenderHelper::buildSpellEffect(
    const std::string& renderKey,
    bool loop, const
    std::function<void ()>& callback) {
    
    SpellEffectBundle ret;
    
    const SpellConfigData* data = DataManager::getInstance()->getSpellConfigData(renderKey);
    
    // create node
    do {
        // check data
        if (!data) break;
        
        // check file
        const std::string& fg = data->getFgResource();
        if (fg.empty()) break;
        
        // set data 2 ret
        ret._spellConfigData = data;
        
        // create foreground node
        function<void(cocos2d::Node*)> func = nullptr;
        if (!loop) {
            func = [callback](cocos2d::Node* sender) {
                if (sender) sender->removeFromParent();
                if (callback) callback();
            };
        }
        ret._foregroundNode = CocosUtils::playAnimation(fg, DEFAULT_FRAME_DELAY, loop, 0, -1, func);
        if (ret._foregroundNode) ret._foregroundNode->setScale(data->getScale());
        
        // create background node
        const std::string& bg = data->getBgResource();
        if (!bg.empty()) {
            if (!loop) {
                func = [callback](cocos2d::Node* sender) {
                    if (sender) sender->removeFromParent();
                };
            }
            ret._backgroundNode = CocosUtils::playAnimation(bg, DEFAULT_FRAME_DELAY, loop, 0, -1, func);
            if (ret._backgroundNode) ret._backgroundNode->setScale(data->getScale());
        }
        
    } while (0);

    return ret;
}









}}