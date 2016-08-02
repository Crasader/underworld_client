//
//  DisplayBar.h
//  Underworld_Client
//
//  Created by Mac on 16/7/19.
//
//

#ifndef DisplayBar_h
#define DisplayBar_h

#include "cocos2d.h"

namespace UnderWorld{ namespace Core{
    
// ===============================
// 	class DisplayBar
//
//	the view show horizontal progress
// ===============================
    
class DisplayBar : public cocos2d::Node {
public:

private:
    cocos2d::ProgressTimer* _pt;
    cocos2d::Sprite* _bg;
    std::string _backgroundFile;
    std::string _foregroundFile;

public:
    static DisplayBar* create(const std::string& background,
        const std::string& foreground);
    virtual ~DisplayBar();
    
    /** interface */
    void setPercentage(float percentage);
    void setBackground(const std::string& background);
    void setForeground(const std::string& foreground);
    
    /** getters */
    float getPercentage() const                 {return _pt ? _pt->getPercentage() : 0.f;}
    const std::string& getBgFile() const        {return _backgroundFile;}
    const std::string& getFgFile() const        {return _foregroundFile;}

protected:
    DisplayBar();
    bool init(const std::string& background, const std::string& foreground);
};
    
    
}}

#endif /* DisplayBar_h */
