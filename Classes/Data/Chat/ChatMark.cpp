//
//  ChatMark.cpp
//  Underworld_Client
//
//  Created by burst on 16/1/19.
//
//

#include "ChatMark.h"
#include "cocos2d.h"
#include "Global.h"

USING_NS_CC;

ChatMark::ChatMark()
:_groupId(INVALID_VALUE)
,_wid(INVALID_VALUE)
,_pid(INVALID_VALUE)
,_gid(INVALID_VALUE)
,_tid(INVALID_VALUE)
{
    
}

ChatMark::~ChatMark()
{
    
}

void ChatMark::toMap(std::unordered_map<std::string, std::string> &params) const
{
    params.insert(make_pair("group", StringUtils::format("%d", _groupId)));
    params.insert(make_pair("w", StringUtils::format("%d", _wid)));
    params.insert(make_pair("p", StringUtils::format("%d", _pid)));
    params.insert(make_pair("g", StringUtils::format("%d", _gid)));
    params.insert(make_pair("t", StringUtils::format("%d", _tid)));
}
