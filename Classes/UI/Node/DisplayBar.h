//
//  DisplayBar.h
//  Underworld_Client
//
//  Created by Andy on 15/11/25.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef DisplayBar_h
#define DisplayBar_h

#include "cocos2d.h"
#include "UnitType.h"

USING_NS_CC;

typedef enum
{
    kHP,
    kMP
}DisplayBarType;

class DisplayBar : public Node
{
public:
    static DisplayBar* create(DisplayBarType type, UnderWorld::Core::UnitClass unitClass);
    virtual ~DisplayBar();
    void setPercentage(float percentage);
    float getPercentage() const;
    
protected:
    DisplayBar();
    bool init(DisplayBarType type, UnderWorld::Core::UnitClass unitClass);
    void getFiles(bool lowPercentage, std::string& bgFile, std::string& ptFile) const;
    
private:
    ProgressTimer* _pt;
    DisplayBarType _type;
    UnderWorld::Core::UnitClass _unitClass;
};

#endif /* DisplayBar_h */
