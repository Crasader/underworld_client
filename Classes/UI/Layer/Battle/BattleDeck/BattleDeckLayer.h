//
//  BattleDeckLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/2/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckLayer_h
#define BattleDeckLayer_h

#include "AbstractUILayer.h"
#include "cocos-ext.h"
#include "BattleDeckUnitNode.h"
#include "BattleDeckTestNode.h"
#include "UnitType.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BattleDeckUnitInfoNode;

class BattleDeckLayerObserver
{
public:
    virtual ~BattleDeckLayerObserver() {}
};

class BattleDeckLayer : public AbstractUILayer, public TableViewDelegate, public TableViewDataSource, public BattleDeckUnitNodeObserver, public BattleDeckTestNodeObserver
{
public:
    static BattleDeckLayer* create();
    virtual ~BattleDeckLayer();
    void registerObserver(BattleDeckLayerObserver *observer);
    
protected:
    BattleDeckLayer();
    virtual bool init() override;
    
    // LayerColor
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual void onTouchMoved(Touch *touch, Event *unused_event) override;
    virtual void onTouchEnded(Touch *touch, Event *unused_event) override;
    
    // TableViewDelegate
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override;
    virtual void tableCellHighlight(TableView* table, TableViewCell* cell) override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // BattleDeckUnitNodeObserver
    virtual void onBattleDeckUnitNodeTouchedBegan(BattleDeckUnitNode* node) override;
    virtual void onBattleDeckUnitNodeTouchedEnded(BattleDeckUnitNode* node, bool isValid) override;
    virtual void onBattleDeckUnitNodeTouchedCanceled(BattleDeckUnitNode* node) override;
    
    // BattleDeckTestNodeObserver
    virtual void onBattleDeckTestNodeTouchedBegan(BattleDeckTestNode* node) override;
    virtual void onBattleDeckTestNodeTouchedEnded(BattleDeckTestNode* node, bool isValid) override;
    virtual void onBattleDeckTestNodeTouchedCanceled(BattleDeckTestNode* node) override;
    
    void createTableViews(float width);
    Node* createTableView(UnderWorld::Core::UnitClass uc, float width);
    UnderWorld::Core::UnitClass getUnitClass(TableView* table) const;
    ssize_t getCellsCount(TableView* table) const;
    ssize_t getCellsCount(UnderWorld::Core::UnitClass uc) const;
    Rect getRealBoundingBox(Node* node) const;
    void configTable(UnderWorld::Core::UnitClass uc, bool reload);
    void createDragNode(const std::string& name);
    void removeDragNode();
    void reloadCardDecks();
    void onTableCardMoved(const Point& pos);
    void onTableCardEnded(const Point& pos);
    void cardBackToTable();
    
    int getIntersectedCardDeckIdx(const Rect& rect) const;
    
    const std::vector<std::string>& getCandidateCards(UnderWorld::Core::UnitClass uc) const;
    const std::set<std::string>& getPickedCards() const;
    UnderWorld::Core::UnitClass getUnitClass(const std::string& name) const;
    void loadData(UnderWorld::Core::UnitClass uc);
    void saveData();
    void extract(UnderWorld::Core::UnitClass uc, const std::string& name);
    void insert(const std::string& name);
    void exchange(const std::string& picked, UnderWorld::Core::UnitClass uc, const std::string& candidate);
    
private:
    struct TableViewNode {
        Label* titleLabel;
        Button* leftButton;
        Button* rightButton;
        TableView* table;
        float maxWidth;
    };
    
    BattleDeckLayerObserver *_observer;
    Point _tableViewPos;
    Size _cellSize;
    std::string _touchedCard;
    std::pair<ssize_t, std::string> _selectedTableCard;
    std::string _selectedCard;
    
    std::map<UnderWorld::Core::UnitClass, std::vector<std::string>> _candidateCards;
    std::set<std::string> _pickedCards;
    
    BattleDeckUnitInfoNode* _infoNode;
    std::map<UnderWorld::Core::UnitClass, TableViewNode> _tableViewNodes;
    Label* _selectedCardsLabel;
    std::vector<Sprite*> _cardDecks;
    Node* _dragNode;
};

#endif /* BattleDeckLayer_h */
