//
//  CardUpgradeData.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardUpgradeData_h
#define CardUpgradeData_h

namespace tinyxml2 { class XMLElement; }

class CardUpgradeData
{
public:
    CardUpgradeData(tinyxml2::XMLElement *xmlElement);
    virtual ~CardUpgradeData();
    
    int getId() const;
    int getLevel() const;
    int getGold() const;
    int getCard() const;
    
private:
    int _id;
    int _level;
    int _gold;
    int _card;
};

#endif /* CardUpgradeData_h */
