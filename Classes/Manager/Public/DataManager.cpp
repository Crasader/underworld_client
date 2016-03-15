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
#include "QuestLocalData.h"
#include "AchievementLocalData.h"
#include "ObjectLocalData.h"
#include "GearLocalData.h"
#include "GearUpgradeData.h"
#include "GearAttributeData.h"
#include "GearSetLocalData.h"
#include "CardConfigData.h"
#include "URConfigData.h"
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
{
}

DataManager::~DataManager()
{
    Utils::clearMap(_levels);
    
    for (map<QuestType, map<int, QuestLocalData*>>::iterator iter = _quests.begin(); iter != _quests.end(); ++iter) {
        Utils::clearMap(iter->second);
    }
    _quests.clear();
    
    Utils::clearMap(_achievements);
    Utils::clearMap(_gears);
    Utils::clearMap(_gearUpgradeDatas);
    Utils::clearMap(_gearAttributeDatas);
    Utils::clearMap(_gearSets);
    Utils::clearMap(_animationParameters);
    Utils::clearMap(_cardConfigData);
    Utils::clearMap(_unitResourceConfigData);
    
    for (map<int, vector<MapParticleConfigData*>>::iterator iter = _mapParticleConfigData.begin(); iter != _mapParticleConfigData.end(); ++iter) {
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
}

void DataManager::init()
{
    parseLevelData();
    parseCardDecks();
    parseQuestData(QuestType::Daily);
    parseQuestData(QuestType::Life);
    parseAchievementData();
    parseObjectData();
    parseGearData();
    parseGearUpgradeData();
    parseGearAttributeData();
    parseGearSetData();
    parseAnimationConfigData();
    parseCardConfigData();
    parseURConfigData();
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
}

string DataManager::getMapData(int mapId) const
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(StringUtils::format("MapData/%d.xml", mapId));
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            return LocalHelper::loadFileContentString(fileName);
        }
    }
    
    return "";
}

string DataManager::getTechTreeData(int mapId) const
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(StringUtils::format("TechTreeData/%d.xml", mapId));
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            return LocalHelper::loadFileContentString(fileName);
        }
    }
    
    return "";
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

