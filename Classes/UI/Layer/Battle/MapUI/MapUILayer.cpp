//
//  MapUILayer.cpp
//  Underworld_Client
//
//  Created by Andy on 15/11/3.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "MapUILayer.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "Camp.h"
#include "BattleResourceNode.h"
#include "CampInfoNode.h"
#include "SoundManager.h"

using namespace std;
using namespace cocostudio;
using namespace UnderWorld::Core;

static const UnitClass tableUnitClass[] = {
    kUnitClass_Warrior,
    kUnitClass_Hero,
};
static const size_t tablesCount = sizeof(tableUnitClass) / sizeof(UnitClass);

static ProgressTimer* createProgressTimer(const string& file)
{
    Sprite* s = Sprite::create(file);
    ProgressTimer* pt = ProgressTimer::create(s);
    pt->setType(ProgressTimer::Type::BAR);
    pt->setBarChangeRate(Vec2(1.0f, 0.0f));
    pt->setMidpoint(Point::ANCHOR_BOTTOM_LEFT);
    
    return pt;
}

MapUILayer* MapUILayer::create(const string& myAccount, const string& opponentsAccount)
{
    MapUILayer *ret = new (nothrow) MapUILayer();
    if (ret && ret->init(myAccount, opponentsAccount))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

MapUILayer::MapUILayer()
:_observer(nullptr)
,_tableViewPos(Point::ZERO)
,_isTouchingTableView(false)
,_highlightedCamp(nullptr)
,_selectedCamp(nullptr)
,_timeLabel(nullptr)
,_goldResourceNode(nullptr)
,_woodResourceNode(nullptr)
,_myHpProgress(nullptr)
,_myHpPercentageLabel(nullptr)
,_opponentsHpProgress(nullptr)
,_opponentsHpPercentageLabel(nullptr)
,_pauseMenuItem(nullptr)
,_upgradeNode(nullptr)
{
    setHighlightedCamp(nullptr);
}

MapUILayer::~MapUILayer()
{
    removeAllChildren();
}

void MapUILayer::registerObserver(MapUILayerObserver *observer)
{
    _observer = observer;
}

void MapUILayer::reload()
{
    const size_t cnt = _tableViews.size();
    if (0 == cnt) {
        createTableViews();
    } else {
        for (auto iter = begin(_tableViews); iter != end(_tableViews); ++iter) {
            iter->second->reload(_goldResourceNode->getCount(), _woodResourceNode->getCount(), _highlightedCamp);
        }
    }
}

#if ENABLE_CAMP_INFO
void MapUILayer::insertCampInfo(size_t idx, const vector<pair<const Camp*, const UnitBase*>>& units)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->insert(units);
    }
}

void MapUILayer::updateCampInfos(size_t idx)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->update();
    }
}
#else
void MapUILayer::insertUnitInfos(size_t idx, const vector<const UnitBase*>& units)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->insert(units);
    }
}

void MapUILayer::updateUnitInfos(size_t idx)
{
    if (_campInfoNodes.size() > idx) {
        CampInfoNode* node = _campInfoNodes.at(idx);
        node->update();
    }
}
#endif

void MapUILayer::closeAllUnitInfoNodes()
{
    for (int i = 0; i < _campInfoNodes.size(); ++i) {
        CampInfoNode* node = _campInfoNodes.at(i);
        node->closeUnitInfoNode();
    }
}

void MapUILayer::updateMyHpProgress(int progress)
{
    if (_myHpProgress) {
        _myHpProgress->setPercentage(progress);
    }
    
    if (_myHpPercentageLabel) {
        progress = MAX(0, MIN(progress, 100));
        _myHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
    }
}

void MapUILayer::updateOpponentsHpProgress(int progress)
{
    if (_opponentsHpProgress) {
        _opponentsHpProgress->setPercentage(progress);
    }
    
    if (_opponentsHpPercentageLabel) {
        progress = MAX(0, MIN(progress, 100));
        _opponentsHpPercentageLabel->setString(StringUtils::format("%d%%", progress));
    }
}

void MapUILayer::updateRemainingTime(int time)
{
    if (_timeLabel) {
        _timeLabel->setString(CocosUtils::getFormattedTime(time));
        
        if (time <= 180) {
            _timeLabel->setTextColor(Color4B::RED);
        } else {
            _timeLabel->setTextColor(Color4B::WHITE);
        }
    }
}

void MapUILayer::updateResources(float gold, float wood)
{
    auto node = _goldResourceNode;
    if (node) {
        node->setCount(gold, false);
        node->setSubCount(gold - (int)gold);
    }
    
    node = _woodResourceNode;
    if (node) {
        node->setCount(wood, false);
        node->setSubCount(wood - (int)wood);
    }
    
    if (_upgradeNode) {
        _upgradeNode->check(gold, wood);
    }
}

