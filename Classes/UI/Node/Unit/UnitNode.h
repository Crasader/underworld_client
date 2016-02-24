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
#include "Unit.h"
#include "SpellConfigData.h"

USING_NS_CC;
using namespace ui;

namespace UnderWorld { namespace Core {
    class Skill;
} }

class UnitNode;
class DisplayBar;
class URConfigData;

class UnitNodeObserver
{
public:
    virtual ~UnitNodeObserver() {}
    virtual void onUnitNodeUpdatedFeatures(int unitId) = 0;
    virtual void onUnitNodePlayDeadAnimationFinished(int unitId) = 0;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) = 0;
    virtual void onUnitNodeShakeScreen(UnitNode* node) = 0;
};

class UnitNode : public Node
{
public:
    static UnitNode* create(const UnderWorld::Core::Unit* unit, bool rightSide);
    virtual ~UnitNode();
    const UnderWorld::Core::Unit* getUnit() const;
    void registerObserver(UnitNodeObserver *observer);
    void update();
    
    void addSpellRing(int range);
    void removeSpellRing();
    
    // --------------- callbacks ---------------
    void onHurt(const std::string& trigger);
    void onDead();
    void onWin();
    void onLose();
    
protected:
    UnitNode(const UnderWorld::Core::Unit* unit, bool rightSide);
    virtual bool init() override;
    virtual void setOpacity(GLubyte opacity) override;
    virtual GLubyte getOpacity() const override;
    void getCsbFiles(std::vector<std::string>& output, UnderWorld::Core::Unit::Direction direction, bool isHealthy);
    const std::string getStandbyCsbFile(UnderWorld::Core::Unit::Direction direction, bool isHealthy);
    void getAttackCsbFiles(std::vector<std::string>& output, UnderWorld::Core::Unit::Direction direction, bool isHealthy);
    bool needToChangeStandbyStatus();
    bool needToFlip(UnderWorld::Core::Unit::Direction direction);
    float calculateHpPercentage();
    void addActionNode(const std::string& file, bool play, bool loop, float playTime, int frameIndex, bool flip, const std::function<void()>& lastFrameCallFunc);
    // standby
    void addStandbyActionNode();
    // attack
    void addNextAttackActionNode(float playTime, int frameIndex);
    void onAttackAnimationFinished();
    void reset();
    // public
    void updateActionNode(const UnderWorld::Core::Skill* skill, int frameIndex, bool flip);
    void removeActionNode();
    void scheduleSpeed();
    void scaleActionNode();
    // --------------- effects ---------------
    void updateBufs();
    void addBuf(const std::string& name);
    void removeBuf(const std::string& name);
    void removeAllBufs();
    void updateFeatures();
    void addHPBar();
    void updateHPBar();
    void removeHPBar();
    Point getHPBarPosition() const;
    void addShadow();
    void removeShadow();
    void addSwordEffect();
    Node* addEffect(const std::string& file);
    Node* addEffect(const std::string& file, const SpellConfigData::SpellDirection& direction,  const SpellConfigData::SpellPosition& position, bool scale, bool loop, const std::function<void()>& callback);
    void rollHintResource(const std::string& resource, float amount, float delay = 0.f);
    void rollHintNode(Node* hintNode, float delay = 0.f);
    
private:
    // --------------- base ---------------
    UnitNodeObserver *_observer;
    const UnderWorld::Core::Unit* _unit;
    std::string _unitName;
    bool _isBuilding;
    const URConfigData* _configData;
    bool _needToFlip;
    // --------------- animation ---------------
    Node *_actionNode;
    cocostudio::timeline::ActionTimeline *_currentAction;
    Scheduler *_speedScheduler;
    ActionManager *_actionManager;
    Node *_shadow;
    Node *_spellRing;
    std::set<std::string> _bufNames;
    std::map<std::string, Node*> _bufs;
    DisplayBar* _hpBar;
    Sprite *_sprite;
    Label *_idLabel;
    // save last status
    const UnderWorld::Core::Skill* _lastSkill;
    UnderWorld::Core::Unit::Direction _lastDirection;
    bool _isLastHealthy;
    bool _isLastFlipped;
    bool _isStandby;
    std::vector<std::string> _animationFiles;
    // only used for attack animations
    bool _isPlayingAttackAnimation;
    int _animationCounter;
    int _rollHintCounter;
    float _baseSpeed;
    float _baseScale;
    float _extraCasterScale;
    std::map<UnderWorld::Core::SkillClass, float> _extraBufSpeeds;
    std::map<UnderWorld::Core::SkillClass, float> _extraBufScales;
};

#endif /* UnitNode_h */
