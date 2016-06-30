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
    
    for (auto iter = _quests.begin(); iter != _quests.end(); ++iter) {
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
    
    for (auto iter = _mapParticleConfigData.begin(); iter != _mapParticleConfigData.end(); ++iter) {
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
    parseQuestData(QuestType::Main);
    parseQuestData(QuestType::Daily);
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
    if (_levels.find(levelId) != _levels.end()) {
        return _levels.at(levelId);
    }
    
    return nullptr;
}

const set<string>& DataManager::getCardDecks() const
{
    return _cardDecks;
}

const CardLocalData* DataManager::getCardData(int idx) const
{
    if (_cards.find(idx) != end(_cards)) {
        return _cards.at(idx);
    }
    
    return nullptr;
}

const CardUpgradeData* DataManager::getCardUpgradeData(int idx, int level) const
{
    auto key = StringUtils::format("%d_%d", idx, level);
    if (_cardUpgradeDatas.find(key) != end(_cardUpgradeDatas)) {
        return _cardUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const TalentUpgradeData* DataManager::getTalentUpgradeData(int idx, int level) const
{
    auto key = StringUtils::format("%d_%d", idx, level);
    if (_talentUpgradeDatas.find(key) != end(_talentUpgradeDatas)) {
        return _talentUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const QuestLocalData* DataManager::getQuestData(QuestType type, int questId) const
{
    if (_quests.find(type) != _quests.end()) {
        const auto& quests = _quests.at(type);
        if (quests.find(questId) != quests.end()) {
            return quests.at(questId);
        }
    }
    
    return nullptr;
}

const AchievementLocalData* DataManager::getAchievementData(int achievementId) const
{
    if (_achievements.find(achievementId) != _achievements.end()) {
        return _achievements.at(achievementId);
    }
    
    return nullptr;
}

const ObjectLocalData* DataManager::getObjectData(int objectId) const
{
    if (_objects.find(objectId) != _objects.end()) {
        return _objects.at(objectId);
    }
    
    return nullptr;
}

const GearLocalData* DataManager::getGearData(int gearId) const
{
    if (_gears.find(gearId) != _gears.end()) {
        return _gears.at(gearId);
    }
    
    return nullptr;
}

const GearUpgradeData* DataManager::getGearUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_gearUpgradeDatas.find(key) != _gearUpgradeDatas.end()) {
        return _gearUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const GearAttributeData* DataManager::getGearAttributeData(int id, int level, int attributeId) const
{
    auto key = StringUtils::format("%d_%d_%d", id, level, attributeId);
    if (_gearAttributeDatas.find(key) != _gearAttributeDatas.end()) {
        return _gearAttributeDatas.at(key);
    }
    
    return nullptr;
}

const GearSetLocalData* DataManager::getGearSetData(int id) const
{
    if (_gearSets.find(id) != _gearSets.end()) {
        return _gearSets.at(id);
    }
    
    return nullptr;
}

void DataManager::getAnimationParameters(const string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction, float& scale, float& speed) const
{
    scale = speed = 1.0f;
    
    auto key = name + StringUtils::format("_%d", skillClass);
    if (_animationParameters.find(key) != _animationParameters.end()) {
        UAConfigData* data = _animationParameters.at(key);
        if (data) {
            data->getAnimationParameters(direction, scale, speed);
        }
    }
}

const CardConfigData* DataManager::getCardConfigData(const string& name) const
{
    if (_cardConfigData.find(name) != _cardConfigData.end()) {
        return _cardConfigData.at(name);
    }
    
    return nullptr;
}

const URConfigData* DataManager::getURConfigData(const string& name) const
{
    if (_urConfigData.find(name) != _urConfigData.end()) {
        return _urConfigData.at(name);
    }
    
    return nullptr;
}

const BRConfigData* DataManager::getBRConfigData(const string& name) const
{
    if (_brConfigData.find(name) != _brConfigData.end()) {
        return _brConfigData.at(name);
    }
    
    return nullptr;
}

const vector<MapParticleConfigData*>& DataManager::getMapParticleConfigData(int mapId) const
{
    if (_mapParticleConfigData.find(mapId) != _mapParticleConfigData.end()) {
        return _mapParticleConfigData.at(mapId);
    }
    
    static vector<MapParticleConfigData*> empty;
    return empty;
}

const SpellConfigData* DataManager::getSpellConfigData(const string& name) const
{
    if (_spellConfigData.find(name) != _spellConfigData.end()) {
        return _spellConfigData.at(name);
    }
    
    return nullptr;
}

const ArtifactLocalData* DataManager::getArtifactData(int id) const
{
    if (_artifacts.find(id) != _artifacts.end()) {
        return _artifacts.at(id);
    }
    
    return nullptr;
}

const ArtifactUpgradeData* DataManager::getArtifactUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_artifactUpgradeData.find(key) != _artifactUpgradeData.end()) {
        return _artifactUpgradeData.at(key);
    }
    
    return nullptr;
}

const AttributeLocalData* DataManager::getAttributeData(int id) const
{
    if (_attributes.find(id) != _attributes.end()) {
        return _attributes.at(id);
    }
    
    return nullptr;
}

const HeroLocalData* DataManager::getHeroData(int id) const
{
    if (_heroes.find(id) != _heroes.end()) {
        return _heroes.at(id);
    }
    
    return nullptr;
}

const HeroPieceData* DataManager::getHeroPieceData(int id) const
{
    if (_heroPieceDatas.find(id) != _heroPieceDatas.end()) {
        return _heroPieceDatas.at(id);
    }
    
    return nullptr;
}

const HeroUpgradeData* DataManager::getHeroUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_heroUpgradeDatas.find(key) != _heroUpgradeDatas.end()) {
        return _heroUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const SkillLocalData* DataManager::getSkillData(int id) const
{
    if (_skills.find(id) != _skills.end()) {
        return _skills.at(id);
    }
    
    return nullptr;
}

const SoldierLocalData* DataManager::getSoldierData(int id) const
{
    if (_soldiers.find(id) != _soldiers.end()) {
        return _soldiers.at(id);
    }
    
    return nullptr;
}

const SoldierPieceData* DataManager::getSoldierPieceData(int id) const
{
    if (_soldierPieceDatas.find(id) != _soldierPieceDatas.end()) {
        return _soldierPieceDatas.at(id);
    }
    
    return nullptr;
}

const SoldierUpgradeData* DataManager::getSoldierUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_soldierUpgradeDatas.find(key) != _soldierUpgradeDatas.end()) {
        return _soldierUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const SoldierQualityData* DataManager::getSoldierQualityData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_soldierQualityDatas.find(key) != _soldierQualityDatas.end()) {
        return _soldierQualityDatas.at(key);
    }
    
    return nullptr;
}

const SoldierTalentData* DataManager::getSoldierTalentData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_soldierTalentDatas.find(key) != _soldierTalentDatas.end()) {
        return _soldierTalentDatas.at(key);
    }
    
    return nullptr;
}

