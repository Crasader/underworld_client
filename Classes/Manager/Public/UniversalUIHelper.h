//
//  UniversalUIHelper.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FadeMessageManager_h
#define FadeMessageManager_h

#include "cocos2d.h"

USING_NS_CC;

class AbstractData;
class CardInfoUIObserver;

class UniversalUIHelper
{
public:
    static UniversalUIHelper* getInstance();
    static void purge();
    
    Node* showMessage(const std::string& message);
    Node* showCardInfoLayer(Node* parent, int cardId, const AbstractData* data);
    
private:
    UniversalUIHelper();
    virtual ~UniversalUIHelper();
    UniversalUIHelper(const UniversalUIHelper &) = delete;
    UniversalUIHelper &operator =(const UniversalUIHelper &) = delete;
    
private:
    Point _messageStartPoint;
    Action* _messageAction;
    std::queue<Label*> _labels;
    CardInfoUIObserver* _cardInfoUIObserver;
};

#endif /* FadeMessageManager_h */
