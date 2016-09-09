//
//  ChatTCPClientObserver.h
//  Underworld_Client
//
//  Created by Andy on 16/9/9.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef ChatTCPClientObserver_h
#define ChatTCPClientObserver_h

class ChatData;

class ChatTCPClientObserver
{
public:
    virtual ~ChatTCPClientObserver() {}
    virtual void onChatTCPClientConnected() = 0;
    virtual void onChatTCPClientReconnected() = 0;
    virtual void onChatTCPClientSent(const ChatData* data) = 0;
    virtual void onChatTCPClientReceived(const ChatData* data) = 0;
    virtual void onChatTCPClientError(int code) = 0;
};

#endif /* ChatTCPClientObserver_h */
