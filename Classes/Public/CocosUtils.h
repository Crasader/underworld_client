//
//  CocosUtils.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef CocosUtils_h
#define CocosUtils_h

#include "cocos2d.h"
#include "CocosGlobal.h"

namespace CocosUtils
{
#pragma mark - labels
    cocos2d::Label *createLabel(const std::string& text, float fontSize, const std::string& fontName = DEFAULT_FONT, const cocos2d::Size& dimensions = cocos2d::Size::ZERO, cocos2d::TextHAlignment hAlignment = cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment vAlignment = cocos2d::TextVAlignment::TOP);
    
    cocos2d::LabelAtlas *create42x52Number(const std::string& text);
    
    /** fixed **/
    cocos2d::LabelAtlas *create10x25Number(const std::string& text);
    cocos2d::LabelAtlas *create10x25Number_Green(const std::string& text);
    cocos2d::LabelAtlas *create10x25Number_Red(const std::string& text);
    
    cocos2d::LabelAtlas *create12x30Number(const std::string& text);
    cocos2d::LabelAtlas *create12x30Number_Green(const std::string& text);
    cocos2d::LabelAtlas *create12x30Number_Red(const std::string& text);
    
    cocos2d::LabelAtlas *create16x40Number(const std::string& text);
    cocos2d::LabelAtlas *create16x40Number_Green(const std::string& text);
    cocos2d::LabelAtlas *create16x40Number_Red(const std::string& text);
    
    cocos2d::LabelAtlas *create28x44Number(const std::string& text);
    
    // numer jump
    void jumpNumber(cocos2d::LabelAtlas *label, int toNumer, float duration);
    void jumpNumber(cocos2d::Label *label, int toNumer, float duration);
    
    float getFitScreenScale(cocos2d::Node *root);
    void replaceScene(cocos2d::Scene* scene, bool animated);
    
    void cleanMemory();
    
    // test
    cocos2d::Sprite* createPureColorSprite(const cocos2d::Size& size, const cocos2d::Color4B& color);
    cocos2d::Sprite* createTitle(const std::string& title, float fontSize);
    
    // pvr animation
    void loadPVR(const std::string& file);
    cocos2d::SpriteFrame* getFrame(const std::string& folder, int frameIndex);
    size_t playAnimation(cocos2d::Node* node, const std::string& folder, bool loop, int startIdx, int endIdx, float frameDelay, const std::function<void()>& callback);
    cocos2d::Sprite* playAnimation(const std::string& folder, bool loop, int startIdx, int endIdx, float frameDelay, const std::function<void()>& callback);
    cocos2d::Sprite* playAnimation(const std::string& folder, bool loop, float frameDelay, const std::function<void()>& callback);
    
    // csb animation
    cocos2d::Node* playCSBAnimation(const std::string& file, bool loop, int frameIndex, const std::function<void(cocos2d::Node*)>& callback);
    
    // buttons
    cocos2d::Node* createExitButton(cocos2d::Node* parent, const std::function<void()>& callback);
    
#pragma mark - notifications
    void postNotification(const std::string& notification);
    
#pragma mark - time
    double getMillSecond();
    std::string getTimeString(int64_t timeMillSecond);
    std::string getCurrentTime(const char* format);
    std::string getFormattedTime(long timeInterval);
    std::string getBeautyTime(long timeInterval);
    
#pragma mark - security
    int hexCharToInt(char c);
    void charToHex(const unsigned char *input, int inputLen, unsigned char *output);
    void hexToChar(const unsigned char *input, int inputLen, unsigned char *output);
    std::string charToHex(const std::string &input);
    std::string hexToChar(const std::string &input);
    
    std::string itoa(int i);
    std::string lTrim(const std::string &s);
    std::string rTrim(const std::string &s);
    std::string trim(const std::string &s);
    bool sort_by_tag(cocos2d::Node* a, cocos2d::Node* b);
    std::string urlEncode(const std::string &value);
}

#endif /* CocosUtils_h */
