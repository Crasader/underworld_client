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

#define RESOLUTION_WIDTH    (640)
#define RESOLUTION_HEIGHT   (1136)

#define DEFAULT_FONT_SIZE   (20.0f)
#define BIG_FONT_SIZE       (30.0f)
#define TITLE_FONT_SIZE     (40.0f)

#define LAYER_DEFAULT_COLOR (Color4B(0, 0, 0, 0))
#define LAYER_MASK_COLOR    (Color4B(0, 0, 0, 128))

typedef std::function<void()> httpRequestCallback;

typedef enum
{
    kResourceType_Gem = 100,
    kResourceType_Gold = 101,
    
    kResourceType_MAX,          // Used to network parsing
}ResourceType;

#endif /* CocosGlobal_h */
