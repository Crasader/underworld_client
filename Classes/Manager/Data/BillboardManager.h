//
//  BillboardManager.h
//  Underworld_Client
//
//  Created by Andy on 16/9/13.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BillboardManager_h
#define BillboardManager_h

#include "json/document.h"
#include <vector>
#include <functional>

class BillboardData;

class BillboardManager
{
public:
    static BillboardManager* getInstance();
    static void purge();
    
    //------------------------- NETWORKING REQUEST -------------------------//
    void fetchBillboard(const std::function<void()>& callback);
    
    // data
    const std::vector<BillboardData*>& getBillboard() const;
    
private:
    BillboardManager();
    virtual ~BillboardManager();
    BillboardManager(const BillboardManager &) = delete;
    BillboardManager &operator =(const BillboardManager &) = delete;
    
private:
    std::vector<BillboardData*> _list;
};

#endif /* BillboardManager_h */
