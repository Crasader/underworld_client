
/****************************************************************************
 Copyright (c) 2012      greathqy
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
 tcp://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "TCPClient.h"
#include <queue>
#include <errno.h>
//#include <curl/curl.h>
#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"

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
#include <event.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
typedef int int32_t;
#endif

//static TCPClient* _tcpClient = nullptr; // pointer to singleton
//static struct event notify_event,server_event;
#if 0
typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

// Callback function used by libcurl for collect response data
static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::vector<char> *recvBuffer = (std::vector<char>*)stream;
    size_t sizes = size * nmemb;
    
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
    
    return sizes;
}

// Callback function used by libcurl for collect header data
static size_t writeHeaderData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::vector<char> *recvBuffer = (std::vector<char>*)stream;
    size_t sizes = size * nmemb;
    
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
    
    return sizes;
}


static int processGetTask(TCPClient* client, TCPRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
static int processPostTask(TCPClient* client, TCPRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
static int processPutTask(TCPClient* client,  TCPRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
static int processDeleteTask(TCPClient* client,  TCPRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
// int processDownloadTask(TCPRequest *task, write_callback callback, void *stream, int32_t *errorCode);

#endif




using namespace std;

static int _set_non_block(int fd) {
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

static int _set_block(int fd) {
        int flags;

        if ((flags = fcntl(fd, F_GETFL)) == -1) {
                return -1;
        }
        if (fcntl(fd, F_SETFL, flags | ~O_NONBLOCK) == -1) {
                return -1;
        }
        return 0;
}


int _connect(const string& host, int port, int timeout) {
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

                if (_set_non_block(s) == -1) {
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
                                                        _set_block(s);
                                                        break;
                                                }
                                                errno = error;
                                        }
                                }
                        }
                }
                if (_set_block(s) < 0) {
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


int _write(int fd, const char* data, int count) {

        if (fd == -1 || data == 0 || count <= 0) {
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

int TCPClient::_create_pipe() {
        int options;
        int pipe_fd[2];

        if (pipe(pipe_fd))
                return -1;

        _pipeRead = pipe_fd[0];
        _pipeWrite = pipe_fd[1];

        for (int i = 0; i < 2; i++) {
                if ((options = fcntl(pipe_fd[i], F_GETFL)) == -1)
                        return -1;
                if (fcntl(pipe_fd[i], F_SETFL, options | O_NONBLOCK) == -1)
                        return -1;
        }

        return 0;
}



static int _write(int fd, const string& data) {
        return _write(fd, data.data(), data.size());
}

static int _read(int fd, char *data, int count) {
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

static int _read(int fd, string& data, int count) {

        if (fd == -1 || count <= 0) {
                //TODO:log invalid args
                return -1;
        }
        char* buf = new char[count];
        int ret = _read(fd, buf, count);
        if (ret > 0)
                data.assign(buf, ret);
        delete[] buf;
        return ret;

}

static void _close(int fd) {
        ::close(fd);
}



void on_notify(evutil_socket_t sock, short flags, void * args)  
{  
    struct TCPClient *tc = (TCPClient *)args;   
    tc->processRequest(""); 
}  


void on_server(evutil_socket_t sock, short flags, void * args)
{
    //struct echo_context *ec = (struct echo_context *)args;
    struct TCPClient *tc = (TCPClient *)args;
    tc->processResponse("");

}

// Worker thread
void TCPClient::networkThread()
{   
	increaseThreadCount();
   
#if 0
    while (true) 
    {
        TCPRequest *request;

        // step 1: send tcp request if the requestQueue isn't empty
        {
            std::lock_guard<std::mutex> lock(_requestQueueMutex);
            while (_requestQueue.empty())
			{
                _sleepCondition.wait(_requestQueueMutex);
            }
            request = _requestQueue.at(0);
            _requestQueue.erase(0);
        }

        if (request == _requestSentinel) {
            break;
        }

        processResponse(request,_responseMessage);

    }

#endif

    //struct event_base* base;
    base = event_base_new();
    //struct event notify_event,server_event;
    event_set(&notify_event, _pipeRead, EV_READ|EV_PERSIST, on_notify, this);
    event_base_set(base,&notify_event);
    event_set(&server_event, _fd, EV_READ|EV_PERSIST, on_server, this);
    event_base_set(base,&server_event);
    event_add(&notify_event, NULL);
    event_add(&server_event, NULL);
    event_base_dispatch(base);
    event_base_free(base);   
  
    // cleanup: if worker thread received quit signal, clean up un-completed request queue
    _requestQueueMutex.lock();
    _requestQueue.clear();
    _requestQueueMutex.unlock();
    
	_responseQueueMutex.lock();
	_responseQueue.clear();
	_responseQueueMutex.unlock();

	decreaseThreadCountAndMayDeleteThis();    
}

// Worker thread
void TCPClient::networkThreadAlone(TCPRequest* request)
{
	increaseThreadCount();

	char responseMessage[RESPONSE_BUFFER_SIZE] = { 0 };
	//processResponse(request, _responseMessage);


	decreaseThreadCountAndMayDeleteThis();
}

//Configure curl's timeout property

#if 0
//Process Get Request
static int processGetTask(TCPClient* client, TCPRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
	bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

//Process POST Request
static int processPostTask(TCPClient* client, TCPRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
	bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_POST, 1)
            && curl.setOption(CURLOPT_POSTFIELDS, request->getRequestData())
            && curl.setOption(CURLOPT_POSTFIELDSIZE, request->getRequestDataSize())
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

//Process PUT Request
static int processPutTask(TCPClient* client, TCPRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
	bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
		&& curl.setOption(CURLOPT_CUSTOMREQUEST, "PUT")
            && curl.setOption(CURLOPT_POSTFIELDS, request->getRequestData())
            && curl.setOption(CURLOPT_POSTFIELDSIZE, request->getRequestDataSize())
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

//Process DELETE Request
static int processDeleteTask(TCPClient* client, TCPRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
	bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_CUSTOMREQUEST, "DELETE")
            && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}
#endif

#if 0
// TCPClient implementation
TCPClient* TCPClient::getInstance()
{
    if (_tcpClient == nullptr) 
	{
        _tcpClient = new (std::nothrow) TCPClient();
    }
    
    return _tcpClient;
}

void TCPClient::destroyInstance()
{
	if (nullptr == _tcpClient)
	{
		CCLOG("TCPClient singleton is nullptr");
		return;
	}

	CCLOG("TCPClient::destroyInstance begin");
	auto thiz = _tcpClient;
	_tcpClient = nullptr;

	thiz->_scheduler->unscheduleAllForTarget(thiz);
	thiz->_schedulerMutex.lock();
	thiz->_scheduler = nullptr;
	thiz->_schedulerMutex.unlock();

	thiz->_requestQueueMutex.lock();
	thiz->_requestQueue.pushBack(thiz->_requestSentinel);
	thiz->_requestQueueMutex.unlock();

	thiz->_sleepCondition.notify_one();
	thiz->decreaseThreadCountAndMayDeleteThis();

	CCLOG("TCPClient::destroyInstance() finished!");
}

void TCPClient::enableCookies(const char* cookieFile)
{
    std::lock_guard<std::mutex> lock(_cookieFileMutex);
    if (cookieFile)
    {
        _cookieFilename = std::string(cookieFile);
    }
    else
    {
        _cookieFilename = (FileUtils::getInstance()->getWritablePath() + "cookieFile.txt");
    }
}
    
void TCPClient::setSSLVerification(const std::string& caFile)
{
    std::lock_guard<std::mutex> lock(_sslCaFileMutex);
    _sslCaFilename = caFile;
}
#endif 


TCPClient::TCPClient()
: _timeoutForConnect(30)
, _timeoutForRead(60)
, _isInited(false)
, _threadCount(0)
, _requestSentinel(new TCPRequest())
,_fd(-1)
,_url("")
,_port(-1)
//, _cookie(nullptr)
{
	CCLOG("In the constructor of TCPClient!");
	memset(_responseMessage, 0, RESPONSE_BUFFER_SIZE * sizeof(char));
	_scheduler = Director::getInstance()->getScheduler();
	increaseThreadCount();
//	_create_pipe();
}

TCPClient::~TCPClient()
{
	CC_SAFE_DELETE(_requestSentinel);
	if(_fd!=-1)
             _close(_fd);
		
	CCLOG("TCPClient destructor");
	
        _scheduler->unscheduleAllForTarget(this);
        _schedulerMutex.lock();
        _scheduler = nullptr;
        _schedulerMutex.unlock();

        _requestQueueMutex.lock();
        _requestQueue.pushBack(_requestSentinel);
        _requestQueueMutex.unlock();
	 int flag=1;
        _write(_pipeWrite,(char*)&flag,sizeof(int));

        //_sleepCondition.notify_one();
         if(_pipeRead!=-1)
             _close(_pipeRead);
        if(_pipeWrite!=-1)
             _close(_pipeWrite);
        decreaseThreadCountAndMayDeleteThis();

}

//Lazy create semaphore & mutex & thread
bool TCPClient::lazyInitThreadSemphore()
{
    if (_isInited)
	{
        return true;
    } 
	else 
	{
	//_fd=_connect(_url,_port,_timeoutForConnect);
        auto t = std::thread(CC_CALLBACK_0(TCPClient::networkThread, this));
        t.detach();
		_isInited = true;
    }
    
    return true;
}

//Add a get task to queue
void TCPClient::send(TCPRequest* request)
{    
    if (false == lazyInitThreadSemphore()) 
    {
        return;
    }
    
    if (!request)
    {
        return;
    }
        
    request->retain();

	_requestQueueMutex.lock();
	_requestQueue.pushBack(request);
	_requestQueueMutex.unlock();

	// Notify thread start to work
	//_sleepCondition.notify_one();
	int flag=1;
	_write(_pipeWrite,(char*)&flag,sizeof(int));
}

/*void TCPClient::sendImmediate(TCPRequest* request)
{
    if(!request)
    {
        return;
    }

    request->retain();
    // Create a TCPResponse object, the default setting is tcp access failed
    //TCPResponse *response = new (std::nothrow) TCPResponse(request);

    auto t = std::thread(&TCPClient::networkThreadAlone, this, request);
    t.detach();
}*/

