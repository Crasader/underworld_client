//
//  UnitRender.h
//  Underworld_Client
//
//  Created by Mac on 16/7/18.
//
//

#ifndef UnitRender_h
#define UnitRender_h

#include <functional>
#include <unordered_map>
#include <queue>
#include "cocos2d.h"

#include "WorldObjectRender.h"
#include "Unit.h"
#include "GameConstants.h"
#include "UnitView.h"

class URConfigData;
class DisplayBar;
class MoveSkill;
class MoveSkillType;

namespace UnderWorld{ namespace Core{
    
class BodyAnimationPoseBundle {
private:
    //TODO:comment
    UnitAnimationPose _pose;
    bool _specificScale;
    
    float _playDuration;
    float _playBeginTime;
    
    float _playDurationScale;
    
public:
    BodyAnimationPoseBundle() :
    _pose(UnitAnimationPose::None),
    _playDuration(0.f),
    _playBeginTime(0.f),
    _playDurationScale(1.f),
    _specificScale(true) {}
    
    UnitAnimationPose getPose() const        {return _pose;}
    bool isSpecificScale() const             {return _specificScale;}
    float getPlayDuration() const            {return _playDuration;}
    float getPlayBeginTime() const           {return _playBeginTime;}
    float getPlayDurationScale() const       {return _playDurationScale;}
    
    void setPose(UnitAnimationPose pose)     {_pose = pose;}
    void specificDurationAndBeginTime(float duration, float beginTime);
    void specificDurationScale(float scale);
};
    
class BuffAnimation {
private:
    int _contributors;
    cocos2d::Node* _node;
    
public:
    BuffAnimation() : _contributors(0), _node(nullptr) {}
    
    /** getters */
    int getContributors() const                     {return _contributors;}
    cocos2d::Node* getNode()                        {return _node;}
    
    /** setters */
    void increaseContributors();
    void descreaseContributors();
    void setNode(cocos2d::Node* node)               {_node = node;}

};


// ===============================
// 	class UnitRender
//
//	render unit with animation
// ===============================

class UnitRender : public AbstractWorldObjectRender, public UnitObserver {
public:
    
    /** zorder */
    static const int IN_MAIN_FOOT_EFFECT_BACKGROUND_ZORDER;
    static const int IN_MAIN_FOOT_EFFECT_FOREGROUND_ZORDER;
    static const int IN_MAIN_BODY_EFFECT_BACKGROUND_ZORDER;
    static const int IN_MAIN_BODY_NODE_ZORDER;
    static const int IN_MAIN_HP_BAR_ZORDER;
    static const int IN_MAIN_BODY_EFFECT_FOREGROUND_ZORDER;
    
    /** consts */
    static const float HEALTHY_HP_THRESHOLD;
    static const int MAX_HP_PERCENT;
    static const std::string HEAL_EFFECT_RENDER_KEY;
    static const std::string ROLL_SCHEDULE_KEY_PREFIX;
    static const float ROLL_NEXT_DELAY_IN_SECOND;
    static const float DUARTION_SCALE_MAX;
    static const float INIT_ANIM_DURATION;
    static const float INIT_MOVE_DOWN_OFFSET;
    static const float INIT_SCALE_Y_OFFSET;
    static const float INIT_SCALE_X_OFFSET;
    
private:
    
    class UnitStatus {
    public:
        /** unit's positoin */
        Coordinate32 _position;
        
        /** unit's current field */
        FieldType _field;
        
        /** unit's direction */
        Unit::Direction _direction;
        
        /** unit's hp */
        int _hp;
        
        /** unit's hp percent, [0 - 100] */
        int _hpPercent;
        
        /** unit's skill */
        const Skill* _skill;
        
        /** whether the unit is dead */
        bool _isAlive;
        
        /** whether the unit's hp is above healthy hp */
        bool _isHealthy;
        
        /** unit's faction index */
        int _factionIndex;
        
    public:
        UnitStatus() : _position(Coordinate32::ZERO), _direction(Unit::DIRECTION_COUNT), _hp(0), _skill(nullptr), _isAlive(true), _isHealthy(false) {}
        
        void reset();
    };
    
private:
    /** data */
    const Unit* _unit;
    const UnitType* _unitType;
    const URConfigData* _configData;
    std::string _unitRenderKey;
    
    /** status */
    UnitStatus _status;
    UnitStatus _lastStatus;
    std::unordered_map<creatureid_t, std::string> _renderBufs;     //bufId vs renderKey
    std::unordered_map<std::string, BuffAnimation> _bufAnimations; //renderKey vs animation obj
    bool _inited;
    
    /** events*/
    std::vector<Unit::EventLog> _events;
    std::unordered_map<creatureid_t, int> _usefulAddBuffLogs;
    std::unordered_map<creatureid_t, int> _usefuleliminateBuffLogs;
    std::unordered_map<creatureid_t, int> _usefulHoldAuraLogs;
    std::unordered_map<creatureid_t, int> _usefulUnholdAuraLogs;
    std::vector<int> _featureLogs;
    
    /** cocos */
    cocos2d::Node* _mainNode;
    cocos2d::Node* _bodyNode;
    cocos2d::Node* _groundNode;
    UnitView* _unitView;
    DisplayBar* _hpBar;
    std::queue<cocos2d::Node*> _rollNodes;
    std::string _rollScheduleKey;
    
    /** refs*/
    WorldRender* _worldRender;
    
public:
    /** init */
    UnitRender();
    virtual ~UnitRender();
    virtual bool init(const WorldObject* object, WorldRender* worldRender) override;
    
    /** override WorldObjectRender */
    virtual void render() override;
    virtual void attach2WorldRender(WorldRender* worldRender) override;
    virtual void dettachFromWorldRender(WorldRender* worldRender) override;
    
    /** override UnitObserver */
    virtual void onNotifyUnitEvents(const std::vector<Unit::EventLog>& events) override;
    
    /** interface */
    cocos2d::Node* addEffect(const std::string& renderKey, bool loop);
    
private:
    /** sub render */
    void handleEvents();
    void updateStatus();
    void renderPosition(bool init = false);
    void renderHp(bool init = false);
    void renderSkill();
    void renderBuffAndAura();
    void renderEffects();
    void renderInit();
    void initRenderSkill();
    void initBuffAndAura();
    
    /** body animation */
    void getCurrentPoseBundle(BodyAnimationPoseBundle& output);
    //output._pose is BodyAnimationPose::None when need not rebuild, else ouput the target pose
    void needRebuildBodyAnim(const BodyAnimationPoseBundle& curentPoseBundle,
        BodyAnimationPoseBundle& output) const;
    void buildAndPlayBodyAnimWithCurrentStatus(BodyAnimationPoseBundle& poseBundle);
    void bodyAnimationCallback();
    static float getMoveAnimationDurationScale(const MoveSkill* skill, const MoveSkillType* skillType, const URConfigData* configData);
    static float getStandAnimationDurationScale(const URConfigData* configData);
    
    /** buf */
    void renderBuf(const Buff* buf);
    void stopRenderBuf(creatureid_t bufId);
    
    /** cocos misc */
    void rollNode(cocos2d::Node* node);
    void rollNodeCallback(float dt);
    cocos2d::Vec2 calculateHpBarPosition();
    cocos2d::Node* createResourceOuputHintNode(const std::string& resourceName, int amount);
    
    /** get resource */
    static void getHpBarFiles(const UnitRender* ur, std::string& background, std::string& foreground);
};
    
}}

#endif /* UnitRender_h */