const QuestLocalData* DataManager::getQuestData(QuestType type, int questId) const
{
    if (_quests.find(type) != _quests.end()) {
        const map<int, QuestLocalData*>& quests = _quests.at(type);
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
    string key = StringUtils::format("%d_%d", id, level);
    if (_gearUpgradeDatas.find(key) != _gearUpgradeDatas.end()) {
        return _gearUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const GearAttributeData* DataManager::getGearAttributeData(int id, int level, int attributeId) const
{
    string key = StringUtils::format("%d_%d_%d", id, level, attributeId);
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

const AnimationParameters& DataManager::getAnimationParameters(const string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction) const
{
    string key = name + StringUtils::format("_%d", skillClass);
    if (_animationParameters.find(key) != _animationParameters.end()) {
        UAConfigData* data = _animationParameters.at(key);
        if (data) {
            return data->getAnimationParameters(direction);
        }
    }
    
    static AnimationParameters ret;
    return ret;
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
    if (_unitResourceConfigData.find(name) != _unitResourceConfigData.end()) {
        return _unitResourceConfigData.at(name);
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
    string key = StringUtils::format("%d_%d", id, level);
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
    string key = StringUtils::format("%d_%d", id, level);
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
    string key = StringUtils::format("%d_%d", id, level);
    if (_soldierUpgradeDatas.find(key) != _soldierUpgradeDatas.end()) {
        return _soldierUpgradeDatas.at(key);
    }
    
    return nullptr;
}

const SoldierQualityData* DataManager::getSoldierQualityData(int id, int level) const
{
    string key = StringUtils::format("%d_%d", id, level);
    if (_soldierQualityDatas.find(key) != _soldierQualityDatas.end()) {
        return _soldierQualityDatas.at(key);
    }
    
    return nullptr;
}

const SoldierTalentData* DataManager::getSoldierTalentData(int id, int level) const
{
    string key = StringUtils::format("%d_%d", id, level);
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
    string key = StringUtils::format("%d_%d", id, level);
    if (_towerUpgradeDatas.find(key) != _towerUpgradeDatas.end()) {
        return _towerUpgradeDatas.at(key);
    }
    
    return nullptr;
}

#pragma mark - parsers
void DataManager::parseLevelData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("LevelData.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                LevelLocalData* data = new (nothrow) LevelLocalData(item);
                const int levelId = data->getId();
                if (_levels.find(levelId) != _levels.end()) {
                    assert(false);
                } else {
                    _levels.insert(make_pair(levelId, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseCardDecks()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("CardDecks.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                _cardDecks.insert(item->Attribute("name"));
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseQuestData(QuestType type)
{
    string fileName;
    
    if (QuestType::Daily == type) {
        fileName = LocalHelper::getLocalizedConfigFilePath("DailyQuestProperty.xml");
    } else if (QuestType::Life == type) {
        fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    }
    
    // clear first
    if (_quests.find(type) != _quests.end()) {
        Utils::clearMap(_quests.at(type));
    } else {
        _quests.insert(make_pair(type, map<int, QuestLocalData*>()));
    }
    
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            map<int, QuestLocalData*>& quests = _quests.at(type);
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                QuestLocalData* data = new (nothrow) QuestLocalData(item);
                const int questId = data->getId();
                if (quests.find(questId) != quests.end()) {
                    assert(false);
                } else {
                    quests.insert(make_pair(questId, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseAchievementData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("EquipProperty.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                AchievementLocalData* data = new (nothrow) AchievementLocalData(item);
                const int id = data->getId();
                if (_achievements.find(id) != _achievements.end()) {
                    assert(false);
                } else {
                    _achievements.insert(make_pair(id, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseObjectData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("ObjectProperty.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                ObjectLocalData* data = new (nothrow) ObjectLocalData(item);
                const int id = data->getId();
                if (_objects.find(id) != _objects.end()) {
                    assert(false);
                } else {
                    _objects.insert(make_pair(id, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseGearData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("EquipProperty.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                GearLocalData* data = new (nothrow) GearLocalData(item);
                const int gearId = data->getId();
                if (_gears.find(gearId) != _gears.end()) {
                    assert(false);
                } else {
                    _gears.insert(make_pair(gearId, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseGearUpgradeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                GearUpgradeData* data = new (nothrow) GearUpgradeData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_gearUpgradeDatas.find(key) != _gearUpgradeDatas.end()) {
                    assert(false);
                } else {
                    _gearUpgradeDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseGearAttributeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                GearAttributeData* data = new (nothrow) GearAttributeData(item);
                string key = StringUtils::format("%d_%d_%d", data->getId(), data->getLevel(), data->getAttributeId());
                if (_gearAttributeDatas.find(key) != _gearAttributeDatas.end()) {
                    assert(false);
                } else {
                    _gearAttributeDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseGearSetData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                GearSetLocalData* data = new (nothrow) GearSetLocalData(item);
                const int key = data->getId();
                if (_gearSets.find(key) != _gearSets.end()) {
                    assert(false);
                } else {
                    _gearSets.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseAnimationConfigData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("AnimationConfig.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                const char* name = item->Attribute("name");
                const char* skill = item->Attribute("skill");
                if (name && skill) {
                    UAConfigData* data = new (nothrow) UAConfigData(item);
                    string key = StringUtils::format("%s_%s", name, skill);
                    if (_animationParameters.find(key) != _animationParameters.end()) {
                        assert(false);
                    } else {
                        _animationParameters.insert(make_pair(key, data));
                    }
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseCardConfigData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("CardConfig.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                CardConfigData* data = new (nothrow) CardConfigData(item);
                const string& key = data->getName();
                if (_cardConfigData.find(key) != _cardConfigData.end()) {
                    assert(false);
                } else {
                    _cardConfigData.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseURConfigData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("UnitResourceConfig.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                URConfigData* data = new (nothrow) URConfigData(item);
                const string& key = data->getName();
                if (_unitResourceConfigData.find(key) != _unitResourceConfigData.end()) {
                    assert(false);
                } else {
                    _unitResourceConfigData.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseMapParticleConfigData()
{
    for (int i = 0; i < 2; ++i) {
        string fileName = LocalHelper::getLocalizedConfigFilePath(StringUtils::format("MapParticleConfig_%d.xml", i));
        if (FileUtils::getInstance()->isFileExist(fileName)) {
            // clear first
            if (_mapParticleConfigData.find(i) != _mapParticleConfigData.end()) {
                Utils::clearVector(_mapParticleConfigData.at(i));
            } else {
                _mapParticleConfigData.insert(make_pair(i, vector<MapParticleConfigData*>()));
            }
            
            tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
            if (xmlDoc)
            {
                string content = LocalHelper::loadFileContentString(fileName);
                xmlDoc->Parse(content.c_str());
                
                vector<MapParticleConfigData*>& configs = _mapParticleConfigData.at(i);
                
                for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                     item;
                     item = item->NextSiblingElement())
                {
                    MapParticleConfigData* data = new (nothrow) MapParticleConfigData(item);
                    configs.push_back(data);
                }
                
                CC_SAFE_DELETE(xmlDoc);
            }
        }
    }
}

void DataManager::parseSpellConfigData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("SpellConfig.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SpellConfigData* data = new (nothrow) SpellConfigData(item);
                const string& key = data->getSpellName();
                if (_spellConfigData.find(key) != _spellConfigData.end()) {
                    assert(false);
                } else {
                    if (key.length() > 0) {
                        _spellConfigData.insert(make_pair(key, data));
                    } else {
                        CC_SAFE_DELETE(data);
                    }
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseArtifactData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                ArtifactLocalData* data = new (nothrow) ArtifactLocalData(item);
                const int key = data->getId();
                if (_artifacts.find(key) != _artifacts.end()) {
                    assert(false);
                } else {
                    _artifacts.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseArtifactUpgradeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                ArtifactUpgradeData* data = new (nothrow) ArtifactUpgradeData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_artifactUpgradeData.find(key) != _artifactUpgradeData.end()) {
                    assert(false);
                } else {
                    _artifactUpgradeData.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseAttributeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                AttributeLocalData* data = new (nothrow) AttributeLocalData(item);
                const int key = data->getId();
                if (_attributes.find(key) != _attributes.end()) {
                    assert(false);
                } else {
                    _attributes.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseHeroData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                HeroLocalData* data = new (nothrow) HeroLocalData(item);
                const int key = data->getId();
                if (_heroes.find(key) != _heroes.end()) {
                    assert(false);
                } else {
                    _heroes.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseHeroPieceData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                HeroPieceData* data = new (nothrow) HeroPieceData(item);
                const int key = data->getId();
                if (_heroPieceDatas.find(key) != _heroPieceDatas.end()) {
                    assert(false);
                } else {
                    _heroPieceDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseHeroUpgradeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                HeroUpgradeData* data = new (nothrow) HeroUpgradeData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_heroUpgradeDatas.find(key) != _heroUpgradeDatas.end()) {
                    assert(false);
                } else {
                    _heroUpgradeDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseSkillData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SkillLocalData* data = new (nothrow) SkillLocalData(item);
                const int key = data->getId();
                if (_skills.find(key) != _skills.end()) {
                    assert(false);
                } else {
                    _skills.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseSoldierData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SoldierLocalData* data = new (nothrow) SoldierLocalData(item);
                const int key = data->getId();
                if (_soldiers.find(key) != _soldiers.end()) {
                    assert(false);
                } else {
                    _soldiers.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseSoldierPieceData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SoldierPieceData* data = new (nothrow) SoldierPieceData(item);
                const int key = data->getId();
                if (_soldierPieceDatas.find(key) != _soldierPieceDatas.end()) {
                    assert(false);
                } else {
                    _soldierPieceDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseSoldierUpgradeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SoldierUpgradeData* data = new (nothrow) SoldierUpgradeData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_soldierUpgradeDatas.find(key) != _soldierUpgradeDatas.end()) {
                    assert(false);
                } else {
                    _soldierUpgradeDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseSoldierQualityData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SoldierQualityData* data = new (nothrow) SoldierQualityData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_soldierQualityDatas.find(key) != _soldierQualityDatas.end()) {
                    assert(false);
                } else {
                    _soldierQualityDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseSoldierTalentData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                SoldierTalentData* data = new (nothrow) SoldierTalentData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_soldierTalentDatas.find(key) != _soldierTalentDatas.end()) {
                    assert(false);
                } else {
                    _soldierTalentDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseTowerData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                TowerLocalData* data = new (nothrow) TowerLocalData(item);
                const int key = data->getId();
                if (_towers.find(key) != _towers.end()) {
                    assert(false);
                } else {
                    _towers.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseTowerUpgradeData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new (nothrow) tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                TowerUpgradeData* data = new (nothrow) TowerUpgradeData(item);
                string key = StringUtils::format("%d_%d", data->getId(), data->getLevel());
                if (_towerUpgradeDatas.find(key) != _towerUpgradeDatas.end()) {
                    assert(false);
                } else {
                    _towerUpgradeDatas.insert(make_pair(key, data));
                }
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}
