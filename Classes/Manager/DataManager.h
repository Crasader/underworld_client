//
//  DataManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef DataManager_h
#define DataManager_h

#include "cocos2d.h"

class TowerData;
class UnitData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    
protected:
    DataManager();
    virtual ~DataManager();
    CC_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    void parseTowersData();
    void parseUnitsData();
    void parseHeroesData();
    
private:
    std::map<int, TowerData*> _towers;
    std::map<int, UnitData*> _units;
    std::map<int, UnitData*> _heroes;
};

#endif /* DataManager_h */
