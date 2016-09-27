//
//  GuildUISearchNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/8.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildUISearchNode.h"
#include "CocosUtils.h"
#include "XEditBox.h"
#include "XButton.h"

using namespace std;

GuildUISearchNode* GuildUISearchNode::create(const Size& size)
{
    auto ret = new (nothrow) GuildUISearchNode();
    if (ret && ret->init(size)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GuildUISearchNode::GuildUISearchNode()
:_observer(nullptr)
,_tableTemplate(nullptr) {}

GuildUISearchNode::~GuildUISearchNode()
{
    CC_SAFE_DELETE(_tableTemplate);
    removeAllChildren();
}

void GuildUISearchNode::registerObserver(GuildUISearchNodeObserver *observer)
{
    _observer = observer;
}

bool GuildUISearchNode::init(const Size& size)
{
    if (Node::init()) {
        setAnchorPoint(Point::ANCHOR_MIDDLE);
        setContentSize(size);
        
        static const float edgeY(15);
        auto editBox = XEditBox::create(Size(239, 50));
        const auto& ebsize(editBox->getContentSize());
        editBox->setDelegate(this);
        editBox->setFontSize(DEFAULT_FONT_SIZE);
        editBox->setPosition(Point(size.width / 2, size.height - (edgeY + ebsize.height / 2)));
        addChild(editBox);
        
        static const float spaceX(15);
        auto label = CocosUtils::createLabel("Search clans", BIG_FONT_SIZE);
        label->setAlignment(TextHAlignment::RIGHT, TextVAlignment::CENTER);
        label->setTextColor(Color4B::BLACK);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
        label->setPosition(Point(size.width / 2 - (ebsize.width / 2 + spaceX), editBox->getPositionY()));
        addChild(label);
        
        auto button = XButton::create(XButton::BSize::Big, XButton::BType::Blue);
        button->setTitleText("Search");
        button->setPosition(Point(size.width / 2 + (ebsize.width + button->getContentSize().width) / 2 + spaceX, editBox->getPositionY()));
        addChild(button);
        
        do {
            static const float edgeBottom(8.0f);
            Size tableSize(size.width, size.height - (ebsize.height + edgeY * 2 + edgeBottom));
            Point position(0, edgeBottom + tableSize.height);
            _tableTemplate = new (nothrow) TableTemplate(this, position, false, tableSize, 1, TableTemplate::DefaultGap, this);
            _tableTemplate->setContentOffsetType(TableTemplate::ContentOffsetType::BEGIN);
        } while (false);
        
        return true;
    }
    
    return false;
}

#pragma mark - TableTemplateObserver
Node* GuildUISearchNode::onTableTemplateCreateNodeModel(TableTemplate* tt)
{
    auto node(GuildCell::create());
    node->registerObserver(this);
    return node;
}

void GuildUISearchNode::onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node)
{
    do {
        CC_BREAK_IF(idx < 0 || !node);
        CC_BREAK_IF(idx >= _guilds.size());
        auto guildCell(dynamic_cast<GuildCell*>(node));
        CC_BREAK_IF(!guildCell);
        guildCell->update(_guilds.at(idx));
    } while (false);
}

ssize_t GuildUISearchNode::numberOfNodesForTableTemplate(const TableTemplate* tt)
{
    return _guilds.size();
}

#pragma mark - EditBoxDelegate
void GuildUISearchNode::editBoxReturn(ui::EditBox* editBox)
{
    
}

#pragma mark - GuildCellObserver
void GuildUISearchNode::onGuildCellTouched(const GuildData* data)
{
    
}
