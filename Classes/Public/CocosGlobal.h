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

#define TOUCH_CANCEL_BY_MOVING_DISTANCE (40.0f)

#define DEFAULT_MAP_ID              (2)
#define BATTLE_RESOURCE_MAX_COUNT   (10)
#define MAP_OFFSET_Y                (0)
#define DEFAULT_FRAME_DELAY         (1.0f/12.0f)

#define ONLINE_UPDATE_SEARCH_PATH       (FileUtils::getInstance()->getWritablePath() + "Resource/")
#define DEFAULT_RESOURCE_FOLDER         ("res")
#define AUDIO_PREFIX                    ("sounds/effect/")
#define AUDIO_SUFFIX                    (".mp3")
#define PILIST_PREFIX                   ("particle/")
#define PILIST_SUFFIX                   (".plist")

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

#define SPELL_NAME_FIREBALL (101)
#define SPELL_NAME_CURE     (888)
#define SPELL_NAME_SPEEDUP  (999)

#define JSON_KEY ("code")

// Global Notifications
#define AppDidEnterBackgroundNotification   ("UWDidEnterBackgroundNotification")
#define AppWillEnterForegroundNotification  ("UWWillEnterForegroundNotification")

enum class ResourceType {
    Gem = 100,
    Gold = 101,
    Wood = 102,
    Stamina = 103,
    Exp = 104,
    
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
    None,
    World,
    Private,
    Guild,
    Group,
    Mail,
    Notice,
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

static ResourceType core_resource_type_key_2_ui_resource_type(const std::string& key) {
    if (key == RES_NAME_GOLD) {
        return ResourceType::Gold;
    } else if (key == RES_NAME_WOOD) {
        return ResourceType::Wood;
    } else {
        return ResourceType::MAX;
    }
}

#endif /* CocosGlobal_h */
