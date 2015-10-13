//
//  FixedLabelAtlas.h
//  Underworld_Client
//
//  Created by burst on 15/9/5.
//
//

#ifndef FixedLabelAtlas__
#define FixedLabelAtlas__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

class FixedLabelAtlas :public LabelAtlas
{
public:
    FixedLabelAtlas();
    virtual ~FixedLabelAtlas();
    static FixedLabelAtlas* create(const std::string& string, const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);

    bool initWithString(const std::string& string, const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
public:
    virtual void setString(const std::string &label) override;
    virtual const std::string& getString() const override;
private:
    std::string _originalLabel;
};

#endif /* defined(FixedLabelAtlas__) */
