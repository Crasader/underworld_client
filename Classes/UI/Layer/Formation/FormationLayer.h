//
//  FormationLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/6/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FormationLayer_h
#define FormationLayer_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include "CardNode.h"

namespace UnderWorld { namespace Core {
    class TechTree;
    class GameModeHMM;
}}

class FormationUnitNode;

USING_NS_CC;
USING_NS_CC_EXT;
using namespace UnderWorld::Core;

class FormationLayerObserver
{
public:
    virtual ~FormationLayerObserver() {}
};

class FormationLayer
: public LayerColor
, public TableViewDelegate
, public TableViewDataSource
, public CardNodeObserver
{
public:
    static FormationLayer* create();
    virtual ~FormationLayer();
    void registerObserver(FormationLayerObserver *observer);
    
protected:
    FormationLayer();
    virtual bool init() override;
    
    // AbstractUILayer
    virtual void onEnter() override;
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
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) override;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) override;
    
    // table
    void createTiles();
    void createTableView();
    void refreshTable(bool reload);
    ssize_t getCellsCount() const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // buttons
    void createExitButton();
    void createSwitchFormationButton();
    void createSaveFormationButton(const Point& position);
    void createSetDefaultFormationButton(const Point& position);
    
    // labels
    void updateResourceCount(int count);
    
    // card
    CardNode* createCardNode(const std::string& name) const;
    void updateCardNode(CardNode* node, const std::string& name) const;
    void createDraggingNode(const std::string& name, const Point& point);
    void removeDraggingNode();
    void unitBackToTable();
    void unitBackToFormation();
    void replace(FormationUnitNode* node);
    
    // functions
    void saveFormation();
    void loadFormation(int idx);
    void setDefaultFormation();
    void onPlaceEnded(const std::string& name, const Point& point);
    void onUnitTouchedBegan(FormationUnitNode* node);
    void onUnitTouchedMoved(FormationUnitNode* node);
    void onUnitTouchedEnded(FormationUnitNode* node);
    int getUnitZOrder(const Point& point) const;
    
protected:
    struct FormationInfo {
        FormationUnitNode* node;
        std::string cardName;
    };
    
    FormationLayerObserver *_observer;
    
    // cocos
    TableView* _table;
    Size _cardSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    std::vector<Node*> _tiles;
    Size _tileSize;
    Node* _draggingNode;
    Label* _resourceLabel;
    
    // data
    UnderWorld::Core::TechTree* _techTree;
    UnderWorld::Core::GameModeHMM* _gameModeHMM;
    std::string _selectedCard;
    std::vector<std::string> _candidateCards;
    std::unordered_set<std::string> _pickedCards;
    std::unordered_map<int, FormationInfo> _formation;
};

#endif /* FormationLayer_h */