void MapUILayer::pauseGame()
{
    
}

void MapUILayer::resumeGame()
{
    
}

bool MapUILayer::isPointInTableView(const Point& point)
{
    for (auto iter = begin(_tableViews); iter != end(_tableViews); ++iter) {
        if (getTableViewBoundingBox(iter->first).containsPoint(point)) {
            return true;
        }
    }
    
    return false;
}

void MapUILayer::clearHighlightedCamp()
{
    setHighlightedCamp(nullptr);
}

void MapUILayer::removeUpgradeNode()
{
    if (_upgradeNode) {
        _upgradeNode->removeFromParent();
        _upgradeNode = nullptr;
    }
}

#pragma mark - MapUITableObserver
void MapUILayer::onMapUITableUnitAdd(const Camp* camp)
{
    setHighlightedCamp(camp, false, true, false);
    
    if (_observer) {
        _observer->onMapUILayerUnitAdd(camp);
    }
}

void MapUILayer::onMapUITableUnitUpgrade(MapUIUnitNode* node)
{
    const Camp* camp = node->getCamp();
    setHighlightedCamp(camp, false, false, false);
    
    createUpgradeNode(camp);
    const Point& point = convertToNodeSpace(node->getParent()->convertToWorldSpace(node->getPosition()));
    _upgradeNode->setPosition(point);
}

void MapUILayer::onMapUITableUnitTouchedBegan(const Camp* camp)
{
    _selectedCamp = (camp->getProduction() > 0) ? camp : nullptr;
}

void MapUILayer::onMapUITableUnitTouchedEnded(const Camp* camp)
{
    if (!isGameOver()) {
        onUnitTouched(camp);
    }
}

#pragma mark - CampInfoNodeObserver
void MapUILayer::onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnitBase* unit)
{
    for (int i = 0; i < _campInfoNodes.size(); ++i) {
        CampInfoNode* node = _campInfoNodes.at(i);
        if (pSender != node) {
            node->closeUnitInfoNode();
        }
    }
}

#pragma mark - MapUIUpgradeNodeObserver
void MapUILayer::onMapUIUpgradeNodeClickedButton(const Camp* camp, int idx)
{
    if (_observer) {
        _observer->onMapUILayerUnitUpgrade(camp, idx);
    }
}

#pragma mark -
bool MapUILayer::init(const string& myAccount, const string& opponentsAccount)
{
    if (LayerColor::initWithColor(LAYER_DEFAULT_COLOR))
    {
        const Size& winSize = Director::getInstance()->getWinSize();
#if false
        // yezi
        {
            static const string file("particle/yezi/yezi.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 - 10));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 + 10));
            addChild(effect);
        }
        {
            static const string file("particle/yezi/yezi_0_0_0.plist");
            ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
            effect->setPosition(Point(-50, winSize.height / 2 + 20));
            addChild(effect);
        }
#endif
        
#if false
        static const string CsbFile("rankInfo_UI.csb");
        Node *mainNode = CSLoader::createNode(CsbFile);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        timeline::ActionTimeline *action = CSLoader::createTimeline(CsbFile);
        mainNode->runAction(action);
        action->gotoFrameAndPlay(0, false);
        Widget* root = dynamic_cast<Widget *>(mainNode->getChildByTag(777));
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                            
                    }
                }
            }
        }
