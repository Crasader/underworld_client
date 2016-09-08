//
//  GuildGenericCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildGenericCell.h"
#include "CocosUtils.h"

GuildGenericCell::GuildGenericCell()
:_touchInvalid(false) {}

GuildGenericCell::~GuildGenericCell()
{
    removeAllChildren();
}

bool GuildGenericCell::init()
{
    static const auto& file(CocosUtils::getResourcePath("ui_tiao_15.png"));
    if (ui::Button::init(file, file)) {
        setScale9Enabled(true);
        setSwallowTouches(false);
        return true;
    }
    
    return false;
}
