//
//  Gui.hpp
//  Underworld_Client
//
//  Created by Mac on 16/6/23.
//
//

#ifndef Gui_h
#define Gui_h

#include "cocos2d.h"

namespace UnderWorld{ namespace Core{

class Game;
class Commander;
class WorldRender;
    
class Gui {
public:
    Gui() {}
    virtual ~Gui() {}
    
    /** init */
    virtual void init(const Game* game, Commander* commander, WorldRender* worldRender) = 0;
    /** update */
    virtual void render(const Game* game) = 0;
    /** getter */
    virtual cocos2d::Node* getGuiView() = 0;
};

}}


#endif /* Gui_h */