// Poll and notify main thread if responses exists in queue
void TCPClient::dispatchResponseCallbacks()
{
    // log("CCTCPClient::dispatchResponseCallbacks is running");
    //occurs when cocos thread fires but the network thread has already quited
    TCPResponse* response = nullptr;

    _responseQueueMutex.lock();
    if (!_responseQueue.empty())
    {
        response = _responseQueue.at(0);
        _responseQueue.erase(0);
    }
    _responseQueueMutex.unlock();
    
    if (response)
    {
        //TCPRequest *request = response->getTCPRequest();
        const ccTCPRequestCallback& callback = _pCallback;
        //Ref* pTarget = request->getTarget();
        //SEL_TCPResponse pSelector = request->getSelector();

        if (callback != nullptr)
        {
            callback(this, response);
        }
        /*else if (pTarget && pSelector)
        {
            (pTarget->*pSelector)(this, response);
        }*/
        
        response->release();
        // do not release in other thread
        //request->release();
    }
}

// Process Response
void TCPClient::processResponse(char* responseMessage)
{
#if 0
	// Process the request -> get response packet
	switch (request->getRequestType())
	{
	case TCPRequest::Type::GET: // HTTP GET
		retValue = processGetTask(this, request,
			writeData,
			response->getResponseData(),
			&responseCode,
			writeHeaderData,
			response->getResponseHeader(),
			responseMessage);
		break;

	case TCPRequest::Type::POST: // HTTP POST
		retValue = processPostTask(this, request,
			writeData,
			response->getResponseData(),
			&responseCode,
			writeHeaderData,
			response->getResponseHeader(),
			responseMessage);
		break;

	case TCPRequest::Type::PUT:
		retValue = processPutTask(this, request,
			writeData,
			response->getResponseData(),
			&responseCode,
			writeHeaderData,
			response->getResponseHeader(),
			responseMessage);
		break;

	case TCPRequest::Type::DELETE:
		retValue = processDeleteTask(this, request,
			writeData,
			response->getResponseData(),
			&responseCode,
			writeHeaderData,
			response->getResponseHeader(),
			responseMessage);
		break;

	default:
		CCASSERT(true, "CCTCPClient: unknown request type, only GET and POSt are supported");
		break;
	}
#endif
#if 0
	 long writeSuccess = -1;
 
        // write data to server

        writeSuccess=_write(_fd,request->getRequestData(),request->getRequestDataSize());

	//reconnect once
	if(writeSuccess==-1){
	    _close(_fd);
	    _fd=_connect(_url,_port,_timeoutForConnect);
            writeSuccess=_write(_fd,request->getRequestData(),request->getRequestDataSize());    
	}
#endif
#if 1
        int responseLen=-1;

        int retVal=-1;
        /*
 *first 4 byte,the length of package
 *if length==0,the end package,break the loop
 */
        do{
            responseLen=-1;
            retVal=-1;
            retVal=_read(_fd,(char*)(&responseLen),4);
	    CCLOG("processResponse,responseLen=%d",responseLen);
            string reponseData;
            if(responseLen>0){//handle full package
                string reponseData;
                retVal=_read(_fd,reponseData,responseLen);
		 CCLOG("processResponse,read response data=%d",retVal);
        #if 0        
         // write data to TCPResponse
                TCPResponse *response = new (std::nothrow) TCPResponse(request);
                response->setResponseCode(retVal);
                if (retVal <= 0)
                {
                    response->setSucceed(false);
                    response->setErrorBuffer(responseMessage);
                }
               else
                {
                   response->setResponseDataString(reponseData.data(),reponseData.size());
                   response->setSucceed(true);
                }

                // add response packet into queue
                _responseQueueMutex.lock();
                _responseQueue.pushBack(response);
                _responseQueueMutex.unlock();

                _schedulerMutex.lock();
                if (nullptr != _scheduler)
                {
                        _scheduler->performFunctionInCocosThread(CC_CALLBACK_0(TCPClient::dispatchResponseCallbacks, this));
                }
                _schedulerMutex.unlock();
        #endif

            }/*else if(writeSuccess!=-1&&responseLen==0){
                break;
            }*/


           // write data to TCPResponse
           TCPResponse *response = new (std::nothrow) TCPResponse();
                response->setResponseCode(retVal);
                if (retVal <= 0)
                {
                    response->setSucceed(false);
                    response->setErrorBuffer(responseMessage);
                }
               else
                {
                   response->setResponseDataString(reponseData.data(),reponseData.size());
                   response->setSucceed(true);
                }
                 // add response packet into queue
         _responseQueueMutex.lock();
                _responseQueue.pushBack(response);
                _responseQueueMutex.unlock();

                _schedulerMutex.lock();
                if (nullptr != _scheduler)
                {
                        _scheduler->performFunctionInCocosThread(CC_CALLBACK_0(TCPClient::dispatchResponseCallbacks, this));
                }
                _schedulerMutex.unlock();

        }while(0);

#endif


}

