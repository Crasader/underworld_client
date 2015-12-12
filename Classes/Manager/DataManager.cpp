//
//  DataManager.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "DataManager.h"
#include "tinyxml2/tinyxml2.h"
#include "LocalHelper.h"
#include "LevelLocalData.h"
#include "QuestLocalData.h"
#include "URConfigData.h"

USING_NS_CC;
using namespace std;

template<typename _type>
void clearVector(vector<_type> &vec)
{
    for(typename vector<_type>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
    {
        CC_SAFE_DELETE(*iter);
    }
    vec.clear();
}

template<typename _Key, typename _Value>
void clearMap(map<_Key, _Value> &m)
{
    for(typename map<_Key, _Value>::iterator iter = m.begin(); iter != m.end(); ++iter)
    {
        CC_SAFE_DELETE(iter->second);
    }
    m.clear();
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
{
}

DataManager::~DataManager()
{
    clearMap(_levels);
    
    if (_quests.find(kQuestType_Daily) != _quests.end()) {
        clearMap(_quests.at(kQuestType_Daily));
    }
    
    if (_quests.find(kQuestType_Life) != _quests.end()) {
        clearMap(_quests.at(kQuestType_Life));
    }
    
    clearMap(_animationParameters);
    clearMap(_unitResourceConfigData);
}

void DataManager::init()
{
    parseLevelData();
    parseQuestData(kQuestType_Daily);
    parseQuestData(kQuestType_Life);
    parseAnimationConfigData();
    parseURConfigData();
}

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
        fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    } else if (kQuestType_Life == type) {
        fileName = LocalHelper::getLocalizedConfigFilePath(".xml");
    }
    
    // clear first
    if (_quests.find(type) != _quests.end()) {
        clearMap(_quests.at(type));
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