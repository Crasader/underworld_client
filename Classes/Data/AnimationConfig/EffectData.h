//
//  EffectData.h
//  Underworld_Client
//
//  Created by wenchengye on 16/9/14.
//
//

#ifndef EffectData_h
#define EffectData_h

#include "CocosGlobal.h"

class EffectData {
private:
    static const std::string EFFECT_DATA_PARAMS_SPLITOR;
    
private:
    std::string _resourceFile;
    float _scale;
    float _frameDelay;
    
public:
    EffectData() : _scale(1.f), _frameDelay(DEFAULT_FRAME_DELAY) {};
    virtual ~EffectData() {};
    
    void init(const std::string& s);
    
    /** getters */
    const std::string& getResourceFile() const        {return _resourceFile;}
    float getScale() const                            {return _scale;}
    float getFrameDelay() const                       {return _frameDelay;}

};

#endif /* EffectData_h */
