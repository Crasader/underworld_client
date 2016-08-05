//
//  BulletRender.h
//  Underworld_Client
//
//  Created by Mac on 16/7/18.
//
//

#ifndef BulletRender_h
#define BulletRender_h

#include <functional>
#include "cocos2d.h"

#include "WorldObjectRender.h"
#include "WorldRender.h"
#include "BRConfigData.h"
#include "Bullet.h"
#include "Coordinate.h"

namespace UnderWorld{ namespace Core{
    
class BulletRender : public AbstractWorldObjectRender, public BulletObserver {
private:
    enum class BulletPathType {
        Line,
        Parabola
    };
    
private:
    /** data */
    const Bullet* _bullet;
    const BulletType* _bulletType;
    const BRConfigData* _configData;
    std::string _bulletRenderKey;
    WorldRender::RenderLayer _renderLayer;
    BulletPathType _pathType;
    mapsize_t _startHeight;
    Coordinate32 _startPos;
    mapsize_t _targetHeight;
    
    /** status */
    // parabola
    Coordinate32 _targetPos;
    float _parabolaA;
    float _parabolaB;
    float _parabolaH;
    float _alpha;
    
    //render params
    float _currentHeight;
    float _scale;
    float _rotation;
    
    //flag
    bool _bulletExploded;
    bool _renderingExplode;
    bool _initing;
    
    /** events */
    std::vector<Bullet::EventLog> _eventLogs;
    Bullet::EventLog* _explodeLog;
    
    /** cocos */
    cocos2d::Node* _mainNode;
    cocos2d::Node* _shadowNode;
    cocos2d::Node* _body;
    cocos2d::Node* _shadow;
    
public:
    /** init */
    BulletRender();
    virtual ~BulletRender();
    virtual bool init(const WorldObject* object, WorldRender* worldRender) override;
    
    /** override WorldObjectRender */
    virtual void render() override;
    virtual void attach2WorldRender(WorldRender* worldRender) override;
    virtual void dettachFromWorldRender(WorldRender* worldRender) override;
    
    /** override BulletObserver*/
    virtual void onNotifyBulletEvents(const std::vector<Bullet::EventLog>& events) override;
    
    /** interface */
    cocos2d::Node* addEffect(const std::string& file, bool loop, bool toBody, const std::function<void ()>& callback = nullptr);
    
private:
    void updateParams(const Coordinate32& currentPos,
        const Coordinate32& targetPos, bool init = false);
    void renderPosition(const Coordinate32& currentPos);
    void renderFlyingAnimation();
    void renderExplodeAnimation();
    void explodeCallback();
};
    
}}


#endif /* BulletRender_h */
