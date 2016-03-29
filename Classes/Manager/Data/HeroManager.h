//
//  HeroManager.h
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef HeroManager_h
#define HeroManager_h

#include "json/document.h"
#include <unordered_map>
#include "CocosGlobal.h"

class HeroData;
class ArtifactData;

class HeroManager
{
public:
    HeroManager();
    virtual ~HeroManager();
    
    void init(const rapidjson::Value& jsonDict);
    void upgrade(int id);
    void setArtifact(int id, int index, const ArtifactData* artifact);
    void upgradeArtifact(int id, int index);
    
    const HeroData* getHeroData(int id);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(HeroManager);
    
private:
    std::unordered_map<int, HeroData*> _heroes;
};

#endif /* HeroManager_h */
