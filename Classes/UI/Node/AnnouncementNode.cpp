//
//  AnnouncementNode.cpp
//  Underworld_Client
//
//  Created by burst on 15/12/21.
//
//

#include "AnnouncementNode.h"
#include "cocos2d.h"
#include "Utils.h"

using namespace std;

static const int config_fields = 4;

AnnouncementNode::AnnouncementNode()
{
    
}

AnnouncementNode::~AnnouncementNode()
{
    
}

AnnouncementNode* AnnouncementNode::create(const std::string &content)
{
    AnnouncementNode* pRet = new AnnouncementNode();
    if (pRet && pRet->init(content)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool AnnouncementNode::init(const std::string &content)
{
    if (!Node::init()) {
        return false;
    }
    
    
    static const Color3B colors[] =
    {
        Color3B::WHITE,
        Color3B::YELLOW,
        Color3B::BLUE,
        Color3B::GREEN,
        Color3B::RED,
        Color3B::MAGENTA,
        Color3B::BLACK,
        Color3B::ORANGE,
        Color3B::GRAY
    };
    
    static const int colorNum = sizeof(colors) / sizeof(Color3B);
    
    vector<string> blocks;
    Utils::split(blocks, content, "\n");
    
    float x = .0f;
    float y = .0f;
    float lineHeight = .0f;
    Node* root = Node::create();
    addChild(root);
    for(vector<string>::const_iterator iter = blocks.begin(); iter != blocks.end(); ++iter){
        const string& block = *iter;
        vector<string> config;
        Utils::split(config, block, "_", "", config_fields);
        if (config.size() != config_fields) {
            return false;
        }
        
        int i = 0;
        int newLine = atoi(config.at(i++).c_str());
        float fontSize = atof(config.at(i++).c_str());
        int color = atoi(config.at(i++).c_str());
        string content = config.at(i++);
        
        Label* label = CocosUtils::createLabel(content, fontSize);
        label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        int height = label->getContentSize().height;
        Point position;
        if (newLine) {
            y -= lineHeight;
            x = .0f;
            
            lineHeight = height;
            
            root = Node::create();
            addChild(root);
            
            root->setPositionY(y - lineHeight * .5f);
        } else {
            float tmpHeight = height;
            if (tmpHeight > lineHeight) {
                lineHeight = tmpHeight;
                root->setPositionY(y - lineHeight * .5f);
            }
        }
        if(color >= 0 && color < colorNum)
        {
            const Color3B& color3b = colors[color];
            label->setColor(color3b);
        }
        label->setPositionX(x);
        x += label->getContentSize().width;
        root->addChild(label);
    }
    
    return true;
}



