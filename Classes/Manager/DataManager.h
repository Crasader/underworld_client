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

class QuestData;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    const QuestData* getQuestData(int questId) const;
    
protected:
    DataManager();
    virtual ~DataManager();
    CC_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    void parseQuestData();
    
private:
    std::map<int, QuestData*> _quests;
};

#endif /* DataManager_h */
