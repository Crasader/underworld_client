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
#include "GearLocalData.h"
#include "URConfigData.h"

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
    
    if (_quests.find(kQuestType_Daily) != _quests.end()) {
        Utils::clearMap(_quests.at(kQuestType_Daily));
    }
    
    if (_quests.find(kQuestType_Life) != _quests.end()) {
        Utils::clearMap(_quests.at(kQuestType_Life));
    }
    
    Utils::clearMap(_achievements);
    Utils::clearMap(_gears);
    Utils::clearMap(_animationParameters);
    Utils::clearMap(_unitResourceConfigData);
}

void DataManager::init()
{
    parseLevelData();
    parseQuestData(kQuestType_Daily);
    parseQuestData(kQuestType_Life);
    parseAchievementData();
    parseGearData();
    parseAnimationConfigData();
    parseURConfigData();
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

const URConfigData* DataManager::getURConfigData(const std::string& name) const
{
    if (_unitResourceConfigData.find(name) != _unitResourceConfigData.end()) {
        return _unitResourceConfigData.at(name);
    }
    
    return nullptr;
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