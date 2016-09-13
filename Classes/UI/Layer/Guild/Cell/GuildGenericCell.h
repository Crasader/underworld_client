//
//  GuildGenericCell.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef GuildGenericCell_h
#define GuildGenericCell_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class AvatarNode;
class GuildTorphyNode;

class GuildGenericCell: public ui::Button
{
public:
    virtual ~GuildGenericCell() = 0;
    
protected:
    GuildGenericCell();
    virtual bool init() override;
    void setAvatarEdgeX(float x);
    
protected:
    bool _touchInvalid;
    AvatarNode* _avatar;
    Label* _name;
    Label* _description;
    Label* _hint;
    GuildTorphyNode* _trophy;
};

#endif /* GuildGenericCell_h */
