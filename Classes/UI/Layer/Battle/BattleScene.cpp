//
//  BattleScene.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleScene.h"
#include "GameSettings.h"
#include "SoundManager.h"

BattleScene* BattleScene::create(int mapId)
{
    BattleScene *ret = new (std::nothrow) BattleScene();
    if (ret && ret->init(mapId))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

BattleScene::BattleScene()
:_mapId(0)
,_render(nullptr)
,_looper(nullptr)
,_sch(nullptr)
{
    
}

BattleScene::~BattleScene()
{
    clear();
}

// on "init" you need to initialize your instance
bool BattleScene::init(int mapId)
{
    if (Scene::init() )
    {
        _mapId = mapId;
        return true;
    }
    
    return false;
}

void BattleScene::onEnter()
{
    Scene::onEnter();
    start(_mapId);
}

void BattleScene::onExit()
{
    SoundManager::getInstance()->stopBackgroundMusic();
    Scene::onExit();
}

#pragma mark - GameRenderObserver
void BattleScene::onGameRenderRestart()
{
    start(_mapId);
}

void BattleScene::start(int mapId)
{
    if (_render) {
        clear();
    }
    
    // 1. add map layer
    string mapSettingXml = "<root>\
    <location_setting index=\"0\" core_location=\"200,550\" building_location=\"1000,800;1700,800\" assemble_location=\"0:500,600;1:600,600\"/>\
    <resource_settings index=\"0\">\
    <resource_setting resource_name=\"金子\" init_balance=\"3000\" init_salary=\"5\" salary_accelerate=\"0\"/>\
    <resource_setting resource_name=\"木头\" init_balance=\"3000\" init_salary=\"5\" salary_accelerate=\"0\"/>\
    <resource_setting resource_name=\"人口\" init_balance=\"100\" init_salary=\"0\" salary_accelerate=\"0\"/>\
    </resource_settings>\
    <fixed_unit_settings index=\"0\">\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"时光女神\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"狼人森林之魂\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"天空女神\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"时光法师\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"狼人射手\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"狼人巫师\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"950,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"狼人步兵\"/>\
    </fixed_unit_setting>\
    </fixed_unit_settings>\
    <location_setting index=\"1\" core_location=\"4400,550\" building_location=\"3600,800;2900,800\" assemble_location=\"0:4100,600;2:4000,600\"/>\
    <resource_settings index=\"1\">\
    <resource_setting resource_name=\"金子\" init_balance=\"0\" init_salary=\"0\" salary_accelerate=\"0\"/>\
    <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/>\
    </resource_settings>\
    <fixed_unit_settings index=\"1\">\
    <fixed_unit_setting position=\"4000,550\" count=\"10\" wave=\"1\">\
    <unit_setting name=\"吸血鬼战士\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"4000,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"吸血鬼弓箭手\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"4000,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"巨型蜘蛛\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"4000,550\" count=\"1\" wave=\"1\">\
    <unit_setting name=\"蛇女莉莉丝\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"4000,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"七恶魔拉玛什图\"/>\
    </fixed_unit_setting>\
    <fixed_unit_setting position=\"4000,550\" count=\"0\" wave=\"1\">\
    <unit_setting name=\"吸血鬼巫师\"/>\
    </fixed_unit_setting>\
    </fixed_unit_settings>\
    </root>";
    
    // 2. add map ui layer
    _render = new (nothrow) GameRender(this, mapId, mapSettingXml, "Vampire");
    _render->registerObserver(this);
    
    _sch = new (nothrow) GameScheduler();
    
    UnderWorld::Core::GameSettings setting;
    
    //1. set map setting
    setting.setMap(_render->getMapSetting());
    
    //2. set techTree;
    
    std::string techTree = "<root>\
    <!-- ==========单位&势力配置========== -->\
    <faction name=\"狼人族\">\
    <unit name=\"狼人基地\" class=\"core\" hp=\"600\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"0\" attacksight=\"800\" size=\"300\" height=\"100\" render_key=\"狼人基地\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\" occupy=\"0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"2.0\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"800\" fields=\"3\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    </unit>\
    <unit name=\"狼人箭塔\" class=\"building\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"0\" attacksight=\"800\" size=\"100\" height=\"50\" render_key=\"狼人箭塔\" reward=\"金子:200\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.5\" min_damage=\"20\" max_damage=\"40\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"800\" fields=\"3\">\
    <bullet speed=\"800\" size=\"0\"/>\
    </skill>\
    </unit>\
    <!-- 力量号角 -->\
    <unit name=\"时光女神\" class=\"hero\" hp=\"550\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"英雄\" armor=\"0\" magical_defense=\"0.f\" attacksight=\"600\" size=\"75\" height=\"0\" cost=\"金子:225;木头:750;人口:5\" priority=\"1\" max_level=\"100\" spell_names=\"全体恢复_等级1\" render_key=\"时光女神\" produce=\"木头:225\" reward=\"金子:2\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.5\" min_damage=\"28\" max_damage=\"32\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"600\" fields=\"3\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 爆 -->\
    <unit name=\"狼人森林之魂\" class=\"hero\" hp=\"550\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"英雄\" armor=\"0\" magical_defense=\"0.f\" attacksight=\"600\" size=\"75\" height=\"0\" cost=\"金子:100;木头:100;人口:5\" priority=\"1\" max_level=\"100\" spell_names=\"火球术_等级1\" render_key=\"狼人森林之魂\" produce=\"木头:100\" reward=\"金子:1\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.5\" min_damage=\"28\" max_damage=\"32\" deliver_class=\"magical\" damage_distance=\"faraway\" range=\"600\" fields=\"3\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 战鼓 -->\
    <unit name=\"天空女神\" class=\"hero\" hp=\"550\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"英雄\" armor=\"0\" attacksight=\"600\" size=\"75\" height=\"0\" cost=\"金子:650;人口:5\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" spell_names=\"激励_等级1\" render_key=\"天空女神\" produce=\"木头:650\" reward=\"金子:6\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"0.8\" min_damage=\"28\" max_damage=\"32\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"600\" fields=\"3\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 拥护 -->\
    <unit name=\"狼人步兵\" class=\"warrior\" hp=\"575\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"700\" armor_preference=\"中甲\" armor_preference_factor=\"1.5f\" attacksight=\"400\" size=\"60\" height=\"0\" cost=\"金子:275;人口:2\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"狼人步兵\" produce=\"木头:275\" reward=\"金子:2\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.4\" min_damage=\"45\" max_damage=\"60\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"35\" fields=\"1\">\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 十字军 -->\
    <unit name=\"狼人机械狼\" class=\"warrior\" hp=\"575\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"600\" attacksight=\"400\" size=\"60\" height=\"0\" cost=\"金子:280;人口:2\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" spell_names=\"王者祝福_等级1\" render_key=\"狼人机械狼\" produce=\"木头:280\" reward=\"金子:2\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.4\" min_damage=\"50\" max_damage=\"70\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"35\" fields=\"1\">\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 增幅 -->\
    <unit name=\"狼人射手\" class=\"warrior\" hp=\"500\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"0\" armor_preference=\"无甲\" armor_preference_factor=\"1.5f\" attacksight=\"600\" size=\"25\" height=\"0\" cost=\"金子:320;人口:2\" priority=\"0\" passive_names=\"增幅_等级1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"狼人射手\" produce=\"木头:320\" reward=\"金子:3\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.5\" min_damage=\"60\" max_damage=\"100\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"550\" fields=\"1\">\
    <bullet speed=\"800\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 迫击炮小队 -->\
    <unit name=\"狼人巫师\" class=\"warrior\" hp=\"280\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"0\" armor_preference=\"城甲\" armor_preference_factor=\"1.5f\" attacksight=\"1100\" size=\"40\" height=\"0\" cost=\"金子:210;人口:2\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"狼人巫师\" produce=\"木头:210\" reward=\"金子:2\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"3.5\" min_damage=\"68\" max_damage=\"80\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"1000\" fields=\"1\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 术士 -->\
    <unit name=\"时光法师\" class=\"warrior\" hp=\"320\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"无甲\" armor=\"100\" attacksight=\"600\" size=\"60\" height=\"0\" cost=\"金子:300;人口:3\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"时光法师\" produce=\"木头:3\" reward=\"金子:3\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"2\" min_damage=\"30\" max_damage=\"33\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"160\" fields=\"1\">\
    <bullet speed=\"800\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    </faction>\
    <faction name=\"吸血鬼族\">\
    <unit name=\"吸血鬼基地\" class=\"core\" hp=\"600\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"0\" attacksight=\"600\" size=\"300\" height=\"0\" render_key=\"吸血鬼核心\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\" occupy=\"0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.0\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"600\" fields=\"3\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    </unit>\
    <unit name=\"吸血鬼箭塔\" class=\"building\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"0\" attacksight=\"600\" size=\"100\" height=\"70\" render_key=\"吸血鬼箭塔\" reward=\"金子:100;木头:20\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.3\" perform=\"0.0\" cd=\"1.0\" min_damage=\"8\" max_damage=\"12\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"300\" fields=\"3\">\
    <bullet speed=\"800\" size=\"0\"/>\
    </skill>\
    </unit>\
    <!-- 战争古树 -->\
    <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"640\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"500\" armor_preference=\"无甲\" armor_preference_factor=\"1.5f\" attacksight=\"600\" size=\"50\" height=\"0\" cost=\"金子:250;人口:2\" priority=\"0\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" spell_names=\"冲锋_等级1\" render_key=\"吸血鬼战士\" produce=\"木头:250\" reward=\"金子:3\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.4\" min_damage=\"50\" max_damage=\"55\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"30\" fields=\"1\">\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 虚空守护者 -->\
    <unit name=\"吸血鬼弓箭手\" class=\"warrior\" hp=\"740\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"400\" armor_preference=\"重甲\" armor_preference_factor=\"1.5f\"  attacksight=\"600\" size=\"25\" height=\"0\" cost=\"金子:380;人口:2\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"吸血鬼弓兵\" produce=\"木头:2\" reward=\"金子:2\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"2.5\" perform=\"0.0\" cd=\"1.6\" min_damage=\"75\" max_damage=\"85\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"400\" fields=\"3\">\
    <bullet speed=\"800\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 地狱坦克 -->\
    <unit name=\"巨型蜘蛛\" class=\"warrior\" hp=\"850\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"400\" armor_preference=\"城甲\" armor_preference_factor=\"1.5f\" attacksight=\"800\" size=\"50\" height=\"0\" cost=\"金子:450;人口:4\" priority=\"0\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"刚毅_等级1\" render_key=\"吸血鬼蜘蛛\" produce=\"木头:4\" reward=\"金子:4\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.45\" min_damage=\"70\" max_damage=\"90\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"600\" fields=\"1\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 堕落之眼 -->\
    <unit name=\"蛇女莉莉丝\" class=\"hero\" hp=\"550\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"500\" attacksight=\"500\" size=\"75\" height=\"0\" cost=\"金子:500;木头:1350;人口:5\" priority=\"1\" max_level=\"100\" spell_names=\"强风护盾_等级1\" render_key=\"莉莉丝\" produce=\"木头:5\" reward=\"金子:5\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.0\" min_damage=\"55\" max_damage=\"75\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"450\" fields=\"3\">\
    <bullet speed=\"800\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 毁灭古树 -->\
    <unit name=\"七恶魔拉玛什图\" class=\"hero\" hp=\"1800\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"8\" attacksight=\"400\" size=\"70\" height=\"0\" cost=\"金子:750;木头:350;人口:4\" priority=\"1\" max_level=\"100\" render_key=\"七恶魔拉玛什图\" passive_names=\"闪电攻击_等级1\" produce=\"木头:4\" reward=\"金子:4\" armor_preference=\"轻甲\" armor_preference_factor=\"1.5f\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.3\" min_damage=\"140\" max_damage=\"160\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"120\" fields=\"1\"/>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    <!-- 恶魔雕像 -->\
    <unit name=\"吸血鬼巫师\" class=\"warrior\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"中甲\" armor=\"400\" attacksight=\"500\" size=\"25\" height=\"0\" cost=\"金子:520;人口:2\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"血雾_等级1\" render_key=\"吸血鬼巫师\"  produce=\"木头:2\" reward=\"金子:2\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"0.5\" perform=\"0.0\" cd=\"1.6\" min_damage=\"90\" max_damage=\"115\" deliver_class=\"magical\" damage_distance=\"faraway\" range=\"480\" fields=\"3\">\
    <bullet speed=\"600\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"130\"/>\
    </unit>\
    </faction>\
    <!-- ==========资源类别配置========== -->\
    <resource name=\"金子\" class=\"consumable\"/>\
    <resource name=\"木头\" class=\"consumable\"/>\
    <resource name=\"人口\" class=\"holdable\"/>\
    <!-- ==========护甲类别配置&护甲规则配置========== -->\
    <armor name=\"英雄\"/>\
    <armor name=\"轻甲\"/>\
    <armor name=\"重甲\"/>\
    <armor name=\"城甲\"/>\
    <armor name=\"无甲\"/>\
    <armor name=\"中甲\"/>\
    <armor_rule armor_base=\"1000\"/>\
    <!-- ==========效果分类配置========== -->\
    <effect_alias name=\"减移动速度\" unique=\"1\"/>\
    <effect_alias name=\"加移动速度\"/>\
    <effect_alias name=\"减攻击速度\" unique=\"1\"/>\
    <effect_alias name=\"加攻击速度\"/>\
    <effect_alias name=\"减受到伤害\" unique=\"1\"/>\
    <effect_alias name=\"加受到伤害\"/>\
    <effect_alias name=\"加攻击特效\"/>\
    <effect_alias name=\"加攻击力\"/>\
    <effect_alias name=\"恢复效果\"/>\
    <effect_alias name=\"未分类\"/>\
    <!-- ==========被动技能配置========== -->\
    <!-- 减速弹 -->\
    <passive alias=\"减速弹\" name=\"减速弹_等级1\" level=\"1\" buff_types=\"减速弹_等级1_持有者增益效果\" desc=\"被减速弹击中的目标移动速度会降低50%，持续3秒。\"/>\
    <!-- 拥护 -->\
    <passive alias=\"拥护\" name=\"拥护_等级1\" level=\"1\" buff_types=\"拥护_等级1_拥护效果\" desc=\"15%概率闪避近战攻击，减少其他类型伤害50%。\"/>\
    <!-- 冲击波 -->\
    <passive alias=\"冲击波\" name=\"冲击波_等级1\" level=\"1\" buff_types=\"冲击波_等级1_持有者增益效果\" desc=\"每次攻击都会对目标以及目标周围的敌人造成伤害。\"/>\
    <!-- 轰击 -->\
    <passive alias=\"轰击\" name=\"轰击_等级1\" level=\"1\" buff_types=\"轰击_等级1_持有者增益效果\" desc=\"向目标投掷一个炸弹，炸弹爆炸会对目标造成30点伤害，同时也会伤害目标周围的敌人。\"/>\
    <!-- 轰击 -->\
    <passive alias=\"轰炸\" name=\"轰炸_等级1\" level=\"1\" buff_types=\"轰炸_等级1_持有者增益效果\" desc=\"轰炸目标造成220码溅射。\"/>\
    <!-- 血雾 -->\
    <passive alias=\"血雾\" name=\"血雾_等级1\" level=\"1\" buff_types=\"血雾_等级1_持有者增益效果\" desc=\"向目标区域施放一个血雾，区域内的敌人会受到大量伤害。\"/>\
    <!-- 闪电攻击 -->\
    <passive alias=\"闪电攻击\" name=\"闪电攻击_等级1\" level=\"1\" buff_types=\"闪电攻击_等级1_持有者增益效果\" desc=\"机械公爵的闪电攻击可以伤害到前方的所有敌人。\"/>\
    <!-- 刚毅 -->\
    <passive alias=\"刚毅\" name=\"刚毅_等级1\" level=\"1\" buff_types=\"刚毅_等级1_刚毅效果\" desc=\"每次受到伤害不会超过生命最大值的5%。\"/>\
    <!-- 增幅 -->\
    <passive alias=\"增幅\" name=\"增幅_等级1\" level=\"1\" buff_types=\"增幅_等级1_加攻击力\" desc=\"提升相当于自身护甲值的攻击力。\"/>\
    <!-- ==========主动技能配置========== -->\
    <!-- 激励 -->\
    <spell alias=\"激励\" spell_name=\"激励_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,激励_等级1_激励施法效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"60.0\" class=\"cast\" desc=\"提升周围500码内友军的攻击速度30%。\"/>\
    <!-- 战鼓 -->\
    <spell alias=\"战鼓\" spell_name=\"战鼓_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,战鼓_等级1_战鼓效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"20.0\" class=\"cast\" desc=\"提升所有友军的攻击力33%。\"/>\
    <!-- 时空扭曲 -->\
    <spell alias=\"时空扭曲\" spell_name=\"时空扭曲_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"enemy\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,时空扭曲_等级1_时空扭曲效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"10.0\" class=\"cast\" desc=\"降低所有敌人的移动速度\"/>\
    <!-- 治疗 -->\
    <spell alias=\"治疗\" spell_name=\"治疗_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"unit\" immediate_elements=\"0,治疗_等级1_治疗效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"5.0\" class=\"cast\" desc=\"治疗一个受伤的友军，每秒恢复10点生命值。\"/>\
    <!-- 狂暴 -->\
    <spell alias=\"狂暴\" spell_name=\"狂暴_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,狂暴_等级1_狂暴效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"10.0\" class=\"cast\" desc=\"攻击敌人时会狂暴，损失10%最大生命值，攻击速度提升30%，持续10秒。\"/>\
    <!-- 全体恢复 -->\
    <spell alias=\"全体恢复\" spell_name=\"全体恢复_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,全体恢复_等级1_恢复效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"120.0\" class=\"cast\" desc=\"施放一片大范围的治疗，恢复所有区域内的友军50点生命。\"/>\
    <!-- 冲锋 -->\
    <spell alias=\"冲锋\" spell_name=\"冲锋_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,冲锋_等级1_加移动速度,1\" preperform=\"0.0\" perform=\"0.0\" cd=\"20.0\" class=\"cast\" desc=\"当视野内有出现敌人时，会提升120%移动速度，持续1秒。\"/>\
    <!-- 王者祝福 -->\
    <spell alias=\"王者祝福\" spell_name=\"王者祝福_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,王者祝福_等级1_王者祝福效果,1\" preperform=\"0.0\" perform=\"0.0\" cd=\"20.0\" class=\"cast\" desc=\"增加周围友军700点护甲。\"/>\
    <!-- 强风护盾 -->\
    <spell alias=\"强风护盾\" spell_name=\"强风护盾_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,强风护盾_等级1_强风护盾效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"120.0\" class=\"cast\" desc=\"周围的友军会获得一个护盾，护盾持续期间收到的所有伤害减少10点。持续15秒\"/>\
    <!-- 吸血 -->\
    <spell alias=\"吸血\" spell_name=\"吸血_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,吸血_等级1_吸血效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"30.0\" class=\"cast\" desc=\"周围的友军会获得一个吸血效果。持续15秒\"/>\
    <!-- 火球术 -->\
    <spell alias=\"火球术\" spell_name=\"火球术_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"enemy\" cast_distance=\"600\" cast_type=\"position\" immediate_elements=\"0,火球术_等级1_火球术效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"60.0\" class=\"cast\" desc=\"向目标区域施放一个火球，对区域内的所有敌方单位造成50点伤害。\"/>\
    <!-- 暗影烈焰 -->\
    <spell alias=\"暗影烈焰\" spell_name=\"暗影烈焰_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"enemy\" cast_distance=\"300\" cast_type=\"unit\" immediate_elements=\"0,暗影烈焰_等级1_暗影烈焰效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"12.0\" class=\"cast\" desc=\"向目标区域施放一个火球，对区域内的所有敌方单位造成50点伤害。\"/>\
    <!-- ==========主动技能原子配置========== -->\
    <!-- 激励 -->\
    <spell_pattern name=\"激励_等级1_激励施法效果\" class=\"damage_all\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"激励_等级1_攻速增益效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 战鼓 -->\
    <spell_pattern name=\"战鼓_等级1_战鼓效果\" class=\"damage_all\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"战鼓_等级1_战鼓效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 时空扭曲 -->\
    <spell_pattern name=\"时空扭曲_等级1_时空扭曲效果\" class=\"damage_all\" deliver_nature=\"enemy\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"减速弹_等级1_减速减益效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 治疗 -->\
    <spell_pattern name=\"治疗_等级1_治疗效果\" class=\"damage_target\" deliver_nature=\"ally\">\
    <damage damage_value=\"100\" damage_nature=\"heal\"/>\
    </spell_pattern>\
    <!-- 全体恢复 -->\
    <spell_pattern name=\"全体恢复_等级1_恢复效果\" class=\"damage_all\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"全体恢复_等级1_恢复效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 狂暴 -->\
    <spell_pattern name=\"狂暴_等级1_狂暴效果\" class=\"damage_target\" deliver_nature=\"ally\">\
    <damage damage_value=\"d:target:owner;type:max_hp;v:0.1f;0\" damage_nature=\"hurt\">\
    <feature type=\"add_buff\" buff_type_name=\"狂暴_等级1_攻速增益效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 冲锋 -->\
    <spell_pattern name=\"冲锋_等级1_加移动速度\" class=\"damage_target\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"冲锋_等级1_加移动速度\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 强风护盾 -->\
    <spell_pattern name=\"强风护盾_等级1_强风护盾效果\" class=\"damage_all\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"强风护盾_等级1_伤害减少\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 吸血 -->\
    <spell_pattern name=\"吸血_等级1_吸血效果\" class=\"damage_target_position_circle\" radius=\"600\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"吸血_等级1_吸血效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 王者祝福 -->\
    <spell_pattern name=\"王者祝福_等级1_王者祝福效果\" class=\"damage_target_position_circle\" radius=\"600\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"王者祝福_等级1_王者祝福效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 火球术 -->\
    <spell_pattern name=\"火球术_等级1_火球术效果\" class=\"damage_target_position_circle\" radius=\"300\" deliver_nature=\"enemy\">\
    <damage damage_value=\"200\" damage_nature=\"hurt\"/>\
    </spell_pattern>\
    <!-- 暗影烈焰 -->\
    <spell_pattern name=\"暗影烈焰_等级1_暗影烈焰效果\" class=\"damage_target\" deliver_nature=\"enemy\">\
    <damage damage_value=\"270\" damage_nature=\"hurt\"/>\
    </spell_pattern>\
    <!-- ==========增益减益效果配置========== -->\
    <!-- 激励 -->\
    <buff name=\"激励_等级1_攻速增益效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\" render_key=\"激励\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加攻击速度\" attr_name=\"attack_speed\" value=\"p:0.66f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 战鼓 -->\
    <buff name=\"战鼓_等级1_战鼓效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"20.f\" max_overlay=\"1\" render_key=\"激励\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加攻击力\" attr_name=\"attack_damage\" value=\"p:0.33f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 减速弹 -->\
    <buff name=\"减速弹_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"加攻击特效\">\
    <feature type=\"add_buff\" buff_type_name=\"减速弹_等级1_减速减益效果\" overlay=\"1\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <buff name=\"减速弹_等级1_减速减益效果\" deliver_nature=\"enemy\" deliver_class=\"physical\" span_type=\"limited\" span=\"5.f\" max_overlay=\"1\" render_key=\"减速弹\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"减移动速度\" attr_name=\"move_speed\" value=\"p:-0.4f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 冲击波 -->\
    <buff name=\"冲击波_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:0.3f\" range=\"40\" render_key=\"冲击波\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 轰击 -->\
    <buff name=\"轰击_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:0.3f\" range=\"180\" />\
    <feature type=\"out_damage_enhance\" value=\"p:2.f\" probability=\"0.15f\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 轰炸 -->\
    <buff name=\"轰炸_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:0.5f\" range=\"220\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 狂暴 -->\
    <buff name=\"狂暴_等级1_攻速增益效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\"  render_key=\"狂暴\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加攻击力\" attr_name=\"attack_damage\" value=\"p:0.5f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 全体恢复 -->\
    <buff name=\"全体恢复_等级1_恢复效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\" render_key=\"全体恢复\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"loop\" buff_effect_time=\"3.f\"/>\
    <effect class=\"damage\" alias_name=\"恢复效果\">\
    <damage damage_value=\"200\" damage_nature=\"heal\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 冲锋 -->\
    <buff name=\"冲锋_等级1_加移动速度\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"1.f\" max_overlay=\"1\" render_key=\"冲锋\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加移动速度\" attr_name=\"move_speed\" value=\"p:0.5f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 增幅 -->\
    <buff name=\"增幅_等级1_加攻击力\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"1.f\" max_overlay=\"1\" render_key=\"冲锋\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"out_damage_enhance\" value=\"d:target:owner;type:armor;v:1.0f;0\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 血雾 -->\
    <buff name=\"血雾_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:0.3f\" range=\"100\" render_key=\"血雾\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 闪电攻击 -->\
    <buff name=\"闪电攻击_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:0.3f\" range=\"100\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 强风护盾 -->\
    <buff name=\"强风护盾_等级1_伤害减少\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\" render_key=\"强风护盾\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"income_damage_reduce\" value=\"20.f\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 吸血 -->\
    <buff name=\"吸血_等级1_吸血效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\" render_key=\"强风护盾\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"drain\" value=\"p:0.2f\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 王者祝福 -->\
    <buff name=\"王者祝福_等级1_王者祝福效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"未分类\" attr_name=\"armor\" value=\"700.f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 刚毅 -->\
    <buff name=\"刚毅_等级1_刚毅效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"eternal\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\" >\
    <feature type=\"tough\" value=\"d:target:owner;type:max_hp;v:0.1f;0\" render_key=\"刚毅\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 拥护 -->\
    <buff name=\"拥护_等级1_拥护效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"eternal\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\" >\
    <feature type=\"income_damage_reduce\" value=\"p:0.5f\"  conditions=\"type:damage_distance;target:opposite;condition:not:nearby\"/>\
    <feature type=\"evade\" probability=\"0.15f\" conditions=\"type:damage_distance;target:opposite;condition:is:nearby|type:deliver_class;target:opposite;condition:physcal\"/>\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 暗影烈焰 -->\
    <buff name=\"暗影烈焰_等级1_暗影烈焰效果\" deliver_nature=\"enemy\" deliver_class=\"magical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:1.0f\" range=\"370\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    </root>";
    setting.setTechTree(techTree);
    
    //3. set faction data
    setting.setFactionCount(2);
    setting.setThisFactionIndex(0);
    setting.setFactionTypeKey(0, "狼人族");
    setting.setFactionControlType(0, UnderWorld::Core::kFactionControlType_Human);
    setting.setTeam(0, 0);
    setting.setMapIndex(0, 0);
    setting.setFactionTypeKey(1, "吸血鬼族");
    setting.setFactionControlType(1, UnderWorld::Core::kFactionControlType_Cpu);
    setting.setTeam(1, 1);
    setting.setMapIndex(1, 1);
    
    //4. set core & tower
    UnderWorld::Core::UnitSetting core0;
    core0.setUnitTypeName("狼人基地");
    setting.setCore(0, core0);
    UnderWorld::Core::UnitSetting core1;
    core1.setUnitTypeName("吸血鬼基地");
    setting.setCore(1, core1);
    UnderWorld::Core::UnitSetting tower0;
    tower0.setUnitTypeName("狼人箭塔");
    setting.setTower(0, tower0);
    UnderWorld::Core::UnitSetting tower1;
    tower1.setUnitTypeName("吸血鬼箭塔");
    setting.setTower(1, tower1);
    
    // 5. set camps
    // 5.1 werewolf
    {
        vector<UnderWorld::Core::CampSetting> cs;
        static const string hero_camp_names[] = {
            "时光女神",
            "狼人森林之魂",
            "天空女神",
        };
        const unsigned int heroes_count = sizeof(hero_camp_names) / sizeof(string);
        
        static const string soldier_camp_names[] = {
            "时光法师",
            "狼人射手",
            "狼人巫师",
            "狼人步兵",
            "狼人机械狼"
        };
        const unsigned int soldiers_count = sizeof(soldier_camp_names) / sizeof(string);
        
        cs.resize(heroes_count + soldiers_count);
        
        int i = 0;
        for (; i < heroes_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(hero_camp_names[i]);
            us.setLevel(0);
            us.setQuality(0);
            us.setTalentLevel(0);
            cs[i].setUnitSetting(us);
            
            cs[i].setMaxProduction(1);
        }
        
        for (; i < heroes_count + soldiers_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(soldier_camp_names[i - heroes_count]);
            us.setLevel(0);
            us.setQuality(0);
            us.setTalentLevel(0);
            cs[i].setUnitSetting(us);
            
            cs[i].setMaxProduction(2);
        }
        
        setting.setCamps(0, cs);
    }
    // 5.2 vampire
    {
        vector<UnderWorld::Core::CampSetting> cs;
        static const string camp_names[] = {
            "吸血鬼战士",
            "吸血鬼弓箭手",
            "巨型蜘蛛",
            "蛇女莉莉丝",
            "七恶魔拉玛什图",
            "吸血鬼巫师"
        };
        const unsigned int camp_count = sizeof(camp_names) / sizeof(string);
        cs.resize(camp_count);
        
        for (int i = 0; i < camp_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(camp_names[i]);
            us.setLevel(0);
            us.setQuality(0);
            us.setTalentLevel(0);
            cs[i].setUnitSetting(us);
            cs[i].setMaxProduction(5);
        }
        setting.setCamps(1, cs);
    }
    
    _looper = new (nothrow) UnderWorld::Core::GameLooper(_render, _sch);
    _looper->init(setting);
    _looper->start();
}

void BattleScene::clear()
{
    if (_looper) {
        _looper->end();
    }
    CC_SAFE_DELETE(_looper);
    CC_SAFE_DELETE(_sch);
    CC_SAFE_DELETE(_render);
    removeAllChildren();
}