const TowerLocalData* DataManager::getTowerData(int id) const
{
    if (_towers.find(id) != _towers.end()) {
        return _towers.at(id);
    }
    
    return nullptr;
}

const TowerUpgradeData* DataManager::getTowerUpgradeData(int id, int level) const
{
    auto key = StringUtils::format("%d_%d", id, level);
    if (_towerUpgradeDatas.find(key) != _towerUpgradeDatas.end()) {
        return _towerUpgradeDatas.at(key);
    }
    
    return nullptr;
}

#pragma mark - parsers
string DataManager::getFileData(const std::string& file) const
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
        const int levelId = data->getLevel();
        if (_levels.find(levelId) != _levels.end()) {
            assert(false);
        } else {
            _levels.insert(make_pair(levelId, data));
        }
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
        const int idx = data->getId();
        if (_cards.find(idx) != _cards.end()) {
            assert(false);
        } else {
            _cards.insert(make_pair(idx, data));
        }
    });
}

void DataManager::parseCardUpgradeData()
{
    parseData("CardUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_cardUpgradeDatas.find(key) != end(_cardUpgradeDatas)) {
            assert(false);
        } else {
            _cardUpgradeDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseTalentUpgradeData()
{
    parseData("TalentUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) TalentUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_talentUpgradeDatas.find(key) != end(_talentUpgradeDatas)) {
            assert(false);
        } else {
            _talentUpgradeDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseQuestData(QuestType type)
{
    string fileName;
    
    if (QuestType::Main == type) {
        fileName = "MainQuestProperty.xml";
    } else if (QuestType::Daily == type) {
        fileName = "DailyQuestProperty.xml";
    }
    
    // clear first
    if (_quests.find(type) != _quests.end()) {
        Utils::clearMap(_quests.at(type));
    } else {
        _quests.insert(make_pair(type, unordered_map<int, QuestLocalData*>()));
    }
    
    parseData(fileName, [this, type](tinyxml2::XMLElement* item) {
        auto& quests = _quests.at(type);
        auto data = new (nothrow) QuestLocalData(item);
        const int questId = data->getId();
        if (quests.find(questId) != quests.end()) {
            assert(false);
        } else {
            quests.insert(make_pair(questId, data));
        }
    });
}

void DataManager::parseAchievementData()
{
    parseData("EquipProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) AchievementLocalData(item);
        const int id = data->getId();
        if (_achievements.find(id) != _achievements.end()) {
            assert(false);
        } else {
            _achievements.insert(make_pair(id, data));
        }
    });
}

void DataManager::parseObjectData()
{
    parseData("ObjectProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ObjectLocalData(item);
        const int id = data->getId();
        if (_objects.find(id) != _objects.end()) {
            assert(false);
        } else {
            _objects.insert(make_pair(id, data));
        }
    });
}

void DataManager::parseGearData()
{
    parseData("EquipProperty.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearLocalData(item);
        const int gearId = data->getId();
        if (_gears.find(gearId) != _gears.end()) {
            assert(false);
        } else {
            _gears.insert(make_pair(gearId, data));
        }
    });
}

void DataManager::parseGearUpgradeData()
{
    parseData("GearUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_gearUpgradeDatas.find(key) != _gearUpgradeDatas.end()) {
            assert(false);
        } else {
            _gearUpgradeDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseGearAttributeData()
{
    parseData("GearAttributeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearAttributeData(item);
        auto key = StringUtils::format("%d_%d_%d", data->getId(), data->getLevel(), data->getAttributeId());
        if (_gearAttributeDatas.find(key) != _gearAttributeDatas.end()) {
            assert(false);
        } else {
            _gearAttributeDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseGearSetData()
{
    parseData("GearSetData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) GearSetLocalData(item);
        const int key = data->getId();
        if (_gearSets.find(key) != _gearSets.end()) {
            assert(false);
        } else {
            _gearSets.insert(make_pair(key, data));
        }
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
            if (_animationParameters.find(key) != _animationParameters.end()) {
                assert(false);
            } else {
                _animationParameters.insert(make_pair(key, data));
            }
        }
    });
}

void DataManager::parseCardConfigData()
{
    parseData("CardConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) CardConfigData(item);
        const auto& key = data->getName();
        if (_cardConfigData.find(key) != _cardConfigData.end()) {
            assert(false);
        } else {
            _cardConfigData.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseURConfigData()
{
    parseData("UnitResourceConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) URConfigData(item);
        const auto& key = data->getName();
        if (_urConfigData.find(key) != _urConfigData.end()) {
            assert(false);
        } else {
            _urConfigData.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseBRConfigData()
{
    parseData("BulletResourceConfig.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) BRConfigData(item);
        const auto& key = data->getName();
        if (_brConfigData.find(key) != _brConfigData.end()) {
            assert(false);
        } else {
            _brConfigData.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseMapParticleConfigData()
{
    for (int i = 0; i < 2; ++i) {
        // clear first
        if (_mapParticleConfigData.find(i) != _mapParticleConfigData.end()) {
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
        const auto& key = data->getName();
        if (_spellConfigData.find(key) != _spellConfigData.end()) {
            assert(false);
        } else {
            if (key.length() > 0) {
                _spellConfigData.insert(make_pair(key, data));
            } else {
                CC_SAFE_DELETE(data);
            }
        }
    });
}

void DataManager::parseArtifactData()
{
    parseData("ArtifactData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ArtifactLocalData(item);
        const int key = data->getId();
        if (_artifacts.find(key) != _artifacts.end()) {
            assert(false);
        } else {
            _artifacts.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseArtifactUpgradeData()
{
    parseData("ArtifactUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) ArtifactUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_artifactUpgradeData.find(key) != _artifactUpgradeData.end()) {
            assert(false);
        } else {
            _artifactUpgradeData.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseAttributeData()
{
    parseData("AttributeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) AttributeLocalData(item);
        const int key = data->getId();
        if (_attributes.find(key) != _attributes.end()) {
            assert(false);
        } else {
            _attributes.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseHeroData()
{
    parseData("HeroData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) HeroLocalData(item);
        const int key = data->getId();
        if (_heroes.find(key) != _heroes.end()) {
            assert(false);
        } else {
            _heroes.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseHeroPieceData()
{
    parseData("HeroPieceData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) HeroPieceData(item);
        const int key = data->getId();
        if (_heroPieceDatas.find(key) != _heroPieceDatas.end()) {
            assert(false);
        } else {
            _heroPieceDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseHeroUpgradeData()
{
    parseData("HeroUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) HeroUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_heroUpgradeDatas.find(key) != _heroUpgradeDatas.end()) {
            assert(false);
        } else {
            _heroUpgradeDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseSkillData()
{
    parseData("SkillData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SkillLocalData(item);
        const int key = data->getId();
        if (_skills.find(key) != _skills.end()) {
            assert(false);
        } else {
            _skills.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseSoldierData()
{
    parseData("SoldierData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierLocalData(item);
        const int key = data->getId();
        if (_soldiers.find(key) != _soldiers.end()) {
            assert(false);
        } else {
            _soldiers.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseSoldierPieceData()
{
    parseData("SoldierPieceData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierPieceData(item);
        const int key = data->getId();
        if (_soldierPieceDatas.find(key) != _soldierPieceDatas.end()) {
            assert(false);
        } else {
            _soldierPieceDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseSoldierUpgradeData()
{
    parseData("SoldierUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_soldierUpgradeDatas.find(key) != _soldierUpgradeDatas.end()) {
            assert(false);
        } else {
            _soldierUpgradeDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseSoldierQualityData()
{
    parseData("SoldierQualityData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierQualityData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_soldierQualityDatas.find(key) != _soldierQualityDatas.end()) {
            assert(false);
        } else {
            _soldierQualityDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseSoldierTalentData()
{
    parseData("SoldierTalentData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) SoldierTalentData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_soldierTalentDatas.find(key) != _soldierTalentDatas.end()) {
            assert(false);
        } else {
            _soldierTalentDatas.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseTowerData()
{
    parseData("TowerData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) TowerLocalData(item);
        const int key = data->getId();
        if (_towers.find(key) != _towers.end()) {
            assert(false);
        } else {
            _towers.insert(make_pair(key, data));
        }
    });
}

void DataManager::parseTowerUpgradeData()
{
    parseData("TowerUpgradeData.xml", [this](tinyxml2::XMLElement* item) {
        auto data = new (nothrow) TowerUpgradeData(item);
        auto key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
        if (_towerUpgradeDatas.find(key) != _towerUpgradeDatas.end()) {
            assert(false);
        } else {
            _towerUpgradeDatas.insert(make_pair(key, data));
        }
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
