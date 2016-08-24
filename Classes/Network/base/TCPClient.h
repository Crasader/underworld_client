/****************************************************************************
 Copyright (c) 2012      greathqy
 Copyright (c) 2012      cocos2d-x.org
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
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

#ifndef __CCTCPCLIENT_H__
#define __CCTCPCLIENT_H__

#include <event.h>
#include <thread>
#include <condition_variable>
#include "base/CCVector.h"
#include "base/CCScheduler.h"
#include "TCPRequest.h"
#include "TCPResponse.h"


/**
 * @addtogroup network
 * @{
 */


/** Singleton that handles asynchronous http requests.
 *
 * Once the request completed, a callback will issued in main thread when it provided during make request.
 *
 * @lua NA
 */
class CC_DLL TCPClient
{
public:
    /**
     * The buffer size of _responseMessage
     */
    static const int RESPONSE_BUFFER_SIZE = 256;
    
    /**
     * Get instance of TCPClient.
     *
     * @return the instance of TCPClient.
     */
    //static TCPClient *getInstance();
    
    /**
     * Release the instance of TCPClient.
     */
    //static void destroyInstance();
    
    /**
     * Enable cookie support.
     *
     * @param cookieFile the filepath of cookie file.
     */
    //void enableCookies(const char* cookieFile);
    
    /**
     * Get the cookie filename
     *
     * @return the cookie filename
     */
    //const std::string& getCookieFilename();
    
    /**
     * Set root certificate path for SSL verification.
     *
     * @param caFile a full path of root certificate.if it is empty, SSL verification is disabled.
     */
    //void setSSLVerification(const std::string& caFile);
    
    /**
     * Get the ssl CA filename
     *
     * @return the ssl CA filename
     */
    //const std::string& getSSLVerification();
    
    /**
     * Add a get request to task queue
     *
     * @param request a TCPRequest object, which includes url, response callback etc.
     please make sure request->_requestData is clear before calling "send" here.
     */
    void send(TCPRequest* request);
    
    /**
     * Immediate send a request
     *
     * @param request a TCPRequest object, which includes url, response callback etc.
     please make sure request->_requestData is clear before calling "sendImmediate" here.
     */
    //void sendImmediate(TCPRequest* request);
    
    /**
     * Set the timeout value for connecting.
     *
     * @param value the timeout value for connecting.
     */
    void setTimeoutForConnect(int value);
    
    /**
     * Get the timeout value for connecting.
     *
     * @return int the timeout value for connecting.
     */
    int getTimeoutForConnect();
    
    /**
     * Set the timeout value for reading.
     *
     * @param value the timeout value for reading.
     */
    void setTimeoutForRead(int value);
    
    /**
     * Get the timeout value for reading.
     *
     * @return int the timeout value for reading.
     */
    int getTimeoutForRead();
    
    //TCPCookie* getCookie() const {return _cookie; }
    
    //std::mutex& getCookieFileMutex() {return _cookieFileMutex;}
    
    //std::mutex& getSSLCaFileMutex() {return _sslCaFileMutex;}
    //
    bool init(const std::string& host,int port);
    
    
    inline void setResponseCallback(const ccTCPRequestCallback& callback)
    {
        _pCallback = callback;
    }
    
    /**
     *      * Get ccTCPRequestCallback callback function.
     *           *
     *                * @return const ccTCPRequestCallback& ccTCPRequestCallback callback function.
     *                     */
    inline const ccTCPRequestCallback& getCallback()
    {
        return _pCallback;
    }
    
    inline void setReconnectCallback(const ccTCPReconnectCallback& callback)
    {
        _rCallback = callback;
    }
    
    /**
     *      * Get ccTCPRequestCallback callback function.
     *           *
     *                * @return const ccTCPRequestCallback& ccTCPRequestCallback callback function.
     *                     */
    inline const ccTCPReconnectCallback& getReconnectCallback()
    {
        return _rCallback;
    }
    
    inline void pushBack(TCPResponse *resp)
    {
        _responseQueue.pushBack(resp);
        ++_countForKey;
    }
    
    void destroy();
    
    /*
     *set the interval of timer
     *timeout seconds
     */
    void setTimerInterval(int timeout);
    
    /*
     *get the interval of timer
     */
    int getTimerInterval();
    
    void processResponse(char* responseMessage);
    void processRequest(char* responseMessage);
    void processTimer();
    
    TCPClient();
private:
    int _create_pipe();
    virtual ~TCPClient();
    /**
     * Init pthread mutex, semaphore, and create new thread for http requests
     * @return bool
     */
    bool lazyInitThreadSemphore();
    void networkThread();
    void networkThreadAlone(TCPRequest* request);
    /** Poll function called from main thread to dispatch callbacks when http requests finished **/
    void dispatchResponseCallbacks();
    
    void dispatchReconnectCallbacks();
    
    void increaseThreadCount();
    void decreaseThreadCountAndMayDeleteThis();
    int reconnect2Server();
    
private:
    bool _isInited;
    
    std::string                 _url;
    int _port;
    int _fd;
    
    int _pipeRead;
    int _pipeWrite;
    
    int _timerInterval;
    
    int _timeoutForConnect;
    std::mutex _timeoutForConnectMutex;
    
    int _timeoutForRead;
    std::mutex _timeoutForReadMutex;
    
    int  _threadCount;
    std::mutex _threadCountMutex;
    
    Scheduler* _scheduler;
    std::mutex _schedulerMutex;
    
    Vector<TCPRequest*>  _requestQueue;
    std::mutex _requestQueueMutex;
    
    Vector<TCPResponse*> _responseQueue;
    std::mutex _responseQueueMutex;
    
    std::mutex _reconnectMutex;
    
    //std::string _cookieFilename;
    //std::mutex _cookieFileMutex;
    
    //std::string _sslCaFilename;
    //std::mutex _sslCaFileMutex;
    
    
    std::condition_variable_any _sleepCondition;
    
    char _responseMessage[RESPONSE_BUFFER_SIZE];
    
    TCPRequest* _requestSentinel;
    ccTCPRequestCallback       _pCallback;      /// C++11 style callbacks
    ccTCPReconnectCallback      _rCallback;      /// C++11 style callbacks
    
    struct event notify_event;
    struct event server_event;
    struct event timer_event;
    struct event_base* base;
    
    int _countForKey;
};

// end group
/// @}

#endif //__CCTCPCLIENT_H__
