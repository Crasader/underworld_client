//
//  CocosUtils.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef CocosUtils_h
#define CocosUtils_h

#include "Global.h"
#include "Coordinate.h"
#include "ui/CocosGUI.h"

namespace CocosUtils
{
#pragma mark - labels
    cocos2d::Label *createLabel(const std::string& text, float fontSize, const cocos2d::Size& dimensions = cocos2d::Size::ZERO, cocos2d::TextHAlignment hAlignment = cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment vAlignment = cocos2d::TextVAlignment::TOP);
    
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
    
    float getFitScreenScale(cocos2d::Node *root);
    
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
