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
#include <unordered_map>

namespace UnderWorld { namespace Core {
    class TechTree;
    class GameModeHMM;
}}

class LevelLocalData;
class CardLocalData;
class CardUpgradeData;
class TalentUpgradeData;
class QuestLocalData;
class AchievementLocalData;
class ObjectLocalData;
class GearLocalData;
class GearUpgradeData;
class GearAttributeData;
class GearSetLocalData;
class AnimationParameter;
class UAConfigData;
class CardConfigData;
class URConfigData;
class BRConfigData;
class MapParticleConfigData;
class SpellConfigData;
class ArtifactLocalData;
class ArtifactUpgradeData;
class AttributeLocalData;
class HeroLocalData;
class HeroPieceData;
class HeroUpgradeData;
class SkillLocalData;
class SoldierLocalData;
class SoldierPieceData;
class SoldierUpgradeData;
class SoldierQualityData;
class SoldierTalentData;
class TowerLocalData;
class TowerUpgradeData;
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
    
    const LevelLocalData* getLevelData(int levelId) const;
    const std::set<std::string>& getCardDecks() const;
    const CardLocalData* getCardData(int idx) const;
    const CardUpgradeData* getCardUpgradeData(int idx, int level) const;
    const TalentUpgradeData* getTalentUpgradeData(int idx, int level) const;
    const QuestLocalData* getQuestData(QuestType type, int questId) const;
    const AchievementLocalData* getAchievementData(int achievementId) const;
    const ObjectLocalData* getObjectData(int objectId) const;
    
    // ---------- gear ----------
    const GearLocalData* getGearData(int gearId) const;
    const GearUpgradeData* getGearUpgradeData(int id, int level) const;
    const GearAttributeData* getGearAttributeData(int id, int level, int attributeId) const;
    const GearSetLocalData* getGearSetData(int id) const;
    
    // ---------- animations ----------
    const AnimationParameter* getAnimationParameter(const std::string& name, UnderWorld::Core::SkillClass skillClass, UnderWorld::Core::Unit::Direction direction) const;
    const CardConfigData* getCardConfigData(const std::string& name) const;
    const URConfigData* getURConfigData(const std::string& name) const;
    const BRConfigData* getBRConfigData(const std::string& name) const;
    const std::vector<MapParticleConfigData*>& getMapParticleConfigData(int mapId) const;
    const SpellConfigData* getSpellConfigData(const std::string& name) const;
    
    // ---------- artifact ----------
    const ArtifactLocalData* getArtifactData(int id) const;
    const ArtifactUpgradeData* getArtifactUpgradeData(int id, int level) const;
    
    // ---------- attribute ----------
    const AttributeLocalData* getAttributeData(int id) const;
    
    // ---------- hero ----------
    const HeroLocalData* getHeroData(int id) const;
    const HeroPieceData* getHeroPieceData(int id) const;
    const HeroUpgradeData* getHeroUpgradeData(int id, int level) const;
    
    // ---------- skill ----------
    const SkillLocalData* getSkillData(int id) const;
    
    // ---------- soldier ----------
    const SoldierLocalData* getSoldierData(int id) const;
    const SoldierPieceData* getSoldierPieceData(int id) const;
    const SoldierUpgradeData* getSoldierUpgradeData(int id, int level) const;
    const SoldierQualityData* getSoldierQualityData(int id, int level) const;
    const SoldierTalentData* getSoldierTalentData(int id, int level) const;
    
    // ---------- tower ----------
    const TowerLocalData* getTowerData(int id) const;
    const TowerUpgradeData* getTowerUpgradeData(int id, int level) const;
    
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
    void parseLevelData();
    void parseCardDecks();
    void parseCardData();
    void parseCardUpgradeData();
    void parseTalentUpgradeData();
    void parseQuestData();
    void parseAchievementData();
    void parseObjectData();
    void parseGearData();
    void parseGearUpgradeData();
    void parseGearAttributeData();
    void parseGearSetData();
    void parseAnimationConfigData();
    void parseCardConfigData();
    void parseURConfigData();
    void parseBRConfigData();
    void parseMapParticleConfigData();
    void parseSpellConfigData();
    void parseArtifactData();
    void parseArtifactUpgradeData();
    void parseAttributeData();
    void parseHeroData();
    void parseHeroPieceData();
    void parseHeroUpgradeData();
    void parseSkillData();
    void parseSoldierData();
    void parseSoldierPieceData();
    void parseSoldierUpgradeData();
    void parseSoldierQualityData();
    void parseSoldierTalentData();
    void parseTowerData();
    void parseTowerUpgradeData();
    
    // ---------- binaryjson ----------
    void parseBinaryjsonTemplates();
private:
    std::unordered_map<int, LevelLocalData*> _levels;
    std::set<std::string> _cardDecks;
    std::unordered_map<int, CardLocalData*> _cards;
    std::unordered_map<std::string, CardUpgradeData*> _cardUpgradeDatas;
    std::unordered_map<std::string, TalentUpgradeData*> _talentUpgradeDatas;
    std::map<QuestType, std::unordered_map<int, QuestLocalData*>> _quests;
    std::unordered_map<int, AchievementLocalData*> _achievements;
    std::unordered_map<int, ObjectLocalData*> _objects;
    std::unordered_map<int, GearLocalData*> _gears;
    std::unordered_map<std::string, GearUpgradeData*> _gearUpgradeDatas;
    std::unordered_map<std::string, GearAttributeData*> _gearAttributeDatas;
    std::unordered_map<int, GearSetLocalData*> _gearSets;
    std::unordered_map<std::string, UAConfigData*> _animationParameters;
    std::unordered_map<std::string, CardConfigData*> _cardConfigData;
    std::unordered_map<std::string, URConfigData*> _urConfigData;
    std::unordered_map<std::string, BRConfigData*> _brConfigData;
    std::unordered_map<int, std::vector<MapParticleConfigData*>> _mapParticleConfigData;
    std::unordered_map<std::string, SpellConfigData*> _spellConfigData;
    std::unordered_map<int, ArtifactLocalData*> _artifacts;
    std::unordered_map<std::string, ArtifactUpgradeData*> _artifactUpgradeData;
    std::unordered_map<int, AttributeLocalData*> _attributes;
    std::unordered_map<int, HeroLocalData*> _heroes;
    std::unordered_map<int, HeroPieceData*> _heroPieceDatas;
    std::unordered_map<std::string, HeroUpgradeData*> _heroUpgradeDatas;
    std::unordered_map<int, SkillLocalData*> _skills;
    std::unordered_map<int, SoldierLocalData*> _soldiers;
    std::unordered_map<int, SoldierPieceData*> _soldierPieceDatas;
    std::unordered_map<std::string, SoldierUpgradeData*> _soldierUpgradeDatas;
    std::unordered_map<std::string, SoldierQualityData*> _soldierQualityDatas;
    std::unordered_map<std::string, SoldierTalentData*> _soldierTalentDatas;
    std::unordered_map<int, TowerLocalData*> _towers;
    std::unordered_map<std::string, TowerUpgradeData*> _towerUpgradeDatas;
    BinaryJsonTool* _binaryJsonTool;
    UnderWorld::Core::TechTree* _techTree;
    UnderWorld::Core::GameModeHMM* _gameModeHMM;
};

#endif /* DataManager_h */
