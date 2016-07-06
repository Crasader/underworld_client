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
#include "LocalHelper.h"
#include "LevelLocalData.h"
#include "CardLocalData.h"
#include "CardUpgradeData.h"
#include "TalentUpgradeData.h"
#include "QuestLocalData.h"
#include "AchievementLocalData.h"
#include "ObjectLocalData.h"
#include "GearLocalData.h"
#include "GearUpgradeData.h"
#include "GearAttributeData.h"
#include "GearSetLocalData.h"
#include "UAConfigData.h"
#include "CardConfigData.h"
#include "URConfigData.h"
#include "BRConfigData.h"
#include "MapParticleConfigData.h"
#include "SpellConfigData.h"
#include "ArtifactLocalData.h"
#include "ArtifactUpgradeData.h"
#include "AttributeLocalData.h"
#include "HeroLocalData.h"
#include "HeroPieceData.h"
#include "HeroUpgradeData.h"
#include "SkillLocalData.h"
#include "SoldierLocalData.h"
#include "SoldierPieceData.h"
#include "SoldierUpgradeData.h"
#include "SoldierQualityData.h"
#include "SoldierTalentData.h"
#include "TowerLocalData.h"
#include "TowerUpgradeData.h"
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
    if (!s_pSharedInstance)
    {
        s_pSharedInstance = new (nothrow) DataManager();
        CCASSERT(s_pSharedInstance, "FATAL: Not enough memory");
    }
    
    return s_pSharedInstance;
}

void DataManager::purge()
{
    if (s_pSharedInstance)
    {
        delete s_pSharedInstance;
        s_pSharedInstance = nullptr;
    }
}

DataManager::DataManager()
:_binaryJsonTool(new (nothrow) BinaryJsonTool())
{
    // tech tree
    _techTree = new (nothrow) UnderWorld::Core::TechTree();
    _techTree->init(getTechTreeData());
    
    // game mode
    _gameModeHMM = new (nothrow) UnderWorld::Core::GameModeHMM();
}

DataManager::~DataManager()
{
    Utils::clearMap(_levels);
    Utils::clearMap(_cards);
    Utils::clearMap(_cardUpgradeDatas);
    Utils::clearMap(_talentUpgradeDatas);
    
    for (auto iter = _quests.begin(); iter != end(_quests); ++iter) {
        Utils::clearMap(iter->second);
    }
    _quests.clear();
    
    Utils::clearMap(_achievements);
    Utils::clearMap(_objects);
    Utils::clearMap(_gears);
    Utils::clearMap(_gearUpgradeDatas);
    Utils::clearMap(_gearAttributeDatas);
    Utils::clearMap(_gearSets);
    Utils::clearMap(_animationParameters);
    Utils::clearMap(_cardConfigData);
    Utils::clearMap(_urConfigData);
    Utils::clearMap(_brConfigData);
    
    for (auto iter = _mapParticleConfigData.begin(); iter != end(_mapParticleConfigData); ++iter) {
        Utils::clearVector(iter->second);
    }
    _mapParticleConfigData.clear();
    Utils::clearMap(_spellConfigData);
    
    Utils::clearMap(_artifacts);
    Utils::clearMap(_artifactUpgradeData);
    Utils::clearMap(_attributes);
    Utils::clearMap(_heroes);
    Utils::clearMap(_heroPieceDatas);
    Utils::clearMap(_heroUpgradeDatas);
    Utils::clearMap(_skills);
    Utils::clearMap(_soldiers);
    Utils::clearMap(_soldierPieceDatas);
    Utils::clearMap(_soldierUpgradeDatas);
    Utils::clearMap(_soldierQualityDatas);
    Utils::clearMap(_soldierTalentDatas);
    Utils::clearMap(_towers);
    Utils::clearMap(_towerUpgradeDatas);
    
    CC_SAFE_DELETE(_binaryJsonTool);
    CC_SAFE_DELETE(_techTree);
    CC_SAFE_DELETE(_gameModeHMM);
}

