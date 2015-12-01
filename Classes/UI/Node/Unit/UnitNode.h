//
//  UnitNode.h
//  Underworld_Client
//
//  Created by Andy on 15/10/29.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef UnitNode_h
#define UnitNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Unit;
    class Skill;
} }

class UnitNode;
class DisplayBar;

class UnitNodeObserver
{
public:
    virtual ~UnitNodeObserver() {}
    virtual void onUnitNodePlayDeadAnimationFinished(UnitNode* node) = 0;
    virtual void onUnitNodeFootmanAttackedTheTarget(UnitNode* node) = 0;
};

class UnitNode : public Node
{
public:
    static UnitNode* create(const UnderWorld::Core::Unit* unit);
    virtual ~UnitNode();
    const UnderWorld::Core::Unit* getUnit() const;
    void registerObserver(UnitNodeObserver *observer);
    void update();
    void addCritEffect();
    void addBlockEffect();
    void addRecoveryEffect();
    void addBuf();
    void removeBuf();
    
protected:
    typedef enum {
        kUnitDirection_LeftDown = 2,
        kUnitDirection_Left,
        kUnitDirection_LeftUp,
    }UnitDirection;
    
protected:
    UnitNode();
    bool init(const UnderWorld::Core::Unit* unit);
    const std::string getCsbFile(const UnderWorld::Core::Unit* unit, UnitDirection direction, float hpPercentage, bool& flip);
    UnitDirection calculateDirection(const UnderWorld::Core::Unit* unit);
    void updateActionNode(const UnderWorld::Core::Unit* unit, UnitDirection direction, float hpPercentage, int currentFrame);
    void updateHPBar();
    void removeHPBar();
    void addShadow();
    void removeShadow();
    Node* addEffect(const std::string& file);
    
private:
    UnitNodeObserver *_observer;
    Node *_actionNode;
    cocostudio::timeline::ActionTimeline *_currentAction;
    Node *_shadow;
    Node *_buf;
    DisplayBar* _hpBar;
    Sprite *_sprite;
    const UnderWorld::Core::Unit* _unit;
    const UnderWorld::Core::Skill* _lastSkill;
    UnitDirection _lastDirection;
    float _lastHpPercentage;
    int _switchAnimationCounter;
};

#endif /* UnitNode_h */
