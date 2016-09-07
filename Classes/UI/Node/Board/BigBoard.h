//
//  BigBoard.h
//  Underworld_Client
//
//  Created by Andy on 16/9/7.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef BigBoard_h
#define BigBoard_h

#include "UniversalBoard.h"

class BigBoard: public UniversalBoard
{
public:
    static BigBoard* create();
    virtual ~BigBoard();
    
    Node* getSubNode() const;
    
protected:
    BigBoard();
    virtual bool init() override;
    
private:
    Node* _subNode;
};

#endif /* BigBoard_h */
