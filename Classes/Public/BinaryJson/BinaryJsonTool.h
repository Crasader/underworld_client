//
//  BinaryJsonTool.h
//  Underworld_Client
//
//  Created by burst on 16/4/15.
//
//

#ifndef BinaryJsonTool_h
#define BinaryJsonTool_h

#include <stdio.h>
#include "json/document.h"
#include <string>
#include <map>

class BinaryJsonTool {
public:
    BinaryJsonTool();
    virtual ~BinaryJsonTool();
    void initTemplates(const std::string& data, const std::string& keyField);
    std::string encode(const rapidjson::Value &root) const;
    void decode(const std::string &src, rapidjson::Document& document) const;
private:
    std::map<int, const rapidjson::Value*> templates;
private:
    void clearTemplates();
};
#endif /* BinaryJsonTool_h */
