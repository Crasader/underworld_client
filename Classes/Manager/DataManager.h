//
//  DataManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef DataManager_h
#define DataManager_h

class TowerData;
class UnitData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
protected:
    DataManager();
    virtual ~DataManager();
    DataManager(DataManager const&) = delete;
    void operator=(DataManager const&) = delete;
    
protected:
    
private:
    std::map<int, TowerData*> _towers;
    std::map<int, UnitData*> _units;
    std::map<int, UnitData*> _heros;
};

#endif /* DataManager_h */