#else
        Node* root = this;
        static const float leftOffset(5.0f);
        static const float ceilOffset(5.0f);
        static const Size progressSize(170.0f, 20.0f);
        
        createUserInfo(true, myAccount);
        
        //
        {
            Sprite* sprite = Sprite::create("GameImages/test/ui_time.png");
            root->addChild(sprite);
            
            const Size& size = sprite->getContentSize();
            sprite->setPosition(Point(winSize.width / 2, winSize.height - size.height / 2 - ceilOffset));
            
            _timeLabel = CocosUtils::createLabel("", 24, DEFAULT_NUMBER_FONT);
            _timeLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            _timeLabel->setPosition(Point(size.width * 0.45f, size.height / 2));
            sprite->addChild(_timeLabel);
        }
        
        createUserInfo(false, opponentsAccount);
        
        //
        {
            _woodResourceNode = BattleResourceNode::create(kResourceType_Wood);
            root->addChild(_woodResourceNode);
            
            const Size& size = _woodResourceNode->getContentSize();
            _woodResourceNode->setAnchorPoint(Point::ANCHOR_MIDDLE);
            _woodResourceNode->setPosition(Point(size.width / 2 + leftOffset, size.height / 2 + ceilOffset));
            
            _goldResourceNode = BattleResourceNode::create(kResourceType_Gold);
            root->addChild(_goldResourceNode);
            
            _goldResourceNode->setAnchorPoint(_woodResourceNode->getAnchorPoint());
            _goldResourceNode->setPosition(_woodResourceNode->getPosition() + Point(size.width + leftOffset, 0));
            
            // units table
            _tableViewPos = Point(_goldResourceNode->getPositionX() + size.width / 2 + leftOffset, 0);
        }
        
        // buttons
        {
            _pauseMenuItem = MenuItemImage::create("GameImages/test/ui_zt.png", "GameImages/test/ui_zt.png", [this](Ref*) {
                if (_observer) {
                    _observer->onMapUILayerClickedPauseButton();
                }
            });
            _pauseMenuItem->setAnchorPoint(Point::ANCHOR_TOP_RIGHT);
            _pauseMenuItem->setPosition(Point(winSize.width - 5.0f, winSize.height - 5.0f));
            Menu *menu = Menu::create(_pauseMenuItem, nullptr);
            menu->setPosition(Point::ZERO);
            root->addChild(menu);
        }
        
        {
            CampInfoNode* campInfoNode = CampInfoNode::create(false);
            campInfoNode->registerObserver(this);
            const Size& size = campInfoNode->getContentSize();
            static const float posY = 160.0f;
            campInfoNode->setPosition(Point(0, posY));
            root->addChild(campInfoNode);
            _campInfoNodes.push_back(campInfoNode);
            
            campInfoNode = CampInfoNode::create(true);
            campInfoNode->registerObserver(this);
            campInfoNode->setPosition(Point(winSize.width - size.width, posY));
            root->addChild(campInfoNode);
            _campInfoNodes.push_back(campInfoNode);
        }
#endif
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->setSwallowTouches(true);
        eventListener->onTouchBegan = CC_CALLBACK_2(MapUILayer::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(MapUILayer::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(MapUILayer::onTouchEnded, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

bool MapUILayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    const Point& point = touch->getLocation();
    if (isPointInTableView(point)) {
        _isTouchingTableView = true;
        return true;
    }
    
    for (int i = 0; i < _campInfoNodes.size(); ++i) {
        CampInfoNode* node = _campInfoNodes.at(i);
        if (node) {
            const Rect& bb = node->getIconsBoundingBox();
            Rect boundingBox(Rect::ZERO);
            boundingBox.origin = convertToNodeSpace(node->convertToWorldSpace(bb.origin));
            boundingBox.size = bb.size;
            if (boundingBox.containsPoint(point)) {
                return true;
            }
        }
    }
    
    return false;
}

void MapUILayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    if (_isTouchingTableView && _selectedCamp && _observer) {
        const Point& point = touch->getLocation();
        if (_highlightedCamp != _selectedCamp && !isPointInTableView(point)) {
            setHighlightedCamp(_selectedCamp);
        }
        
        _observer->onMapUILayerTouchMoved(_selectedCamp, point);
    }
}

void MapUILayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    if (_isTouchingTableView && _selectedCamp) {
        if (_observer) {
            const Point& point = touch->getLocation();
            if (!isPointInTableView(point)) {
                _observer->onMapUILayerTouchEnded(_selectedCamp, point);
            }
        }
        
        _selectedCamp = nullptr;
        _isTouchingTableView = false;
    }
}

