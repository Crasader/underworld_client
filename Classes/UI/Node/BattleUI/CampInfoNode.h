//
//  CampInfoNode.h
//  Underworld_Client
//
//  Created by Andy on 16/1/19.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CampInfoNode_h
#define CampInfoNode_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"
#include "DisplayIconNode.h"
#include "UnitInfoNode.h"

USING_NS_CC;
USING_NS_CC_EXT;

namespace UnderWorld { namespace Core {
    class Camp;
    class UnitBase;
} }

class CampInfoNode;

class CampInfoNodeObserver
{
public:
    virtual ~CampInfoNodeObserver() {}
    virtual void onCampInfoNodeClickedIcon(CampInfoNode* pSender, const UnderWorld::Core::UnitBase* unit) = 0;
};

class CampInfoNode : public Node, public DisplayIconNodeObserver, public UnitInfoNodeObserver
{
public:
    static CampInfoNode* create(bool scrollToLeft);
    virtual ~CampInfoNode();
    virtual void setPosition(const Point& position) override;
    
    void registerObserver(CampInfoNodeObserver *observer);
#if ENABLE_CAMP_INFO
    void insert(const std::vector<std::pair<const UnderWorld::Core::Camp*, const UnderWorld::Core::UnitBase*>>& units);
#else
    void insert(const std::vector<const UnderWorld::Core::UnitBase*>& units);
#endif
    void update();
    void closeUnitInfoNode();
    Rect getIconsBoundingBox();
    
protected:
    CampInfoNode();
    bool init(bool scrollToLeft);
    std::string getButtonFile(bool isFold);
    void setFold(bool fold, bool animated);
    void resetButtonPosition();
    
    // DisplayIconNodeObserver
    virtual void onDisplayIconNodeTouchedEnded(const UnderWorld::Core::UnitBase* unit) override;
    virtual void onDisplayIconNodeChangedContentSize(const Size& lastSize, const Size& newSize) override;
    
    // UnitInfoNodeObserver
    
private:
    CampInfoNodeObserver* _observer;
    DisplayIconNode* _displayIconNode;
    UnitInfoNode* _unitInfoNode;
    ui::Button* _button;
    bool _isScrollToLeft;
    bool _isFold;
    Point _basePosition;
};

#endif /* CampInfoNode_h */
