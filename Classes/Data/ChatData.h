//
//  ChatData.h
//  Underworld_Client
//
//  Created by Andy on 15/12/1.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef ChatData_h
#define ChatData_h

#include <iostream>
#include <vector>
#include "CocosGlobal.h"

namespace tinyxml2 { class XMLElement; }

// =====================================================
// Chat Data
// =====================================================

class ChatData
{
public:
    ChatData(ChatType type, tinyxml2::XMLElement *xmlElement);
    virtual ~ChatData();
    
    int getId() const;
    ChatType getType() const;
    long getTimeStamp() const;
    int getUserId() const;
    const std::string& getMessage() const;
    
private:
    int _id;
    ChatType _type;
    long _timeStamp;
    int _userId;
    std::string _message;
};



#endif /* ChatData_h */
