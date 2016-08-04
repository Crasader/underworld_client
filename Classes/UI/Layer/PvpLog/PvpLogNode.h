//
//  PvpLogNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/1.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PvpLogNode_h
#define PvpLogNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class PvpLogData;
class UserSimpleData;
class CardSimpleData;
class PvpResultNode;
class TrophyGapNode;

class PvpLogNodeObserver
{
public:
    virtual ~PvpLogNodeObserver() {}
    virtual void onPvpLogNodeReplay(const PvpLogData* data) = 0;
    virtual void onPvpLogNodeShare(const PvpLogData* data) = 0;
    virtual void onPvpLogNodeExpand(Node* sender, bool expanded) = 0;
};

class PvpLogNode: public Node
{
public:
    static constexpr float Width = 975;
    static constexpr float FoldedHeight = 131;
    static constexpr float ExpandedHeight = 278;
    
    static PvpLogNode* create(const PvpLogData* data, bool expand);
    virtual ~PvpLogNode();
    void registerObserver(PvpLogNodeObserver *observer);
    void update(const PvpLogData* data, bool expand);
    void show();
    
protected:
    struct UserInfo;
    
    PvpLogNode();
    bool init(const PvpLogData* data, bool expand);
    void createTopNode();
    void createBottomNode();
    void removeBottomNode();
    void adjust();
    
private:
    PvpLogNodeObserver* _observer;
    bool _isExpanded;
    const PvpLogData* _data;
    ui::Scale9Sprite* _background;
    
    PvpResultNode* _result;
    TrophyGapNode* _trophyGap;
    Label* _time;
    std::unordered_map<bool, UserInfo*> _userInfos;
    ui::Button* _expandButton;
    Node* _top;
    Node* _bottom;
};

#endif /* PvpLogNode_h */
