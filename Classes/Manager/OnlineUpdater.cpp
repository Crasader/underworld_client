//
//  OnlineUpdater.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "OnlineUpdater.h"
#include "GameData.h"

const int maxFailCount = 5;
const char * manifestPath = "project.manifest";
const char * storagePath = "Resource/";

OnlineUpdater * OnlineUpdater::create()
{
    OnlineUpdater *ret = new (std::nothrow) OnlineUpdater();
    return ret;
}

void OnlineUpdater::destroy(OnlineUpdater *instance)
{
    CC_SAFE_DELETE(instance);
}

OnlineUpdater::OnlineUpdater()
:_listener(nullptr)
,_am(nullptr)
,_amListener(nullptr)
{
    
}

OnlineUpdater::~OnlineUpdater()
{
    
}

void OnlineUpdater::startUpdate(OnlineUpdaterListener * listener)
{
    _listener = listener;
    
    CC_SAFE_RELEASE(_am);
    _am = AssetsManagerEx::create(manifestPath, FileUtils::getInstance()->getWritablePath() + storagePath);
    _am->retain();
    
    if (!_am->getLocalManifest()->isLoaded())
    {
        if (_listener)
        {
            _listener->onLocalManifestNotExist();
        }
    }
    else
    {
        CCLOG("Update start. %s", _am->getLocalManifest()->getVersion().c_str());
        GameData::getInstance()->onUpdateVersionID(_am->getLocalManifest()->getVersion());
        _amListener = cocos2d::extension::EventListenerAssetsManagerEx::create(_am, [this](EventAssetsManagerEx* event) {
            static int failCount = 0;
            if(failCount >= maxFailCount) return;
            EventAssetsManagerEx::EventCode code = event->getEventCode();
            switch (code)
            {
                case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
                case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
                case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
                case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
                case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
                {
                    failCount ++;
                    if (_listener)
                    {
                        _listener->onUpdateFailed(code);
                    }
                }
                    break;
                case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
                {
                    if (_listener)
                    {
                        _listener->onUpdateProgressed(event->getPercent());
                    }
                }
                    break;
                case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
                case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
                {
                    if (_listener)
                    {
                        _listener->onUpdateFinished();
                    }
                    CCLOG("Update done. %s", _am->getLocalManifest()->getVersion().c_str());
                    GameData::getInstance()->onUpdateVersionID(_am->getLocalManifest()->getVersion());
                }
                    break;
                case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
                {
                    CCLOG("Update failed. %s", event->getMessage().c_str());
                    
                    failCount ++;
                    if (failCount < maxFailCount)
                    {
                        _am->downloadFailedAssets();
                    }
                    else
                    {
                        if (_listener)
                        {
                            _listener->onUpdateFailed(code);
                        }
                    }
                }
                    break;
                default:
                    break;
            }
        });
        
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_amListener, 1);
        
        _am->update();
    }
}