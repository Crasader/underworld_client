//
//  CardLayer.h
//  Underworld_Client
//
//  Created by Andy on 16/6/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardLayer_h
#define CardLayer_h

#include "cocos-ext.h"
#include <vector>
#include <unordered_map>
#include "XCardNode.h"
#include "CardInfoLayer.h"
#include "CardXInfoLayer.h"

class TabButton;

USING_NS_CC_EXT;

class CardLayerObserver
{
public:
    virtual ~CardLayerObserver() {}
};

enum class CardTableType {
    All,
    Heroes,
    Soldiers,
    Spells,
};

class CardLayer
: public LayerColor
, public TableViewDataSource
, public CardNodeObserver
, public CardInfoLayerObserver
, public CardXInfoLayerObserver
{
public:
    static CardLayer* create();
    virtual ~CardLayer();
    void registerObserver(CardLayerObserver *observer);
    
protected:
    CardLayer();
    virtual bool init() override;
    
    // AbstractUILayer
    virtual void onEnter() override;
    
    // TableViewDataSource
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override;
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override;
    virtual ssize_t numberOfCellsInTableView(TableView *table) override;
    
    // CardNodeObserver
    virtual void onCardNodeTouchedBegan(CardNode* node) override;
    virtual void onCardNodeTouchedEnded(CardNode* node, bool isValid) override;
    
    // CardInfoLayerObserver
    virtual void onCardInfoLayerClosed(int card) override;
    virtual void onCardInfoLayerUpgradeCard(int card) override;
    
    // CardXInfoLayerObserver
    virtual void onCardXInfoLayerClosed(int card) override;
    virtual void onCardXInfoLayerUpgradeCard(int card) override;
    virtual void onCardXInfoLayerUpgradeTalent(int card) override;
    
    // table
    void createTable(CardTableType type);
    void refreshTable(TableView* table, bool reload);
    ssize_t getCellsCount(TableView *table) const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // buttons
    void createTabButtons(const Point& position);
    
    // labels
    void updateCardsCount(int count);
    
    // card
    XCardNode* createXCardNode(int idx) const;
    void updateXCardNode(XCardNode* node, int idx) const;
    
    // functions
    void reloadAllCandidateCards();
    void reloadCandidateCards(CardTableType type);
    void insertCandidateCard(CardTableType type, int idx);
    void removeCandidateCard(CardTableType type, int idx);
    CardTableType getTableType(TableView* table) const;
    void setTableType(CardTableType type);
    std::string getTableName(CardTableType type) const;
    
private:
    CardLayerObserver *_observer;
    
    // table
    std::map<CardTableType, TableView*> _tables;
    CardTableType _thisTableType;
    TableView* _thisTable;
    Size _cardSize;
    Size _tableMaxSize;
    Point _tableBasePosition;
    
    // labels
    Label* _cardsCountLabel;
    std::map<CardTableType, TabButton*> _tabButtons;
    
    // data
    std::string _touchedCard;
    std::map<CardTableType, std::vector<int>> _candidateCards;
};

#endif /* CardLayer_h */