void DataManager::init()
{
    parseLevelData();
    parseCardDecks();
    parseCardData();
    parseCardUpgradeData();
    parseTalentUpgradeData();
    parseQuestData();
    parseAchievementData();
    parseObjectData();
    parseGearData();
    parseGearUpgradeData();
    parseGearAttributeData();
    parseGearSetData();
    parseAnimationConfigData();
    parseCardConfigData();
    parseURConfigData();
    parseBRConfigData();
    parseMapParticleConfigData();
    parseSpellConfigData();
    parseArtifactData();
    parseArtifactUpgradeData();
    parseAttributeData();
    parseHeroData();
    parseHeroPieceData();
    parseHeroUpgradeData();
    parseSkillData();
    parseSoldierData();
    parseSoldierPieceData();
    parseSoldierUpgradeData();
    parseSoldierQualityData();
    parseSoldierTalentData();
    parseTowerData();
    parseTowerUpgradeData();
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
const LevelLocalData* DataManager::getLevelData(int levelId) const
{
    return getMapValue(_levels, levelId);
}

const set<string>& DataManager::getCardDecks() const
{
    return _cardDecks;
}

const CardLocalData* DataManager::getCardData(int idx) const
{
    return getMapValue(_cards, idx);
}

const CardUpgradeData* DataManager::getCardUpgradeData(int idx, int level) const
{
    auto key = StringUtils::format("%d_%d", idx, level);
    return getMapValue(_cardUpgradeDatas, key);
}

const TalentUpgradeData* DataManager::getTalentUpgradeData(int idx, int level) const
{
    auto key = StringUtils::format("%d_%d", idx, level);
    return getMapValue(_talentUpgradeDatas, key);
}

const QuestLocalData* DataManager::getQuestData(QuestType type, int questId) const
{
    if (_quests.find(type) != end(_quests)) {
        return getMapValue(_quests.at(type), questId);
    }
    
    return nullptr;
}

const AchievementLocalData* DataManager::getAchievementData(int achievementId) const
{
    return getMapValue(_achievements, achievementId);
}

const ObjectLocalData* DataManager::getObjectData(int objectId) const
{
    return getMapValue(_objects, objectId);
}

const GearLocalData* DataManager::getGearData(int gearId) const
{
    return getMapValue(_gears, gearId);
}

const GearUpgradeData* DataManager::getGearUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_gearUpgradeDatas, key);
}

const GearAttributeData* DataManager::getGearAttributeData(int id, int level, int attributeId) const
{
    auto key = StringUtils::format("%d_%d_%d", id, level, attributeId);
    return getMapValue(_gearAttributeDatas, key);
}

const GearSetLocalData* DataManager::getGearSetData(int id) const
{
    return getMapValue(_gearSets, id);
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

const CardConfigData* DataManager::getCardConfigData(const string& name) const
{
    return getMapValue(_cardConfigData, name);
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

const ArtifactLocalData* DataManager::getArtifactData(int id) const
{
    return getMapValue(_artifacts, id);
}

const ArtifactUpgradeData* DataManager::getArtifactUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_artifactUpgradeData, key);
}

const AttributeLocalData* DataManager::getAttributeData(int id) const
{
    return getMapValue(_attributes, id);
}

const HeroLocalData* DataManager::getHeroData(int id) const
{
    return getMapValue(_heroes, id);
}

const HeroPieceData* DataManager::getHeroPieceData(int id) const
{
    return getMapValue(_heroPieceDatas, id);
}

const HeroUpgradeData* DataManager::getHeroUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_heroUpgradeDatas, key);
}

const SkillLocalData* DataManager::getSkillData(int id) const
{
    return getMapValue(_skills, id);
}

const SoldierLocalData* DataManager::getSoldierData(int id) const
{
    return getMapValue(_soldiers, id);
}

const SoldierPieceData* DataManager::getSoldierPieceData(int id) const
{
    return getMapValue(_soldierPieceDatas, id);
}

const SoldierUpgradeData* DataManager::getSoldierUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_soldierUpgradeDatas, key);
}

const SoldierQualityData* DataManager::getSoldierQualityData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_soldierQualityDatas, key);
}

const SoldierTalentData* DataManager::getSoldierTalentData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_soldierTalentDatas, key);
}

const TowerLocalData* DataManager::getTowerData(int id) const
{
    return getMapValue(_towers, id);
}

const TowerUpgradeData* DataManager::getTowerUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    return getMapValue(_towerUpgradeDatas, key);
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

void DataManager::parseLevelData()
{
    parseData("LevelData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) LevelLocalData(item);
        setMapValue(_levels, data->getLevel(), data);
    });
}

void DataManager::parseCardDecks()
{
    parseData("CardDecks.xml", [this](tinyxml2::XMLElement* item) {
        _cardDecks.insert(item->Attribute("name"));
    });
}

void DataManager::parseCardData()
{
    parseData("CardData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardLocalData(item);
        setMapValue(_cards, data->getId(), data);
    });
}

void DataManager::parseCardUpgradeData()
{
    parseData("CardUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_cardUpgradeDatas, key, data);
    });
}

