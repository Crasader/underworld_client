//
//  BaseCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BaseCard_h
#define BaseCard_h

#include "AbstractCard.h"

class BaseCard;

class BaseCardObserver
{
public:
    virtual ~BaseCardObserver() {}
    virtual void onBaseCardClicked(BaseCard* pSender) {}
    virtual void onBaseCardInfo(BaseCard* pSender) {}
    virtual void onBaseCardClickedResourceButton(BaseCard* pSender) {}
};

class BaseCard : public AbstractCard
{
public:
    static constexpr float Width = 82;
    static constexpr float Height = 104;
    static BaseCard* create();
    virtual ~BaseCard();
    void registerObserver(BaseCardObserver *observer);
    
    void shake();
    void stopShake();
    void move(const Point& point, const std::function<void()>& callback);
    
protected:
    BaseCard();
    virtual bool init() override;
    virtual void updateProperty(const AbstractProperty* property) override;
    virtual void updateData(const AbstractData* data) override;
    void resetPositions(const Point& offset);
    
protected:
    BaseCardObserver* _observer;
    Node* _costNode;
    Label* _cost;
    ui::Button* _infoButton;
    bool _touchInvalid;
    Point _originalPoint;
};

#endif /* BaseCard_h */
