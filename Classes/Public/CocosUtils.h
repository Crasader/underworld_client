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
#include "ui/CocosGUI.h"
#include "CocosGlobal.h"

namespace CocosUtils
{
    std::string getResourcePath(const std::string &file);
    ResourceType core_resource_type_key_2_ui_resource_type(const std::string& key);
    
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
    void readdChild(cocos2d::Node* parent, cocos2d::Node* child);
    void replaceScene(cocos2d::Scene* scene, bool animated);
    
    void cleanMemory();
    
    // animation
    cocos2d::Node* getAnimationNode(const std::string& file, int idx);
    float playAnimation(cocos2d::Node* node,
                        const std::string& file,
                        float frameDelay,
                        bool loop,
                        int startIdx = 0,
                        int endIdx = -1,
                        const std::function<void(cocos2d::Node*)>& callback = nullptr);
    cocos2d::Node* playAnimation(const std::string& folder,
                                 float frameDelay,
                                 bool loop,
                                 int startIdx = 0,
                                 int endIdx = -1,
                                 const std::function<void(cocos2d::Node*)>& callback = nullptr);
    cocos2d::SpriteFrame* getPVRFrame(const std::string& folder, int idx);

    
    // buttons
    cocos2d::ui::Button* createGrayExitButton(cocos2d::Node* parent, const std::function<void()>& callback);
    cocos2d::ui::Button* createRedExitButton(cocos2d::Node* parent, const std::function<void()>& callback);
    void fixWidgetTouchEvent(cocos2d::ui::Widget* widget, bool& flag, const cocos2d::ui::Widget::ccWidgetClickCallback& clickedCallback);
    
#pragma mark - notifications
    void postNotification(const std::string& notification);
    
    bool sort_by_tag(cocos2d::Node* a, cocos2d::Node* b);
}

#endif /* CocosUtils_h */
