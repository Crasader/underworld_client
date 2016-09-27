//
//  QuestNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestNode.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "QuestProperty.h"
#include "QuestData.h"
#include "ContentData.h"

using namespace std;
using namespace ui;

QuestNode* QuestNode::create()
{
    auto ret = new (nothrow) QuestNode();
    if (ret && ret->init())
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
,_nameLabel(nullptr)
,_descriptionLabel(nullptr)
,_progressLabel(nullptr)
,_button(nullptr)
,_buttonHintLabel(nullptr) {}

QuestNode::~QuestNode()
{
    removeAllChildren();
}

void QuestNode::registerObserver(QuestNodeObserver *observer)
{
    _observer = observer;
}

bool QuestNode::init()
{
    if (Node::init()) {
        return true;
    }
    
    return false;
}

void QuestNode::update(const QuestData* data)
{
    if (data) {
        _data = data;
        
        auto property = _data->getProperty();
        if (_nameLabel) {
            _nameLabel->setString(property ? LocalHelper::getString(property->getName()) : "");
        }
        
        if (_descriptionLabel) {
            _descriptionLabel->setString(property ? LocalHelper::getString(property->getDescription()) : "");
        }
        
        updateProgress();
    }
}

void QuestNode::updateProgress()
{
    if (_data) {
        auto property = _data->getProperty();
        const auto& contents = property->getContents();
        if (contents.size() > 0) {
            auto progress = _data->getProgress();
            auto total = contents.at(0)->getCount(0);
            auto s = StringUtils::format("[%d/%d]", progress, total);
            const auto& ap = _descriptionLabel->getAnchorPoint();
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
