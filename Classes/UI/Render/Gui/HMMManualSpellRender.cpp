//
//  HMMManualSpellRender.cpp
//  Underworld_Client
//
//  Created by Mac on 16/9/13.
//
//

#include "HMMManualSpellRender.h"
#include "HMMManualSpellView.h"
#include "Spell.h"

namespace UnderWorld{ namespace Core{
    
HMMManualSpellRender::HMMManualSpellRender()
: _spellView(nullptr) {
}
    
HMMManualSpellRender::~HMMManualSpellRender() {
    CC_SAFE_RELEASE(_spellView);
}
    
void HMMManualSpellRender::init(const Spell *spell) {
    if (!_spellView) {
        _spellView = HMMManualSpellView::create(spell);
        if (_spellView) _spellView->retain();
    } else {
        _spellView->init(spell);
    }
    
    render(spell);
}

void HMMManualSpellRender::render(const Spell *spell) {
    if (!_spellView || !spell) return;
    
    _spellView->setSpellUseable(spell->isReady());
    
    int progress = spell->getCDProgress() == 0 ? 0 : spell->getCDProgress() * 100 / spell->getTotalCDFrames();
    _spellView->setColdDownProgress(progress);
}

}}
