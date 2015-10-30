#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class GameRender;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual ~HelloWorld();

private:
    HelloWorld();
    
    virtual bool init() override;
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event) override;
    
private:
    GameRender* _render;
};

#endif // __HELLOWORLD_SCENE_H__
