//
//  ClientTCPNetworkProxy.h
//  Underworld_Client
//
//  Created by wenchengye on 16/3/22.
//
//

#ifndef ClientTCPNetworkProxy_h
#define ClientTCPNetworkProxy_h

#include <list>
#include <unordered_set>

#include "TCPClient.h"
#include "ExternalInterface.h"
#include "Global.h"

namespace UnderWorld { namespace Core {
    class NetworkMessage;
}}

class ClientTCPNetworkProxy : public UnderWorld::Core::AbstractNetworkProxy {
private:
    typedef UnderWorld::Core::AbstractNetworkProxy::NetworkProxyListener
        ProxyListener;
    enum ClientStatus {
        Ready = 0,
        Matched = 1,
        Fighting = 2,
        Finished = 3
    };
    
private:
    std::unordered_set<ProxyListener*> _listeners;
    std::string _host;
    int _port;
    std::string _name;
    int _uid;
    TCPClient* _tcpClient;
    int _battleid;
    ClientStatus _status;
    
public:
    ClientTCPNetworkProxy(const std::string& host, int port,
        const std::string& name, int uid)
    : _host(host), _port(port), _name(name), _uid(uid), _tcpClient(nullptr), _battleid(INVALID_VALUE), _status(ClientStatus::Ready) {}
    virtual ~ClientTCPNetworkProxy();
    
    /** interface */
    
    /** override AbstractNetworkProxy*/
    virtual void connect() override;
    virtual void send(UnderWorld::Core::NetworkMessage* msg) override;
    virtual void registerListener(ProxyListener* listener) override;
    virtual void unregisterListener(ProxyListener* listener) override;
    
private:
    
    /** TCP Interface */
    void onReceiveTCPResponse(TCPClient* client, TCPResponse* response);
    void onReconncected(TCPClient* client);
    
    /** parse */
    TCPRequest* parseMsg2Request(const UnderWorld::Core::NetworkMessage* msg);
    
    void parseResponse2Msg(
        const TCPResponse* response,
        std::vector<UnderWorld::Core::NetworkMessage*>& output);
    
    /** destroy TCPClient **/
    void destroyTCPClient();
    
    
};


#endif /* ClientTCPNetworkProxy_h */
