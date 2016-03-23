//
//  ClientTCPNetworkProxy.h
//  Underworld_Client
//
//  Created by wenchengye on 16/3/22.
//
//

#ifndef ClientTCPNetworkProxy_h
#define ClientTCPNetworkProxy_h

#include "NetworkService.h"
#include "TCPClient.h"

namespace UnderWorld { namespace Core {
    class NetworkMessage;
}}

class ClientTCPNetworkProxy : public UnderWorld::Core::AbstractNetworkProxy {
    
private:
    std::list<UnderWorld::Core::NetworkMessage*> _incomeMessage;
    std::string _host;
    int _port;
    
public:
    ClientTCPNetworkProxy(const std::string& host, int port)
        : _host(host), _port(port) {}
    virtual ~ClientTCPNetworkProxy();
    
    /** override AbstractNetworkProxy*/
    virtual void connect();
    virtual void disconnect();
    virtual bool isConnected() = 0;
    virtual void send(UnderWorld::Core::NetworkMessage* msg) = 0;
    virtual UnderWorld::Core::NetworkMessage* pop() = 0;
    
private:
    
    /** TCP Interface */
    void onReceiveTCPResponse(cocos2d::network::TCPResponse* response);
    
    /** parse */
    cocos2d::network::TCPRequest* parseMsg2Request(const UnderWorld::Core::NetworkMessage* msg);
    
    void parseResponse2Msg(
        const cocos2d::network::TCPResponse* response,
        std::vector<UnderWorld::Core::NetworkMessage*>& output);
    
    
    
};


#endif /* ClientTCPNetworkProxy_h */
