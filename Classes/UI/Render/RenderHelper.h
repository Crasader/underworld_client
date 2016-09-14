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

class SpellConfigData;

namespace UnderWorld{ namespace Core{

class RenderHelper {
public:
    class SpellEffectBundle {
    public:
        const SpellConfigData* _spellConfigData;
        cocos2d::Node* _foregroundNode;
        cocos2d::Node* _backgroundNode;
        
    public:
        SpellEffectBundle() :
            _spellConfigData(nullptr),
            _foregroundNode(nullptr),
            _backgroundNode(nullptr) {}
    };
    
public:
    static SpellEffectBundle buildSpellEffect(
        const std::string& renderKey,
        bool loop,
        const std::function<void ()>& callback);

    
private:
    RenderHelper() {}
};

}}


#endif /* RenderHelper_h */
