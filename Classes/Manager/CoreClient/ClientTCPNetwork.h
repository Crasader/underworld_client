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
#include "GameModeHMM.h"
#include "GameSettings.h"

namespace UnderWorld { namespace Core {
    class OutsideCommand;
}}

class NetworkMessage;
class NetworkMessageMatched2C;
class NetworkMessageStart2C;
class NetworkMessageMatch2S;

class ClientTCPNetwork : public UnderWorld::Core::AbstractNetwork {
public:
    class LaunchListener {
    public:
        virtual void onLaunched(const NetworkMessageStart2C& l2c) = 0;
        virtual void onMatched(const NetworkMessageMatched2C& l2c) = 0;
    };

private:
    
    enum ClientStatus {
        Idle = 0,
        Matching = 1,
        Launching = 2,
        Fighting = 3,
        Finished = 4
    };
    
private:
    // base
    std::string _host;
    int _port;
    std::string _name;
    int _uid;
    TCPClient* _tcpClient;
    int _battleid;
    ClientStatus _status;
    
    // launch
    LaunchListener* _launchListener;
    NetworkMessageMatch2S* _matchMsg;
    
    // sync
    std::vector<NetworkMessage*> _incomeNetworkMessages;
    std::vector<UnderWorld::Core::OutsideCommand*> _outputCommands;
    int _safeFrame;
    int _lastSafeFrame;

public:
    ClientTCPNetwork(const std::string& host, int port,
        const std::string& name, int uid)
    : _host(host)
    , _port(port)
    , _name(name)
    , _uid(uid)
    , _tcpClient(nullptr)
    , _battleid(INVALID_VALUE)
    , _status(ClientStatus::Idle)
    , _launchListener(nullptr)
    , _matchMsg(nullptr)
    , _safeFrame(0)
    , _lastSafeFrame(0) {}
    virtual ~ClientTCPNetwork();
    
    /** interface */
    void connect();
    void matchGame(LaunchListener* launchListener,
        const UnderWorld::Core::GameContentSetting& contentSetting,
        const std::vector<int>& cards,
        const UnderWorld::Core::GameModeHMMSetting::InitUnitList& initList,
        const vector<UnderWorld::Core::HMMCardSetting>& unitPool);
    void startGame();
    void closeGame();

    /** override AbstractNetwork */
    virtual void init() override;
    virtual void sendOutsideCommand(UnderWorld::Core::OutsideCommand* commmd) override;
    virtual void update(int nextFrame,
        std::vector<UnderWorld::Core::CmdFramePair>& incomeCommands) override;
    virtual int getSynchronizedFrame() const override;
    
private:
    virtual void send(NetworkMessage* msg);
    
    /** TCP Interface */
    void onReceiveTCPResponse(TCPClient* client, TCPResponse* response);
    void onReconncected(TCPClient* client);
    
    /** parse */
    TCPRequest* parseMsg2Request(const NetworkMessage* msg);
    void parseResponse2Msg(
        const TCPResponse* response,
        std::vector<NetworkMessage*>& output);
    
    /** destroy TCPClient **/
    void destroyTCPClient();
    
    /** sync */
    void cleanSyncInstance();
    void updateFrame(int nextFrame);
    void updateNetworkKeyFrame(int nextFrame,
        std::vector<UnderWorld::Core::CmdFramePair>& incomeCommands);
    void flushOutputCommands(int frame);

};

class NetworkMessage {
public:
    virtual ~NetworkMessage() {};
    virtual NetworkMessage* clone() const = 0;
};

class NetworkMessageMatch2S : public NetworkMessage {
private:
    typedef UnderWorld::Core::GameContentSetting ContentSetting;
    typedef UnderWorld::Core::GameModeHMMSetting::InitUnitList InitUnits;
    typedef std::vector<UnderWorld::Core::HMMCardSetting> UnitPool;
    
private:
    ContentSetting _setting;
    std::vector<int> _cards;
    InitUnits _initUnits;
    UnitPool _unitPool;
    
public:
    NetworkMessage* clone() const override                     {return new NetworkMessageMatch2S(*this);}
    
    /** getters */
    const ContentSetting& getGameContentSetting() const        {return _setting;}
    const std::vector<int>& getCards() const                   {return _cards;}
    const InitUnits& getInitUnits() const                      {return _initUnits;}
    const UnitPool& getUnitPool() const                        {return _unitPool;}
    
    /** setters */
    void setGameContentSetting(const ContentSetting& setting)  {_setting = setting;}
    void setCards(const std::vector<int>& cards)               {_cards = cards;}
    void setInitUnits(const InitUnits& initUnits)              {_initUnits = initUnits;}
    void setUnitPool(const UnitPool& unitPool)                 {_unitPool = unitPool;}
};

class NetworkMessageMatched2C : public NetworkMessage {
private:
    typedef UnderWorld::Core::FactionSetting FactionSetting;
    typedef std::vector<std::vector<int> > Cards;
    typedef std::vector<UnderWorld::Core::GameModeHMMSetting::InitUnitList> InitUnitLists;
    typedef std::vector<std::vector<UnderWorld::Core::HMMCardSetting> > UnitPools;
    
private:
    FactionSetting _factionSetting;
    Cards _cards;
    InitUnitLists _initUnitLists;
    UnitPools _unitPools;
    int _mapId;
    
public:
    NetworkMessage* clone() const override                 {return new NetworkMessageMatched2C(*this);}
    const FactionSetting& getFactionSetting() const        {return _factionSetting;}
    const Cards& getCards() const                          {return _cards;}
    const InitUnitLists& getInitUnitLists() const          {return _initUnitLists;}
    const UnitPools& getUnitPools() const                  {return _unitPools;}
    int getMapId() const                                   {return _mapId;}
    void setFactionSetting(const FactionSetting& fs)       {_factionSetting = fs;}
    void setCards(const Cards& cards)                      {_cards = cards;}
    void setInitUnitLists(const InitUnitLists& list)       {_initUnitLists = list;}
    void setUnitPools(const UnitPools& pools)              {_unitPools = pools;}
    void setMapId(int mapId)                               {_mapId = mapId;}
};

class NetworkMessageCancel2S : public NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageCancel2S(*this);}
    
};

class NetworkMessageCancel2C : public NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageCancel2C(*this);}
};

class NetworkMessageStart2S : public NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageStart2S(*this);}
};

class NetworkMessageStart2C : public NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageStart2C(*this);}
};

class NetworkMessageSync : public NetworkMessage {
private:
    int _frame;
    std::vector<UnderWorld::Core::CmdFramePair> _commands;
    bool _finished;
    
public:
    NetworkMessageSync(int frame) : _frame(frame), _finished(false) {};
    virtual ~NetworkMessageSync();
    NetworkMessage* clone() const;
    
    /** getters */
    int getFrame() const                                 {return _frame;}
    int getCommandCount() const                          {return (int)_commands.size();}
    const UnderWorld::Core::CmdFramePair& getCommand(int index) const
                                                         {return _commands[index];}
    bool isFinished() const                              {return _finished;}
    
    /** setters */
    void addCommand(UnderWorld::Core::OutsideCommand* cmd, int frame)
                                                         {_commands.push_back(std::make_pair(cmd, frame));}
    void setFinished(bool finished)                      {_finished = finished;}
};

class NetworkMessageReconnect2S : public NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageReconnect2S(*this);}
};

class NetworkMessageFinish2S : public NetworkMessage {
public:
    NetworkMessage* clone() const override    {return new NetworkMessageFinish2S(*this);}
};


#endif /* ClientTCPNetworkProxy_h */
