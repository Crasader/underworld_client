//
//  BillboardCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "BillboardCell.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "BillboardData.h"
#include "UserSimpleData.h"
#include "UserGuildData.h"
#include "GuildData.h"
#include "AvatarNode.h"
#include "GuildTorphyNode.h"

using namespace std;

static string getRankBg(int rank) {
    if (rank >= 1 && rank <= 3) {
        return CocosUtils::getResourcePath(StringUtils::format("ui_kuang_%d.png", 22 + rank));
    }
    
    return "";
}

BillboardCell* BillboardCell::create()
{
    auto ret = new (nothrow) BillboardCell();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BillboardCell::BillboardCell()
:_observer(nullptr)
,_data(nullptr)
,_topRankBg(nullptr)
,_rank(nullptr) {}

BillboardCell::~BillboardCell()
{
    removeAllChildren();
}

bool BillboardCell::init()
{
    if (GuildGenericCell::init())
    {
        CocosUtils::fixWidgetTouchEvent(this, _touchInvalid, nullptr, [this](Ref*) {
            if (_observer) {
                _observer->onBillboardCellClicked(_data);
            }
        });
        
        static const float redge(15);
        auto rankBg = Sprite::create(getRankBg(1));
        const auto& rsize(rankBg->getContentSize());
        rankBg->setPosition(Point(redge + rsize.width / 2, _avatar->getPositionY()));
        addChild(rankBg);
        _topRankBg = rankBg;
        
        auto label = CocosUtils::createLabel("", BIG_FONT_SIZE);
        label->setTextColor(Color4B::BLACK);
        label->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE);
        label->setPosition(rankBg->getPosition());
        addChild(label);
        _rank = label;
        
        static const float rspace(15);
        setAvatarEdgeX(rankBg->getPositionX() + rsize.width / 2 + rspace);
        
        return true;
    }
    
    return false;
}

void BillboardCell::registerObserver(BillboardCellObserver *observer)
{
    _observer = observer;
}

void BillboardCell::update(const BillboardData* data)
{
    if (_data != data) {
        _data = data;
        
        if (_avatar) {
            _avatar->update(data ? 0 : 0);
        }
        
        if (_name) {
            const bool show(data && data->getUserData());
            _name->setString(show ? data->getUserData()->getName() : "Player");
        }
        
        if (_trophy) {
            const bool show(data &&
                            data->getUserData() &&
                            data->getUserData()->getGuildData() &&
                            data->getUserData()->getGuildData()->getGuildData());
            _trophy->setCount(show ? data->getUserData()->getGuildData()->getGuildData()->getTotalTrophies() : 0);
        }
        
        
        const auto rank(data ? data->getRank() : 0);
        if (_topRankBg) {
            const auto& file(getRankBg(rank));
            const bool show(!file.empty());
            _topRankBg->setVisible(show);
            if (show) {
                _topRankBg->setTexture(file);
            }
        }
        
        if (_rank) {
            _rank->setString(StringUtils::format("%d", rank));
        }
    }
}
