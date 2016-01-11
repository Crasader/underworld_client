//
//  PieceManager.h
//  Underworld_Client
//
//  Created by Andy on 16/1/11.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef PieceManager_h
#define PieceManager_h

#include "json/document.h"
#include <map>
#include "CocosGlobal.h"

class PieceManager
{
public:
    PieceManager();
    virtual ~PieceManager();
    
    void init(const rapidjson::Value& jsonDict);
    void compose(int id, int count);
    void sell(int id, int count);
    
    int getPieceCount(int id);
    
protected:
    M_DISALLOW_COPY_AND_ASSIGN(PieceManager);
    
private:
    std::map<int, int> _soldierPieces;
    std::map<int, int> _heroPieces;
};

#endif /* PieceManager_h */
