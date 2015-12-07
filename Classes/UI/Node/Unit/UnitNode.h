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
#include "CocosGlobal.h"

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
    void addCritEffect(const std::string& triggerName);
    void addBlockEffect();
    void addRecoveryEffect();
    void addSwordEffect();
    void addBuf();
    void removeBuf();
    
protected:
    UnitNode();
    bool init(const UnderWorld::Core::Unit* unit);
    const std::string getCsbFile(UnitDirection direction, float hpPercentage);
    void getMultipleAnimationFiles(std::vector<std::string>& output);
    bool checkIsStandby(const UnderWorld::Core::Skill* skill);
    UnitDirection calculateDirection();
    void addActionNode(const std::string& file, bool play, bool loop, float playTime, int frameIndex, const std::function<void()>& lastFrameCallFunc);
    void addMultipleAnimationNode(int frameIndex, const std::function<void()>& lastFrameCallFunc);
    void updateActionNode(const UnderWorld::Core::Skill* skill, const std::string& file, int currentFrame, bool flip);
    void removeActionNode();
    void addHPBar();
    void updateHPBar();
    void removeHPBar();
    void addShadow();
    void removeShadow();
    Node* addEffect(const std::string& file);
    void getActionParameters(float& actionNodeScale, float& animationSpeed) const;
    
private:
    UnitNodeObserver *_observer;
    Node *_actionNode;
    cocostudio::timeline::ActionTimeline *_currentAction;
    Scheduler *_speedScheduler;
    ActionManager *_actionManager;
    Node *_shadow;
    Node *_buf;
    DisplayBar* _hpBar;
    Sprite *_sprite;
    const UnderWorld::Core::Unit* _unit;
    const UnderWorld::Core::Skill* _lastSkill;
    UnitDirection _lastDirection;
    float _lastHpPercentage;
    int _switchAnimationCounter;
    bool _isStandby;
    std::vector<std::string> _multipleAnimationFiles;
};

#endif /* UnitNode_h */
