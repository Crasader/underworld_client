//
//  AnnouncementNode.h
//  Underworld_Client
//
//  Created by burst on 15/12/21.
//
//

#ifndef AnnouncementNode_h
#define AnnouncementNode_h

#include <stdio.h>
#include "CocosUtils.h"

USING_NS_CC;

class AnnouncementNode: public Node
{
public:
    AnnouncementNode();
    virtual ~AnnouncementNode();
    virtual bool init(const std::string& content);
    static AnnouncementNode* create(const std::string& content);
private:
    Size contentSize;
};


#endif /* AnnouncementNode_h */
