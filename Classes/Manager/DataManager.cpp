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
#include "QuestData.h"

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
    if (_quests.find(kQuestType_Daily) != _quests.end()) {
        clearMap(_quests.at(kQuestType_Daily));
    }
    
    if (_quests.find(kQuestType_Life) != _quests.end()) {
        clearMap(_quests.at(kQuestType_Life));
    }
}

void DataManager::init()
{
    parseQuestData(kQuestType_Daily);
    parseQuestData(kQuestType_Life);
}

const QuestData* DataManager::getQuestData(QuestType type, int questId) const
{
    if (_quests.find(type) != _quests.end()) {
        const map<int, QuestData*>& quests = _quests.at(type);
        if (quests.find(questId) != quests.end()) {
            return quests.at(questId);
        }
    }
    
    return nullptr;
}

void DataManager::parseQuestData(QuestType type)
{
    string fileName;
    
    if (kQuestType_Daily == type) {
        fileName.assign("");
    } else if (kQuestType_Life == type) {
        fileName.assign("");
    }
    
    // clear first
    if (_quests.find(type) != _quests.end()) {
        clearMap(_quests.at(type));
    } else {
        _quests.insert(make_pair(type, map<int, QuestData*>()));
    }
    
    if (LocalHelper::isFileExists(fileName))
    {
        tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
        if (xmlDoc)
        {
            string content = LocalHelper::loadFileContentString(fileName);
            xmlDoc->Parse(content.c_str());
            
            map<int, QuestData*>& quests = _quests.at(type);
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
                 item;
                 item = item->NextSiblingElement())
            {
                QuestData* data = new (nothrow) QuestData(item);
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
