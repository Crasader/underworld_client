//
//  PVRAnimation.h
//  Underworld_Client
//
//  Created by wenchengye on 16/8/1.
//
//

#ifndef PVRAnimation_h
#define PVRAnimation_h

#include <functional>
#include "cocos2d.h"

class PVRAnimation : public cocos2d::Ref {
private:
    static const float PVR_ANIMATION_FRAME_DELAY;

private:
    cocos2d::Vector<cocos2d::SpriteFrame*> _frames;
    cocos2d::Animate* _animate;
    cocos2d::Animate* _playingAnimate;
    int _beginFrameIndex;
    
public:
    /** init */
    static PVRAnimation* create(const std::string& folder);
    virtual ~PVRAnimation();
    bool init(const std::string& folder);
    
    /** interface */
    void play(cocos2d::Node* node,
        bool loop,
        int beginFrameIndex,
        const std::function<void()>& callback);
    cocos2d::Node* buildNode() const;
    
    /** getters */
    int getFrameCount() const        {return (int)_frames.size();}
    int getCurrentFrame() const;
    float getDuration() const;
    
private:
    PVRAnimation();
};

#endif /* PVRAnimation_h */