void TCPClient::processRequest(char* responseMessage){
   int flag=-1;  
   int ret = _read(_pipeRead, (char*)&flag, sizeof(int));  
      
    CCLOG("read_cb, read %d bytes,flag=%d", ret,flag);  
    if(flag==1)  
    {
   TCPRequest *request;

        // step 1: send tcp request if the requestQueue isn't empty
        {
            std::lock_guard<std::mutex> lock(_requestQueueMutex);
           /* while (_requestQueue.empty())
                        {
                _sleepCondition.wait(_requestQueueMutex);
            }*/
            request = _requestQueue.at(0);
            _requestQueue.erase(0);
        }

        if (request == _requestSentinel) {
<<<<<<< HEAD
=======
	    CCLOG("event_base_loopbreak");
>>>>>>> tcpclient test
	    event_base_loopbreak(base);
            return;
        }

        // step 2: libcurl sync access

        // Create a TCPResponse object, the default setting is tcp access failed
       // TCPResponse *response = new (std::nothrow) TCPResponse(request);

         //       processResponse(response, _responseMessage);


         long writeSuccess = -1;

        // write data to server

        writeSuccess=_write(_fd,request->getRequestData(),request->getRequestDataSize());

        //reconnect once
        if(writeSuccess==-1){
            _close(_fd);
	    event_del(&server_event);
            _fd=_connect(_url,_port,_timeoutForConnect);
	    if(_fd!=-1){
		CCLOG("processRequest,reconnect once");
		event_set(&server_event, _fd, EV_READ|EV_PERSIST, on_server, this);
		event_base_set(base,&server_event);
		event_add(&server_event, NULL);
            	writeSuccess=_write(_fd,request->getRequestData(),request->getRequestDataSize());
	    }
        }
	CCLOG("processRequest,writeSuccess=%d",writeSuccess);

                if (writeSuccess <= 0)
                {

                    TCPResponse *response = new (std::nothrow) TCPResponse();
                    response->setSucceed(false);
                    response->setErrorBuffer(responseMessage);
                 // add response packet into queue
         _responseQueueMutex.lock();
                _responseQueue.pushBack(response);
                _responseQueueMutex.unlock();

                _schedulerMutex.lock();
                if (nullptr != _scheduler)
                {
                        _scheduler->performFunctionInCocosThread(CC_CALLBACK_0(TCPClient::dispatchResponseCallbacks, this));
                }
                _schedulerMutex.unlock();

        }
	request->release();
    }
}


