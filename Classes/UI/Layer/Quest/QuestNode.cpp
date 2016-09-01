//
//  QuestNode.cpp
//  Underworld_Client
//
//  Created by Andy on 15/12/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "QuestNode.h"
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "QuestProperty.h"
#include "QuestData.h"
#include "ContentData.h"

using namespace std;
using namespace ui;

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
,_buttonHintLabel(nullptr) {}

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
    if (Node::init()) {
        
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
        
        const QuestProperty* property = _data->getProperty();
        if (_nameLabel) {
            _nameLabel->setString(property ? property->getName() : "");
        }
        
        if (_descriptionLabel) {
            _descriptionLabel->setString(property ? property->getDescription() : "");
        }
        
        updateProgress();
    }
}

void QuestNode::updateProgress()
{
    if (_data) {
        const QuestProperty* property = _data->getProperty();
        const vector<ContentData*>& contents = property->getContents();
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