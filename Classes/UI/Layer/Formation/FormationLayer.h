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
class FormationData;

USING_NS_CC;
USING_NS_CC_EXT;
using namespace UnderWorld::Core;

class FormationLayerObserver
{
public:
    virtual ~FormationLayerObserver() {}
};

enum class FormationTableType {
    Hero,
    Spell,
};

class FormationLayer
: public LayerColor
, public TableViewDelegate
, public TableViewDataSource
, public CardNodeObserver
{
protected:
    class TileInfo {
    public:
        int idx;
        Node* node;
        Point midPoint;
        TileInfo():idx(-1), node(nullptr), midPoint(Point::ZERO) {}
    };
    
    class FormationInfo {
    public:
        FormationUnitNode* node;
        std::string cardName;
        FormationInfo():node(nullptr) {}
        FormationInfo(const FormationInfo& instance):node(instance.node),cardName(instance.cardName) {}
    };
    
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
    
    void createTiles();
    void createDeck();
    
    // table
    void createTableView(FormationTableType type);
    void refreshTable(TableView* table, bool reload);
    ssize_t getCellsCount(TableView *table) const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // buttons
    void createExitButton();
    void createSwitchFormationButton(const Point& position);
    void createSwitchTableButton(const Point& position);
    void createSaveFormationButton(const Point& position);
    void createSetDefaultFormationButton(const Point& position);
    
    // labels
    void updatePopulationCount(int count);
    void updateSpellsCount(int count);
    
    // card
    CardNode* createCardNode(const std::string& name) const;
    void updateCardNode(CardNode* node, const std::string& name) const;
    void createDraggingNode(const std::string& name, const Point& point);
    void removeDraggingNode();
    void unitBackToTable();
    void unitBackToFormation();
    FormationUnitNode* createUnitNode(const std::string& name);
    
    // decks
    ssize_t getIntersectedDeckIdx(const Rect& rect) const;
    void pickSpellCard(const std::string& name);
    void cancelSpellCard(const std::string& name);
    void exchangeSpellCards(const std::string& picked, const std::string& candidate);
    void reloadDecks();
    void selectCardOnDecks(const std::string& name);
    
    // functions
    void insertCandidateCard(FormationTableType type, const std::string& name);
    FormationTableType getTableType(TableView* table) const;
    void setTableType(FormationTableType type);
    std::string getTableName(FormationTableType type) const;
    void saveFormation();
    void loadFormation(int idx);
    void setDefaultFormation();
    void placeUnit(FormationUnitNode* node, const Point& point);
    void onPlacedEnded(const std::string& name, const Point& point);
    void onUnitTouchedBegan(FormationUnitNode* node);
    void onUnitTouchedMoved(FormationUnitNode* node);
    void onUnitTouchedEnded(FormationUnitNode* node);
    const TileInfo& getTileInfo(const Point& point) const;
    int getUnitZOrder(const Point& point) const;
    
protected:
    FormationLayerObserver *_observer;
    
    // table
    std::map<FormationTableType, TableView*> _tables;
    TableView* _thisTable;
    Size _cardSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    
    // deck
    std::vector<Sprite*> _decks;
    Size _deckSize;
    Point _deckBasePosition;
    
    // tile
    std::vector<TileInfo> _tiles;
    Size _tileSize;
    Point _tileBasePosition;
    
    Node* _draggingNode;
    std::vector<Button*> _switchFormationButtons;
    Label* _populationLabel;
    Label* _spellCountLabel;
    
    // data
    FormationTableType _thisTableType;
    UnderWorld::Core::TechTree* _techTree;
    UnderWorld::Core::GameModeHMM* _gameModeHMM;
    bool _isPickingSpell;
    std::string _touchedCard;
    std::string _selectedCard;
    std::map<FormationTableType, std::vector<std::string>> _candidateCards;
    std::vector<FormationData*> _formations;
    std::unordered_set<std::string> _pickedSpells;
    int _thisFormationIdx;
    std::unordered_map<int, FormationInfo> _formation;
};

#endif /* FormationLayer_h */
