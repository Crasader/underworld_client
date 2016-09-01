//
//  CardUpgradeProperty.h
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef CardUpgradeProperty_h
#define CardUpgradeProperty_h

#include "AbstractUpgradeProperty.h"

class CardUpgradeProperty : public AbstractUpgradeProperty
{
public:
    CardUpgradeProperty(tinyxml2::XMLElement *xmlElement);
    virtual ~CardUpgradeProperty();
    
    int getCardCost() const;
    
private:
    int _cardCost;
};

#endif /* CardUpgradeProperty_h */
