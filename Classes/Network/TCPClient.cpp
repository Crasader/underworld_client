//
//  TCPClient.cpp
//  Underworld_Client
//
//  Created by jazy on 16/3/15.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "TCPClient.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>

using namespace std;

int TCPClient::set_non_block(int fd) {
	int flags;

	/* Set the socket non-blocking.
	 *          * Note that fcntl(2) for F_GETFL and F_SETFL can't be
	 *                   * interrupted by a signal. */
	if ((flags = fcntl(fd, F_GETFL)) == -1) {
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;
}

int TCPClient::set_block(int fd) {
	int flags;

	if ((flags = fcntl(fd, F_GETFL)) == -1) {
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | ~O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;
}

int TCPClient::connect(const string& host, int port, int timeout) {
	int s, rv;
	struct addrinfo hints, *servinfo, *p;
	char portstr[6]; /* strlen("65535") + 1; */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	snprintf(portstr, sizeof(portstr), "%d", port);

	if ((rv = getaddrinfo(host.c_str(), portstr, &hints, &servinfo)) != 0) {
		//SS_DEBUG((LM_ERROR,"getaddrinfo error\n"));
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {

		if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			//SS_DEBUG((LM_ERROR,"socket create error:%s\n",strerror(errno)));
			continue;
		}

		int yes = 1;

		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
			//SS_DEBUG((LM_ERROR,"setsockopt error:%s\n",strerror(errno)));
			continue;
		}

		if (set_non_block(s) == -1) {
			//SS_DEBUG((LM_ERROR,"set_non_block error:%s\n",strerror(errno)));
			continue;
		}

		if (connect(s, p->ai_addr, p->ai_addrlen) == -1) {
			if (errno != EINPROGRESS) {
				close(s);
				//SS_DEBUG((LM_ERROR,"connect error:%s\n",strerror(errno)));
				continue;
			} else {
				struct pollfd connect_fd;
				connect_fd.fd = s;
				connect_fd.events = POLLOUT;

				int poll_ret = poll(&connect_fd, 1, timeout);
				if (poll_ret <= 0 || !(connect_fd.revents & POLLOUT)) {
					errno = ETIMEDOUT;
					//SS_DEBUG(
					//              (LM_ERROR,"connect timeout error:%s\n",strerror(errno)));
					close(s);
					continue;
				} else {
					socklen_t optlen = sizeof(int);
					int error;
					if (getsockopt(s, SOL_SOCKET, SO_ERROR, &error, &optlen)
							>= 0) {
						if (error == 0) {
							set_block(s);
							break;
						}
						errno = error;
					}
				}
			}
		}
		if (set_block(s) < 0) {
			//SS_DEBUG((LM_ERROR,"set_block error:%s\n",strerror(errno)));
		}
		break;

	}
	freeaddrinfo(servinfo);
	if (p == NULL) {
		close(s);
		return -1;
	}
	return s;
}

int TCPClient::write(int fd, const char* data, int count) {

	if (fd == -1 || data == 0 || count <= 0) {
		//TODO:log invalid args
		return -1;
	}
	int nwritten, totlen = 0;
	while (totlen != count) {
		nwritten = ::write(fd, data, count - totlen);
		if (nwritten == 0)
			return totlen;
		if (nwritten == -1)
			return -1;
		totlen += nwritten;
		data += nwritten;
	}
	return totlen;
}

int TCPClient::write(int fd, const string& data) {
	return TCPClient::write(fd, data.data(), data.size());
}

int TCPClient::read(int fd, char *data, int count) {
	if (fd == -1 || data == 0 || count <= 0) {
		//TODO:log invalid args
		return -1;
	}

	int nread, totlen = 0;
	while (totlen != count) {
		nread = ::read(fd, data, count - totlen);
		if (nread == 0)
			return totlen;
		if (nread == -1) {
			return -1;
		}
		totlen += nread;
		data += nread;
	}
	return totlen;
}

int TCPClient::read(int fd, string& data, int count) {

	if (fd == -1 || count <= 0) {
		//TODO:log invalid args
		return -1;
	}
	char* buf = new char[count];
	int ret = TCPClient::read(fd, buf, count);
	if (ret > 0)
		data.assign(buf, ret);
	delete[] buf;
	return ret;

}

void TCPClient::close(int fd) {
	::close(fd);
}
