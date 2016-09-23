//
//  ShopManager.h
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ShopManager_h
#define ShopManager_h

#include "json/document.h"
#include <vector>
#include <functional>

class CommodityData;

class ShopManager
{
public:
    static ShopManager* getInstance();
    static void purge();
    
    // network request
    void fetchCardList(const std::function<void()>& callback);
    void buyCard(int cardId, const std::function<void(int cardId)>& callback);
    
    // data
    const std::vector<CommodityData*>& getCardList() const;
    
private:
    ShopManager();
    virtual ~ShopManager();
    ShopManager(const ShopManager &) = delete;
    ShopManager &operator =(const ShopManager &) = delete;
    
private:
    std::vector<CommodityData*> _cardList;
};

#endif /* ShopManager_h */
