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
#include "DataManager.h"
#include "URConfigData.h"

using namespace std;

FormationUnitNode* FormationUnitNode::create(int cardId, const string& renderKey, const Size& size)
{
    auto ret = new (nothrow) FormationUnitNode();
    if (ret && ret->init(cardId, renderKey, size))
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

bool FormationUnitNode::init(int cardId, const string& renderKey, const Size& size)
{
    if (Widget::init())
    {
        _cardId = cardId;
        
        setContentSize(size);
        
        string file;
        bool flip(false);
        auto data = DataManager::getInstance()->getURConfigData(renderKey);
        if (data) {
            if (data->getAnimationType() == UnitAnimationType::PVR) {
                file = data->getNormalPrefix() + "/stand/body/3";
                flip = true;
            } else {
                file = data->getNormalPrefix() + StringUtils::format("-standby-%d.csb", 3);
                if (!data->isFaceRight()) {
                    flip = true;
                }
            }
        }
        
        if (file.size() > 0) {
            auto node = CocosUtils::playAnimation(file, DEFAULT_FRAME_DELAY, true);
            if (flip) {
                node->setScaleX(-1 * node->getScaleX());
            }
            node->setPosition(Point(size.width / 2, size.height / 2));
            addChild(node);
            
            /*
            Sprite* sprite = dynamic_cast<Sprite*>(node->getChildren().front());
            if (sprite) {
                const auto& spriteSize = sprite->getContentSize();
                if (spriteSize.width > size.width || spriteSize.height > size.height) {
                    setScale(MIN(size.width / spriteSize.width, size.height / spriteSize.height));
                }
            }
             */
        }
        
        return true;
    }
    
    return false;
}

int FormationUnitNode::getCardId() const
{
    return _cardId;
}
