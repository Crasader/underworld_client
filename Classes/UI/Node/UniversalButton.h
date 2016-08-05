//
//  UniversalButton.h
//  Underworld_Client
//
//  Created by Andy on 16/7/27.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef UniversalButton_h
#define UniversalButton_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UniversalButton : public Node
{
public:
    enum class BSize {
        Big,
        Small,
    };
    
    enum class BType {
        Blue,
        Red,
        Green
    };
    
    typedef std::function<void(Ref*)> Callback;
    
public:
    static UniversalButton* create(BSize size, BType type, const std::string& title);
    virtual ~UniversalButton();
    
    void setType(BType type);
    void setTitle(const std::string& title);
    void setCallback(const Callback& callback);
    void setPressedActionEnabled(bool enabled);
    void setSwallowTouches(bool swallow);
    void setEnabled(bool enabled);
    
    BType getType() const;
    Label* getLabel() const;
    
protected:
    static const std::string DefaultTitle;
    UniversalButton();
    bool init(BSize size, BType type, const std::string& title);
    void adjust();
    
private:
    BSize _bSize;
    BType _bType;
    Callback _callback;
    ui::Button* _button;
    bool _touchInvalid;
};

#endif /* UniversalButton_h */
