/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
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

#ifndef __TCP_REQUEST_H__
#define __TCP_REQUEST_H__

#include <string>
#include <vector>
#include "base/CCRef.h"
#include "base/ccMacros.h"

/**
 * @addtogroup network
 * @{
 */

NS_CC_BEGIN

namespace network {

class TCPClient;
class TCPResponse;

typedef std::function<void(TCPClient* client, TCPResponse* response)> ccTCPRequestCallback;
typedef void (cocos2d::Ref::*SEL_TCPResponse)(TCPClient* client, TCPResponse* response);
#define tcpresponse_selector(_SELECTOR) (cocos2d::network::SEL_TCPResponse)(&_SELECTOR)

/** 
 * Defines the object which users must packed for TCPClient::send(TCPRequest*) method.
 * Please refer to tests/test-cpp/Classes/ExtensionTest/NetworkTest/TCPClientTest.cpp as a sample
 * @since v2.0.2
 *
 * @lua NA
 */

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#ifdef DELETE
#undef DELETE
#endif
#endif

class CC_DLL TCPRequest : public Ref
{
public:
    /**
     * The TCPRequest type enum used in the TCPRequest::setRequestType.
     */
#if 0   
 enum class Type
    {
        GET,
        POST,
        PUT,
        DELETE,
        UNKNOWN,
    };
#endif
    
    /** 
     *  Constructor.
     *   Because TCPRequest object will be used between UI thread and network thread,
         requestObj->autorelease() is forbidden to avoid crashes in AutoreleasePool
         new/retain/release still works, which means you need to release it manually
         Please refer to TCPRequestTest.cpp to find its usage.
     */
    TCPRequest()
    {
        //_requestType = Type::UNKNOWN;
        //_url.clear();
        _requestData.clear();
        //_tag.clear();
        //_pTarget = nullptr;
        //_pSelector = nullptr;
        //_pCallback = nullptr;
        //_pUserData = nullptr;
    };
    
    /** Destructor. */
    virtual ~TCPRequest()
    {
        /*if (_pTarget)
        {
            _pTarget->release();
        }*/
    };
    
    /** 
     * Override autorelease method to avoid developers to call it.
     * If this function was called, it would trigger assert in debug mode
     *
     * @return Ref* always return nullptr.
     */
    Ref* autorelease(void)
    {
        CCASSERT(false, "TCPResponse is used between network thread and ui thread \
                 therefore, autorelease is forbidden here");
        return NULL;
    }
            
    // setter/getters for properties
     
    /** 
     * Set request type of TCPRequest object before being sent,now it support the enum value of TCPRequest::Type.
     *
     * @param type the request type.
     */
    //inline void setRequestType(Type type)
    //{
      //  _requestType = type;
    //};
    /** 
     * Get the request type of TCPRequest object.
     *
     * @return TCPRequest::Type.
     */
    //inline Type getRequestType()
    //{
    //    return _requestType;
    //};
    
    /** 
     * Set the url address of TCPRequest object.
     * The url value could be like these: "http://httpbin.org/ip" or "https://httpbin.org/get"
     *
     * @param url the string object.
     */
    /*inline void setUrl(const std::string& url)
    {
        _url = url;
    };*/
    /** 
     * Get the url address of TCPRequest object.
     *
     * @return const char* the pointer of _url.
     */
    /*inline const char* getUrl()
    {
        return _url.c_str();
    };*/


      /** 
 *      * Set the port address of TCPRequest object.
 *           * The prot value could be like these: "80"
 *                *
 *                     * @param port the int.
 *                          */
    /*inline void setPort(int port)
    {
        _port = port;
    };*/
    /** 
 *      * Get the port of TCPRequest object.
 *           *
 *                * @return int  the pointer of _port.
 *                     */
    /*inline const int  getPort()
    {
        return _port;
    }*/
    
    /** 
     * Set the request data of TCPRequest object.
     *
     * @param buffer the buffer of request data, it support binary data.
     * @param len    the size of request data.
     */
    inline void setRequestData(const char* buffer, size_t len)
    {
        _requestData.assign(buffer, buffer + len);
    };
    /** 
     * Get the request data pointer of TCPRequest object.
     *
     * @return char* the request data pointer.
     */
    inline char* getRequestData()
    {
        if(_requestData.size() != 0)
            return &(_requestData.front());

        return nullptr;
    }
    /** 
     * Get the size of request data
     *
     * @return ssize_t the size of request data
     */
    inline ssize_t getRequestDataSize()
    {
        return _requestData.size();
    }
    
    /** 
     * Set a string tag to identify your request.
     * This tag can be found in TCPResponse->getTCPRequest->getTag().
     *
     * @param tag the string object.
     */
  /*  inline void setTag(const std::string& tag)
    {
        _tag = tag;
    };*/
    /** 
     * Get the string tag to identify the request.
     * The best practice is to use it in your MyClass::onMyTCPRequestCompleted(sender, TCPResponse*) callback.
     *
     * @return const char* the pointer of _tag
     */
    /*inline const char* getTag()
    {
        return _tag.c_str();
    };*/
    
