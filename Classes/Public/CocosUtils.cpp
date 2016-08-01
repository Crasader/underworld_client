//
//  CocosUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "CocosUtils.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Global.h"
#include <iomanip>
#include "FixedLabelAtlas.h"
#include "NumberJump.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#endif

USING_NS_CC;
using namespace ui;
using namespace std;

#pragma mark - labels
Label *CocosUtils::createLabel(const string& text, float fontSize, const string& fontName, const Size& dimensions, TextHAlignment hAlignment, TextVAlignment vAlignment)
{
#if true
    Label *ret = Label::createWithSystemFont(text, fontName, fontSize, dimensions, hAlignment, vAlignment);
#else
    static const string fontName("fonts/Font.ttf");
    Label *ret = Label::createWithTTF(text, fontName, fontSize, dimensions, hAlignment, vAlignment);
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

void CocosUtils::replaceScene(Scene* scene, bool animated)
{
    if (scene) {
        auto director = Director::getInstance();
        if (animated && director->getRunningScene()) {
            auto pScene = TransitionFade::create(1.2f, scene);
            if (pScene) {
                director->replaceScene(pScene);
            } else {
                director->runWithScene(scene);
            }
        } else {
            director->runWithScene(scene);
        }
    }
}

void CocosUtils::cleanMemory()
{
    AnimationCache::destroyInstance();
    SpriteFrameCache::destroyInstance();
    Director::getInstance()->getTextureCache()->removeAllTextures();
}

Sprite* CocosUtils::createPureColorSprite(const Size& size, const Color4B& color)
{
    // 1. create texture
    static const uint len = 4;
    GLubyte data[len] = {color.r, color.g, color.b, 180};
    
    Texture2D* texture = new (nothrow) Texture2D();
    if (texture) {
        bool ret = texture->initWithData(data, len, Texture2D::PixelFormat::RGBA8888, 1, 1, Size(1, 1));
        if (ret) {
            texture->autorelease();
        } else {
            CC_SAFE_DELETE(texture);
        }
    }
    
    // 2. create sprite
    Sprite* sprite = Sprite::createWithTexture(texture);
    sprite->setScale(size.width, size.height);
    
    // 3.you need to create a new sprite, you cannot change the contentSize of the old sprite directly
    Sprite* newSprite = Sprite::create();
    newSprite->setContentSize(size);
    sprite->setPosition(Point(size.width / 2, size.height / 2));
    newSprite->addChild(sprite);
    
    return newSprite;
}

Sprite* CocosUtils::createTitle(const string& title, float fontSize)
{
    Sprite* titleBg = Sprite::create("GameImages/test/ui_black_6.png");
    Label* titleLabel = CocosUtils::createLabel(title, fontSize);
    titleBg->addChild(titleLabel);
    const Size& titleSize = titleBg->getContentSize();
    titleLabel->setPosition(Point(titleSize.width / 2, titleSize.height / 2));
    return titleBg;
}

#pragma mark - animations
void CocosUtils::loadPVR(const string& file)
{
    static const string root("pvr/");
    const string plist = root + file + ".plist";
    const string pvr = root + file + ".pvr.ccz";
    if (FileUtils::getInstance()->isFileExist(plist)) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist, pvr);
    }
}

SpriteFrame* CocosUtils::getFrame(const string& folder, int frameIndex)
{
    std::string fileName = "1";
    fileName.append(std::to_string(frameIndex / 1000));
    frameIndex = frameIndex % 1000;
    fileName.append(std::to_string(frameIndex / 100));
    frameIndex = frameIndex % 100;
    fileName.append(std::to_string(frameIndex / 10));
    fileName.append(std::to_string(frameIndex % 10));
    
    auto file = folder + "/" + fileName;
    return SpriteFrameCache::getInstance()->getSpriteFrameByName(file);
}

static void playAnimation(Node* node,
                          const Vector<SpriteFrame*>& frames,
                          bool loop,
                          float frameDelay,
                          const function<void()>& callback)
{
    if (node) {
        node->stopAllActions();
        
        if (frames.size() > 0) {
            auto animation = Animation::createWithSpriteFrames(frames);
            animation->setDelayPerUnit(frameDelay);
            animation->setRestoreOriginalFrame(false);
            
            Action* action(nullptr);
            auto animate = Animate::create(animation);
            if (loop) {
                action = RepeatForever::create(animate);
            } else {
                action = Sequence::create(animate, CallFunc::create(callback), nullptr) ;
            }
            
            node->runAction(action);
        } else {
            assert(false);
        }
    }
}

void CocosUtils::playAnimation(Node* node,
                               const string& folder,
                               bool loop,
                               int startIdx,
                               int endIdx,
                               float frameDelay,
                               const function<void()>& callback)
{
    Vector<SpriteFrame*> frames;
    for (int i = startIdx; ; ++i) {
        if (endIdx >= 0 && endIdx < i) {
            break;
        }
        
        auto frame = getFrame(folder, i);
        if (frame) {
            frames.pushBack(frame);
        }
        else {
            break;
        }
    }
    
    if (loop && endIdx < 0) {
        for (int i = 0; i < startIdx; ++i) {
            auto frame = getFrame(folder, i);
            if (frame) {
                frames.pushBack(frame);
            }
            else {
                break;
            }
        }
    }
    
    ::playAnimation(node, frames, loop, frameDelay, callback);
}

Sprite* CocosUtils::playAnimation(const string& folder,
                                  bool loop,
                                  int startIdx,
                                  int endIdx,
                                  float frameDelay,
                                  const function<void()>& callback)
{
    auto frame = getFrame(folder, startIdx);
    CCASSERT(frame, "Null frame");
    if (frame) {
        auto sprite = Sprite::createWithSpriteFrame(frame);
        playAnimation(sprite, folder, loop, startIdx, endIdx, frameDelay, callback);
        return sprite;
    }
    
    return nullptr;
}

Sprite* CocosUtils::playAnimation(const string& folder,
                                  bool loop,
                                  float frameDelay,
                                  const function<void()>& callback)
{
    return playAnimation(folder, loop, 0, -1, frameDelay, callback);
}

Node* CocosUtils::playCSBAnimation(const string& file,
                                   bool loop,
                                   int frameIndex,
                                   const function<void(Node*)>& callback)
{
    auto node = CSLoader::createNode(file);
    auto action = CSLoader::createTimeline(file);
    node->runAction(action);
    action->gotoFrameAndPlay(frameIndex, loop);
    if (!loop) {
        action->setLastFrameCallFunc([=]() {
            if (callback) {
                callback(node);
            }
        });
    } else {
        assert(!callback);
    }
    
    return node;
}

Node* CocosUtils::createExitButton(Node* parent, const function<void()>& callback)
{
    const Size& winSize = parent->getContentSize();
    static const string file("GameImages/public/ui_guanbi.png");
    auto button = Button::create(file, file);
    button->addClickEventListener([callback](Ref*) {
        if (callback) {
            callback();
        }
    });
    parent->addChild(button);
    
    static const Vec2 offset(5.0f, 5.0f);
    const Size& size = button->getContentSize();
    button->setPosition(Point(winSize.width - size.width / 2, winSize.height - size.height / 2) - offset);
    
    return button;
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