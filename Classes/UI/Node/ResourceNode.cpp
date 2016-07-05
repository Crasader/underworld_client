//
//  ResourceNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "ResourceNode.h"
#include "CocosUtils.h"

using namespace std;

ResourceNode::ResourceNode()
:_type(ResourceType::Gold)
,_count(INVALID_VALUE)
,_icon(nullptr)
,_countLabel(nullptr)
{
    
}

ResourceNode::~ResourceNode()
{
    removeFromParent();
}

ResourceNode* ResourceNode::create(ResourceType type, int count)
{
    ResourceNode *p = new (nothrow) ResourceNode();
    if(p && p->init(type, count))
    {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

bool ResourceNode::init(ResourceType type, int count)
{
    if(Node::init())
    {
        static const string csbFile("UI_Res.csb");
        Node *mainNode = CocosUtils::playAnimation(csbFile, 0, true);
        addChild(mainNode);
        
        Node* root = mainNode->getChildByTag(20);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const Point& position = child->getPosition();
                Node *parent = child->getParent();
                switch (child->getTag()) {
                    case 21:
                    {
                        _icon = Sprite::create(StringUtils::format("GameImages/resources/icon_%dB.png", type));
                        _icon->setPosition(position);
                        parent->addChild(_icon);
                    }
                        break;
                    case 22:
                    {
                        _countLabel = CocosUtils::createLabel(StringUtils::format("%d", count), DEFAULT_FONT_SIZE);
                        _countLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
                        _countLabel->setPosition(position);
                        parent->addChild(_countLabel);
                    }
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        setType(type);
        setContentSize(root->getContentSize());
        
        return true;
    }
    
    return false;
}

ResourceType ResourceNode::getType() const
{
    return _type;
}

int ResourceNode::getCount() const
{
    return _count;
}

void ResourceNode::setType(ResourceType type)
{
    _type = type;
    
    if (_icon)
    {
        string fileName = StringUtils::format("GameImages/resources/icon_%dB.png", type);
        assert(FileUtils::getInstance()->isFileExist(fileName));
        _icon->setTexture(fileName);
    }
}

void ResourceNode::setCount(int count, bool animated)
{
    _count = count;
    
    if (_countLabel)
    {
        if (animated) {
            static const float duration(0.5f);
            CocosUtils::jumpNumber(_countLabel, count, duration);
        } else {
            _countLabel->setString(StringUtils::format("%d", count));
        }
    }
}
