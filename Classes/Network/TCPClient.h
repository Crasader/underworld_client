//
//  TCPClient.h
//  Underworld_Client
//
//  Created by Andy on 16/3/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef TCPClient_h
#define TCPClient_h

#include <string>

enum class TcpErrorCode {
    Success,
    TimeOut,
    ConnectionFailure,
    Unknown
};

class TCPClientObserver
{
public:
    virtual ~TCPClientObserver() {}
    virtual void onConnect(const std::string& url, uint16_t port) {}
    virtual void onDisconnect(TcpErrorCode code) {}
    virtual void onWriteData(long tag) {}
    virtual void onReadData(const std::string& data, long tag) {}
    virtual void onError(TcpErrorCode code) {}
};

namespace TCPClient {
    void init(TCPClientObserver* observer);
    void purge();
    
    void connect(const std::string& url, uint16_t port);
    void disconnect();
    
    void writeData(const std::string& data, long tag);
    void writeData(const char* data, unsigned long len, long tag);
    void readData(long tag);
}

#endif /* TCPClient_h */