    /**
     * Set user-customed data of TCPRequest object.
     * You can attach a customed data in each request, and get it back in response callback.
     * But you need to new/delete the data pointer manually.
     *
     * @param pUserData the string pointer
     */
    /*inline void setUserData(void* pUserData)
    {
        _pUserData = pUserData;
    };*/
    /** 
     * Get the user-customed data pointer which were pre-setted.
     * Don't forget to delete it. TCPClient/TCPResponse/TCPRequest will do nothing with this pointer.
     *
     * @return void* the pointer of user-customed data.
     */
    /*inline void* getUserData()
    {
        return _pUserData;
    };*/
    
    /**
     * Set the target and related callback selector.
     * When response come back, it would call (pTarget->*pSelector) to process something.
     *
     * @param pTarget the target object pointer.
     * @param pSelector the callback function.
     */
    /*CC_DEPRECATED_ATTRIBUTE inline void setResponseCallback(Ref* pTarget, SEL_CallFuncND pSelector)
    {
        doSetResponseCallback(pTarget, (SEL_TCPResponse)pSelector);
    }*/
    
    /**
     * Set the target and related callback selector of TCPRequest object.
     * When response come back, we would call (pTarget->*pSelector) to process response data.
     *
     * @param pTarget the target object pointer.
     * @param pSelector the SEL_TCPResponse function.
     */
    /*inline void setResponseCallback(Ref* pTarget, SEL_TCPResponse pSelector)
    {
        doSetResponseCallback(pTarget, pSelector);
    }*/
    /**
     * Set response callback function of TCPRequest object.
     * When response come back, we would call _pCallback to process response data.
     *
     * @param callback the ccTCPRequestCallback function.
     */
    /*inline void setResponseCallback(const ccTCPRequestCallback& callback)
    {
        _pCallback = callback;
    }*/
    
    /** 
     * Get the target of callback selector function, mainly used by TCPClient.
     *
     * @return Ref* the target of callback selector function
     */
    /*inline Ref* getTarget()
    {
        return _pTarget;
    }*/

    /**
     * This sub class is just for migration SEL_CallFuncND to SEL_TCPResponse,someday this way will be removed.
     * 
     * @lua NA
     */
#if 0
    class _prxy
    {
    public:
        /** Constructor. */
        _prxy( SEL_TCPResponse cb ) :_cb(cb) {}
        /** Destructor. */
        ~_prxy(){};
        /** Destructor. */
        operator SEL_TCPResponse() const { return _cb; }
        CC_DEPRECATED_ATTRIBUTE operator SEL_CallFuncND()   const { return (SEL_CallFuncND) _cb; }
    protected:
        SEL_TCPResponse _cb;
    };
    
    /** 
     * Get _prxy object by the _pSelector.
     *
     * @return _prxy the _prxy object
     */
    inline _prxy getSelector()
    {
        return _prxy(_pSelector);
    }
 #endif
    /**
     * Get ccTCPRequestCallback callback function.
     *
     * @return const ccTCPRequestCallback& ccTCPRequestCallback callback function.
     */
    /*inline const ccTCPRequestCallback& getCallback()
    {
        return _pCallback;
    }*/
    
    /** 
     * Set custom-defined headers.
     *
     * @param pHeaders the string vector of custom-defined headers.
     */
/*
    inline void setHeaders(std::vector<std::string> pHeaders)
   	{
   		_headers=pHeaders;
   	}*/
   
    /** 
     * Get custom headers.
     *
     * @return std::vector<std::string> the string vector of custom-defined headers.
     */
   	/*inline std::vector<std::string> getHeaders()
   	{
   		return _headers;
   	}*/
    
private:
#if 0
    inline void doSetResponseCallback(Ref* pTarget, SEL_TCPResponse pSelector)
    {
        if (_pTarget)
        {
            _pTarget->release();
        }
        
        _pTarget = pTarget;
        _pSelector = pSelector;
        if (_pTarget)
        {
            _pTarget->retain();
        }
    }
#endif

protected:
    // properties
    ///Type                        _requestType;    /// kTCPRequestGet, kTCPRequestPost or other enums
    //std::string                 _url;            /// target url that this request is sent to
    std::vector<char>   _requestData;    /// used for POST
    //std::string                 _tag;            /// user defined tag, to identify different requests in response callback
    //Ref*                        _pTarget;        /// callback target of pSelector function
    //SEL_TCPResponse            _pSelector;      /// callback function, e.g. MyLayer::onTCPResponse(TCPClient *sender, TCPResponse * response)
    //ccTCPRequestCallback       _pCallback;      /// C++11 style callbacks
    //void*                       _pUserData;      /// You can add your customed data here 
   // std::vector<std::string>    _headers;
    //int _port;
};

}

NS_CC_END

// end group
/// @}

#endif //__TCP_REQUEST_H__
