//
//  UnitView.h
//  Underworld_Client
//
//  Created by wenchengye on 16/7/31.
//
//

#ifndef UnitView_h
#define UnitView_h

#include <functional>
#include "cocos2d.h"

#include "Unit.h"

class URConfigData;

namespace UnderWorld{ namespace Core{
    
class UnitType;
    
enum class UnitAnimationType {
    PVR,
    CSB,
    
    UNIT_ANIMATION_TYPE_COUNT
};

enum class UnitAnimationPose {
    Stand,
    Move,
    Attack,
    AttackPost,
    Cast,
    CastPost,
    Killed,
    Dead,
    None,
    
    UNIT_ANIMATION_POSE_COUT
};

class UnitView : public cocos2d::Node {
private:
    /** data */
    const UnitType* _unitType;
    const URConfigData* _configData;
    
    /** instance */
    std::function<void ()> _playCallback;
    
    /** status */
    std::vector<std::string> _bodyAnimationFiles;
    std::vector<std::string> _shadowAnimationFiles;
    UnitAnimationType _type;
    UnitAnimationPose _pose;
    Unit::Direction _direction;
    float _durationScale;
    float _nodeScale;
    float _beginTime;
    bool _flip;
    bool _playing;
    
    /** cocos */
    cocos2d::Node* _shadowNode;
    cocos2d::Node* _bodyAnimNode;
    cocos2d::Ref* _bodyAnimation;
    cocos2d::Node* _shadowAnimNode;
    cocos2d::Ref* _shadowAnimation;
    cocos2d::Scheduler* _animScheduler;
    cocos2d::ActionManager* _actionManager;
    
public:
    /** init */
    static UnitView* create(const UnitType* unitType);
    virtual ~UnitView();
    bool init(const UnitType* unitType);
    
    /** amination interface */
    void buildAnimation();
    bool runAnimation(bool loop, const std::function<void ()>& callback);
    void stopAnimation();
    void destroyAnimation();
    
    /** getters */
    UnitAnimationType getBodyAnimationType() const        {return _type;}
    UnitAnimationPose getBodyAnimationPose() const        {return _pose;}
    cocos2d::Node* getBodyNode()                          {return this;}
    cocos2d::Node* getShadowNode()                        {return _shadowNode;}
    bool isFilp() const                                   {return _flip;}
    bool isPlaying() const                                {return _playing;}
    
    float getAnimationDuration() const;
    float getPlayTime() const;
    float getDurationScale() const                        {return _durationScale;}
    
    /** setters */
    void setBodyAnimationPose(UnitAnimationPose pose)     {_pose = pose;}
    void setDirection(Unit::Direction direction)          {_direction = direction;}
    void setDurationScale(float durationScale);
    void setNodeScale(float nodeScale);
    void setBeginTime(float beginTime)                    {_beginTime = beginTime;}
    
protected:
    UnitView();
    
    /** animation */
    static void loadAnimation(const std::string& file,
       UnitAnimationType type,
       cocos2d::Node*& animNodeOutput,
       cocos2d::Ref*& animationOutput);
    void playAnimationCallback();
    
    /** misc */
    void updateNodeScale();
    void updateDurationScale();
    
    /** cocos resource */
    static void getAnimationFiles(UnitAnimationType type,
        UnitAnimationPose pose,
        Unit::Direction direction,
        const URConfigData* configData,
        std::vector<std::string>& bodyAnimationOutput,
        std::vector<std::string>& shadowAnimationOutput);
    static int getResourceId(UnitAnimationType type, Unit::Direction direction);
    static bool needToFlip(Unit::Direction direction, const URConfigData* configData);
};
    
}}



#endif /* UnitView_h */
