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

USING_NS_CC_EXT;

class TabButton;

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
    
    // table
    void createTableView(CardTableType type);
    void refreshTable(TableView* table, bool reload);
    ssize_t getCellsCount(TableView *table) const;
    Size getCellSize() const;
    Rect getBoundingBox(Node* node) const;
    
    // buttons
    void createTabButtons(const Point& position);
    
    // labels
    void updateCardsCount(int count);
    
    // card
    XCardNode* createXCardNode(const std::string& name) const;
    void updateXCardNode(XCardNode* node, const std::string& name) const;
    
    // functions
    void reloadAllCandidateCards();
    void reloadCandidateCards(CardTableType type);
    void insertCandidateCard(CardTableType type, const std::string& name);
    void removeCandidateCard(CardTableType type, const std::string& name);
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
    std::map<CardTableType, std::vector<std::string>> _candidateCards;
};

#endif /* CardLayer_h */
