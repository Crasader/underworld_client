//
//  CocosGlobal.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef CocosGlobal_h
#define CocosGlobal_h

#include <iostream>
#include <functional>

#define USING_PVR   (0)

#define INVALID_VALUE   (-1)

#define USER_TOKEN_LENGTH   (64)

#define RESOLUTION_WIDTH    (1136)
#define RESOLUTION_HEIGHT   (640)

#define DEFAULT_FONT        ("TimesNewRomanPS-BoldMT")
#define DEFAULT_NUMBER_FONT ("HelveticaNeue-Bold")

#define SMALL_FONT_SIZE     (14.0f)
#define DEFAULT_FONT_SIZE   (18.0f)
#define BIG_FONT_SIZE       (24.0f)
#define TITLE_FONT_SIZE     (40.0f)

#define LAYER_DEFAULT_COLOR (cocos2d::Color4B(0, 0, 0, 0))
#define LAYER_MASK_COLOR    (cocos2d::Color4B(0, 0, 0, 128))

#define GOLD_LABEL_COLOR    (cocos2d::Color4B(255, 246, 0, 255))
#define WOOD_LABEL_COLOR    (cocos2d::Color4B(0, 228, 255, 255))

#define ENABLED_COLOR       (cocos2d::Color4B::WHITE)
#define DISABLED_COLOR      (cocos2d::Color4B::RED)

#define TOUCH_CANCEL_BY_MOVING_DISTANCE (10.0f)

#define DEFAULT_MAP_ID              (2)
#define BATTLE_RESOURCE_MAX_COUNT   (10)
#define MAP_OFFSET_Y                (0)
#define DEFAULT_FRAME_DELAY         (1.0f/12.0f)

#define FORMATION_MAX_COUNT         (3)
#define FORMATION_POPULATION_COUNT  (80)
#define FORMATION_SPELLS_COUNT      (12)
#define FORMATION_WIDTH             (5)
#define FORMATION_HEIGHT            (5)
#define FORMATION_TILE_WIDTH        (48)
#define FORMATION_TILE_HEIGHT       (48)

#define ONLINE_UPDATE_SEARCH_PATH       (FileUtils::getInstance()->getWritablePath() + "Resource/")
#define DEFAULT_RESOURCE_FOLDER         ("res")

#ifndef M_DISALLOW_COPY_AND_ASSIGN
#define M_DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName &) = delete; \
TypeName &operator =(const TypeName &) = delete;
#endif // DISALLOW_COPY_AND_ASSIGN


#define LOW_PIXELFORMAT (cocos2d::Texture2D::PixelFormat::RGB5A1)

typedef std::function<void()> httpRequestCallback;
typedef std::function<void(long errorCode)> httpErrorCallback;

// TODO: remove temp code
#define RES_NAME_GOLD       ("金子")
#define RES_NAME_WOOD       ("木头")

#define SPELL_NAME_FIREBALL ("全局技能火焰雨")
#define SPELL_NAME_CURE     ("治愈")
#define SPELL_NAME_SPEEDUP  ("顺风之力")
#define SPELL_NAME_GLOBAL   ("全局技能")

#define JSON_KEY ("code")

enum class ResourceType {
    Gem = 100,
    Gold = 101,
    Wood = 102,
    Jade = 103,
    
    MAX,          // Used to network parsing
};

enum class QuestType {
    None,
    Main,
    Branch,
    Daily,
    TimeLimited,
};

enum class ChatType {
    World = 0,
    Private,
    Guild,
    Group,
};

enum class GearType {
    Weapon = 1,
    Armor,
    Accessory,
};

enum class GearQuality {
    White = 0,
    Blue,
    Purple,
    Orange,
};

ResourceType core_resource_type_key_2_ui_resource_type(const std::string& key) {
    if (key == RES_NAME_GOLD) {
        return ResourceType::Gold;
    } else if (key == RES_NAME_WOOD) {
        return ResourceType::Wood;
    } else {
        return ResourceType::MAX;
    }
}

#endif /* CocosGlobal_h */
