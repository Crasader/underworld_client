//
//  DataManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "DataManager.h"
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#include "Utils.h"
#include "LocalHelper.h"
#include "LevelLocalData.h"
#include "QuestLocalData.h"
#include "AchievementLocalData.h"
#include "ObjectLocalData.h"
#include "GearLocalData.h"
#include "URConfigData.h"
#include "MapParticleConfigData.h"

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
    Utils::clearMap(_animationParameters);
    Utils::clearMap(_unitResourceConfigData);
    
    for (map<int, vector<MapParticleConfigData*>>::iterator iter = _mapParticleConfigData.begin(); iter != _mapParticleConfigData.end(); ++iter) {
        Utils::clearVector(iter->second);
    }
    _mapParticleConfigData.clear();
}

void DataManager::init()
{
    parseLevelData();
    parseQuestData(kQuestType_Daily);
    parseQuestData(kQuestType_Life);
    parseAchievementData();
    parseObjectData();
    parseGearData();
    parseAnimationConfigData();
    parseURConfigData();
    parseMapParticleConfigData();
}

#pragma mark - getters
const LevelLocalData* DataManager::getLevelData(int levelId) const
{
    if (_levels.find(levelId) != _levels.end()) {
        return _levels.at(levelId);
    }
    
    return nullptr;
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

AnimationParameters DataManager::getAnimationParameters(const string& name, UnderWorld::Core::SkillClass skillClass, UnitDirection direction) const
{
    string key = name + StringUtils::format("_%d", skillClass);
    if (_animationParameters.find(key) != _animationParameters.end()) {
        UAConfigData* data = _animationParameters.at(key);
        if (data) {
            return data->getAnimationParameters(direction);
        }
    }
    
    return {1.0f, 1.0f};
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

#pragma mark - parsers
void DataManager::parseLevelData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("LevelData.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                LevelLocalData* data = new (nothrow) LevelLocalData(item);
                const int levelId = data->getLevelId();
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

void DataManager::parseQuestData(QuestType type)
{
    string fileName;
    
    if (kQuestType_Daily == type) {
        fileName = LocalHelper::getLocalizedConfigFilePath("DailyQuestProperty.xml");
    } else if (kQuestType_Life == type) {
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
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
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
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
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
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                ObjectLocalData* data = new (nothrow) ObjectLocalData(item);
                const int id = data->getObjectId();
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
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
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

void DataManager::parseAnimationConfigData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("AnimationConfig.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
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

void DataManager::parseURConfigData()
{
    string fileName = LocalHelper::getLocalizedConfigFilePath("UnitResourceConfig.xml");
    if (FileUtils::getInstance()->isFileExist(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
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
            
            tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
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