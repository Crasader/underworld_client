//
//  AvatarNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef AvatarNode_h
#define AvatarNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class AvatarNode : public Node
{
public:
    static constexpr float Width = 68;
    static constexpr float Height = 68;
    typedef std::function<void()> Callback;
    
    static AvatarNode* create();
    virtual ~AvatarNode();
    
    void setAvatar(int idx);
    void setCallback(const Callback& callback);
    
private:
    AvatarNode();
    virtual bool init() override;
    
private:
    ui::Button* _avatar;
    Callback _callback;
    bool _touchInvalid;
};

#endif /* AvatarNode_h */
