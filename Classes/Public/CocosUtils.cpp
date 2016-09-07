//
//  CocosUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "CocosUtils.h"
#include "cocostudio/CocoStudio.h"
#include "Global.h"
#include <iomanip>
#include "FixedLabelAtlas.h"
#include "NumberJump.h"
#include "CoreUtils.h"

// for "getFileLists"
#include <dirent.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#endif

USING_NS_CC;
using namespace ui;
using namespace std;

string CocosUtils::getResourcePath(const string &file)
{
    return "GameImages/public/" + file;
}

void CocosUtils::getFileLists(const string& folder, const string& prefix, const string& extension, vector<string>& output)
{
    if (folder.empty()) {
        return;
    }
    
    DIR* dir = opendir(folder.c_str());
    if (dir) {
        string rfolder(folder);
        if ('/' != rfolder.back()) {
            rfolder += "/";
        }
        
        string rprefix(prefix);
        if ('/' != rprefix.back()) {
            rprefix.append("/");
        }
        
        struct dirent* ent(nullptr);
        while (nullptr != (ent = readdir(dir))) {
            const char* name(ent->d_name);
            if (!name || strlen(name) == 0) {
                continue;
            }
            
            const string file(name);
            auto type(ent->d_type);
            if (DT_REG == type) {
                if (extension.empty() || FileUtils::getInstance()->getFileExtension(file) == extension) {
                    output.push_back(rprefix + file);
                }
            } else if (DT_DIR == type) {
                if ("." != file && ".." != file) {
                    const string subpath(rfolder + file + "/");
                    const string subprefix(rprefix + file + "/");
                    getFileLists(subpath, subprefix, extension, output);
                }
            }
        }
        
        closedir(dir);
    }
}

#pragma mark - labels
Label *CocosUtils::createLabel(const string& text, float fontSize, const string& fontName, const Size& dimensions, TextHAlignment hAlignment, TextVAlignment vAlignment)
{
    const string msg(text.empty() ? " " : text);
#if true
    Label *ret = Label::createWithSystemFont(msg, fontName, fontSize, dimensions, hAlignment, vAlignment);
#else
    static const string fontName("fonts/Font.ttf");
    Label *ret = Label::createWithTTF(msg, fontName, fontSize, dimensions, hAlignment, vAlignment);
    float posOffset = MIN(5.0f, MAX(1.0f, fontSize * 0.05f));
    ret->enableShadow(Color4B::BLACK, Size(.0f, -posOffset));
    ret->enableOutline(Color4B::BLACK, 1);
#endif
    
    return ret;
}

