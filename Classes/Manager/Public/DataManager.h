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

class LevelProperty;
class CardProperty;
class CardUpgradeProperty;
class RuneProperty;
class RuneUpgradeProperty;
class SkillProperty;
class SkillUpgradeProperty;
class SkillBookProperty;
class QuestProperty;
class AchievementProperty;
class ObjectProperty;
class AnimationParameter;
class UAConfigData;
class CardConfigData;
class URConfigData;
class BRConfigData;
class MapParticleConfigData;
class SpellConfigData;
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
    const LevelProperty* getLevelProperty(int levelId) const;
    const std::set<int>& getCardDecks() const;
    
    // ========== development ==========
    // ---------- card ----------
    const CardProperty* getCardProperty(int cardId) const;
    const CardUpgradeProperty* getCardUpgradeProperty(int cardId, int level) const;
    
    // ---------- rune ----------
    const RuneProperty* getRuneProperty(int runeId) const;
    const RuneUpgradeProperty* getRuneUpgradeProperty(int runeId, int level) const;
    
    // ---------- skill ----------
    const SkillProperty* getSkillProperty(int skillId) const;
    const SkillUpgradeProperty* getSkillUpgradeProperty(int skillId, int level) const;
    const SkillBookProperty* getSkillBookProperty(int bookId) const;
    
    const QuestProperty* getQuestProperty(QuestType type, int questId) const;
    const AchievementProperty* getAchievementProperty(int achievementId) const;
    const ObjectProperty* getObjectProperty(int objectId) const;
    
    // ========== animations ==========
    const AnimationParameter* getAnimationParameter(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction) const;
    const CardConfigData* getCardConfigData(int idx) const;
    const URConfigData* getURConfigData(const std::string& name) const;
    const BRConfigData* getBRConfigData(const std::string& name) const;
    const std::vector<MapParticleConfigData*>& getMapParticleConfigData(int mapId) const;
    const SpellConfigData* getSpellConfigData(const std::string& name) const;
    
    // ========== tools ==========
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
    void parseLevelProperty();
    void parseCardDecks();
    void parseCardProperty();
    void parseCardUpgradeProperty();
    void parseRuneProperty();
    void parseRuneUpgradeProperty();
    void parseSkillProperty();
    void parseSkillUpgradeProperty();
    void parseSkillBookProperty();
    void parseQuestProperty();
    void parseAchievementProperty();
    void parseObjectProperty();
    void parseAnimationConfigData();
    void parseCardConfigData();
    void parseURConfigData();
    void parseBRConfigData();
    void parseMapParticleConfigData();
    void parseSpellConfigData();
    
    // ---------- tools ----------
    void parseBinaryjsonTemplates();
private:
    std::vector<std::string> _pvrFiles;
    std::unordered_map<int, LevelProperty*> _levels;
    std::set<int> _cardDecks;
    std::unordered_map<int, CardProperty*> _cards;
    std::unordered_map<std::string, CardUpgradeProperty*> _cardUpgradeProperties;
    std::unordered_map<int, RuneProperty*> _runes;
    std::unordered_map<std::string, RuneUpgradeProperty*> _runeUpgradeProperties;
    std::unordered_map<int, SkillProperty*> _skills;
    std::unordered_map<std::string, SkillUpgradeProperty*> _skillUpgradeProperties;
    std::unordered_map<int, SkillBookProperty*> _skillBooks;
    std::map<QuestType, std::unordered_map<int, QuestProperty*>> _quests;
    std::unordered_map<int, AchievementProperty*> _achievements;
    std::unordered_map<int, ObjectProperty*> _objects;
    std::unordered_map<std::string, UAConfigData*> _animationParameters;
    std::unordered_map<int, CardConfigData*> _cardConfigData;
    std::unordered_map<std::string, URConfigData*> _urConfigData;
    std::unordered_map<std::string, BRConfigData*> _brConfigData;
    std::unordered_map<int, std::vector<MapParticleConfigData*>> _mapParticleConfigData;
    std::unordered_map<std::string, SpellConfigData*> _spellConfigData;
    BinaryJsonTool* _binaryJsonTool;
    UnderWorld::Core::TechTree* _techTree;
    UnderWorld::Core::GameModeHMM* _gameModeHMM;
};

#endif /* DataManager_h */
