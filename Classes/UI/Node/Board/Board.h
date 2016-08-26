//
//  Board.h
//  Underworld_Client
//
//  Created by Andy on 16/8/23.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef Board_h
#define Board_h

#include "UniversalBoard.h"

class Board: public UniversalBoard
{
public:
    static Board* create(int blocks);
    virtual ~Board();
    
    Node* getSubNode(int idx) const;
    
protected:
    Board();
    bool init(int blocks);
    
private:
    std::vector<Node*> _subNodes;
};

#endif /* Board_h */
