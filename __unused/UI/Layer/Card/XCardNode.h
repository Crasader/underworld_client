//
//  XCardNode.h
//  Underworld_Client
//
//  Created by Andy on 16/6/28.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef XCardNode_h
#define XCardNode_h

#include "CardNode.h"

class XCardNode : public Node
{
public:
    static XCardNode* create();
    virtual ~XCardNode();
    
    CardNode* getCardNode() const;
    void updateCount(int count);
    
protected:
    XCardNode();
    virtual bool init() override;
    
private:
    CardNode* _cardNode;
    Label* _countLabel;
};

#endif /* XCardNode_h */
