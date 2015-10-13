//
//  CocosUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "CocosUtils.h"
#include <iomanip>
#include "FixedLabelAtlas.h"
#include "NumberJump.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#endif

USING_NS_CC;
using namespace ui;
using namespace std;

#pragma mark - labels
Label *CocosUtils::createLabel(const string& text, float fontSize, const Size& dimensions, TextHAlignment hAlignment, TextVAlignment vAlignment)
{
    static const string fontName("fonts/Font.ttf");
    Label *ret = Label::createWithTTF(text, fontName, fontSize, dimensions, hAlignment, vAlignment);
    float posOffset = MIN(5.0f, MAX(1.0f, fontSize * 0.05f));
    ret->enableShadow(Color4B::BLACK, Size(.0f, -posOffset));
    ret->enableOutline(Color4B::BLACK, 1);
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
cocos2d::LabelAtlas *CocosUtils::create10x25Number(const std::string& text) {
    static const char *charMapFile = "fonts/10X25.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create10x25Number_Green(const std::string& text) {
    static const char *charMapFile = "fonts/10X25_green.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create10x25Number_Red(const std::string& text) {
    static const char *charMapFile = "fonts/10X25_red.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


cocos2d::LabelAtlas *CocosUtils::create12x30Number(const std::string& text) {
    static const char *charMapFile = "fonts/12X30.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create12x30Number_Green(const std::string& text) {
    static const char *charMapFile = "fonts/12X30_green.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create12x30Number_Red(const std::string& text) {
    static const char *charMapFile = "fonts/12X30_red.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


cocos2d::LabelAtlas *CocosUtils::create16x40Number(const std::string& text) {
    static const char *charMapFile = "fonts/16X40.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create16x40Number_Green(const std::string& text) {
    static const char *charMapFile = "fonts/16X40_green.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create16x40Number_Red(const std::string& text) {
    static const char *charMapFile = "fonts/16X40_red.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

cocos2d::LabelAtlas *CocosUtils::create28x44Number(const std::string& text){
    static const char *charMapFile = "fonts/28X44.png";
    static const int charMapWidth = 28;
    static const int charMapHeight = 44;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

void CocosUtils::jumpNumber(cocos2d::LabelAtlas *label, int toNumer, float duration)
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

float CocosUtils::getFitScreenScale(Node *root)
{
    CCASSERT(dynamic_cast<Widget *>(root) != nullptr, "Only support widget currently");
    
    const Size& winSize = Director::getInstance()->getWinSize();
    const Size& rootSize = root->getContentSize();
    const float scaleX = winSize.width / rootSize.width;
    const float scaleY = winSize.height / rootSize.height;
    
    return ((scaleX < 1.0f || scaleY < 1.0f) ? MIN(scaleX, scaleY) : 1.0f);
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
    return StringUtils::format("%02ld:%02d", minute % 60, second);
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