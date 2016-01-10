//
//  BattleScene.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "BattleScene.h"
#include "GameSettings.h"
#include "MapLayer.h"
#include "SoundManager.h"

USING_NS_CC;

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
,_mapLayer(nullptr)
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
    if (_mapLayer) {
        clear();
    }
    
    // 1. add map layer
    string mapSettingXml = "<root> <location_setting index=\"0\" core_location=\"400,600\" building_location=\"\" assemble_location=\"0:600,600;2:800,600\"/> <resource_settings index=\"0\"> <resource_setting resource_name=\"金子\" init_balance=\"300\" init_salary=\"5\" salary_accelerate=\"0\"/> <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/> </resource_settings> <fixed_unit_setting index=\"0\"> </fixed_unit_setting> <location_setting index=\"1\" core_location=\"2000,600\" building_location=\"\" assemble_location=\"0:1600,600;2:1200,600\"/> <resource_settings index=\"1\"> <resource_setting resource_name=\"金子\" init_balance=\"300\" init_salary=\"5\" salary_accelerate=\"0\"/> <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/> </resource_settings> <fixed_unit_setting index=\"1\">  </fixed_unit_setting> </root>";
    
    _mapLayer = MapLayer::create(mapId, mapSettingXml);
    addChild(_mapLayer);
    
    // 2. add map ui layer
    _render = new GameRender(this, mapId, _mapLayer, "对手的名字");
    _render->registerObserver(this);
    
    _sch = new GameScheduler();
    
    UnderWorld::Core::GameSettings setting;
    
    //1. set map setting
    setting.setMap(_mapLayer->getMapSetting());
    
    //2. set techTree;

    std::string techTree = "<root>\
    <!-- ==========单位&势力配置========== -->\
    <faction name=\"狼人族\">\
    <unit name=\"狼人基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"1000\" attacksight=\"400\" size=\"300\" render_key=\"狼人基地\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\" occupy=\"0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"1.5\" min_damage=\"30\" max_damage=\"70\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"400\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    </unit>\
    <unit name=\"狼人箭塔\" class=\"building\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"500\" attacksight=\"400\" size=\"64\" render_key=\"狼人箭塔\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"8\" max_damage=\"10\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"400\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    </unit>\
    <!-- 射手英雄 -->\
    <unit name=\"时光女神\" class=\"hero\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"300\" magical_defense=\"0.25f\" attacksight=\"400\" size=\"25\" cost=\"金子:50;人口:1\" priority=\"1\" max_level=\"100\" spell_names=\"激励_等级1\" render_key=\"时光女神\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"280\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <!-- 法师英雄 -->\
    <unit name=\"狼人森林之魂\" class=\"hero\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"无甲\" armor=\"100\" armor_preference=\"重甲\" armor_preference_factor=\"1.5f\" magical_defense=\"0.25f\" attacksight=\"400\" size=\"25\" cost=\"金子:1000;人口:1\" priority=\"1\" max_level=\"100\" spell_names=\"全体恢复_等级1\" render_key=\"狼人森林之魂\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"magical\" damage_distance=\"faraway\" range=\"280\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/>\
    </unit>\
    <!-- 大型远程兵 -->\
    <unit name=\"被救赎的精灵卫士\" class=\"warrior\" hp=\"40\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"400\" armor_preference=\"无甲\" armor_preference_factor=\"1.5f\" attacksight=\"400\" size=\"25\" cost=\"金子:200;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"减速弹_等级1\" render_key=\"被救赎的精灵卫士\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"280\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <!-- 近战兵 -->\
    <unit name=\"狼人刺客\" class=\"warrior\" hp=\"60\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"400\" armor_preference=\"轻甲\" armor_preference_factor=\"1.5f\" attacksight=\"300\" size=\"25\" cost=\"金子:200;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"冲击波_等级1\" render_key=\"狼人步兵\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"20\" fields=\"1\">\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"80\"/>\
    </unit>\
    <!-- 小型远程兵 -->\
    <unit name=\"狼人射手\" class=\"warrior\" hp=\"40\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"300\" armor_preference=\"无甲\" armor_preference_factor=\"1.5f\" attacksight=\"400\" size=\"25\" cost=\"金子:200;人口:1\" priority=\"0\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" spell_names=\"狂暴_等级1\" render_key=\"狼人射手\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"280\" fields=\"1\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <!-- 奶妈 -->\
    <unit name=\"狼人巫师\" class=\"warrior\" hp=\"40\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"无甲\" armor=\"300\" armor_preference=\"无甲\" armor_preference_factor=\"1.5f\" attacksight=\"400\" size=\"25\" cost=\"金子:200;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" spell_names=\"治疗_等级1\" render_key=\"狼人巫师\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"magical\" damage_distance=\"faraway\" range=\"280\" fields=\"1\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/>\
    </unit>\
    <!-- 投石车 -->\
    <unit name=\"时光法师\" class=\"warrior\" hp=\"80\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"500\" armor_preference=\"城甲\" armor_preference_factor=\"2.0f\" attacksight=\"400\" size=\"25\" cost=\"金子:400;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"轰击_等级1\" render_key=\"时光法师\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"400\" fields=\"1\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"40\"/>\
    </unit>\
    </faction>\
    <faction name=\"吸血鬼族\">\
    <unit name=\"吸血鬼基地\" class=\"core\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"1000\" attacksight=\"400\" size=\"300\" render_key=\"吸血鬼核心\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\" occupy=\"0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"1.5\" min_damage=\"30\" max_damage=\"70\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"400\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    </unit>\
    <unit name=\"吸血鬼箭塔\" class=\"building\" hp=\"1000\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"城甲\" armor=\"500\" attacksight=\"400\" size=\"64\" render_key=\"吸血鬼箭塔\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"0.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"8\" max_damage=\"10\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"400\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    </unit>\
    <unit name=\"吸血鬼战士\" class=\"warrior\" hp=\"80\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"800\" armor_preference=\"无甲\" armor_preference_factor=\"1.5f\" attacksight=\"300\" size=\"25\" cost=\"金子:300;人口:1\" priority=\"0\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" spell_names=\"冲锋_等级1\" render_key=\"吸血鬼战士\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"30\" fields=\"1\">\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <unit name=\"吸血鬼弓箭手\" class=\"warrior\" hp=\"40\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"无甲\" armor=\"200\" armor_preference=\"重甲\" armor_preference_factor=\"2.0f\" attacksight=\"400\" size=\"25\" cost=\"金子:200;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"吸血鬼弓兵\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"280\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <unit name=\"巨型蜘蛛\" class=\"warrior\" hp=\"100\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"700\" armor_preference=\"无甲\" armor_preference_factor=\"2.0f\" attacksight=\"300\" size=\"25\" cost=\"金子:1000;人口:1\" priority=\"0\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"刚毅_等级1\" render_key=\"吸血鬼蜘蛛\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"150\" fields=\"1\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <unit name=\"蛇女莉莉丝\" class=\"hero\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"500\" attacksight=\"400\" size=\"25\" cost=\"金子:1000;人口:1\" priority=\"1\" max_level=\"100\" spell_names=\"强风护盾_等级1\" render_key=\"莉莉丝\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"300\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <unit name=\"昆虫学家鲁斯凡\" class=\"hero\" hp=\"200\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"轻甲\" armor=\"500\" attacksight=\"400\" size=\"25\" cost=\"金子:1000;人口:1\" priority=\"1\" max_level=\"100\" render_key=\"吸血鬼战士\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"nearby\" range=\"30\" fields=\"1\"/>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <unit name=\"被腐化的科瑞隆\" class=\"warrior\" hp=\"80\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"重甲\" armor=\"700\" armor_preference=\"无甲\" armor_preference_factor=\"2.0f\" attacksight=\"300\" size=\"25\" cost=\"金子:500;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" render_key=\"吸血鬼弓兵\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"physical\" damage_distance=\"faraway\" range=\"300\" fields=\"1\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    <unit name=\"吸血鬼法师2号\" class=\"warrior\" hp=\"40\" mp=\"0\" hpregen=\"0\" mpregen=\"0\" field=\"0\" armor_type=\"无甲\" armor=\"200\" armor_preference=\"重甲\" armor_preference_factor=\"1.5f\" attacksight=\"300\" size=\"25\" cost=\"金子:300;人口:1\" priority=\"1\" max_level=\"100\" max_quality=\"5\" max_talent=\"20\" passive_names=\"血雾_等级1\" render_key=\"吸血鬼巫师\">\
    <skill class=\"stop\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\"/>\
    <skill class=\"die\" preperform=\"1.0\" perform=\"10.0\" cd=\"0.0\"/>\
    <skill class=\"attack\" preperform=\"1.0\" perform=\"0.0\" cd=\"0.8\" min_damage=\"18\" max_damage=\"22\" deliver_class=\"magical\" damage_distance=\"faraway\" range=\"300\" fields=\"3\">\
				<bullet speed=\"400\" size=\"0\"/>\
    </skill>\
    <skill class=\"move\" preperform=\"0.0\" perform=\"0.0\" cd=\"0.0\" speed=\"60\"/>\
    </unit>\
    </faction>\
    <!-- ==========资源类别配置========== -->\
    <resource name=\"金子\" class=\"consumable\"/>\
    <resource name=\"木头\" class=\"consumable\"/>\
    <resource name=\"人口\" class=\"holdable\"/>\
    <!-- ==========护甲类别配置&护甲规则配置========== -->\
    <armor name=\"无甲\"/>\
    <armor name=\"轻甲\"/>\
    <armor name=\"重甲\"/>\
    <armor name=\"城甲\"/>\
    <armor_rule armor_base=\"1000\"/>\
    <!-- ==========效果分类配置========== -->\
    <effect_alias name=\"减移动速度\" unique=\"1\"/>\
    <effect_alias name=\"加移动速度\"/>\
    <effect_alias name=\"减攻击速度\" unique=\"1\"/>\
    <effect_alias name=\"加攻击速度\"/>\
    <effect_alias name=\"减受到伤害\" unique=\"1\"/>\
    <effect_alias name=\"加受到伤害\"/>\
    <effect_alias name=\"加攻击特效\"/>\
    <effect_alias name=\"未分类\"/>\
    <!-- ==========被动技能配置========== -->\
    <!-- 减速弹 -->\
    <passive alias=\"减速弹\" name=\"减速弹_等级1\" level=\"1\" buff_types=\"减速弹_等级1_持有者增益效果\"/>\
    <!-- 冲击波 -->\
    <passive alias=\"冲击波\" name=\"冲击波_等级1\" level=\"1\" buff_types=\"冲击波_等级1_持有者增益效果\"/>\
    <!-- 轰击 -->\
    <passive alias=\"轰击\" name=\"轰击_等级1\" level=\"1\" buff_types=\"轰击_等级1_持有者增益效果\"/>\
    <!-- 血雾 -->\
    <passive alias=\"血雾\" name=\"血雾_等级1\" level=\"1\" buff_types=\"血雾_等级1_持有者增益效果\"/>\
    <!-- 闪电攻击 -->\
    <passive alias=\"闪电攻击\" name=\"闪电攻击_等级1\" level=\"1\" buff_types=\"闪电攻击_等级1_持有者增益效果\"/>\
    <!-- ==========主动技能配置========== -->\
    <!-- 激励 -->\
    <spell alias=\"激励\" spell_name=\"激励_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,激励_等级1_激励施法效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"120.0\" class=\"cast\"/>\
    <!-- 治疗 -->\
    <spell alias=\"治疗\" spell_name=\"治疗_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"unit\" immediate_elements=\"0,治疗_治疗等级1_治疗效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"5.0\" class=\"cast\"/>\
    <!-- 狂暴 -->\
    <spell alias=\"狂暴\" spell_name=\"狂暴_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,狂暴_等级1_狂暴效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"120.0\" class=\"cast\"/>\
    <!-- 全体恢复 -->\
    <spell alias=\"全体恢复\" spell_name=\"全体恢复_全体恢复等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,全体恢复_全体恢复等级1_恢复效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"120.0\" class=\"cast\"/>\
    <!-- 冲锋 -->\
    <spell alias=\"冲锋\" spell_name=\"冲锋_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,冲锋_等级1_加移动速度,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"20.0\" class=\"cast\"/>\
    <!-- 强风护盾 -->\
    <spell alias=\"强风护盾\" spell_name=\"强风护盾_等级1\" level=\"1\" deliver_class=\"magical\" deliver_nature=\"ally\" cast_distance=\"0\" cast_type=\"self\" immediate_elements=\"0,强风护盾_等级1_强风护盾效果,1\" preperform=\"0.5\" perform=\"0.0\" cd=\"120.0\" class=\"cast\"/>\
    <!-- ==========主动技能原子配置========== -->\
    <!-- 激励 -->\
    <spell_pattern name=\"激励_等级1_激励施法效果\" class=\"damage_all\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"激励_等级1_攻速增益效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 治疗 -->\
    <spell_pattern name=\"治疗_治疗等级1_治疗效果\" class=\"damage_target\" deliver_nature=\"ally\">\
    <damage damage_value=\"100\" damage_nature=\"heal\">\
    <!--<feature type=\"add_buff\" buff_type_name=\"激励_等级1_攻速增益效果\" overlay=\"1\"/>-->\
    </damage>\
    </spell_pattern>\
    <!-- 全体恢复 -->\
    <spell_pattern name=\"全体恢复_全体恢复等级1_恢复效果\" class=\"damage_all\" deliver_nature=\"ally\">\
    <damage damage_value=\"0\" damage_nature=\"heal\">\
    <feature type=\"add_buff\" buff_type_name=\"全体恢复_全体恢复等级1_恢复效果\" overlay=\"1\"/>\
    </damage>\
    </spell_pattern>\
    <!-- 狂暴 -->\
    <spell_pattern name=\"狂暴_狂暴等级1_狂暴效果\" class=\"damage_target\" deliver_nature=\"ally\">\
    <damage damage_value=\"d:target:opposite;type:max_hp;v:0.1f;0\" damage_nature=\"hurt\">\
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
    <!-- ==========增益减益效果配置========== -->\
    <!-- 激励 -->\
    <buff name=\"激励_等级1_攻速增益效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加攻击速度\" attr_name=\"attack_speed\" value=\"p:0.5f\"/>\
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
    <buff name=\"减速弹_等级1_减速减益效果\" deliver_nature=\"enemy\" deliver_class=\"physical\" span_type=\"limited\" span=\"5.f\" max_overlay=\"1\">\
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
    <feature type=\"splash\" value=\"p:1.0f\" range=\"50\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 轰击 -->\
    <buff name=\"轰击_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:1.0f\" range=\"50\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 激励 -->\
    <buff name=\"狂暴_等级1_攻速增益效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加攻击力\" attr_name=\"attack_damage\" value=\"p:0.3f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 全体恢复 -->\
    <buff name=\"全体恢复_全体恢复等级1_恢复效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"loop\" buff_effect_time=\"3.f\"/>\
    <effect class=\"damage\" alias_name=\"恢复效果\">\
    <damage damage_value=\"100\" damage_nature=\"heal\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 冲锋 -->\
    <buff name=\"冲锋_等级1_加移动速度\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"alter_attr\" alias_name=\"加移动速度\" attr_name=\"move_speed\" value=\"p:3.0f\"/>\
    </buff_effect>\
    </buff>\
    <!-- 血雾 -->\
    <buff name=\"血雾_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:1.0f\" range=\"100\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 闪电攻击 -->\
    <buff name=\"闪电攻击_等级1_持有者增益效果\" deliver_nature=\"ally\" deliver_class=\"physical\" span_type=\"eternal\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"splash\" value=\"p:1.0f\" range=\"100\" />\
    </effect>\
    </buff_effect>\
    </buff>\
    <!-- 强风护盾 -->\
    <buff name=\"强风护盾_等级1_强风护盾效果\" deliver_nature=\"ally\" deliver_class=\"magical\" span_type=\"limited\" span=\"15.f\" max_overlay=\"1\">\
    <buff_effect>\
    <buff_effect_description buff_effect_condition=\"immediate\"/>\
    <effect class=\"add_feature\" alias_name=\"未分类\">\
    <feature type=\"income_damage_reduce\" value=\"20.f\"/>\
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
        static const unsigned int camp_count = 3;
        static const string camp_names[camp_count] = {
            "狼人刺客",
            "狼人射手",
            "狼人巫师"
        };
        cs.resize(camp_count);
        
        for (int i = 0; i < camp_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(camp_names[i]);
            cs[i].setUnitSetting(us);
            cs[i].setMaxProduction(3);
        }
        setting.setCamps(0, cs);
    }
    // 5.2 vampire
    {
        vector<UnderWorld::Core::CampSetting> cs;
        static const unsigned int camp_count = 3;
        static const string camp_names[camp_count] = {
            "吸血鬼战士",
            "吸血鬼弓箭手",
            "巨型蜘蛛"
        };
        cs.resize(camp_count);
        
        for (int i = 0; i < camp_count; ++i) {
            UnderWorld::Core::UnitSetting us;
            us.setUnitTypeName(camp_names[i]);
            cs[i].setUnitSetting(us);
            cs[i].setMaxProduction(5);
        }
        setting.setCamps(1, cs);
    }
    
    _looper = new UnderWorld::Core::GameLooper(_render, _sch);
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
    _mapLayer = nullptr;
}