//
//  AchievementNode.cpp
//  Underworld_Client
//
//  Created by Andy on 16/6/30.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "AchievementNode.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "LocalHelper.h"
#include "SoundManager.h"
#include "AchievementLocalData.h"
#include "AchievementData.h"
#include "ContentData.h"

using namespace std;
using namespace ui;

AchievementNode* AchievementNode::create(const AchievementData* data, ssize_t idx)
{
    AchievementNode *ret = new (nothrow) AchievementNode();
    if (ret && ret->init(data, idx))
    {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return nullptr;
}

AchievementNode::AchievementNode()
:_observer(nullptr)
,_data(nullptr)
,_idx(CC_INVALID_INDEX)
,_nameLabel(nullptr)
,_descriptionLabel(nullptr)
,_progressLabel(nullptr)
,_button(nullptr)
,_buttonHintLabel(nullptr) {}

AchievementNode::~AchievementNode()
{
    removeAllChildren();
}

void AchievementNode::registerObserver(AchievementNodeObserver *observer)
{
    _observer = observer;
}

bool AchievementNode::init(const AchievementData* data, ssize_t idx)
{
    if (Node::init()) {
        
        update(data, idx);
        return true;
    }
    
    return false;
}

ssize_t AchievementNode::getIdx() const
{
    return _idx;
}

void AchievementNode::update(const AchievementData* data, ssize_t idx)
{
    if (data) {
        _data = data;
        _idx = idx;
        
        const AchievementLocalData* localData = _data->getLocalData();
        if (_nameLabel) {
            _nameLabel->setString(localData ? localData->getName() : "");
        }
        
        if (_descriptionLabel) {
            _descriptionLabel->setString(localData ? localData->getDescription() : "");
        }
        
        updateProgress();
    }
}

void AchievementNode::updateProgress()
{
    if (_data) {
        const AchievementLocalData* localData = _data->getLocalData();
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
                    hint = LocalHelper::getString("");
                    file = "";
                    callback = nullptr;
                } else {
                    hint = LocalHelper::getString("");
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
