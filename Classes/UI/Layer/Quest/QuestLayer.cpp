//
//  QuestLayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestLayer.h"
#include "CocosGlobal.h"
#include "Utils.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "QuestManager.h"
#include "QuestData.h"
#include "TabButton.h"
#include "PureNode.h"

using namespace std;
using namespace ui;

static const QuestType tableTypes[] = {
    QuestType::Main,
    QuestType::Branch,
    QuestType::Daily,
    QuestType::TimeLimited
};
static const unsigned int tablesCount = sizeof(tableTypes) / sizeof(QuestType);

QuestLayer* QuestLayer::create(QuestType type)
{
    auto ret = new (nothrow) QuestLayer();
    if (ret && ret->init(type)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

QuestLayer::QuestLayer()
:_observer(nullptr)
,_thisTableType(QuestType::None)
,_thisTableTemplat(nullptr) {}

QuestLayer::~QuestLayer()
{
    Utils::clearMap(_tableTemplats);
    removeAllChildren();
}

void QuestLayer::registerObserver(QuestLayerObserver *observer)
{
    _observer = observer;
}

bool QuestLayer::init(QuestType type)
{
    if (LayerColor::initWithColor(LAYER_MASK_COLOR)) {
        // tables
        for (int i = 0; i < tablesCount; ++i) {
            auto type = tableTypes[i];
            createTableTemplate(type);
        }
        
        // buttons
        CocosUtils::createGrayExitButton(this, [this]() { removeFromParent(); });
        
        do {
            const auto& winSize(Director::getInstance()->getWinSize());
            static const Vec2 edge(180, 50);
            createTabButtons(Point(edge.x, winSize.height - edge.y));
        } while (false);
        
        setQuestType(QuestType::Main);
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = [](Touch *pTouch, Event *pEvent) { return true; };
        eventListener->onTouchEnded = [](Touch *pTouch, Event *pEvent) {};
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

#pragma mark - TableTemplateObserver
Node* QuestLayer::onTableTemplateCreateNodeModel(TableTemplate* tt)
{
    auto node(QuestNode::create());
    node->registerObserver(this);
    return node;
}

void QuestLayer::onTableTemplateUpdateNode(TableTemplate* tt, ssize_t idx, Node* node)
{
    do {
        CC_BREAK_IF(idx < 0 || !node);
        auto type(getQuestType(tt));
        CC_BREAK_IF(QuestType::None == type);
        const auto& lists(QuestManager::getInstance()->getData(type));
        CC_BREAK_IF(idx >= lists.size());
        auto questNode(dynamic_cast<QuestNode*>(node));
        CC_BREAK_IF(!questNode);
        questNode->update(lists.at(idx));
    } while (false);
}

ssize_t QuestLayer::numberOfNodesForTableTemplate(const TableTemplate* tt)
{
    do {
        auto type(getQuestType(tt));
        CC_BREAK_IF(QuestType::None == type);
        return QuestManager::getInstance()->getData(type).size();
    } while (false);
    
    return 0;
}

#pragma mark UI
void QuestLayer::createTableTemplate(QuestType type)
{
    do {
        static const float edgeY(5.0f);
        const auto& winSize(Director::getInstance()->getWinSize());
        Size size(winSize.width, winSize.height - edgeY * 2);
        Point position(0, winSize.height - edgeY);
        auto tt(new (nothrow) TableTemplate(this, position, false, size, 1, TableTemplate::DefaultGap, this));
        tt->setContentOffsetType(TableTemplate::ContentOffsetType::BEGIN);
        tt->setTag(static_cast<int>(type));
        
        if (0 == _tableTemplats.count(type)) {
            _tableTemplats.insert(make_pair(type, tt));
        } else {
            assert(false);
        }
    } while (false);
}

void QuestLayer::createTabButtons(const Point& position)
{
    float width(0);
    float buttonHeight(0);
    static const Vec2 offset(10.0f, 0);
    for (int i = 0; i < tablesCount; ++i) {
        const auto type = tableTypes[i];
        const string title = getTableName(type);
        auto button = TabButton::create(title, [this, i](Ref*) {
            setQuestType(tableTypes[i]);
        });
        addChild(button);
        
        const auto& size = button->getContentSize();
        button->setPosition(position + Point((size.width + offset.x) * i + size.width / 2, -(offset.y + size.height / 2)));
        
        if (_tabButtons.find(type) != end(_tabButtons)) {
            assert(false);
            _tabButtons.at(type) = button;
        } else {
            _tabButtons.insert(make_pair(type, button));
        }
        
        width += size.width + offset.x;
        buttonHeight = MAX(buttonHeight, size.height);
    }
    
    width -= offset.x;
    
    // add lines
    vector<Node*> lines;
    for (int i = 0; i < 2; ++i) {
        auto line = PureNode::createLine(Size(width, 2));
        addChild(line);
        
        lines.push_back(line);
    }
    
    const float x = position.x + width / 2;
    static const float offsetY(5.0f);
    lines.at(0)->setPosition(Point(x, position.y - buttonHeight - offsetY));
    lines.at(1)->setPosition(Point(x, position.y + offsetY));
}

#pragma mark - functions
QuestType QuestLayer::getQuestType(const TableTemplate* tt) const
{
    if (tt) {
        const size_t cnt = _tableTemplats.size();
        if (cnt < tablesCount) {
            return tableTypes[cnt];
        }
        
        return static_cast<QuestType>(tt->getTag());
    }
    
    return static_cast<QuestType>(-1);
}

void QuestLayer::setQuestType(QuestType type)
{
    if (_thisTableType != type) {
        _thisTableType = type;
        
        for (auto iter = begin(_tableTemplats); iter != end(_tableTemplats); ++iter) {
            const bool isSelected(iter->first == type);
            auto tt = iter->second;
            auto table(tt->getTableView());
            if (table) {
                table->setVisible(isSelected);
            }
            
            if (isSelected) {
                _thisTableTemplat = tt;
            }
        }
        
        for (auto iter = begin(_tabButtons); iter != end(_tabButtons); ++iter) {
            const bool isSelected(iter->first == type);
            iter->second->setEnabled(!isSelected);
        }
    }
}

string QuestLayer::getTableName(QuestType type) const
{
    switch (type) {
        case QuestType::Main:
            return LocalHelper::getString("ui_quest_tab_main");
        case QuestType::Branch:
            return LocalHelper::getString("ui_quest_tab_branch");
        case QuestType::Daily:
            return LocalHelper::getString("ui_quest_tab_daily");
        case QuestType::TimeLimited:
            return LocalHelper::getString("ui_quest_tab_timeLimited");
        default:
            return "";
    }
}
