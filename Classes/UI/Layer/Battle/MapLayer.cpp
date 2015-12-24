//
//  MapLayer.cpp
//  Underworld_Client
//
//  Created by burst on 15/10/26.
//
//

#include "MapLayer.h"
#include "CocosGlobal.h"
#include "2d/CCFastTMXLayer.h"
#include "2d/CCFastTMXTiledMap.h"
#include "Coordinate.h"
#include "Map.h"
#include "cocostudio/CocoStudio.h"
#include "tinyxml2/tinyxml2.h"
#include "CoreUtils.h"
#include "Constants.h"
#include "DataManager.h"
#include "MapParticleConfigData.h"

using namespace cocostudio;
using namespace std;

static const int TILEDMAP_TAG = 2;
static const int TILEDMAP_ZORDER = 2;
static const string TILEDMAP_LAYER_LOGIC = "logic";
static const string TILEDMAP_LAYER_FOREGROUND = "fg";

static const std::string CONFIG_KEY_LOCATION_SETTING_TAG("location_setting");
static const std::string CONFIG_KEY_RESOURCE_SETTINGS_TAG("resource_settings");
static const std::string CONFIG_KEY_RESOURCE_SETTING_TAG("resource_setting");
static const std::string CONFIG_KEY_FIXED_UNIT_SETTING_TAG("fixed_unit_setting");
static const std::string CONFIG_KEY_UNIT_SETTING_TAG("unit_setting");
static const std::string CONFIG_KEY_ATTR_INDEX("index");
static const std::string CONFIG_KEY_ATTR_CORE_LOCATION("core_location");
static const std::string CONFIG_KEY_ATTR_BUILDING_LOCAITON("building_location");
static const std::string CONFIG_KEY_ATTR_ASSEMBLE_LOCATION("assemble_location");
static const std::string CONFIG_KEY_ATTR_RESOURCE_NAME("resource_name");
static const std::string CONFIG_KEY_ATTR_INIT_BALANCE("init_balance");
static const std::string CONFIG_KEY_ATTR_INIT_SALARY("init_salary");
static const std::string CONFIG_KEY_ATTR_SALARY_ACCELERATE("salary_accelerate");
static const std::string CONFIG_KEY_ATTR_UNIT_TYPE_NAME("unit_type_name");
static const std::string CONFIG_KEY_ATTR_POS("pos");

static UnderWorld::Core::Coordinate parseCoordinate(const std::string& s) {
    int x = atoi(s.substr(0, s.find_first_of(",")).c_str());
    int y = atoi(s.substr(s.find_first_of(",") + 1).c_str());
    return UnderWorld::Core::Coordinate(x, y);
}

