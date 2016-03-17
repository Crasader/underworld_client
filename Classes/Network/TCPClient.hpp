//
//  TCPClient.hpp
//  Underworld_Client
//
//  Created by jazy on 16/3/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include <string>

#pragma once

#if 0
enum class TcpErrorCode
{
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
	virtual void onWriteData(long tag) {}
	virtual void onReadData(const std::string& data, long tag) {}
	virtual void onDisconnect(TcpErrorCode code) {}
	virtual void onError(TcpErrorCode code) {}
};
#endif

namespace TCPClient {
//void init(TCPClientObserver* observer);

int set_non_block(int fd);
int set_block(int fd);
/*
 * timeout:ms
 *
 */
int connect(const std::string& host, int port, int timeout);
long write(int fd, const std::string& data);
long write(int fd, const char* data, long count);
long read(int fd, std::string& data, long count);
long read(int fd, char *data, long count);
void close(int fd);
}

