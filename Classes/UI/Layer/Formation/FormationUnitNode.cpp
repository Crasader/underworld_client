//
//  FormationUnitNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FormationUnitNode.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"

#if !USING_PVR
#include "DataManager.h"
#include "URConfigData.h"
#endif

using namespace std;

FormationUnitNode* FormationUnitNode::create(const string& name, const string& renderKey, const Size& size)
{
    FormationUnitNode *ret = new (nothrow) FormationUnitNode();
    if (ret && ret->init(name, renderKey, size))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

FormationUnitNode::FormationUnitNode() {}

FormationUnitNode::~FormationUnitNode()
{
    removeAllChildren();
}

bool FormationUnitNode::init(const string& name, const string& renderKey, const Size& size)
{
    if (Widget::init())
    {
        _name = name;
        
        setContentSize(size);
        
# if USING_PVR
        auto sprite = CocosUtils::playAnimation("soldier-Archer/stand/body/3", true, 0, DEFAULT_FRAME_DELAY, nullptr);
        sprite->setPosition(Point(size.width / 2, size.height / 2));
        addChild(sprite);
#else
        auto data = DataManager::getInstance()->getURConfigData(renderKey);
        if (data) {
            auto file = data->getPrefix() + StringUtils::format("-standby-%d.csb", 3);
            auto node = CocosUtils::playCSBAnimation(file, true, 0, nullptr);
            /*
            Sprite* sprite = dynamic_cast<Sprite*>(node->getChildren().front());
            if (sprite) {
                const auto& spriteSize = sprite->getContentSize();
                if (spriteSize.width > size.width || spriteSize.height > size.height) {
                    setScale(MIN(size.width / spriteSize.width, size.height / spriteSize.height));
                }
            }
             */
            if (!data->isFaceRight()) {
                node->setScaleX(-1 * node->getScaleX());
            }
            node->setPosition(Point(size.width / 2, size.height / 2));
            addChild(node);
        }
#endif
        
        return true;
    }
    
    return false;
}

const string& FormationUnitNode::getUnitName() const
{
    return _name;
}
