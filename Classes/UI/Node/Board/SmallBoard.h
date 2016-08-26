//
//  SmallBoard.h
//  Underworld_Client
//
//  Created by Andy on 16/8/26.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef SmallBoard_h
#define SmallBoard_h

#include "UniversalBoard.h"

class SmallBoard: public UniversalBoard
{
public:
    static SmallBoard* create();
    virtual ~SmallBoard();
    
    Node* getSubNode() const;
    
protected:
    SmallBoard();
    virtual bool init() override;
    
private:
    Node* _subNode;
};

#endif /* SmallBoard_h */
