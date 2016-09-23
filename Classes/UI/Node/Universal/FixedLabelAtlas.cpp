//
//  FixedLabelAtlas.cpp
//  Underworld_Client
//
//  Created by burst on 15/9/5.
//
//

#include "FixedLabelAtlas.h"

using namespace std;

FixedLabelAtlas::FixedLabelAtlas(){
    
}

FixedLabelAtlas::~FixedLabelAtlas(){
    
}

FixedLabelAtlas* FixedLabelAtlas::create(const std::string &string, const std::string &charMapFile, int itemWidth, int itemHeight, int startCharMap) {
    FixedLabelAtlas* ret = new (std::nothrow) FixedLabelAtlas();
    if(ret && ret->initWithString(string, charMapFile, itemWidth, itemHeight, startCharMap))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FixedLabelAtlas::initWithString(const std::string& string, const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap) {
    if(LabelAtlas::initWithString(string, charMapFile, itemWidth, itemHeight, startCharMap)){
        setString(string);
        return true;
    }
    return false;
}

void FixedLabelAtlas::setString(const std::string &label) {
    _originalLabel = label;
    std::string fixedLabel;
    fixedLabel.reserve(label.size());
    for(string::size_type i = 0; i < label.size(); i++) {
        fixedLabel.push_back((label[i] - _mapStartChar) * 2 + _mapStartChar);
    }
    LabelAtlas::setString(fixedLabel);
}

const std::string& FixedLabelAtlas::getString() const {
    return _originalLabel;
}