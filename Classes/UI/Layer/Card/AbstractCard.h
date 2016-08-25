//
//  AbstractCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/24.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AbstractCard_h
#define AbstractCard_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class AbstractCard : public cocos2d::ui::Widget
{
public:
    virtual ~AbstractCard() = 0;
    virtual int getCardId() const;
    void move(const cocos2d::Point& point, const std::function<void()>& callback);
    
protected:
    AbstractCard();
    virtual bool init() override;
};

#endif /* AbstractCard_h */