void TCPClient::increaseThreadCount()
{
	_threadCountMutex.lock();
	++_threadCount;
	_threadCountMutex.unlock();
}

void TCPClient::decreaseThreadCountAndMayDeleteThis()
{
	bool needDeleteThis = false;
	_threadCountMutex.lock();
	--_threadCount;
	if (0 == _threadCount)
	{
		needDeleteThis = true;
	}

	_threadCountMutex.unlock();
	if (needDeleteThis)
	{
		delete this;
	}
}

void TCPClient::setTimeoutForConnect(int value)
{
    std::lock_guard<std::mutex> lock(_timeoutForConnectMutex);
    _timeoutForConnect = value;
}
    
int TCPClient::getTimeoutForConnect()
{
    std::lock_guard<std::mutex> lock(_timeoutForConnectMutex);
    return _timeoutForConnect;
}
    
void TCPClient::setTimeoutForRead(int value)
{
    std::lock_guard<std::mutex> lock(_timeoutForReadMutex);
    _timeoutForRead = value;
}
    
int TCPClient::getTimeoutForRead()
{
    std::lock_guard<std::mutex> lock(_timeoutForReadMutex);
    return _timeoutForRead;
}

bool TCPClient::init(const std::string& host,int port){
    _url=host;
    _port=port;
     _fd=_connect(_url,_port,_timeoutForConnect);
    
    if(_fd<0){
	CCLOG("TCPClient::init error,host=%s,port=%d",host.c_str(),port);
	return false;
    }
    int ret=_create_pipe();
    return ret==0?true:false;
}

#if 0
const std::string& TCPClient::getCookieFilename()
{
    std::lock_guard<std::mutex> lock(_cookieFileMutex);
    return _cookieFilename;
}
    
const std::string& TCPClient::getSSLVerification()
{
    std::lock_guard<std::mutex> lock(_sslCaFileMutex);
    return _sslCaFilename;
}
#endif


