//
//  QuestNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestNode.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "QuestLocalData.h"
#include "QuestData.h"
#include "ContentData.h"

using namespace std;
using namespace ui;
using namespace cocostudio;

QuestNode* QuestNode::create(const QuestData* data, ssize_t idx)
{
    QuestNode *ret = new (nothrow) QuestNode();
    if (ret && ret->init(data, idx))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

QuestNode::QuestNode()
:_observer(nullptr)
,_data(nullptr)
,_idx(CC_INVALID_INDEX)
,_nameLabel(nullptr)
,_descriptionLabel(nullptr)
,_progressLabel(nullptr)
,_button(nullptr)
,_buttonHintLabel(nullptr)
{
    
}

QuestNode::~QuestNode()
{
    removeAllChildren();
}

void QuestNode::registerObserver(QuestNodeObserver *observer)
{
    _observer = observer;
}

bool QuestNode::init(const QuestData* data, ssize_t idx)
{
    if (Node::init())
    {
        const Size& winSize = Director::getInstance()->getWinSize();
        
        static const string CsbFile("zhuchangjing.csb");
        Node* mainNode = CocosUtils::playCSBAnimation(CsbFile, false, 0, nullptr);
        mainNode->setPosition(Point(winSize.width / 2, winSize.height / 2));
        addChild(mainNode);
        
        Node* root = mainNode->getChildByTag(7);
        const Vector<Node*>& children = root->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 100:
                        {
                            Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                            child->addChild(label);
                            
                            _nameLabel = label;
                        }
                            break;
                        case 101:
                        {
                            Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                            label->setTextColor(Color4B::ORANGE);
                            child->addChild(label);
                            
                            _descriptionLabel = label;
                        }
                            break;
                        case 102:
                        {
                            Label* label = CocosUtils::createLabel(LocalHelper::getString("ui_questNode_reward_label"), DEFAULT_FONT_SIZE);
                            label->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
                            child->addChild(label);
                        }
                            break;
                        case 104:
                        {
                            Button* button = dynamic_cast<Button*>(child);
                            if (button) {
                                button->setPressedActionEnabled(true);
                                Node* child = button->getChildByTag(100);
                                if (child) {
                                    Label* label = CocosUtils::createLabel("", DEFAULT_FONT_SIZE);
                                    child->addChild(label);
                                    
                                    _buttonHintLabel = label;
                                }
                            }
                            
                            _button = button;
                        }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        update(data, idx);
        
        return true;
    }
    
    return false;
}

ssize_t QuestNode::getIdx() const
{
    return _idx;
}

void QuestNode::update(const QuestData* data, ssize_t idx)
{
    if (data) {
        _data = data;
        _idx = idx;
        
        const QuestLocalData* localData = _data->getLocalData();
        if (_nameLabel) {
            _nameLabel->setString(localData ? localData->getName() : "");
        }
        
        if (_descriptionLabel) {
            _descriptionLabel->setString(localData ? localData->getDescription() : "");
        }
        
        updateProgress();
    }
}

void QuestNode::updateProgress()
{
    if (_data) {
        const QuestLocalData* localData = _data->getLocalData();
        const vector<ContentData*>& contents = localData->getContents();
        if (contents.size() > 0) {
            const int progress = _data->getProgress();
            const int total = contents.at(0)->getCount(0);
            const string s = StringUtils::format("[%d/%d]", progress, total);
            const Point& ap = _descriptionLabel->getAnchorPoint();
            if (nullptr == _progressLabel) {
                _progressLabel = CocosUtils::createLabel(s, DEFAULT_FONT_SIZE);
                _progressLabel->setTextColor(_descriptionLabel->getTextColor());
                _progressLabel->setAnchorPoint(ap);
                _descriptionLabel->getParent()->addChild(_progressLabel);
            } else {
                _progressLabel->setString(s);
            }
            
            // reset position if needed
            _progressLabel->setPosition(_descriptionLabel->getPosition() + Point((1.0f - ap.x) * _descriptionLabel->getContentSize().width + ap.x * _progressLabel->getContentSize().width, 0));
            
            // update button
            if (_button) {
                string hint;
                string file;
                function<void()> callback;
                if (progress < total) {
                    hint = LocalHelper::getString("ui_questNode_buttonHint_unfinished");
                    file = "";
                    callback = nullptr;
                } else {
                    hint = LocalHelper::getString("ui_questNode_buttonHint_finished");
                    file = "";
                    callback = nullptr;
                }
                
                _buttonHintLabel->setString(hint);
                _button->loadTextures(file, file);
                _button->addClickEventListener([=](Ref *pSender){
                    SoundManager::getInstance()->playButtonSound();
                    if (callback) {
                        callback();
                    }
                });
            }
            
        } else if (_progressLabel) {
            _progressLabel->setString("");
        }
    }
}