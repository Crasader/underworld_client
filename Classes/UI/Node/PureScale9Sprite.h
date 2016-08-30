//
//  PureScale9Sprite.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef PureScale9Sprite_h
#define PureScale9Sprite_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class PureScale9Sprite: public ui::Scale9Sprite
{
public:
    enum class Type {
        White,
        Purple,
        BlueLight,
        BlueDeep,
        Blue,
        Orange,
    };
    
    static PureScale9Sprite* create(Type type);
    virtual ~PureScale9Sprite();
    
    void setType(Type type);
    
private:
    PureScale9Sprite();
    bool init(Type type);
    std::string getResourceFile(Type type) const;
    
private:
    Type _type;
};

#endif /* PureScale9Sprite_h */
