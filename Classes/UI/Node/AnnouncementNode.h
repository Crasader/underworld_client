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

struct BlockConfig {
    int _newLine;
    float _fontSize;
    int _color;
    std::string _content;
    BlockConfig(int newLine, float fontSize, int color, const std::string& content)
    :_newLine(newLine)
    ,_fontSize(fontSize)
    ,_color(color)
    ,_content(content) {}
};

/**
 AnchorPoint is Point(.0f, 1.0f);can not change
 **/
class AnnouncementNode: public Node
{
public:
    AnnouncementNode();
    virtual ~AnnouncementNode();
    virtual bool init(const std::string& content, float width);
    static AnnouncementNode* create(const std::string& content, float width = .0f);
    const Size& getContentSize() const;
private:
    Size _contentSize;
};


#endif /* AnnouncementNode_h */
