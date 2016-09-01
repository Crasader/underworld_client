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

class AbstractCard;
class UniversalButton;
class CardSet;

class CardPreviewObserver
{
public:
    virtual ~CardPreviewObserver() {}
    virtual AbstractCard* onCardPreviewCreateCard(int cardId) = 0;
    virtual void onCardPreviewClickedOpButton(DeckCardOpType type, const AbstractData* data) = 0;
};

class CardPreview : public CardOpNodeObserver
{
public:
    CardPreview(DeckManager::FeatureType type, Node* parent, CardPreviewObserver* observer);
    virtual ~CardPreview();
    
    void sortAndRealign();
    
    void readdToScrollView(AbstractCard* card);
    AbstractCard* getFoundCard(int cardId) const;
    void removeFoundCard(int cardId, bool cleanup);
    void insertFoundCard(int cardId, AbstractCard* card);
    
    // Oprations Node
    void showOpNode(AbstractCard* card, const std::vector<DeckCardOpType>& types);
    void hideOpNode();
    bool setOpNodePosition(AbstractCard* card);
    
private:
    // CardOpNodeObserver
    virtual void onCardOpNodeClicked() override;
    virtual void onCardOpNodeClickedButton(DeckCardOpType type, const AbstractData* data) override;
    
    void initCards();
    AbstractCard* createCardNode(int cardId);
    Node* createUnfoundLine() const;
    std::string getSortTypeName(int idx) const;
    void setNextSortType();
    void sortAllCards();
    void realignCards(const std::vector<int>& cards, CardSet* cardSet);
    float getHeight(size_t count, float spaceY) const;
    Point getPosition(int row, int column) const;
    void updateCardsCount();
    void fullyDisplayCard(AbstractCard* card);
    bool isOpNodeOnCard(AbstractCard* card) const;
    
private:
    CardPreviewObserver *_observer;
    
    // UI
    Label* _cardsCountLabel;
    UniversalButton* _sortTypeButton;
    ui::ScrollView* _scrollView;
    Node* _unfoundLine;
    CardSet* _foundCards;
    CardSet* _unfoundCards;
    CardOpNode* _opNode;
    
    // Data
    DeckManager::FeatureType _featureType;
    Size _cardSize;
    int _sortIdx;
    float _cardSpaceX;
    Size _scrollViewMinSize;
};

#endif /* CardPreview_h */
