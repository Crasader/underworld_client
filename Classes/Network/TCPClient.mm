//
//  TCPClient.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/14.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "TCPClient.hpp"

#import "GCDAsyncSocket.h"
#import "NSString+Extended.h"

@class TCPClient_iOS;

using namespace std;

static TCPClientObserver* _observer = nullptr;
static TCPClient_iOS* _tcpClient = nil;

static int32_t EndianConvertLToB(int32_t value)
{
    return ((value & 0x000000FF) << 24) |
    ((value & 0x0000FF00) << 8) |
    ((value & 0x00FF0000) >> 8) |
    ((value & 0xFF000000) >> 24) ;
}

#pragma mark - TCPClient_iOS
@interface TCPClient_iOS : NSObject <GCDAsyncSocketDelegate>

- (GCDAsyncSocket*)getSocket;

@end

@implementation TCPClient_iOS
{
    GCDAsyncSocket* _asyncSocket;
}

- (instancetype)init
{
    if (self = [super init]) {
        dispatch_queue_t mainQueue = dispatch_get_main_queue();
        _asyncSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:mainQueue];
    }
    
    return self;
}

- (void)dealloc
{
    _asyncSocket = nil;
}

- (GCDAsyncSocket*)getSocket
{
    return _asyncSocket;
}

#pragma mark Socket Delegate
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port
{
    if (_observer) {
        _observer->onConnect([host stdString], port);
    }
}

- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag
{
    if (_observer) {
        _observer->onWriteData(tag);
    }
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    if (_observer) {
#if false
        string msg = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] stdString];
        _observer->onReadData(msg, tag);
#else
        NSUInteger len = [data length];
        char* bytes = (char*)calloc(len, sizeof(char));
        if (bytes) {
            [data getBytes:bytes length:len];
            _observer->onReadData(bytes, len, tag);
            free(bytes);
        }
#endif
    }
}

- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err
{
    if (_observer) {
        TcpErrorCode code(TcpErrorCode::Success);
        if (err) {
            code = TcpErrorCode::Unknown;
        }
        _observer->onDisconnect(code);
    }
}

@end

#pragma mark - TCPClient
void TCPClient::init(TCPClientObserver* observer)
{
    _observer = observer;
    
    if (!_tcpClient) {
        _tcpClient = [[TCPClient_iOS alloc] init];
    }
}

void TCPClient::purge()
{
    _observer = nullptr;
    
    if (_tcpClient) {
        _tcpClient = nil;
    }
}

void TCPClient::connect(const string& url, uint16_t port, double timeOut)
{
    if (_tcpClient) {
        GCDAsyncSocket* sock = [_tcpClient getSocket];
        NSError *error = nil;
        BOOL ret = [sock connectToHost:[NSString stringWithUTF8String:url.c_str()]
                                onPort:port
                           withTimeout:timeOut
                                 error:&error];
        if (!ret) {
            if (_observer) {
                _observer->onDisconnect(TcpErrorCode::ConnectionFailure);
            }
        }
    }
}

void TCPClient::disconnect()
{
    if (_tcpClient) {
        GCDAsyncSocket* sock = [_tcpClient getSocket];
        [sock disconnect];
    }
}

void TCPClient::writeData(const string& data, double timeOut, long tag)
{
    writeData(data.c_str(), data.length(), timeOut, tag);
}

void TCPClient::writeData(const char* data, unsigned long len, double timeOut, long tag)
{
    if (_tcpClient) {
        GCDAsyncSocket* sock = [_tcpClient getSocket];
#if true
        // first 4 bytes means the length of the data
        // eg: if the data is "12345", the full data is "000512345" (length == 9)
        static const int32_t lenBytes(4);
        const unsigned long totalLen = len + lenBytes;
        char *formattedData = (char *)calloc(totalLen, sizeof(char));
        if (formattedData) {
            const int32_t intLen = EndianConvertLToB(static_cast<int32_t>(len));
            memcpy(formattedData, &intLen, lenBytes);
            memcpy(formattedData + lenBytes, data, len);
            NSData* requestData = [[NSData alloc] initWithBytes:formattedData length:totalLen];
            [sock writeData:requestData withTimeout:timeOut tag:tag];
            free(formattedData);
        }
#else
        NSData* requestData = [[NSData alloc] initWithBytes:data length:len];
        [sock writeData:requestData withTimeout:timeOut tag:tag];
#endif
    }
}

void TCPClient::readData(double timeOut, long tag)
{
    if (_tcpClient) {
        GCDAsyncSocket* sock = [_tcpClient getSocket];
        [sock readDataWithTimeout:timeOut tag:tag];
    }
}
