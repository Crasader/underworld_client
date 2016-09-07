//
//  GuildGenericCell.cpp
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "GuildGenericCell.h"

GuildGenericCell::GuildGenericCell() {}

GuildGenericCell::~GuildGenericCell()
{
    removeAllChildren();
}

bool GuildGenericCell::init()
{
    if (ui::Button::init()) {
        
        return true;
    }
    
    return false;
}
