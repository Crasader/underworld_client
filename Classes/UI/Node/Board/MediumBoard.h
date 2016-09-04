//
//  MediumBoard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef MediumBoard_h
#define MediumBoard_h

#include "UniversalBoard.h"

class UniversalButton;

class MediumBoard: public UniversalBoard
{
public:
    static MediumBoard* create();
    virtual ~MediumBoard();
    
    void setButtonVisible(bool visible);
    void setButtonEnabled(bool enabled);
    void setButtonTitle(const std::string& title);
    void setButtonCallback(const Callback& callback);
    
    Node* getSubNode() const;
    
protected:
    MediumBoard();
    virtual bool init() override;
    
private:
    Node* _subNode;
    UniversalButton* _button;
    Callback _buttonCallback;
};

#endif /* MediumBoard_h */
