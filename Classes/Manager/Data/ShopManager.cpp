//
//  ShopManager.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "ShopManager.h"
#include "JSonUtils.h"
#include "CommodityData.h"

using namespace std;

static ShopManager* s_pInstance(nullptr);
ShopManager* ShopManager::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) ShopManager();
    }
    
    return s_pInstance;
}

void ShopManager::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

ShopManager::ShopManager() {}

ShopManager::~ShopManager()
{
    for (auto data : _cardList) {
        CC_SAFE_DELETE(data);
    }
}

#pragma mark - public
#pragma mark - network request
void ShopManager::fetchCardList(const function<void()>& callback)
{
    if (_cardList.empty()) {
        static const vector<int> test {23100, 23101, 23102, 23104, 23106, 23108, 23109, 23110, 23115, 22000, 22001,
            22002, 22003, 22004, 22005, 22006, 22007, 22008, 22009, 22010, 22011, 22012,
            22013, 22014, 22015, 22016, 22023, 21100, 21101, 21102, 21103, 21104, 21105,
            21106, 21108, 21109, 21110, 21111, 21112, 21113, 21114, 21115, 21116, 21117,
            21118, 21119, 21120, 21121, 21122, 22020, 22021, 22022, 22023, 22024, 22025,
            22026, 22027, 22028, 22029, 22025, 22026, 22028, 23103, 23105, 23107, 23111,
            23112, 23113, 23114, 22017, 22018, 22019, 22025, 22026, 22028, 21107,};
        
        for (int i = 0; i < test.size(); ++i) {
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
            document.AddMember("db", 0, allocator);
            document.AddMember("id", test.at(i), allocator);
            document.AddMember("amount", 1, allocator);
            do {
                rapidjson::Value resources(rapidjson::kArrayType);
                rapidjson::Value object(rapidjson::kObjectType);
                object.AddMember("id", 101, allocator);
                object.AddMember("amount", i + 1, allocator);
                resources.PushBack(object, allocator);
                document.AddMember("resources", resources, allocator);
            } while (false);
            
            _cardList.push_back(new (nothrow) CommodityData(document));
        }
    }
    
    if (callback) {
        callback();
    }
}

#pragma mark - data
const vector<CommodityData*>& ShopManager::getCardList() const
{
    return _cardList;
}
