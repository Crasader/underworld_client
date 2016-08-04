//
//  FrameLoader.cpp
//  Underworld_Client
//
//  Created by Andy on 16/7/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "FrameLoader.h"
#include "cocos2d.h"
#include "CocosUtils.h"

using namespace std;
USING_NS_CC;

static const string PlistExtension(".plist");
static const string TextureExtension(".pvr.ccz");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
static vector<string> FilesVector;
#else
static const vector<string> FilesVector = {
    "pvr/hero-Fat-equipment-shadows.plist",
    "pvr/hero-Fat-equipment.plist",
    "pvr/hero-Fat-shadows-0.plist",
    "pvr/hero-Fat-shadows-1.plist",
    "pvr/hero-Fat.plist",
    "pvr/hero-Rifleman-equipment-shadows.plist",
    "pvr/hero-Rifleman-equipment.plist",
    "pvr/hero-Rifleman-shadows.plist",
    "pvr/hero-Rifleman.plist",
    "pvr/HumanPriest.plist",
    "pvr/Niutoubing-0123.plist",
    "pvr/Niutoubing-456.plist",
    "pvr/Niutoubing-shadows-0-0123.plist",
    "pvr/Niutoubing-shadows-0-456.plist",
    "pvr/Niutoubing-shadows-1-0123.plist",
    "pvr/Niutoubing-shadows-1-456.plist",
    "pvr/soldier-Archer-shadows.plist",
    "pvr/soldier-Archer.plist",
    "pvr/Tower.plist",
    
    "pvr/effect/effect-1.plist",
    "pvr/effect/effect-2.plist",
    "pvr/effect/jian-test.plist",
    "pvr/effect/jian.plist",
    "pvr/effect/xeffect-1.plist"
};
#endif

static FrameLoader* s_pInstance(nullptr);
FrameLoader* FrameLoader::getInstance()
{
    if (!s_pInstance) {
        s_pInstance = new (nothrow) FrameLoader();
    }
    
    return s_pInstance;
}

void FrameLoader::purge()
{
    if (s_pInstance) {
        CC_SAFE_DELETE(s_pInstance);
    }
}

FrameLoader::FrameLoader()
:_isLoading(false)
,_callback(nullptr)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    static const string Folder("pvr/");
    static const string KeepFile("keep.txt");
    auto folder = FileUtils::getInstance()->fullPathForFilename(Folder + KeepFile);
    folder = folder.substr(0, folder.rfind('/'));
    CocosUtils::getFileLists(folder, Folder, PlistExtension, FilesVector);
#endif
}

FrameLoader::~FrameLoader() {}

#pragma mark - synchronous
void FrameLoader::addAllFrames()
{
    add(FilesVector);
}

void FrameLoader::add(const vector<string>& files)
{
    for (const auto& file : files) {
        add(file);
    }
}

void FrameLoader::add(const string& file)
{
    universalAdd(file, nullptr);
}

#pragma mark - asynchronous
void FrameLoader::addAllFramesAsync(const function<void()>& callback)
{
    addAsync(FilesVector, callback);
}

void FrameLoader::addAsync(const vector<string>& files, const function<void()>& callback)
{
    if (!_isLoading && !files.empty()) {
        _isLoading = true;
        _files = files;
        if (callback) {
            _callback = callback;
        } else {
            _callback = nullptr;
        }
        
        recursiveAddAsync();
    }
}

void FrameLoader::addAsync(const string &file, const function<void(string)>& callback)
{
    function<void(string)> cb(nullptr);
    if (callback) {
        cb = callback;
    } else {
        cb = [](string) {};
    }
    
    universalAdd(file, cb);
}

void FrameLoader::removeCachedFrames()
{
    for (auto iter = begin(_resources); iter != end(_resources); ++iter) {
        SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(*iter);
    }
    
    _resources.clear();
}

void FrameLoader::universalAdd(const string& file, const function<void(string)>& callback)
{
    if (file.empty() || _resources.find(file) != end(_resources)) {
        if (callback) {
            callback(file);
        }
    } else {
        const string& plist(file);
        auto pos = plist.rfind(PlistExtension);
        if (string::npos == pos) {
            return;
        }
        
        const string textureFile = plist.substr(0, pos) + TextureExtension;
        auto fileUtils = FileUtils::getInstance();
        if (!fileUtils->isFileExist(plist) || !fileUtils->isFileExist(textureFile)) {
            CC_ASSERT(false);
            if (callback) {
                callback(file);
            }
        } else {
            if (callback) {
                Director::getInstance()->getTextureCache()->addImageAsync(textureFile, [=](Texture2D* texture) {
                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist, texture);
                    _resources.insert(file);
                    if (callback) {
                        callback(file);
                    }
                });
            } else {
                SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist, textureFile);
                _resources.insert(file);
            }
        }
    }
}

void FrameLoader::recursiveAddAsync()
{
    if (!_files.empty()) {
        auto iter = _files.begin();
        string file = *iter;
        _files.erase(iter);
        addAsync(file, [this](const string& file) {
            recursiveAddAsync();
        });
    } else {
        _isLoading = false;
        
        if (_callback) {
            _callback();
        }
    }
}
