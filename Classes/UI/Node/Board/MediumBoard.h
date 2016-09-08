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

class XButton;

class MediumBoard: public UniversalBoard
{
public:
    static MediumBoard* create();
    virtual ~MediumBoard();
    
    Node* getSubNode() const;
    void addChildToMidBottom(Node *child);
    
protected:
    MediumBoard();
    virtual bool init() override;
    
private:
    Node* _subNode;
    Node* _midBottomChild;
    Point _midBottomPoint;
};

#endif /* MediumBoard_h */