static void loadMapSetting(const std::string& xml,
    UnderWorld::Core:: MapSetting& setting) {
    tinyxml2::XMLDocument *xmlDoc = new tinyxml2::XMLDocument();
    xmlDoc->Parse(xml.c_str());
    for (tinyxml2::XMLElement* item = xmlDoc->RootElement()->FirstChildElement();
         item;
         item = item->NextSiblingElement()) {
        if (item->Name() == CONFIG_KEY_LOCATION_SETTING_TAG) {
            UnderWorld::Core::LocationSetting locationSetting;
            int index = atoi(item->Attribute(CONFIG_KEY_ATTR_INDEX.c_str()));
            std::string coreLoaction = item->Attribute(CONFIG_KEY_ATTR_CORE_LOCATION.c_str());
            locationSetting.setCoreLocation(parseCoordinate(coreLoaction));
            std::string buildingLocations = item->Attribute(CONFIG_KEY_ATTR_BUILDING_LOCAITON.c_str());
            std::vector<std::string> buildingLocationsVec;
            UnderWorld::Core::Utils::split(buildingLocationsVec, buildingLocations, ";");
            for (int i = 0; i < buildingLocationsVec.size(); ++i) {
                locationSetting.addBuildingLocation(parseCoordinate(buildingLocationsVec[i]));
            }
            std::string assembleLocations = item->Attribute(CONFIG_KEY_ATTR_ASSEMBLE_LOCATION.c_str());
            std::vector<std::string> assembleLocationsVec;
            UnderWorld::Core::Utils::split(assembleLocationsVec, assembleLocations, ";");
            for (int i = 0; i < assembleLocationsVec.size(); ++i) {
                int p = atoi(assembleLocationsVec[i].substr(0, assembleLocationsVec[i].find_first_of(":")).c_str());
                UnderWorld::Core::Coordinate c = parseCoordinate(assembleLocationsVec[i].substr(assembleLocationsVec[i].find_first_of(":") + 1));
                locationSetting.addAssembleLocation(p, c);
            }
            setting.setLocationSetting(index, locationSetting);
        } else if (item->Name() == CONFIG_KEY_RESOURCE_SETTINGS_TAG) {
            int index = atoi(item->Attribute(CONFIG_KEY_ATTR_INDEX.c_str()));
            std::vector<UnderWorld::Core::ResourceSetting> vec;
            for (const tinyxml2::XMLElement* node = item->FirstChildElement();
                 node;
                 node = node->NextSiblingElement()) {
                if (node->Name() == CONFIG_KEY_RESOURCE_SETTING_TAG) {
                    UnderWorld::Core::ResourceSetting rs;
                    rs.setInitBalance(atoi(node->Attribute(CONFIG_KEY_ATTR_INIT_BALANCE.c_str())));
                    rs.setResourceTypeName(node->Attribute(CONFIG_KEY_ATTR_RESOURCE_NAME.c_str()));
                    rs.setInitSalary(atoi(node->Attribute(CONFIG_KEY_ATTR_INIT_SALARY.c_str())));
                    rs.setSalaryAccelerate(atoi(node->Attribute(CONFIG_KEY_ATTR_SALARY_ACCELERATE.c_str())));
                    vec.push_back(rs);
                }
            }
            setting.setStartResource(index, vec);
        } else if (item->Name() == CONFIG_KEY_FIXED_UNIT_SETTING_TAG) {
            int index = atoi(item->Attribute(CONFIG_KEY_ATTR_INDEX.c_str()));
            std::vector<std::pair<UnderWorld::Core::UnitSetting, UnderWorld::Core::Coordinate> > vec;
            for (const tinyxml2::XMLElement* node = item->FirstChildElement();
                 node;
                 node = node->NextSiblingElement()) {
                if (node->Name() == CONFIG_KEY_UNIT_SETTING_TAG) {
                    UnderWorld::Core::UnitSetting us;
                    us.setUnitTypeName(node->Attribute(CONFIG_KEY_ATTR_UNIT_TYPE_NAME.c_str()));
                    UnderWorld::Core::Coordinate pos = parseCoordinate(node->Attribute(CONFIG_KEY_ATTR_POS.c_str()));
                    vec.push_back(std::make_pair(us, pos));
                }
            }
            setting.setFixedUnits(index, vec);
        }
    }
}

MapLayer::MapLayer()
:_mapId(INVALID_VALUE)
,_width(0)
,_height(0)
,_tileWidth(0)
,_tileHeight(0)
,_tiledMap(nullptr)
,_scrollView(nullptr)
,_mainLayer(nullptr)
,_butterfly(nullptr)
,_isScrolling(false)
{
    
}

MapLayer::~MapLayer()
{
    
}

