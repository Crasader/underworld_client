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

static Label* createAndSplitLabel(BlockConfig& nextBlockConfig, float width, float fontSize, const string& content) {
    Label* label = CocosUtils::createLabel(content, fontSize);
    float contentWidth = label->getContentSize().width;
    if (contentWidth <= width) {
        return label;
    }
    u16string utf16String;
    if (StringUtils::UTF8ToUTF16(content, utf16String))
    {
        ssize_t contentLen = utf16String.size();
        ssize_t cutContentLen = contentLen * width / contentWidth;
        
        bool checkLonger = false;
        Label* labelShorter = nullptr;
        while (true) {
            u16string cutUtf16Content = utf16String.substr(0, cutContentLen);
            string cutContent;
            if (StringUtils::UTF16ToUTF8(cutUtf16Content, cutContent)) {
                label = CocosUtils::createLabel(cutContent, fontSize);
                float contentWidth = label->getContentSize().width;
                if (contentWidth <= width) {
                    labelShorter = label;
                    if (checkLonger) {
                        nextBlockConfig._content = content.substr(cutContent.size());
                        return label;
                    } else {
                        cutContentLen += 1;
                    }
                } else {
                    checkLonger = true;
                    cutContentLen -= 1;
                    if (labelShorter) {
                        nextBlockConfig._content = content.substr(labelShorter->getString().size());
                        return labelShorter;
                    }
                }
                
            } else {
                return label;
            }
        }
        
    }
    return label;
}

AnnouncementNode::AnnouncementNode()
{
    
}

AnnouncementNode::~AnnouncementNode()
{
    
}

AnnouncementNode* AnnouncementNode::create(const std::string &content, float width)
{
    AnnouncementNode* pRet = new AnnouncementNode();
    if (pRet && pRet->init(content, width)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool AnnouncementNode::init(const std::string &content, float width)
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
    if (blocks.size() == 0) {
        return false;
    }
    stack<BlockConfig> suspense;
    for(vector<string>::const_reverse_iterator iter = blocks.rbegin(); iter != blocks.rend(); ++iter){
        const string& block = *iter;
        vector<string> config;
        Utils::split(config, block, "_", "", config_fields);
        if (config.size() != config_fields) {
            return false;
        }
        
        int i = 0;
        int newLine = stoi(config.at(i++));
        float fontSize = stof(config.at(i++));
        int color = stoi(config.at(i++));
        string content = config.at(i++);
        
        BlockConfig blockConfig(newLine, fontSize, color, content);
        suspense.push(blockConfig);
    }
    
    if (suspense.size() == 1) {
        const BlockConfig& config = suspense.top();
        Label *label = CocosUtils::createLabel(config._content, config._fontSize, DEFAULT_FONT, Size(width, .0f));
        label->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
        addChild(label);
        _contentSize = label->getContentSize();
        return true;
    } else {
        float x = .0f;
        float y = .0f;
        float lineHeight = .0f;
        Node* root = Node::create();
        addChild(root);
        while(!suspense.empty()) {
            
            BlockConfig config = suspense.top();
            suspense.pop();
            
            if (config._newLine)
            {
                x = .0f;
            }
            
            Label* label;
            if (width > .0f) {
                BlockConfig nextBlockConfig(1, config._fontSize, config._color, "");
                label = createAndSplitLabel(nextBlockConfig, width - x, config._fontSize, config._content);
                if (!nextBlockConfig._content.empty()) {
                    suspense.push(nextBlockConfig);
                }
            } else {
                label = CocosUtils::createLabel(config._content, config._fontSize);
            }
            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
            int height = label->getContentSize().height;
            Point position;
            if (config._newLine) {
                y -= lineHeight;
                
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
            int color = config._color;
            if(color >= 0 && color < colorNum)
            {
                const Color3B& color3b = colors[color];
                label->setColor(color3b);
            }
            label->setPositionX(x);
            x += label->getContentSize().width;
            if (width == .0f && x > _contentSize.width) {
                _contentSize.width = x;
            }
            _contentSize.height = -y + lineHeight;
            root->addChild(label);
        }
        
        if (width > .0f) {
            _contentSize.width = width;
        }
    }
    return true;
}

const Size& AnnouncementNode::getContentSize() const
{
    return _contentSize;
}



