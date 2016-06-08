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
#include "CocosGlobal.h"
#include "Unit.h"
#include "SpellConfigData.h"

// UnderWorld::Core
namespace UnderWorld { namespace Core {
    class Game;
    class Skill;
}}

// Global
class DisplayBar;
class URConfigData;

USING_NS_CC;
using namespace UnderWorld::Core;

// Observer
class UnitNode;
class UnitNodeObserver
{
public:
    virtual ~UnitNodeObserver() {}
    virtual void onUnitNodePlayedDeadAnimation(int unitId) = 0;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) = 0;
    virtual void onUnitNodeShakeScreen(UnitNode* node) = 0;
};

#if USING_PVR
typedef Animate* AnimationType;
typedef std::vector<std::string> AnimationFileType;
#else
#include "cocostudio/CocoStudio.h"
typedef cocostudio::timeline::ActionTimeline* AnimationType;
typedef std::string AnimationFileType;
#endif

class UnitNode : public Node
{
public:
    static UnitNode* create(const Unit* unit, bool rightSide);
    virtual ~UnitNode();
    
    void registerObserver(UnitNodeObserver *observer);
    const Unit* getUnit() const;
    void update(const Game* game);
    
    // callbacks
    void onHurt(const std::string& trigger);
    void onDead();
    void onWin();
    void onLose();
    
protected:
    UnitNode(const Unit* unit, bool rightSide);
    
    // inherited from "Node"
    virtual bool init() override;
    virtual void update(float dt) override;
    virtual void setOpacity(GLubyte opacity) override;
    virtual GLubyte getOpacity() const override;
    
    // universal
    void flipX(Node* node) const;
    void scale(Node* node, float scale) const;
    void scale(Node* node, float scaleX, float scaleY) const;
    void playSound(const std::string& file) const;
    
    // getters
    int getResourceId(Unit::Direction direction) const;
    int thisFactionIndex() const;
    UnitClass thisUnitClass() const;
    SkillClass thisSkillClass() const;
    SkillClass getSkillClass(const Skill* skill) const;
    float getHpPercentage() const;
    bool needToChangeStandbyStatus() const;
    bool needToFlip(Unit::Direction direction) const;
    void getAnimationFiles(std::vector<AnimationFileType>& output,
                           SkillClass sc,
                           Unit::Direction direction,
                           bool isHealthy) const;
    void getStandbyFiles(AnimationFileType& output,
                         Unit::Direction direction,
                         bool isHealthy) const;
    void getAttackFiles(std::vector<AnimationFileType>& output,
                        Unit::Direction direction,
                        bool isHealthy) const;
    int getCurrentFrameIndex() const;
    float getAnimationDuration() const;
    
    // animations
    void playAnimation(const AnimationFileType& files,
                       bool play,
                       bool loop,
                       float playTime,
                       int frameIndex,
                       bool flip,
                       const std::function<void()>& lastFrameCallFunc);
    void updateAnimation(const Skill* skill,
                         int frameIndex,
                         bool flip,
                         const Game* game);
    
    // standby
    void playStandbyAnimation();
    
    // attack
    void playNextAttackAnimation(float playTime, int frameIndex);
    void onAttackAnimationFinished();
    void resetAttackParams();
    
    // operations
    void removeNode();
    void scheduleSpeed();
    void scaleNode();
    
    // effects
    Node* addEffect(const std::string& file);
    Node* addCSBAnimation(const std::string& file,
                          bool loop,
                          const std::function<void()>& callback,
                          bool scale,
                          const SpellConfigData::Direction& direction,
                          const SpellConfigData::Position& position);
#if USING_PVR
    Node* addEffect(const std::string& folder, int framesCount);
    Node* addPVRAnimation(const std::string& folder,
                          int framesCount,
                          bool loop,
                          float frameDelay,
                          const std::function<void()>& callback,
                          bool scale,
                          const SpellConfigData::Direction& direction,
                          const SpellConfigData::Position& position);
#endif
    void adjustEffect(Node* effect,
                      bool scale,
                      const SpellConfigData::Direction& direction,
                      const SpellConfigData::Position& position);
    void addSwordEffect();
    
    // bufs
    void addBuf(const std::string& name);
    void removeBuf(const std::string& name);
    void removeAllBufs();
    void updateBufs();
    void updateFeatures(const Game* game);
    
    // hp bar
    void addHPBar();
    void updateHPBar();
    void removeHPBar();
    Point getHPBarPosition() const;
    
    // shadow
    void addShadow();
    void removeShadow();
    
    // hint
    void rollHintResource(const std::string& resource,
                          float amount,
                          float delay = 0.f);
    void rollHintNode(Node* hintNode, float delay = 0.f);
    
private:
    // base
    UnitNodeObserver *_observer;
    const Unit* _unit;
    std::string _unitName;
    const URConfigData* _configData;
    bool _needToFlip;
    bool _isBuilding;
    
    // cocos2d
    Node* _node;
    Sprite* _sprite;
    AnimationType _animation;
    Scheduler* _speedScheduler;
    ActionManager* _actionManager;
    DisplayBar* _hpBar;
    Node* _shadow;
    std::set<std::string> _bufNames;
    std::unordered_map<std::string, Node*> _bufs;
    Label* _idLabel;
    
    // status
    const Skill* _skill;
    Unit::Direction _direction;
    bool _isHealthy;
    bool _isFlipped;
    bool _isStandby;
    
    // animations
    std::vector<AnimationFileType> _animationFiles;
    
    // attack animations
    bool _isAttacking;
    int _attackAnimationsCounter;
    
    // animation params
    float _baseSpeed;
    float _baseScale;
    float _extraCasterScale;
    std::map<SkillClass, float> _extraBufSpeeds;
    std::map<SkillClass, float> _extraBufScales;
    int _rollHintCounter;
};

#endif /* UnitNode_h */
