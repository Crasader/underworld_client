//
//  ChatMark.h
//  Underworld_Client
//
//  Created by burst on 16/1/19.
//
//

#ifndef ChatMark_h
#define ChatMark_h

#include <stdio.h>


class ChatMark
{
public:
    ChatMark();
    virtual ~ChatMark();
    void toMap(std::map<std::string, std::string>& params) const;
public:
    int _groupId; //server list id
    int _wid; //world chat
    int _pid; //private chat
    int _gid; //guild chat
    int _tid; //group chat
    
};
#endif /* ChatMark_h */
