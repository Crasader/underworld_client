//
//  FriendCell.h
//  Underworld_Client
//
//  Created by burst on 16/1/22.
//
//

#ifndef FriendCell_h
#define FriendCell_h

#include <stdio.h>
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class FriendData;
class FriendCell;

class FriendCellObserver
{
public:
    virtual ~FriendCellObserver() {};
    virtual void onFriendCellClicked(FriendCell *friendCell) = 0;
};

class FriendCell: public TableViewCell
{
public:
    static FriendCell* create(const FriendData* friendData);
    FriendCell();
    virtual ~FriendCell();
    void registerObserver(FriendCellObserver* observer);
protected:
    bool init(const FriendData* friendData);
private:
    ui::Button* _rootNode;
    int _userId;
    FriendCellObserver* _observer;
};


#endif /* FriendCell_h */
