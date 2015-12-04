//
//  CocosGlobal.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CocosGlobal_h
#define CocosGlobal_h

#include "Global.h"

#define ENABLE_FUNCTIONS_TUTORIAL   (1)

#define USER_TOKEN_LENGTH   (64)

#define RESOLUTION_WIDTH    (1136)
#define RESOLUTION_HEIGHT   (640)

#define DEFAULT_FONT_SIZE   (20.0f)
#define BIG_FONT_SIZE       (30.0f)
#define TITLE_FONT_SIZE     (40.0f)

#define LAYER_DEFAULT_COLOR (Color4B(0, 0, 0, 0))
#define LAYER_MASK_COLOR    (Color4B(0, 0, 0, 128))

#define TOUCH_CANCEL_BY_MOVING_DISTANCE     (10.0f)

typedef std::function<void()> httpRequestCallback;

typedef enum
{
    kResourceType_Gem = 100,
    kResourceType_Gold = 101,
    
    kResourceType_MAX,          // Used to network parsing
}ResourceType;

typedef enum {
    kQuestType_Daily = 0,
    kQuestType_Life,
}QuestType;

typedef enum {
    kChatType_World = 0,
    kChatType_Guild,
    kChatType_Group,
    kChatType_Private,
}ChatType;

typedef enum {
    kGearType_Weapon = 0,
    kGearType_Armor,
    kGearType_Accessory,
}GearType;

typedef enum {
    kGearQuality_White = 0,
    kGearQuality_Blue,
    kGearQuality_Purple,
    kGearQuality_Orange,
}GearQuality;

#endif /* CocosGlobal_h */