MapLayer* MapLayer::create(int mapId)
{
    MapLayer* pRet = new (nothrow)MapLayer();
    if(pRet && pRet->init(mapId)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool MapLayer::init(int mapId)
{
    if (LayerColor::init()) {
        _mapId = mapId;
        const Size& winSize = Director::getInstance()->getWinSize();
        //--------- container ----------//
        _scrollView = cocos2d::extension::ScrollView::create();
        _scrollView->setDirection(cocos2d::extension::ScrollView::Direction::BOTH);
        _scrollView->setTouchEnabled(true);
        _scrollView->setBounceable(false);
        _scrollView->setViewSize(winSize);
        _scrollView->setPosition(Point::ZERO);
        _scrollView->setDelegate(this);
        addChild(_scrollView);
//        mRoot = Node::create();
//        mScrollView->addChild(mRoot);
        //--------- background ---------//
        int i = 1;
        float offsetX = .0f;
        while (true) {
            string fileName = StringUtils::format("map/bg%d_%d.png", _mapId, i);
            if(FileUtils::getInstance()->isFileExist(fileName)){
                Sprite* bg = Sprite::create(fileName);
                bg->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
                bg->setPosition(Point(offsetX, .0f));
                _scrollView->addChild(bg);
                offsetX += bg->getContentSize().width;
            } else {
                break;
            }
            i++;
        }
        //--------- foreground ---------- //
        _tiledMap = cocos2d::experimental::TMXTiledMap::create(StringUtils::format("map/%d.tmx", _mapId));
        _scrollView->addChild(_tiledMap, TILEDMAP_ZORDER, TILEDMAP_TAG);
        _mainLayer = _tiledMap->getLayer(TILEDMAP_LAYER_FOREGROUND);
        const Size &mapSize = _tiledMap->getMapSize();
        _width = mapSize.width;
        _height = mapSize.height;
        const Size &tileSize = _tiledMap->getTileSize();
        _tileWidth = tileSize.width;
        _tileHeight = tileSize.height;
        if (_mainLayer) {
            
            cocos2d::experimental::TMXLayer* tmxLayer = dynamic_cast<cocos2d::experimental::TMXLayer*>(_mainLayer);
            
            const Size &ls = tmxLayer->getLayerSize();
            for (unsigned int y = 0; y < ls.height; y++)
            {
                int zOrder = calcZOrder(ls.height - y);
                for (unsigned int x = 0; x < ls.width; x++)
                {
                    Sprite *tile = tmxLayer->getTileAt(Vec2(x, y));
                    if (tile) {
                        //                    tile->setLocalZOrder(zOrder);
                        tile->setVisible(false);
                        _mainLayer->reorderChild(tile, zOrder);
                    }
                }
            }
            CCLOG("%zd foreground", _mainLayer->getChildren().size());
        } else {
            _mainLayer = Node::create();
            _tiledMap->addChild(_mainLayer);
        }
//
//        //--------- logic ---------//
        cocos2d::experimental::TMXLayer *logicLayer = _tiledMap->getLayer(TILEDMAP_LAYER_LOGIC);
        logicLayer->setVisible(false);
        const Size &logicSize = logicLayer->getLayerSize();
        mapSetting.setWidth(logicSize.width);
        mapSetting.setHeight(logicSize.height);
        for (unsigned int x = 0; x < logicSize.width; x++)
        {
            for (unsigned int y = 0; y < logicSize.height; y++)
            {
                int gid = logicLayer->getTileGIDAt(Vec2(x, y));
                if (gid == 0) {
                    //can walk
                    mapSetting.addWalkableArea(mapCoordinate2coreCoordinate(x, y));
                } else {
                    //can not walk
                }
            }
        }
        std::string mapSettingXml = "<root> <location_setting index=\"0\" core_location=\"400,600\" building_location=\"\" assemble_location=\"0:600,600;2:800,600\"/> <resource_settings index=\"0\"> <resource_setting resource_name=\"金子\" init_balance=\"300\" init_salary=\"5\" salary_accelerate=\"0\"/> <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/> </resource_settings> <fixed_unit_setting index=\"0\"> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> <unit_setting unit_type_name=\"狼人步兵\" pos=\"600,600\"/> </fixed_unit_setting> <location_setting index=\"1\" core_location=\"2000,600\" building_location=\"\" assemble_location=\"0:1600,600;2:1200,600\"/> <resource_settings index=\"1\"> <resource_setting resource_name=\"金子\" init_balance=\"300\" init_salary=\"5\" salary_accelerate=\"0\"/> <resource_setting resource_name=\"人口\" init_balance=\"200\" init_salary=\"0\" salary_accelerate=\"0\"/> </resource_settings> <fixed_unit_setting index=\"1\"> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> <unit_setting unit_type_name=\"吸血鬼战士\" pos=\"1500,600\"/> </fixed_unit_setting> </root>";
        loadMapSetting(mapSettingXml, mapSetting);
        CCLOG("%zd logicLayer", mapSetting.getWalkableArea().size());
        logicLayer->removeFromParent();
        
        //--------- effect ---------//
        if (0) {
            // 1. butterfly
            addButterfly();
            
            // 2. campfire
            {
                static const string file("effect-place-1.csb");
                Node *effect = CSLoader::createNode(file);
                effect->setPosition(Point(875, 80));
                _tiledMap->addChild(effect);
                timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                effect->runAction(action);
                action->gotoFrameAndPlay(0, true);
            }
            
            // 3. smoke
            {
                static const string file("particle/guohuo.plist");
                ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
                effect->setPosition(Point(1455, 1020));
                effect->setScale(2.5f);
                _tiledMap->addChild(effect);
            }
            
            // 4. spark
            {
                static const string file("particle/huoxing.plist");
                ParticleSystemQuad *effect = ParticleSystemQuad::create(file);
                effect->setPosition(Point(278, 232));
                _tiledMap->addChild(effect);
            }
            
            // 5. fire
            {
                static const string file("effect-fire-1.csb");
                Node *effect = CSLoader::createNode(file);
                effect->setPosition(Point(285, 245));
                _tiledMap->addChild(effect);
                timeline::ActionTimeline *action = CSLoader::createTimeline(file);
                effect->setScaleX(0.6f);
                effect->setScaleY(1.3f);
                effect->runAction(action);
                action->gotoFrameAndPlay(0, true);
            }
        } else {
            const vector<MapParticleConfigData*>& particles = DataManager::getInstance()->getMapParticleConfigData(mapId);
            for (int i = 0; i < particles.size(); ++i) {
                addParticle(particles.at(i));
            }
        }
        
        //--------- scale ---------//
        const Size &mapPSize = _tiledMap->getContentSize();
        float scale = RESOLUTION_HEIGHT / mapPSize.height;
        _scrollView->setContentSize(mapPSize);
        _scrollView->setMaxScale(Constants::TILEDMAP_MAX_SCALE);
        _scrollView->setMinScale(scale);
        _scrollView->setZoomScale(scale);
        _scrollViewOffset = _scrollView->getContentOffset();
        
        //--------- event ---------//
        auto listener = EventListenerTouchAllAtOnce::create();
        listener->onTouchesMoved = CC_CALLBACK_2(MapLayer::onTouchesMoved, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        return true;
    }
    return false;
}

int MapLayer::calcZOrder(int coreCoordinateY)
{
    return 2 * (_height - coreCoordinateY + 1);
}


UnderWorld::Core::Coordinate MapLayer::mapCoordinate2coreCoordinate(int x, int y)
{
    return UnderWorld::Core::Coordinate(x, _height - y);
}

void MapLayer::coordinateConvert(const UnderWorld::Core::Coordinate& coreCoordinate, Point& mapPosition, int& zOrder)
{
    
    mapPosition.x = _tileWidth / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coreCoordinate.x;
    mapPosition.y = _tileHeight / UnderWorld::Core::Map::TILE_2_ELEMENT_SCALE * coreCoordinate.y;
    zOrder = 2 * (_height - coreCoordinate.y + 1);
}

void MapLayer::addUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate)
{
    Point pos;
    int zOrder;
    coordinateConvert(coreCoordinate, pos, zOrder);
    unit->setPosition(pos);
    _mainLayer->addChild(unit, zOrder);
}

void MapLayer::repositionUnit(Node* unit, const UnderWorld::Core::Coordinate& coreCoordinate)
{
    Point pos;
    int zOrder;
    coordinateConvert(coreCoordinate, pos, zOrder);
    if (pos == unit->getPosition()) {
        return;
    }
    unit->setPosition(pos);
    reorderChild(unit, zOrder);
}

void MapLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{
//    Sprite* x = Sprite::create("map/images/tree1.png");
//    addUnit(x, UnderWorld::Core::Coordinate(200,30));
}

void MapLayer::scrollViewDidScroll(cocos2d::extension::ScrollView* view)
{
    if (!_isScrolling) {
        _isScrolling = true;
        scrollChecking(0);
    }
}

void MapLayer::scrollViewDidZoom(cocos2d::extension::ScrollView* view)
{
}

void MapLayer::addParticle(const MapParticleConfigData* data)
{
    const string& name = data->getName();
    Node* effect = nullptr;
    if (name.find(".csb") != string::npos) {
        effect = CSLoader::createNode(name);
        timeline::ActionTimeline *action = CSLoader::createTimeline(name);
        effect->runAction(action);
        action->gotoFrameAndPlay(0, true);
    } else {
        ParticleSystemQuad *particle = ParticleSystemQuad::create(data->getName());
        _particles.insert(particle);
        
        effect = particle;
    }
    
    Point pos = Point(_width * _tileWidth / 2, _height * _tileHeight / 2) + Point(data->getPosX(), data->getPosY());
    effect->setPosition(pos);
    effect->setScaleX(data->getScaleX());
    effect->setScaleY(data->getScaleY());
    _tiledMap->addChild(effect);
}

void MapLayer::removeParticle(ParticleSystemQuad* effect)
{
    effect->removeFromParent();
    if (_particles.find(effect) != _particles.end()) {
        _particles.erase(effect);
    }
}

void MapLayer::addButterfly()
{
    if (_butterfly) {
        _butterfly->stopAllActions();
        _butterfly->removeFromParent();
        _butterfly = nullptr;
    }
    
    static const string file("effect-place-2.csb");
    _butterfly = CSLoader::createNode(file);
    _tiledMap->addChild(_butterfly);
    timeline::ActionTimeline *action = CSLoader::createTimeline(file);
    _butterfly->runAction(action);
    action->gotoFrameAndPlay(0, false);
    action->setLastFrameCallFunc([this]() {
        addButterfly();
    });
    
    // generate a random position
    static const float max_x = _width * _tileWidth;
    static const float valid_y[2] = {140.0f, 1070.0f};
    static const float invalid_x[2][2] = {{625, 1155}, {1220, 1665}};
    
    const int random_y_2 = cocos2d::random() % 2;
    const float y = valid_y[random_y_2];
    
    const int random_x_2 = cocos2d::random() % 2;
    const float random_0_1 = CCRANDOM_0_1();
    float x = invalid_x[random_x_2][random_y_2];
    if (random_x_2 == 0) {
        x = random_0_1 * x;
    } else {
        x = x + (max_x - x) * random_0_1;
    }
    
    _butterfly->setPosition(Point(x, y));
}

void MapLayer::scrollChecking(float dt)
{
    const Vec2& offset = _scrollView->getContentOffset();
    const bool show(offset == _scrollViewOffset);
    for (set<ParticleSystemQuad*>::iterator iter = _particles.begin(); iter != _particles.end(); ++iter) {
        (*iter)->setVisible(show);
    }
    
    SEL_SCHEDULE selector = CC_SCHEDULE_SELECTOR(MapLayer::scrollChecking);
    if (show) {
        _isScrolling = false;
        _scheduler->unschedule(selector, this);
    } else {
        _scrollViewOffset = offset;
        if (false == _scheduler->isScheduled(selector, this)) {
            _scheduler->schedule(selector, this, 0.1f, false);
        }
    }
}