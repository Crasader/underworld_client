//
//  CardPreview.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardPreview_h
#define CardPreview_h

#include "DeckManager.h"
#include "CardOpNode.h"

USING_NS_CC;

class BaseCard;
class XButton;
class CardSet;

class CardPreviewObserver
{
public:
    virtual ~CardPreviewObserver() {}
    virtual BaseCard* onCardPreviewCreateCard(int cardId) = 0;
    virtual void onCardPreviewClickedOpButton(CardOpType type, const AbstractData* data) = 0;
};

class CardPreview : public CardOpNodeObserver
{
public:
    CardPreview(DeckManager::FeatureType type, Node* parent, int column, float unfoundSpaceBottom, CardPreviewObserver* observer);
    virtual ~CardPreview();
    
    void sortAndRealign();
    
    void readdToScrollView(BaseCard* card);
    BaseCard* getFoundCard(int cardId) const;
    void removeFoundCard(int cardId, bool cleanup);
    void insertFoundCard(int cardId, BaseCard* card);
    
    // Oprations Node
    void showOpNode(BaseCard* card, const std::vector<CardOpType>& types);
    void hideOpNode();
    bool setOpNodePosition(BaseCard* card);
    
private:
    // CardOpNodeObserver
    virtual void onCardOpNodeClicked() override;
    virtual void onCardOpNodeClickedButton(CardOpType type, const AbstractData* data) override;
    
    void initCards();
    BaseCard* createCardNode(int cardId);
    Node* createUnfoundLine() const;
    std::string getSortTypeName(int idx) const;
    void setNextSortType();
    void sortAllCards();
    void realignCards(const std::vector<int>& cards, CardSet* cardSet);
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    void updateCardsCount();
    void fullyDisplayCard(BaseCard* card);
    bool isOpNodeOnCard(BaseCard* card) const;
    
private:
    CardPreviewObserver *_observer;
    
    // UI
    Label* _cardsCountLabel;
    XButton* _sortTypeButton;
    ui::ScrollView* _scrollView;
    Node* _unfoundLine;
    CardSet* _foundCards;
    CardSet* _unfoundCards;
    CardOpNode* _opNode;
    
    // Data
    DeckManager::FeatureType _featureType;
    int _column;
    float _unfoundSpaceBottom;
    Size _cardSize;
    int _sortIdx;
    float _cardSpaceX;
    Size _scrollViewMinSize;
};

#endif /* CardPreview_h */
