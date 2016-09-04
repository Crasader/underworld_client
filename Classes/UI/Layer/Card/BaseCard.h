//
//  BaseCard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/11.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BaseCard_h
#define BaseCard_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class AbstractData;
class AbstractProperty;
class BaseCard;

class BaseCardObserver
{
public:
    virtual ~BaseCardObserver() {}
    virtual void onBaseCardTouched(BaseCard* pSender, ui::Widget::TouchEventType type) {}
    virtual void onBaseCardClicked(BaseCard* pSender) {}
    virtual void onBaseCardInfo(BaseCard* pSender) {}
    virtual void onBaseCardUpgrade(BaseCard* pSender) {}
};

class BaseCard : public ui::Widget
{
public:
    static constexpr float Width = 82;
    static constexpr float Height = 104;
    static BaseCard* create();
    virtual ~BaseCard();
    void registerObserver(BaseCardObserver *observer);
    
    bool update(int cardId, const AbstractData* data);
    int getCardId() const;
    const AbstractData* getCardData() const;
    
    void shake();
    void stopShake();
    void move(const Point& point, const std::function<void()>& callback);
    
protected:
    BaseCard();
    virtual bool init() override;
    virtual void updateProperty(const AbstractProperty* property);
    virtual void updateData(const AbstractData* data);
    void resetPositions(const Point& offset);
    
protected:
    BaseCardObserver* _observer;
    Sprite* _icon;
    Node* _costNode;
    Label* _cost;
    Label* _level;
    Sprite* _qualityBox;
    ui::Button* _infoButton;
    int _cardId;
    const AbstractData* _data;
    bool _touchInvalid;
    Point _originalPoint;
};

#endif /* BaseCard_h */
