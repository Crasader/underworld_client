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
int write(int fd, const std::string& data);
int write(int fd, const char* data, int count);
int read(int fd, std::string& data, int count);
int read(int fd, char *data, int count);
void close(int fd);
}