#pragma mark private
void MapUILayer::createUserInfo(bool left, const string& account)
{
    const Size& winSize = Director::getInstance()->getWinSize();
    
    Sprite* nameBg = CocosUtils::createPureColorSprite(Size(163, 26), LAYER_MASK_COLOR);
    addChild(nameBg);
    
    const Size& nameBgSize = nameBg->getContentSize();
    static const float offsetX(192.0f);
    nameBg->setPosition(Point(winSize.width / 2 + offsetX * (left ? -1 : 1), winSize.height - nameBgSize.height / 2));
    
    const Point& position = nameBg->getPosition();
    
    // name
    {
        Label* label = CocosUtils::createLabel(account, DEFAULT_FONT_SIZE);
        label->setPosition(Point(nameBgSize.width / 2, nameBgSize.height / 2));
        nameBg->addChild(label);
    }
    
    // progress
    {
        Sprite* sprite = Sprite::create(left ? "GameImages/test/ui_blood_left_bg.png" : "GameImages/test/ui_blood_right_bg.png");
        addChild(sprite);
        
        const Size& size = sprite->getContentSize();
        sprite->setPosition(position - Point(0, (nameBgSize.height + size.height) / 2));
        
        ProgressTimer* pt = createProgressTimer(left ? "GameImages/test/ui_blood_left.png" : "GameImages/test/ui_blood_right.png");
        pt->setPosition(Point(size.width / 2, size.height / 2));
        pt->setMidpoint(left ? Point::ANCHOR_BOTTOM_RIGHT : Point::ANCHOR_BOTTOM_LEFT);
        sprite->addChild(pt);
        
        Label* ptLabel = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
        ptLabel->setPosition(pt->getPosition());
        sprite->addChild(ptLabel);
        
        if (left) {
            _myHpProgress = pt;
            _myHpPercentageLabel = ptLabel;
        } else {
            _opponentsHpProgress = pt;
            _opponentsHpPercentageLabel = ptLabel;
        }
    }
    
    // icon
    {
        Sprite* sprite = Sprite::create(left ? "GameImages/test/icon_zg.png" : "GameImages/test/icon_bc.png");
        addChild(sprite);
        
        const Size& size = sprite->getContentSize();
        sprite->setPosition(Point(position.x + (nameBgSize.width / 2 + size.width / 2) * (left ? 1 : -1), winSize.height - size.height / 2));
    }
}

bool MapUILayer::isGameOver() const
{
    if (_observer) {
        return _observer->onMapUILayerIsGameOver();
    }
    
    return false;
}

void MapUILayer::onUnitTouched(const Camp* camp)
{
    setHighlightedCamp(camp, true);
    SoundManager::getInstance()->playButtonSelectUnitSound();
}

void MapUILayer::createTableViews()
{
    static float offsetX(0.0f);
    vector<Node*> created;
    for (int i = 0; i < tablesCount; ++i) {
        Node* node = createTableView(tableUnitClass[i], this);
        if (node) {
            const size_t cnt = created.size();
            Point pos(_tableViewPos);
            if (cnt > 0) {
                Node* n = created.back();
                pos = n->getPosition() + Point(n->getContentSize().width + offsetX, 0);
            }
            node->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            node->setPosition(pos);
            created.push_back(node);
        }
    }
}

Node* MapUILayer::createTableView(UnitClass uc, Node* parent)
{
    if (_observer) {
        size_t cnt(0);
        const vector<const Camp*>& camps = _observer->onMapUILayerGetCamps(uc);
        cnt = camps.size();
        if (cnt > 0) {
            MapUITable* table = MapUITable::create(camps);
            table->registerObserver(this);
            parent->addChild(table);
            _tableViews.insert(make_pair(uc, table));
            return table;
        }
    }
    
    return nullptr;
}

Rect MapUILayer::getTableViewBoundingBox(UnitClass uc) const
{
    if (_tableViews.find(uc) != _tableViews.end()) {
        MapUITable* tv = _tableViews.at(uc);
        Rect rect = tv->getBoundingBox();
        Point origin = rect.origin;
        rect.origin = convertToNodeSpace(tv->getParent()->convertToWorldSpace(origin));
        return rect;
    }
    
    return Rect::ZERO;
}

void MapUILayer::setHighlightedCamp(const Camp* camp, bool callback, bool ignoreProduction, bool check)
{
    const Camp* lastCamp = _highlightedCamp;
    
    bool tryToSpell(false);
    if (_observer && camp) {
        tryToSpell = _observer->onMapUILayerIsHeroAlive(camp);
    }
    const bool isNew = (lastCamp != camp);
    
    bool update(true);
    if (isNew) {
        if (!tryToSpell) {
            if (!camp || ((ignoreProduction || camp->getProduction() > 0) && camp->getColdDown() <= 0)) {
                _highlightedCamp = camp;
            }
        } else {
            update = false;
        }
    } else {
        if (check) {
            _highlightedCamp = nullptr;
        } else {
            update = false;
        }
    }
    
    if (isNew || check) {
        removeUpgradeNode();
    }
    
    // callback
    if (_observer) {
        _observer->onMapUILayerUnitSelected(_highlightedCamp);
        
        if (callback) {
            if (tryToSpell) {
                _observer->onMapUILayerUnitTouched(camp);
            } else {
                _observer->onMapUILayerUnitTouched(_highlightedCamp);
            }
        }
    }
}

void MapUILayer::createUpgradeNode(const Camp* camp)
{
    if (_upgradeNode && _upgradeNode->getCamp() != camp) {
        removeUpgradeNode();
    }
    
    if (!_upgradeNode && !camp->isUpgraded()) {
        _upgradeNode = MapUIUpgradeNode::create(camp);
        addChild(_upgradeNode);
    }
}
