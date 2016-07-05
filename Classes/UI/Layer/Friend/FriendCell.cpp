//
//  FriendCell.cpp
//  Underworld_Client
//
//  Created by burst on 16/1/22.
//
//

#include "FriendCell.h"
#include "CocosGlobal.h"
#include "CocosUtils.h"
#include "FriendData.h"

using namespace std;
using namespace ui;

FriendCell::FriendCell()
{
    
}

FriendCell::~FriendCell()
{
    
}

FriendCell* FriendCell::create(const FriendData *friendData)
{
    FriendCell* pRet = new (nothrow) FriendCell();
    if (pRet && pRet->init(friendData)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return pRet;
}

bool FriendCell::init(const FriendData* friendData)
{
    if (TableViewCell::init()) {
        
        static const string CsbFile("UI_Chat/UI_FriendIcon.csb");
        Node* mainNode = CocosUtils::playAnimation(CsbFile, 0, false);
        addChild(mainNode);
        
        _rootNode = dynamic_cast<Button *>(mainNode->getChildByTag(54)->getChildByTag(61));
        _rootNode->setSwallowTouches(false);
        const Vector<Node*>& children = _rootNode->getChildren();
        for (int i = 0; i < children.size(); ++i)
        {
            Node* child = children.at(i);
            if (child) {
                const int tag = child->getTag();
                if (tag > 0) {
                    switch (tag) {
                        case 56:
                            //name
                        {
                            Label* name = CocosUtils::createLabel(friendData->getUserName(), DEFAULT_FONT_SIZE);
                            child->addChild(name);
                        }
                            break;
                        case 57:
                            //level
                        {
                            LabelAtlas* level = CocosUtils::create12x30Number(StringUtils::format("&(.%d", friendData->getLevel()));
                            child->addChild(level);
                        }
                            break;
                        case 60:
                            //icon
                        {
                            Node* node = child->getChildByTag(55);
                            Sprite* icon = Sprite::create(StringUtils::format("GameImages/avatars/%d.png", friendData->getIconId()));
                            node->addChild(icon);
                        }
                            break;
                    }
                }
            }
        }
        _userId = friendData->getUserId();
        return true;
    }
    return false;
}


void FriendCell::registerObserver(FriendCellObserver *observer)
{
    _observer = observer;
}
