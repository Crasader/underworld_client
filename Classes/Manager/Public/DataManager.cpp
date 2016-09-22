//
//  DataManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "DataManager.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "ObjectUtils.h"
#include "LocalHelper.h"
#include "LevelProperty.h"
#include "AbstractProperty.h"
#include "AbstractUpgradeProperty.h"
#include "AttributeProperty.h"
#include "CardProperty.h"
#include "CardUpgradeProperty.h"
#include "RuneProperty.h"
#include "RuneUpgradeProperty.h"
#include "SkillProperty.h"
#include "SkillUpgradeProperty.h"
#include "SkillBookProperty.h"
#include "QuestProperty.h"
#include "AchievementProperty.h"
#include "ObjectProperty.h"
#include "UAConfigData.h"
#include "CardConfigData.h"
#include "URConfigData.h"
#include "BRConfigData.h"
#include "MapParticleConfigData.h"
#include "SpellConfigData.h"
#include "BinaryJsonTool.h"
#include "TechTree.h"
#include "GameModeHMM.h"

USING_NS_CC;
using namespace std;

template<typename _Key, typename _Value>
static void setMapValue(unordered_map<_Key, _Value>& m, const _Key& key, const _Value& value)
{
    if (m.find(key) != end(m)) {
        assert(false);
    } else {
        m.insert(make_pair(key, value));
    }
}

template<typename _Key, typename _Value>
static const _Value& getMapValue(const unordered_map<_Key, _Value>& m, const _Key& key)
{
    if (m.find(key) != end(m)) {
        return m.at(key);
    }
    
    static _Value ret(nullptr);
    return ret;
}

static DataManager *s_pSharedInstance = nullptr;

DataManager* DataManager::getInstance()
{
    if (!s_pSharedInstance) {
        s_pSharedInstance = new (nothrow) DataManager();
        CCASSERT(s_pSharedInstance, "FATAL: Not enough memory");
    }
    
    return s_pSharedInstance;
}

void DataManager::purge()
{
    CC_SAFE_DELETE(s_pSharedInstance);
}

DataManager::DataManager()
:_binaryJsonTool(new (nothrow) BinaryJsonTool())
{
    // tech tree
    _techTree = new (nothrow) UnderWorld::Core::TechTree();
    _techTree->init(getTechTreeData());
    
    // game mode
    _gameModeHMM = new (nothrow) UnderWorld::Core::GameModeHMM();
    _gameModeHMM->initStatic(getHMMTechTreeData());
}

DataManager::~DataManager()
{
    Utils::clearMap(_levels);
    Utils::clearMap(_attributes);
    Utils::clearMap(_cards);
    Utils::clearMap(_cardUpgradeProperties);
    Utils::clearMap(_runes);
    Utils::clearMap(_runeUpgradeProperties);
    Utils::clearMap(_skills);
    Utils::clearMap(_skillUpgradeProperties);
    Utils::clearMap(_skillBooks);
    
    for (auto iter = _quests.begin(); iter != end(_quests); ++iter) {
        Utils::clearMap(iter->second);
    }
    _quests.clear();
    
    Utils::clearMap(_achievements);
    Utils::clearMap(_objects);
    Utils::clearMap(_animationParameters);
    Utils::clearMap(_cardConfigData);
    Utils::clearMap(_urConfigData);
    Utils::clearMap(_brConfigData);
    
    for (auto iter = _mapParticleConfigData.begin(); iter != end(_mapParticleConfigData); ++iter) {
        Utils::clearVector(iter->second);
    }
    _mapParticleConfigData.clear();
    Utils::clearMap(_spellConfigData);
    Utils::clearMap(_skills);
    
    CC_SAFE_DELETE(_binaryJsonTool);
    CC_SAFE_DELETE(_techTree);
    CC_SAFE_DELETE(_gameModeHMM);
}