// only for wave num
LabelAtlas *CocosUtils::create42x52Number(const string& text)
{
    static const char *charMapFile = "fonts/42X52.png";
    static const int charMapWidth = 42;
    static const int charMapHeight = 52;
    static int startCharMap = '-';
    
    LabelAtlas *labelAtlas = LabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


/**
 @brief
 $ % L V v ￥ x + s - . / 0 1 2 3 4 5 6 7 8 9 0 :
 $ % & ' ( ) * + s - . / 0 1 2 3 4 5 6 7 8 9 0 :
 */
LabelAtlas *CocosUtils::create10x25Number(const string& text) {
    static const char *charMapFile = "fonts/10X25.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create10x25Number_Green(const string& text) {
    static const char *charMapFile = "fonts/10X25_green.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create10x25Number_Red(const string& text) {
    static const char *charMapFile = "fonts/10X25_red.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


LabelAtlas *CocosUtils::create12x30Number(const string& text) {
    static const char *charMapFile = "fonts/12X30.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create12x30Number_Green(const string& text) {
    static const char *charMapFile = "fonts/12X30_green.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create12x30Number_Red(const string& text) {
    static const char *charMapFile = "fonts/12X30_red.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


LabelAtlas *CocosUtils::create16x40Number(const string& text) {
    static const char *charMapFile = "fonts/16X40.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create16x40Number_Green(const string& text) {
    static const char *charMapFile = "fonts/16X40_green.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create16x40Number_Red(const string& text) {
    static const char *charMapFile = "fonts/16X40_red.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create28x44Number(const string& text){
    static const char *charMapFile = "fonts/28X44.png";
    static const int charMapWidth = 28;
    static const int charMapHeight = 44;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

void CocosUtils::jumpNumber(LabelAtlas *label, int toNumer, float duration)
{
    if(label) {
        label->stopActionByTag(NUMBERJUMP_ACTION_TAG);
        int fromNumer = 0;
        string curNumerStr = label->getString();
        if(!curNumerStr.empty()){
            fromNumer = atoi(curNumerStr.c_str());
        }
        if (toNumer == fromNumer) {
            return;
        }
        NumberJump* jump = NumberJump::create(duration, fromNumer, toNumer, CC_CALLBACK_1(LabelAtlas::setString, label));
        Sequence* seq = Sequence::create(jump, CallFuncN::create([toNumer](Ref* pSender){
            LabelAtlas *label = dynamic_cast<LabelAtlas*>(pSender);
            if(label) {
                label->setString(StringUtils::format("%d", toNumer));
            }
        }), NULL);
        seq->setTag(NUMBERJUMP_ACTION_TAG);
        
        label->runAction(seq);
    }
}

void CocosUtils::jumpNumber(Label *label, int toNumer, float duration)
{
    if(label) {
        label->stopActionByTag(NUMBERJUMP_ACTION_TAG);
        int fromNumer = 0;
        string curNumerStr = label->getString();
        if(!curNumerStr.empty()){
            fromNumer = atoi(curNumerStr.c_str());
        }
        if (toNumer == fromNumer) {
            return;
        }
        NumberJump* jump = NumberJump::create(duration, fromNumer, toNumer, CC_CALLBACK_1(Label::setString, label));
        Sequence* seq = Sequence::create(jump, CallFuncN::create([toNumer](Ref* pSender){
            Label *label = dynamic_cast<Label*>(pSender);
            if(label) {
                label->setString(StringUtils::format("%d", toNumer));
            }
        }), NULL);
        seq->setTag(NUMBERJUMP_ACTION_TAG);
        
        label->runAction(seq);
    }
}

float CocosUtils::getFitScreenScale(Node *root)
{
    CCASSERT(dynamic_cast<Widget *>(root) != nullptr, "Only support widget currently");
    
    const Size& winSize = Director::getInstance()->getWinSize();
    const Size& rootSize = root->getContentSize();
    const float scaleX = winSize.width / rootSize.width;
    const float scaleY = winSize.height / rootSize.height;
    
    return ((scaleX < 1.0f || scaleY < 1.0f) ? MIN(scaleX, scaleY) : 1.0f);
}

void CocosUtils::readdChild(Node* parent, Node* child)
{
    if (child && parent) {
        auto cp(child->getParent());
        if (cp != parent) {
            Point worldPoint(child->getPosition());
            if (cp) {
                child->retain();
                worldPoint = cp->convertToWorldSpace(worldPoint);
                child->removeFromParent();
            }
            parent->addChild(child);
            
            // reset position after "addChild",
            // because the parent of "child" may not be "parent"
            if (cp) {
                child->setPosition(child->getParent()->convertToNodeSpace(worldPoint));
                child->release();
            }
        }
    }
}

void CocosUtils::replaceScene(Scene* scene, bool animated)
{
    if (scene) {
        auto director = Director::getInstance();
        auto pScene(scene);
        if (animated) {
            pScene = TransitionFade::create(1.2f, scene);
        }
        
        if (director->getRunningScene()) {
            director->replaceScene(pScene);
        } else {
            director->runWithScene(pScene);
        }
    }
}

void CocosUtils::cleanMemory()
{
    AnimationCache::destroyInstance();
    SpriteFrameCache::destroyInstance();
    Director::getInstance()->getTextureCache()->removeAllTextures();
}

#pragma mark - animations
SpriteFrame* CocosUtils::getPVRFrame(const string& folder, int idx)
{
    idx += 1;
    string fileName = "1";
    fileName.append(UnderWorld::Core::UnderWorldCoreUtils::to_string(idx / 1000));
    idx = idx % 1000;
    fileName.append(UnderWorld::Core::UnderWorldCoreUtils::to_string(idx / 100));
    idx = idx % 100;
    fileName.append(UnderWorld::Core::UnderWorldCoreUtils::to_string(idx / 10));
    fileName.append(UnderWorld::Core::UnderWorldCoreUtils::to_string(idx % 10));
    
    auto file = folder + "/" + fileName + ".png";
    return SpriteFrameCache::getInstance()->getSpriteFrameByName(file);
}

Node* CocosUtils::getAnimationNode(const string& folder, int idx)
{
    Node* node(nullptr);
    if (".csb" == FileUtils::getInstance()->getFileExtension(folder)) {
        CC_UNUSED_PARAM(idx);
        node = CSLoader::createNode(folder);
    } else {
        auto frame = getPVRFrame(folder, idx);
        if (frame) {
            node = Sprite::createWithSpriteFrame(frame);
        }
    }
    
    return node;
}

float CocosUtils::playAnimation(Node* node,
                                const string& folder,
                                float frameDelay,
                                bool loop,
                                int startIdx,
                                int endIdx,
                                const function<void(Node*)>& callback)
{
    CCASSERT(node, "Animayion node is null");
    if (node) {
        node->stopAllActions();
        
        if (".csb" == FileUtils::getInstance()->getFileExtension(folder)) {
            CC_UNUSED_PARAM(frameDelay);
            auto action = CSLoader::createTimeline(folder);
            node->runAction(action);
            
            if (endIdx > 0) {
                CCASSERT(endIdx >= startIdx, "Frame idx is not allowed");
                action->gotoFrameAndPlay(startIdx, endIdx, loop);
            } else {
                action->gotoFrameAndPlay(startIdx, loop);
            }
            
            if (loop) {
                CCLOG("CocosUtils::playAnimation : Callback will not be called when loop is enabled");
            } else {
                action->setLastFrameCallFunc([=]() {
                    if (callback) {
                        callback(node);
                    }
                });
            }
            
            return (float)action->getDuration() / 60.0f;
        } else {
            Vector<SpriteFrame*> frames;
            for (int i = startIdx; ; ++i) {
                if (endIdx >= 0 && endIdx < i) {
                    break;
                }
                
                auto frame = getPVRFrame(folder, i);
                if (frame) {
                    frames.pushBack(frame);
                }
                else {
                    break;
                }
            }
            
            if (loop && endIdx < 0) {
                for (int i = 0; i < startIdx; ++i) {
                    auto frame = getPVRFrame(folder, i);
                    if (frame) {
                        frames.pushBack(frame);
                    }
                    else {
                        break;
                    }
                }
            }
            
            const size_t cnt(frames.size());
            CCASSERT(cnt > 0, "Animation is not exist.");
            auto cache = AnimationCache::getInstance();
            auto animation = cache->getAnimation(folder);
            if (!animation && cnt > 0) {
                animation = Animation::createWithSpriteFrames(frames, frameDelay);
                animation->setRestoreOriginalFrame(false);
                cache->addAnimation(animation, folder);
            }
            
            if (animation) {
                Action* action(nullptr);
                auto animate = Animate::create(animation);
                if (loop) {
                    action = RepeatForever::create(animate);
                } else {
                    action = Sequence::create(animate, CallFuncN::create(callback), nullptr) ;
                }
                
                node->runAction(action);
                
                return animation->getDuration();
            }
        }
    }
    
    return 0.0f;
}

Node* CocosUtils::playAnimation(const string& file,
                                float frameDelay,
                                bool loop,
                                int startIdx,
                                int endIdx,
                                const function<void(Node*)>& callback)
{
    Node* node(nullptr);
    if (".plist" == FileUtils::getInstance()->getFileExtension(file)) {
        CC_UNUSED_PARAM(frameDelay);
        CC_UNUSED_PARAM(startIdx);
        CC_UNUSED_PARAM(endIdx);
        CC_UNUSED_PARAM(callback);
        node = ParticleSystemQuad::create(file);
        auto particle = dynamic_cast<ParticleSystemQuad*>(node);
        assert(particle);
        particle->setAutoRemoveOnFinish(!loop);
    } else {
        node = getAnimationNode(file, startIdx);
        playAnimation(node, file, frameDelay, loop, startIdx, endIdx, callback);
    }
    
    return node;
}

static Button* createExitButton(Node* parent, const function<void()>& callback, const string& file)
{
    const auto& psize(parent->getContentSize());
    auto button = Button::create(file, file);
    button->addClickEventListener([callback](Ref*) {
        if (callback) {
            callback();
        }
    });
    parent->addChild(button);
    
    static const Vec2 offset(8.0f, 8.0f);
    const auto& size = button->getContentSize();
    button->setPosition(Point(psize.width - size.width / 2, psize.height - size.height / 2) - offset);
    
    return button;
}

Button* CocosUtils::createGrayExitButton(Node* parent, const function<void()>& callback)
{
    return createExitButton(parent, callback, getResourcePath("ui_guanbi.png"));
}

Button* CocosUtils::createRedExitButton(Node* parent, const function<void()>& callback)
{
    return createExitButton(parent, callback, getResourcePath("button_hongse.png"));
}

void CocosUtils::fixWidgetTouchEvent(Widget* widget, bool& flag, const Widget::ccWidgetTouchCallback& touchedCallback, const Widget::ccWidgetClickCallback& clickedCallback)
{
    if (widget) {
        CC_ASSERT(widget->isTouchEnabled());
        widget->addTouchEventListener([=, &flag](Ref* pSender, Widget::TouchEventType type) {
            if (touchedCallback) {
                touchedCallback(pSender, type);
            }
            
            auto widget = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                flag = false;
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!flag) {
                    const auto& mp(widget->getTouchMovePosition());
                    const auto& bp(widget->getTouchBeganPosition());
                    if (abs(mp.x - bp.x) >= TOUCH_CANCEL_BY_MOVING_DISTANCE ||
                        abs(mp.y - bp.y) >= TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                        flag = true;
                    }
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                if (!flag && clickedCallback) {
                    clickedCallback(pSender);
                }
            }
        });
    }
}

#pragma mark - notifications
void CocosUtils::postNotification(const string& notification)
{
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(notification);
}

#pragma mark - time
double CocosUtils::getMillSecond()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    
    return (double)tv.tv_sec * 1000 + (double)tv.tv_usec / 1000;
}

string CocosUtils::getCurrentTime(const char* format)
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    
    struct tm *tm = localtime(&tv.tv_sec);
    
    char currentTime[128];
    strftime(currentTime, 128, format, tm);
    
    return currentTime;
}

string CocosUtils::getFormattedTime(long timeInterval)
{
    if (timeInterval < 0)
    {
        timeInterval = 0;
    }
    int second = timeInterval % 60;
    long minute = timeInterval / 60;
    return StringUtils::format("%ld:%02d", minute % 60, second);
}

string CocosUtils::getTimeString(int64_t timeMillSecond)
{
    struct tm tm;
    time_t timeT = timeMillSecond / 1000;
    tm = *localtime(&timeT);
    
    return StringUtils::format("%02d-%02d %02d:%02d", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}

string CocosUtils::getBeautyTime(long timeInterval)
{
    if (timeInterval < 0)
    {
        timeInterval = 0;
    }
    int second = timeInterval % 60;
    long minute = timeInterval / 60;
    long hour = minute / 60;
    long day = hour / 24;
    
    if (day >= 1){
        return StringUtils::format("%02ld days %02ld hours", day, hour % 24);
    } else if(hour >= 1){
        return StringUtils::format("%02ld hours %02ld minutes", hour, minute % 60);
    } else {
        return StringUtils::format("%02ld minutes %02d seconds", minute, second);
    }
}

#pragma mark - security
static const char *tohex = "0123456789ABCDEF";

int CocosUtils::hexCharToInt(char c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}

void CocosUtils::charToHex(const unsigned char *input, int inputLen, unsigned char *output)
{
    int i = 0;
    while(i < inputLen)
    {
        *output++ = tohex[input[i] >> 4];
        *output++ = tohex[input[i] & 0x0F];
        ++i;
    }
}

void CocosUtils::hexToChar(const unsigned char *input, int inputLen, unsigned char *output)
{
    int i = 0;
    while(i < inputLen)
    {
        *output++ = (unsigned char)(hexCharToInt(input[i]) << 4 | hexCharToInt(input[i+1]));
        i+=2;
    }
}

string CocosUtils::charToHex(const string &input)
{
    int i = 0;
    size_t inputLen = input.length();
    string output(inputLen * 2, 0);
    while(i < inputLen)
    {
        output[i * 2] = tohex[input[i] >> 4];
        output[i * 2 + 1] = tohex[input[i] & 0x0F];
        ++i;
    }
    return output;
}

string CocosUtils::hexToChar(const string &input)
{
    int i = 0;
    size_t inputLen = input.length();
    string output(inputLen / 2, 0);
    output.resize(inputLen / 2);
    while(i < inputLen)
    {
        output[i / 2] = (unsigned char)(hexCharToInt(input[i]) << 4 | hexCharToInt(input[i+1]));
        i+=2;
    }
    return output;
}

string CocosUtils::itoa(int i)
{
    return StringUtils::format("%d", i);
}

string CocosUtils::lTrim(const string &s)
{
    string ret(s);
    return ret.erase(0, s.find_first_not_of(" \t\n\r"));
}

string CocosUtils::rTrim(const string &s)
{
    string ret(s);
    return ret.erase(s.find_last_not_of(" \t\n\r")+1);
}

string CocosUtils::trim(const string &s)
{
    return rTrim(lTrim(s));
}

bool CocosUtils::sort_by_tag(Node* a, Node* b)
{
    return a->getTag() < b->getTag();
}

string CocosUtils::urlEncode(const string &value)
{
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;
    
    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
    {
        string::value_type c = (*i);
        
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
            continue;
        }
        
        // Any other characters are percent-encoded
        escaped << '%' << uppercase << setw(2) << int((unsigned char) c);
    }
    
    return escaped.str();
}