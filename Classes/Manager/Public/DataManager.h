//
//  DataManager.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef DataManager_h
#define DataManager_h

#include "CocosGlobal.h"
#include "Unit.h"
#include "SkllType.h"
#include <vector>
#include <unordered_map>

namespace UnderWorld { namespace Core {
    class TechTree;
    class GameModeHMM;
}}

class LevelLocalData;
class CardUpgradeData;
class TalentUpgradeData;
class QuestLocalData;
class AchievementLocalData;
class ObjectLocalData;
class AnimationParameter;
class UAConfigData;
class CardConfigData;
class URConfigData;
class BRConfigData;
class MapParticleConfigData;
class SpellConfigData;
class SkillLocalData;
class BinaryJsonTool;

class DataManager
{
public:
    static DataManager* getInstance();
    static void purge();
    
    void init();
    std::string getMapData(int mapId) const;
    std::string getMapDataHMM(int mapId) const;
    std::string getTechTreeData() const;
    std::string getHMMTechTreeData() const;
    
    const std::vector<std::string>& getPVRFiles() const;
    const LevelLocalData* getLevelData(int levelId) const;
    const std::set<int>& getCardDecks() const;
    const CardUpgradeData* getCardUpgradeData(int idx, int level) const;
    const TalentUpgradeData* getTalentUpgradeData(int idx, int level) const;
    const QuestLocalData* getQuestData(QuestType type, int questId) const;
    const AchievementLocalData* getAchievementData(int achievementId) const;
    const ObjectLocalData* getObjectData(int objectId) const;
    
    // ---------- animations ----------
    const AnimationParameter* getAnimationParameter(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction) const;
    const CardConfigData* getCardConfigData(int idx) const;
    const URConfigData* getURConfigData(const std::string& name) const;
    const BRConfigData* getBRConfigData(const std::string& name) const;
    const std::vector<MapParticleConfigData*>& getMapParticleConfigData(int mapId) const;
    const SpellConfigData* getSpellConfigData(const std::string& name) const;
    
    // ---------- skill ----------
    const SkillLocalData* getSkillData(int id) const;
    
    // ---------- binaryjson ----------
    const BinaryJsonTool* getBinaryJsonTool() const;
    UnderWorld::Core::TechTree* getTechTree() const;
    UnderWorld::Core::GameModeHMM* getGameModeHMM() const;
    float getBattleResourceMaxCount() const;
    
protected:
    DataManager();
    virtual ~DataManager();
    M_DISALLOW_COPY_AND_ASSIGN(DataManager);
    
protected:
    string getFileData(const std::string& file) const;
    void parseData(const std::string& file, const std::function<void(tinyxml2::XMLElement*)>& parser) const;
    void parsePvrFiles();
    void parseLevelData();
    void parseCardDecks();
    void parseCardUpgradeData();
    void parseTalentUpgradeData();
    void parseQuestData();
    void parseAchievementData();
    void parseObjectData();
    void parseAnimationConfigData();
    void parseCardConfigData();
    void parseURConfigData();
    void parseBRConfigData();
    void parseMapParticleConfigData();
    void parseSpellConfigData();
    void parseSkillData();
    
    // ---------- binaryjson ----------
    void parseBinaryjsonTemplates();
private:
    std::vector<std::string> _pvrFiles;
    std::unordered_map<int, LevelLocalData*> _levels;
    std::set<int> _cardDecks;
    std::unordered_map<std::string, CardUpgradeData*> _cardUpgradeDatas;
    std::unordered_map<std::string, TalentUpgradeData*> _talentUpgradeDatas;
    std::map<QuestType, std::unordered_map<int, QuestLocalData*>> _quests;
    std::unordered_map<int, AchievementLocalData*> _achievements;
    std::unordered_map<int, ObjectLocalData*> _objects;
    std::unordered_map<std::string, UAConfigData*> _animationParameters;
    std::unordered_map<int, CardConfigData*> _cardConfigData;
    std::unordered_map<std::string, URConfigData*> _urConfigData;
    std::unordered_map<std::string, BRConfigData*> _brConfigData;
    std::unordered_map<int, std::vector<MapParticleConfigData*>> _mapParticleConfigData;
    std::unordered_map<std::string, SpellConfigData*> _spellConfigData;
    std::unordered_map<int, SkillLocalData*> _skills;
    BinaryJsonTool* _binaryJsonTool;
    UnderWorld::Core::TechTree* _techTree;
    UnderWorld::Core::GameModeHMM* _gameModeHMM;
};

#endif /* DataManager_h */