void DataManager::parseTalentUpgradeData()
{
    parseData("TalentUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) TalentUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_talentUpgradeDatas, key, data);
    });
}

void DataManager::parseQuestData()
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
            _quests.insert(make_pair(type, unordered_map<int, QuestLocalData*>()));
        }
        
        parseData(iter->second, [this, type](tinyxml2::XMLElement* item) {
            auto data = new (nothrow) QuestLocalData(item);
            setMapValue(_quests.at(type), data->getId(), data);
        });
    }
}

void DataManager::parseAchievementData()
{
    parseData("EquipProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) AchievementLocalData(item);
        setMapValue(_achievements, data->getId(), data);
    });
}

void DataManager::parseObjectData()
{
    parseData("ObjectProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ObjectLocalData(item);
        setMapValue(_objects, data->getId(), data);
    });
}

void DataManager::parseGearData()
{
    parseData("EquipProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearLocalData(item);
        setMapValue(_gears, data->getId(), data);
    });
}

void DataManager::parseGearUpgradeData()
{
    parseData("GearUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_gearUpgradeDatas, key, data);
    });
}

void DataManager::parseGearAttributeData()
{
    parseData("GearAttributeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearAttributeData(item);
        auto key = StringUtils::format("%d_%d_%d", data->getId(), data->getLevel(), data->getAttributeId());
        setMapValue(_gearAttributeDatas, key, data);
    });
}

void DataManager::parseGearSetData()
{
    parseData("GearSetData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearSetLocalData(item);
        setMapValue(_gearSets, data->getId(), data);
    });
}

void DataManager::parseAnimationConfigData()
{
    parseData("AnimationConfig.xml", [this](tinyxml2::XMLElement* item) {
        const char* name = item->Attribute("name");
        const char* skill = item->Attribute("skill");
        if (name && skill) {
            auto data = new (nothrow) UAConfigData(item);
            auto key = StringUtils::format("%s_%s", name, skill);
            setMapValue(_animationParameters, key, data);
        }
    });
}

void DataManager::parseCardConfigData()
{
    parseData("CardConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardConfigData(item);
        setMapValue(_cardConfigData, data->getName(), data);
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
        setMapValue(_spellConfigData, data->getName(), data);
    });
}

void DataManager::parseArtifactData()
{
    parseData("ArtifactData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ArtifactLocalData(item);
        setMapValue(_artifacts, data->getId(), data);
    });
}

void DataManager::parseArtifactUpgradeData()
{
    parseData("ArtifactUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ArtifactUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_artifactUpgradeData, key, data);
    });
}

void DataManager::parseAttributeData()
{
    parseData("AttributeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) AttributeLocalData(item);
        setMapValue(_attributes, data->getId(), data);
    });
}

void DataManager::parseHeroData()
{
    parseData("HeroData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) HeroLocalData(item);
        setMapValue(_heroes, data->getId(), data);
    });
}

void DataManager::parseHeroPieceData()
{
    parseData("HeroPieceData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) HeroPieceData(item);
        setMapValue(_heroPieceDatas, data->getId(), data);
    });
}

void DataManager::parseHeroUpgradeData()
{
    parseData("HeroUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) HeroUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_heroUpgradeDatas, key, data);
    });
}

void DataManager::parseSkillData()
{
    parseData("SkillData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SkillLocalData(item);
        setMapValue(_skills, data->getId(), data);
    });
}

void DataManager::parseSoldierData()
{
    parseData("SoldierData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierLocalData(item);
        setMapValue(_soldiers, data->getId(), data);
    });
}

void DataManager::parseSoldierPieceData()
{
    parseData("SoldierPieceData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierPieceData(item);
        setMapValue(_soldierPieceDatas, data->getId(), data);
    });
}

void DataManager::parseSoldierUpgradeData()
{
    parseData("SoldierUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_soldierUpgradeDatas, key, data);
    });
}

void DataManager::parseSoldierQualityData()
{
    parseData("SoldierQualityData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierQualityData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_soldierQualityDatas, key, data);
    });
}

void DataManager::parseSoldierTalentData()
{
    parseData("SoldierTalentData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierTalentData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_soldierTalentDatas, key, data);
    });
}

void DataManager::parseTowerData()
{
    parseData("TowerData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) TowerLocalData(item);
        setMapValue(_towers, data->getId(), data);
    });
}

void DataManager::parseTowerUpgradeData()
{
    parseData("TowerUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) TowerUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        setMapValue(_towerUpgradeDatas, key, data);
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
