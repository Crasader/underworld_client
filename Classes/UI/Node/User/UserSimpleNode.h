//
//  UserSimpleNode.h
//  Underworld_Client
//
//  Created by Andy on 16/8/2.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UserSimpleNode_h
#define UserSimpleNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UserSimpleData;
class AvatarNode;
class TrophyNode;
class ExpNode;
class GuildSimpleNode;

class UserSimpleNode : public Node
{
public:
    static UserSimpleNode* create();
    virtual ~UserSimpleNode();
    
    void setIsMe(bool isMe);
    void setIsHome(bool isHome);
    void setData(const UserSimpleData* data);
    void setAvatarCallback(const std::function<void()>& callback);
    
private:
    UserSimpleNode();
    virtual bool init() override;
    Node* createNodeWithoutAvatar(const Size& size);
    void setAvatar(int idx);
    void setUserName(const std::string& name);
    void setTrophy(int count);
    void setGuildIcon(int idx);
    void setGuildName(const std::string& name);
    void setLevel(int count);
    void setExp(int progress, int maxProgress);
    void reset();
    void updateThisData();
    
private:
    const UserSimpleData* _data;
    AvatarNode* _avatar;
    Node* _infoNode;
    Label* _name;
    TrophyNode* _trophy;
    ExpNode* _exp;
    GuildSimpleNode* _guild;
};

#endif /* UserSimpleNode_h */
