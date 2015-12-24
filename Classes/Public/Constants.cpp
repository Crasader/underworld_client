//
//  Constants.cpp
//  ZombiePop
//
//  Created by Andy on 15/12/10.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "Constants.h"
#include "tinyxml2/tinyxml2.h"
#include "Global.h"
#include "LocalHelper.h"

int Constants::ISLAND_TOTAL_COUNT = 0;
int Constants::STAGE_COUNT_PER_ISLAND = 0;
float Constants::TILEDMAP_MAX_SCALE = 0;

void Constants::init()
{
    static std::string file("configs/Constants.xml");
    if (LocalHelper::isFileExists(file))
    {
        tinyxml2::XMLDocument *xmlDoc = new (std::nothrow) tinyxml2::XMLDocument();
        if (xmlDoc) {
            std::string content = LocalHelper::loadFileContentString(file);
            xmlDoc->Parse(content.c_str());
            
            // define
#define ATOI(name) { const char* data = item->Attribute(#name); if (data) {name = atoi(data);} }
#define ATOF(name) { const char* data = item->Attribute(#name); if (data) {name = atof(data);} }
            
            for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement(); item; item = item->NextSiblingElement()) {
                if (item) {
                    ATOI(ISLAND_TOTAL_COUNT)
                    ATOI(STAGE_COUNT_PER_ISLAND)
                    ATOF(TILEDMAP_MAX_SCALE)
                }
            }
#if false
            CCLOG("%d", ISLAND_TOTAL_COUNT);
            CCLOG("%d", STAGE_COUNT_PER_ISLAND);
#endif
            
            M_SAFE_DELETE(xmlDoc);
        }
    }
}