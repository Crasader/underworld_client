//
//  BattleDeckTestNode.h
//  Underworld_Client
//
//  Created by Andy on 16/2/22.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef BattleDeckTestNode_h
#define BattleDeckTestNode_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

USING_NS_CC;
using namespace ui;

class BattleDeckTestNode;

class BattleDeckTestNodeObserver
{
public:
    virtual ~BattleDeckTestNodeObserver() {}
    virtual void onBattleDeckTestNodeTouchedBegan(BattleDeckTestNode* node) = 0;
    virtual void onBattleDeckTestNodeTouchedEnded(BattleDeckTestNode* node, bool isValid) = 0;
    virtual void onBattleDeckTestNodeTouchedCanceled(BattleDeckTestNode* node) = 0;
};

class BattleDeckTestNode: public Node
{
public:
    static BattleDeckTestNode* create(const std::string& name, bool isHero);
    virtual ~BattleDeckTestNode();
    void registerObserver(BattleDeckTestNodeObserver *observer);
    void reuse(const std::string& name, ssize_t idx);
    void update(bool reuse);
    void setSelected(bool selected);
    void setTouched(bool touched, bool isGameOver);
    
    // getters
    const std::string& getUnitName() const;
    
protected:
    BattleDeckTestNode();
    bool init(const std::string& name, bool isHero);
    std::string getIconFile(const std::string& name, bool enable) const;
    bool isHero() const;
    void addTouchedAction(bool touched, bool animated);
    
private:
    BattleDeckTestNodeObserver *_observer;
    Widget *_cardWidget;
    Button *_addButton;
    Sprite *_iconSprite;
    Sprite *_qualitySprite;
    Sprite *_goldSprite;
    Sprite *_woodSprite;
    Label *_countLabel;
    Label *_goldLabel;
    Label *_woodLabel;
    std::map<int, Sprite*> _starSprites;
    
    Sprite *_shiningSprite;
    std::string _unitName;
    ssize_t _idx;
    bool _isHero;
    bool _touchInvalid;
    
    bool _isIconTouched;
};

#endif /* BattleDeckTestNode_h */
