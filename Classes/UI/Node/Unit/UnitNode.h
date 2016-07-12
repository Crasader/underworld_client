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
class AnimationParameter;
class URConfigData;

USING_NS_CC;
using namespace UnderWorld::Core;

// Observer
class UnitNode;
class UnitNodeObserver
{
public:
    virtual ~UnitNodeObserver() {}
    virtual void onUnitNodeCreateShadow(UnitNode* node, Node* shadow, const Point& offset) = 0;
    virtual void onUnitNodeRemoveShadow(UnitNode* node) = 0;
    virtual void onUnitNodePlayedDeadAnimation(int unitId) = 0;
    virtual void onUnitNodeHurtTheTarget(UnitNode* node) = 0;
    virtual void onUnitNodeShakeScreen(UnitNode* node) = 0;
};

class UnitNode : public Node
{
public:
    class AnimationType {
    public:
        AnimationType() {}
        void assign(const AnimationType& instance) {
            _file.assign(instance._file);
            _startIdx = instance._startIdx;
            _endIdx = instance._endIdx;
        }
        void assign(const std::string& file, int startIdx = 0, int endIdx = -1) {
            _file.assign(file); _startIdx = startIdx; _endIdx = endIdx;
        }
        const std::string& getFile() const { return _file; }
        int getStartIdx() const { return _startIdx; }
        int getEndIdx() const { return _endIdx; }
        
    private:
        std::string _file;
        int _startIdx;
        int _endIdx;
    };
    
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
    void getAnimationFiles(std::vector<AnimationType>& output,
                           SkillClass sc,
                           Unit::Direction direction,
                           bool isHealthy) const;
    void getStandbyFiles(AnimationType& output,
                         Unit::Direction direction,
                         bool isHealthy) const;
    void getSegmentalFiles(std::vector<AnimationType>& output,
                           const std::string& mark,
                           Unit::Direction direction,
                           bool isHealthy) const;
    int getCurrentFrameIndex() const;
    
    // animations
    void playAnimation(const AnimationType& at,
                       bool play,
                       bool loop,
                       float playTime,
                       int frameIndex,
                       bool flip,
                       const std::function<void(Node*)>& lastFrameCallFunc);
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
    void setScheduler(Node* node);
    void removeNode();
    void updateAnimationParams();
    
    // effects
    Node* addEffect(const std::string& file);
    Node* addAnimation(const std::string& file,
                       bool loop,
                       const std::function<void()>& callback,
                       bool scale,
                       const SpellConfigData::Direction& direction,
                       const SpellConfigData::Position& position);
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
    void createShadow(const std::string& file, bool flip);
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
    Action* _animation;
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
    std::vector<AnimationType> _animationFiles;
    
    // attack animations
    bool _isAttacking;
    int _attackAnimationsCounter;
    
    // animation params
    AnimationParameter* _baseParams;
    std::map<SkillClass, AnimationParameter*> _extraParams;
    int _rollHintCounter;
};

#endif /* UnitNode_h */