void DataManager::init()
{
    parsePvrFiles();
    parseLevelProperty();
    parseCardDecks();
    parseAttributeProperty();
    parseCardProperty();
    parseCardUpgradeProperty();
    parseRuneProperty();
    parseRuneUpgradeProperty();
    parseSkillProperty();
    parseSkillUpgradeProperty();
    parseSkillBookProperty();
    parseQuestProperty();
    parseAchievementProperty();
    parseObjectProperty();
    parseAnimationConfigData();
    parseCardConfigData();
    parseURConfigData();
    parseBRConfigData();
    parseMapParticleConfigData();
    parseSpellConfigData();
    parseBinaryjsonTemplates();
}

string DataManager::getMapData(int mapId) const
{
    return getFileData(StringUtils::format("MapData/%d.xml", mapId));
}

string DataManager::getMapDataHMM(int mapId) const
{
    return getFileData(StringUtils::format("MapData/%d_hmm.xml", mapId));
}

string DataManager::getTechTreeData() const
{
    return getFileData("TechTreeData/techtree.xml");
}

string DataManager::getHMMTechTreeData() const
{
    return getFileData("TechTreeData/techtree_hmm.xml");
}

#pragma mark - getters
const vector<string>& DataManager::getPVRFiles() const
{
    return _pvrFiles;
}

const LevelProperty* DataManager::getLevelProperty(int levelId) const
{
    return getMapValue(_levels, levelId);
}

const set<int>& DataManager::getCardDecks() const
{
    return _cardDecks;
}

#pragma mark development
const AbstractProperty* DataManager::getProperty(int oid) const
{
    auto type(ObjectUtils::getType(oid));
    switch (type) {
        case ObjectUtils::Type::CARD:
            return getCardProperty(oid);
        case ObjectUtils::Type::SKILL:
            return getSkillProperty(oid);
        case ObjectUtils::Type::RUNE:
            return getRuneProperty(oid);
        case ObjectUtils::Type::BOOK:
            return getSkillBookProperty(oid);
        default:
            return nullptr;
    }
}

const AbstractUpgradeProperty* DataManager::getUpgradeProperty(int oid, int level) const
{
    auto type(ObjectUtils::getType(oid));
    switch (type) {
        case ObjectUtils::Type::CARD:
            return getCardUpgradeProperty(oid, level);
        case ObjectUtils::Type::SKILL:
            return getSkillUpgradeProperty(oid, level);
        case ObjectUtils::Type::RUNE:
            return getRuneUpgradeProperty(oid, level);
        default:
            return nullptr;
    }
}

const AttributeProperty* DataManager::getAttributeProperty(int aid) const
{
    return getMapValue(_attributes, aid);
}

const CardProperty* DataManager::getCardProperty(int cardId) const
{
    return getMapValue(_cards, cardId);
}

const CardUpgradeProperty* DataManager::getCardUpgradeProperty(int cardId, int level) const
{
    auto key = StringUtils::format("%d_%d", cardId, level);
    return getMapValue(_cardUpgradeProperties, key);
}

const RuneProperty* DataManager::getRuneProperty(int runeId) const
{
    return getMapValue(_runes, runeId);
}

const RuneUpgradeProperty* DataManager::getRuneUpgradeProperty(int runeId, int level) const
{
    auto key = StringUtils::format("%d_%d", runeId, level);
    return getMapValue(_runeUpgradeProperties, key);
}

const SkillProperty* DataManager::getSkillProperty(int skillId) const
{
    return getMapValue(_skills, skillId);
}

const SkillUpgradeProperty* DataManager::getSkillUpgradeProperty(int skillId, int level) const
{
    auto key = StringUtils::format("%d_%d", skillId, level);
    return getMapValue(_skillUpgradeProperties, key);
}

const SkillBookProperty* DataManager::getSkillBookProperty(int bookId) const
{
    return getMapValue(_skillBooks, bookId);
}

const QuestProperty* DataManager::getQuestProperty(QuestType type, int questId) const
{
    if (_quests.find(type) != end(_quests)) {
        return getMapValue(_quests.at(type), questId);
    }
    
    return nullptr;
}

