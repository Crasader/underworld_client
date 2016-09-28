//
//  UserSimpleNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "UserSimpleNode.h"
#include "UserSimpleData.h"
#include "UserArenaData.h"
#include "UserGuildData.h"
#include "GuildData.h"
#include "CocosUtils.h"
#include "SoundManager.h"
#include "AvatarNode.h"
#include "TrophyNode.h"
#include "ExpNode.h"
#include "GuildSimpleNode.h"

using namespace std;
using namespace ui;

static const Size avatarSize(68, 68);
static const Vec2 edge(10.0f, 10.0f);

UserSimpleNode* UserSimpleNode::create()
{
    UserSimpleNode *p = new (nothrow) UserSimpleNode();
    if(p && p->init()) {
        p->autorelease();
        return p;
    }
    
    CC_SAFE_DELETE(p);
    return nullptr;
}

UserSimpleNode::UserSimpleNode()
:_data(nullptr)
,_avatar(nullptr)
,_infoNode(nullptr)
,_name(nullptr)
,_trophy(nullptr)
,_exp(nullptr)
,_guild(nullptr) {}

UserSimpleNode::~UserSimpleNode()
{
    removeFromParent();
}

bool UserSimpleNode::init()
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        
        auto background = Sprite::create(CocosUtils::getResourcePath("ui_banzi_1.png"));
        addChild(background);
        
        const auto& size(background->getContentSize());
        setContentSize(size);
        background->setPosition(Point(size.width / 2, size.height / 2));
        
        // avatar
        _avatar = AvatarNode::create();
        background->addChild(_avatar);
        
        // other info
        static const float spaceX(5);
        const auto& asize(_avatar->getContentSize());
        _infoNode = createNodeWithoutAvatar(Size(size.width - (edge.x * 2 + spaceX + asize.width), size.height - edge.y * 2));
        background->addChild(_infoNode);
        
        return true;
    }
    
    return false;
}

#pragma mark - public
void UserSimpleNode::setIsMe(bool isMe)
{
    if (_exp) {
        _exp->setVisible(isMe);
    }
    
    if (_guild) {
        _guild->setVisible(!isMe);
    }
}

void UserSimpleNode::setIsHome(bool isHome)
{
    // set positions
    const auto& size(getContentSize());
    const auto& asize(_avatar->getContentSize());
    const auto& nsize(_infoNode->getContentSize());
    if (isHome) {
        _avatar->setPositionX(asize.width / 2 + edge.x);
        _infoNode->setPositionX(size.width - (edge.x + nsize.width / 2));
    } else {
        _avatar->setPositionX(size.width - (edge.x + asize.width / 2));
        _infoNode->setPositionX(nsize.width / 2 + edge.x);
    }
    
    _avatar->setPositionY(asize.height / 2 + edge.y);
    _infoNode->setPositionY(nsize.height / 2 + edge.y);
}

void UserSimpleNode::setData(const UserSimpleData* data)
{
    if (_data != data) {
        _data = data;
    }
    
    updateThisData();
}

void UserSimpleNode::setAvatarCallback(const function<void()>& callback)
{
    if (_avatar) {
        _avatar->setCallback(callback);
    }
}

#pragma mark - private
Node* UserSimpleNode::createNodeWithoutAvatar(const Size& size)
{
    auto node = Node::create();
    node->setAnchorPoint(Point::ANCHOR_MIDDLE);
    node->setContentSize(size);
    
    // name
    auto name = CocosUtils::createLabel("User Name", SMALL_FONT_SIZE);
    name->setHorizontalAlignment(TextHAlignment::LEFT);
    name->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    node->addChild(name);
    _name = name;
    
    // trophy
    _trophy = TrophyNode::create();
    node->addChild(_trophy);
    const auto& nsize(name->getContentSize());
    const auto& tsize(_trophy->getContentSize());
    const float posY(size.height - MAX(nsize.height / 2, tsize.height / 2));
    name->setPosition(0, posY);
    _trophy->setPosition(size.width - tsize.width / 2, posY);
    
    // exp
    _exp = ExpNode::create();
    node->addChild(_exp);
    const auto& esize(_exp->getContentSize());
    _exp->setPosition(size.width / 2, esize.height / 2);
    
    // guild
    _guild = GuildSimpleNode::create();
    node->addChild(_guild);
    const auto& gsize(_guild->getContentSize());
    _guild->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    _guild->setPosition(0, gsize.height / 2);
    
    return node;
}

void UserSimpleNode::setAvatar(int idx)
{
    if (_avatar) {
        _avatar->setAvatar(idx);
    }
}

void UserSimpleNode::setUserName(const string& name)
{
    if (_name) {
        _name->setString(name);
    }
}

void UserSimpleNode::setTrophy(int count)
{
    if (_trophy) {
        _trophy->setCount(count);
    }
}

void UserSimpleNode::setGuildIcon(int idx)
{
    if (_guild) {
        _guild->setIcon(idx);
    }
}

void UserSimpleNode::setGuildName(const string& name)
{
    if (_guild) {
        _guild->setGuildName(name);
    }
}

void UserSimpleNode::setLevel(int count)
{
    if (_exp) {
        _exp->setLevel(count);
    }
}

void UserSimpleNode::setExp(int progress, int maxProgress)
{
    if (_exp) {
        _exp->setPercentage(100.0f * (float)progress / (float)maxProgress);
    }
}

void UserSimpleNode::reset()
{
    setAvatar(0);
    setUserName("");
    setTrophy(0);
    setGuildIcon(0);
    setGuildName("");
    setLevel(0);
    setExp(0, 1);
}

void UserSimpleNode::updateThisData()
{
    reset();
    
    if (_data) {
        setAvatar(_data->getIcon());
        setUserName(_data->getName());
        setLevel(_data->getLevel());
        
        do {
            auto arenaData(_data->getArenaData());
            CC_BREAK_IF(!arenaData);
            setTrophy(arenaData->getTrophy());
        } while (false);
        
        do {
            auto userGuildData(_data->getGuildData());
            CC_BREAK_IF(!userGuildData);
            auto guildData(userGuildData->getGuildData());
            CC_BREAK_IF(!guildData);
            setGuildIcon(guildData->getBadge());
            setGuildName(guildData->getName());
        } while (false);
    }
}
