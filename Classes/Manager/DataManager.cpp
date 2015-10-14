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
#include "TowerData.h"
#include "UnitData.h"

using namespace std;

static DataManager *s_pSharedInstance = nullptr;

DataManager* DataManager::getInstance()
{
    if (!s_pSharedInstance)
    {
        s_pSharedInstance = new (std::nothrow) DataManager();
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
    
}

void DataManager::init()
{
    parseTowersData();
    parseUnitsData();
    parseHeroesData();
}

void DataManager::parseTowersData()
{
    static string fileName("");
    if (LocalHelper::isFileExists(fileName))
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
                TowerData *data = new TowerData(item);
                _towers.insert(make_pair(data->id(), data));
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseUnitsData()
{
    static string fileName("");
    if (LocalHelper::isFileExists(fileName))
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
                UnitData *data = new UnitData(item);
                _units.insert(make_pair(data->id(), data));
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}

void DataManager::parseHeroesData()
{
    static string fileName("");
    if (LocalHelper::isFileExists(fileName))
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
                UnitData *data = new UnitData(item);
                _heroes.insert(make_pair(data->id(), data));
            }
            
            CC_SAFE_DELETE(xmlDoc);
        }
    }
}