const AchievementProperty* DataManager::getAchievementProperty(int achievementId) const
{
    return getMapValue(_achievements, achievementId);
}

const ObjectProperty* DataManager::getObjectProperty(int objectId) const
{
    return getMapValue(_objects, objectId);
}

const AnimationParameter* DataManager::getAnimationParameter(const string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction) const
{
    auto key = name + StringUtils::format("_%d", skillClass);
    if (_animationParameters.find(key) != end(_animationParameters)) {
        auto data = _animationParameters.at(key);
        if (data) {
            return data->getAnimationParameter(direction);
        }
    }
    
    return nullptr;
}

const CardConfigData* DataManager::getCardConfigData(int idx) const
{
    return getMapValue(_cardConfigData, idx);
}

const URConfigData* DataManager::getURConfigData(const string& name) const
{
    return getMapValue(_urConfigData, name);
}

const BRConfigData* DataManager::getBRConfigData(const string& name) const
{
    return getMapValue(_brConfigData, name);
}

const vector<MapParticleConfigData*>& DataManager::getMapParticleConfigData(int mapId) const
{
    if (_mapParticleConfigData.find(mapId) != end(_mapParticleConfigData)) {
        return _mapParticleConfigData.at(mapId);
    }
    
    static vector<MapParticleConfigData*> empty;
    return empty;
}

const SpellConfigData* DataManager::getSpellConfigData(const string& name) const
{
    return getMapValue(_spellConfigData, name);
}

#pragma mark - parsers
string DataManager::getFileData(const string& file) const
{
    auto fileName = LocalHelper::getLocalizedConfigFilePath(file);
    if (FileUtils::getInstance()->isFileExist(fileName)) {
        return LocalHelper::loadFileContentString(fileName);
    }
    
    return "";
}

void DataManager::parseData(const string& file, const function<void(tinyxml2::XMLElement*)>& parser) const
{
    auto content = getFileData(file);
    if (content.size() > 0) {
        auto xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        xmlDoc->Parse(content.c_str());
        for (auto item = xmlDoc->RootElement()->FirstChildElement(); item; item = item->NextSiblingElement()) {
            if (parser) {
                parser(item);
            }
        }
        
        CC_SAFE_DELETE(xmlDoc);
    }
}

void DataManager::parsePvrFiles()
{
    parseData("pvrlist.xml", [this](tinyxml2::XMLElement* item) {
        _pvrFiles.push_back(item->Attribute("name"));
    });
}

void DataManager::parseLevelProperty()
{
    parseData("LevelProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) LevelProperty(item);
        setMapValue(_levels, data->getLevel(), data);
    });
}

void DataManager::parseCardDecks()
{
    parseData("CardDecks.xml", [this](tinyxml2::XMLElement* item) {
        _cardDecks.insert(atoi(item->Attribute("id")));
    });
}

void DataManager::parseAttributeProperty()
{
    parseData("AttributeProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) AttributeProperty(item);
        setMapValue(_attributes, data->getId(), data);
    });
}

void DataManager::parseCardProperty()
{
    parseData("CardProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardProperty(item);
        setMapValue(_cards, data->getId(), data);
    });
}

void DataManager::parseCardUpgradeProperty()
{
    parseData("CardUpgradeProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardUpgradeProperty(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_cardUpgradeProperties, key, data);
    });
}

void DataManager::parseRuneProperty()
{
    parseData("RuneProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) RuneProperty(item);
        setMapValue(_runes, data->getId(), data);
    });
}

void DataManager::parseRuneUpgradeProperty()
{
    parseData("RuneUpgradeProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) RuneUpgradeProperty(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_runeUpgradeProperties, key, data);
    });
}

void DataManager::parseSkillProperty()
{
    parseData("SkillProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SkillProperty(item);
        setMapValue(_skills, data->getId(), data);
    });
}

void DataManager::parseSkillUpgradeProperty()
{
    parseData("SkillUpgradeProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SkillUpgradeProperty(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_skillUpgradeProperties, key, data);
    });
}

