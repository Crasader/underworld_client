//
//  HMMCardRender.cpp
//  Underworld_Client
//
//  Created by wenchengye on 16/7/30.
//
//

#include "HMMCardRender.h"
#include "HMMCardView.h"
#include "GameModeHMM.h"
#include "Faction.h"

namespace UnderWorld{ namespace Core{

HMMCardRender::HMMCardRender()
: _cardview(nullptr) {
}
    
HMMCardRender::~HMMCardRender() {
    CC_SAFE_RELEASE(_cardview);
}

void HMMCardRender::init(const HMMCard* card) {
    if (!_cardview) {
        _cardview = HMMCardView::create(card ? card->getCardType() : nullptr);
        if (_cardview) _cardview->retain();
    } else {
        _cardview->init(card ? card->getCardType() : nullptr);
    }
    
    render(card);
}
    
void HMMCardRender::render(const HMMCard *card) {
    if (!_cardview || !card) return;
    
    _cardview->setCardUseable(card->isCardReady());
    
    if (card->getFaction()) {
        const Resource* r = card->getFaction()->getResource(HMMCardView::DISPLAY_RESOURCE_KEY);
        if (r) _cardview->setResourceEnough(r->getBalanceMicro());
    }
    
    int progress = card->getRecoverSpan() == 0 ? 100 : card->getRecoverProgress() * 100 / card->getRecoverSpan();
    _cardview->setColdDownProgress(progress);
}
    
    
}}