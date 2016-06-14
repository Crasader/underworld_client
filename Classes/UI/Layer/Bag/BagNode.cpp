//
//  BagNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BagNode.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "ObjectLocalData.h"
#include "ObjectData.h"

using namespace std;
using namespace ui;

BagNode* BagNode::create(const ObjectData* data, ssize_t idx, int column)
{
    BagNode *ret = new (nothrow) BagNode();
    if (ret && ret->init(data, idx, column))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BagNode::BagNode()
:_observer(nullptr)
,_data(nullptr)
,_idx(CC_INVALID_INDEX)
,_column(INVALID_VALUE)
,_icon(nullptr)
,_nameLabel(nullptr)
,_descriptionLabel(nullptr)
{
    
}

BagNode::~BagNode()
{
    removeAllChildren();
}

void BagNode::registerObserver(BagNodeObserver *observer)
{
    _observer = observer;
}

ssize_t BagNode::getIdx() const
{
    return _idx;
}

int BagNode::getColumn() const
{
    return _column;
}

void BagNode::update(const ObjectData* data, ssize_t idx, int column)
{
    if (data) {
        _data = data;
        _idx = idx;
        
        const ObjectLocalData* localData = _data->getLocalData();
        if (_nameLabel) {
            _nameLabel->setString(localData->getName());
        }
        
        if (_descriptionLabel) {
            _descriptionLabel->setString(localData->getDescription());
        }
    }
}

bool BagNode::init(const ObjectData* data, ssize_t idx, int column)
{
    if (Node::init())
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        static const string CsbFile("zhuchangjing.csb");
        auto mainNode = CocosUtils::playCSBAnimation(CsbFile, false, 0, nullptr);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        
        Node* root = mainNode->getChildByTag(7);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 100:
                        {
                            Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                            child->addChild(label);
                            
                            _nameLabel = label;
                        }
                            break;
                        case 101:
                        {
                            Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                            label->setTextColor(Color4B::ORANGE);
                            child->addChild(label);
                            
                            _descriptionLabel = label;
                        }
                            break;
                        case 102:
                        {
                            Sprite* sprite = Sprite::create();
                            child->addChild(sprite);
                            
                            _icon = sprite;
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        update(data, idx, column);
        
        return true;
    }
    
    return false;
}