void DataManager::parseSkillBookProperty()
{
    parseData("BookProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SkillBookProperty(item);
        setMapValue(_skillBooks, data->getId(), data);
    });
}

void DataManager::parseQuestProperty()
{
    map<QuestType, string> quests = {
        {QuestType::Main, "MainQuestProperty.xml"},
        {QuestType::Branch, "BranchQuestProperty.xml"},
        {QuestType::Daily, "DailyQuestProperty.xml"},
        {QuestType::TimeLimited, "TimeLimitedQuestProperty.xml"},
    };
    
    for (auto iter = begin(quests); iter != end(quests); ++iter) {
        const auto& type = iter->first;
        
        // clear first
        if (_quests.find(type) != end(_quests)) {
            Utils::clearMap(_quests.at(type));
        } else {
            _quests.insert(make_pair(type, unordered_map<int, QuestProperty*>()));
        }
        
        parseData(iter->second, [this, type](tinyxml2::XMLElement* item) {
            auto data = new (nothrow) QuestProperty(item);
            setMapValue(_quests.at(type), data->getId(), data);
        });
    }
}

void DataManager::parseAchievementProperty()
{
    parseData("AchievementProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) AchievementProperty(item);
        setMapValue(_achievements, data->getId(), data);
    });
}

void DataManager::parseObjectProperty()
{
    parseData("ObjectProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ObjectProperty(item);
        setMapValue(_objects, data->getId(), data);
    });
}

void DataManager::parseAnimationConfigData()
{
    parseData("AnimationConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) UAConfigData(item);
        auto key = data->getName() + StringUtils::format("_%d", data->getSkill());
        setMapValue(_animationParameters, key, data);
    });
}

void DataManager::parseCardConfigData()
{
    parseData("CardConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardConfigData(item);
        setMapValue(_cardConfigData, data->getId(), data);
    });
}

void DataManager::parseURConfigData()
{
    parseData("UnitResourceConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) URConfigData(item);
        setMapValue(_urConfigData, data->getName(), data);
    });
}

void DataManager::parseBRConfigData()
{
    parseData("BulletResourceConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) BRConfigData(item);
        setMapValue(_brConfigData, data->getName(), data);
    });
}

void DataManager::parseMapParticleConfigData()
{
    for (int i = 0; i < 2; ++i) {
        // clear first
        if (_mapParticleConfigData.find(i) != end(_mapParticleConfigData)) {
            Utils::clearVector(_mapParticleConfigData.at(i));
        } else {
            _mapParticleConfigData.insert(make_pair(i, vector<MapParticleConfigData*>()));
        }
        
        auto file = StringUtils::format("MapParticleConfig_%d.xml", i);
        parseData(file, [this, i](tinyxml2::XMLElement* item) {
            auto& configs = _mapParticleConfigData.at(i);
            auto data = new (nothrow) MapParticleConfigData(item);
            configs.push_back(data);
        });
    }
}

void DataManager::parseSpellConfigData()
{
    parseData("SpellConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SpellConfigData(item);
        setMapValue(_spellConfigData, data->getRenderKey(), data);
    });
}

void DataManager::parseBinaryjsonTemplates()
{
    auto content = getFileData("JsonTemplates.xml");
    if (content.size() > 0) {
        _binaryJsonTool->initTemplates(content, JSON_KEY);
    }
}

const BinaryJsonTool* DataManager::getBinaryJsonTool() const
{
    return _binaryJsonTool;
}

UnderWorld::Core::TechTree* DataManager::getTechTree() const
{
    return _techTree;
}

UnderWorld::Core::GameModeHMM* DataManager::getGameModeHMM() const
{
    return _gameModeHMM;
}

float DataManager::getBattleResourceMaxCount() const
{
    if (_techTree) {
        auto rt = _techTree->findResourceTypeByName(RES_NAME_WOOD);
        if (rt) {
            return ((float)rt->getMax()) / 10.0f;
        }
    }
    
    return 0;
}
