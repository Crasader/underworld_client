//
//  CocosUtils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#include "CocosUtils.h"
#include "cocostudio/CocoStudio.h"
#include "Global.h"
#include "Utils.h"
#include "FixedLabelAtlas.h"
#include "NumberJump.h"

USING_NS_CC;
using namespace ui;
using namespace std;

string CocosUtils::getResourcePath(const string &file)
{
    return "GameImages/public/" + file;
}

ResourceType CocosUtils::core_resource_type_key_2_ui_resource_type(const std::string& key) {
    if (key == RES_NAME_GOLD) {
        return ResourceType::Gold;
    } else if (key == RES_NAME_WOOD) {
        return ResourceType::Wood;
    } else {
        return ResourceType::MAX;
    }
}

#pragma mark - labels
Label *CocosUtils::createLabel(const string& text, float fontSize, const string& fontName, const Size& dimensions, TextHAlignment hAlignment, TextVAlignment vAlignment)
{
    const string msg(text.empty() ? " " : text);
#if true
    Label *ret = Label::createWithSystemFont(msg, fontName, fontSize, dimensions, hAlignment, vAlignment);
#else
    static const string fontName("fonts/Font.ttf");
    Label *ret = Label::createWithTTF(msg, fontName, fontSize, dimensions, hAlignment, vAlignment);
    float posOffset = MIN(5.0f, MAX(1.0f, fontSize * 0.05f));
    ret->enableShadow(Color4B::BLACK, Size(.0f, -posOffset));
    ret->enableOutline(Color4B::BLACK, 1);
#endif
    
    return ret;
}

// only for wave num
LabelAtlas *CocosUtils::create42x52Number(const string& text)
{
    static const char *charMapFile = "fonts/42X52.png";
    static const int charMapWidth = 42;
    static const int charMapHeight = 52;
    static int startCharMap = '-';
    
    LabelAtlas *labelAtlas = LabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


/**
 @brief
 $ % L V v ￥ x + s - . / 0 1 2 3 4 5 6 7 8 9 0 :
 $ % & ' ( ) * + s - . / 0 1 2 3 4 5 6 7 8 9 0 :
 */
LabelAtlas *CocosUtils::create10x25Number(const string& text) {
    static const char *charMapFile = "fonts/10X25.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create10x25Number_Green(const string& text) {
    static const char *charMapFile = "fonts/10X25_green.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create10x25Number_Red(const string& text) {
    static const char *charMapFile = "fonts/10X25_red.png";
    static const int charMapWidth = 10;
    static const int charMapHeight = 25;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


LabelAtlas *CocosUtils::create12x30Number(const string& text) {
    static const char *charMapFile = "fonts/12X30.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create12x30Number_Green(const string& text) {
    static const char *charMapFile = "fonts/12X30_green.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create12x30Number_Red(const string& text) {
    static const char *charMapFile = "fonts/12X30_red.png";
    static const int charMapWidth = 12;
    static const int charMapHeight = 30;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}


LabelAtlas *CocosUtils::create16x40Number(const string& text) {
    static const char *charMapFile = "fonts/16X40.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create16x40Number_Green(const string& text) {
    static const char *charMapFile = "fonts/16X40_green.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create16x40Number_Red(const string& text) {
    static const char *charMapFile = "fonts/16X40_red.png";
    static const int charMapWidth = 16;
    static const int charMapHeight = 40;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

LabelAtlas *CocosUtils::create28x44Number(const string& text){
    static const char *charMapFile = "fonts/28X44.png";
    static const int charMapWidth = 28;
    static const int charMapHeight = 44;
    static int startCharMap = '$';
    
    FixedLabelAtlas *labelAtlas = FixedLabelAtlas::create(text, charMapFile, charMapWidth, charMapHeight, startCharMap);
    labelAtlas->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    return labelAtlas;
}

void CocosUtils::jumpNumber(LabelAtlas *label, int toNumer, float duration)
{
    if(label) {
        label->stopActionByTag(NUMBERJUMP_ACTION_TAG);
        int fromNumer = 0;
        string curNumerStr = label->getString();
        if(!curNumerStr.empty()){
            fromNumer = Utils::stoi(curNumerStr);
        }
        if (toNumer == fromNumer) {
            return;
        }
        NumberJump* jump = NumberJump::create(duration, fromNumer, toNumer, CC_CALLBACK_1(LabelAtlas::setString, label));
        Sequence* seq = Sequence::create(jump, CallFuncN::create([toNumer](Ref* pSender){
            LabelAtlas *label = dynamic_cast<LabelAtlas*>(pSender);
            if(label) {
                label->setString(StringUtils::format("%d", toNumer));
            }
        }), NULL);
        seq->setTag(NUMBERJUMP_ACTION_TAG);
        
        label->runAction(seq);
    }
}

void CocosUtils::jumpNumber(Label *label, int toNumer, float duration)
{
    if(label) {
        label->stopActionByTag(NUMBERJUMP_ACTION_TAG);
        int fromNumer = 0;
        string curNumerStr = label->getString();
        if(!curNumerStr.empty()){
            fromNumer = Utils::stoi(curNumerStr);
        }
        if (toNumer == fromNumer) {
            return;
        }
        NumberJump* jump = NumberJump::create(duration, fromNumer, toNumer, CC_CALLBACK_1(Label::setString, label));
        Sequence* seq = Sequence::create(jump, CallFuncN::create([toNumer](Ref* pSender){
            Label *label = dynamic_cast<Label*>(pSender);
            if(label) {
                label->setString(StringUtils::format("%d", toNumer));
            }
        }), NULL);
        seq->setTag(NUMBERJUMP_ACTION_TAG);
        
        label->runAction(seq);
    }
}

float CocosUtils::getFitScreenScale(Node *root)
{
    CCASSERT(dynamic_cast<Widget *>(root) != nullptr, "Only support widget currently");
    
    const Size& winSize = Director::getInstance()->getWinSize();
    const Size& rootSize = root->getContentSize();
    const float scaleX = winSize.width / rootSize.width;
    const float scaleY = winSize.height / rootSize.height;
    
    return ((scaleX < 1.0f || scaleY < 1.0f) ? MIN(scaleX, scaleY) : 1.0f);
}

void CocosUtils::readdChild(Node* parent, Node* child)
{
    if (child && parent) {
        auto cp(child->getParent());
        if (cp != parent) {
            Point worldPoint(child->getPosition());
            if (cp) {
                child->retain();
                worldPoint = cp->convertToWorldSpace(worldPoint);
                child->removeFromParent();
            }
            parent->addChild(child);
            
            // reset position after "addChild",
            // because the parent of "child" may not be "parent"
            if (cp) {
                child->setPosition(child->getParent()->convertToNodeSpace(worldPoint));
                child->release();
            }
        }
    }
}

void CocosUtils::replaceScene(Scene* scene, bool animated)
{
    if (scene) {
        auto director = Director::getInstance();
        auto pScene(scene);
        if (animated) {
            pScene = TransitionFade::create(1.2f, scene);
        }
        
        if (director->getRunningScene()) {
            director->replaceScene(pScene);
        } else {
            director->runWithScene(pScene);
        }
    }
}

void CocosUtils::cleanMemory()
{
    AnimationCache::destroyInstance();
    SpriteFrameCache::destroyInstance();
    Director::getInstance()->getTextureCache()->removeAllTextures();
}

#pragma mark - animations
SpriteFrame* CocosUtils::getPVRFrame(const string& folder, int idx)
{
    idx += 1;
    string fileName = "1";
    fileName.append(Utils::toString(idx / 1000));
    idx = idx % 1000;
    fileName.append(Utils::toString(idx / 100));
    idx = idx % 100;
    fileName.append(Utils::toString(idx / 10));
    fileName.append(Utils::toString(idx % 10));
    
    auto file = folder + "/" + fileName + ".png";
    return SpriteFrameCache::getInstance()->getSpriteFrameByName(file);
}

Node* CocosUtils::getAnimationNode(const string& folder, int idx)
{
    Node* node(nullptr);
    if (".csb" == FileUtils::getInstance()->getFileExtension(folder)) {
        CC_UNUSED_PARAM(idx);
        node = CSLoader::createNode(folder);
    } else {
        auto frame = getPVRFrame(folder, idx);
        if (frame) {
            node = Sprite::createWithSpriteFrame(frame);
        }
    }
    
    return node;
}

float CocosUtils::playAnimation(Node* node,
                                const string& folder,
                                float frameDelay,
                                bool loop,
                                int startIdx,
                                int endIdx,
                                const function<void(Node*)>& callback)
{
    CCASSERT(node, "Animayion node is null");
    if (node) {
        node->stopAllActions();
        
        if (".csb" == FileUtils::getInstance()->getFileExtension(folder)) {
            CC_UNUSED_PARAM(frameDelay);
            auto action = CSLoader::createTimeline(folder);
            node->runAction(action);
            
            if (endIdx > 0) {
                CCASSERT(endIdx >= startIdx, "Frame idx is not allowed");
                action->gotoFrameAndPlay(startIdx, endIdx, loop);
            } else {
                action->gotoFrameAndPlay(startIdx, loop);
            }
            
            if (loop) {
                CCLOG("CocosUtils::playAnimation : Callback will not be called when loop is enabled");
            } else {
                action->setLastFrameCallFunc([=]() {
                    if (callback) {
                        callback(node);
                    }
                });
            }
            
            return (float)action->getDuration() / 60.0f;
        } else {
            Vector<SpriteFrame*> frames;
            for (int i = startIdx; ; ++i) {
                if (endIdx >= 0 && endIdx < i) {
                    break;
                }
                
                auto frame = getPVRFrame(folder, i);
                if (frame) {
                    frames.pushBack(frame);
                }
                else {
                    break;
                }
            }
            
            if (loop && endIdx < 0) {
                for (int i = 0; i < startIdx; ++i) {
                    auto frame = getPVRFrame(folder, i);
                    if (frame) {
                        frames.pushBack(frame);
                    }
                    else {
                        break;
                    }
                }
            }
            
            const size_t cnt(frames.size());
            CCASSERT(cnt > 0, "Animation is not exist.");
            auto cache = AnimationCache::getInstance();
            auto animation = cache->getAnimation(folder);
            if (!animation && cnt > 0) {
                animation = Animation::createWithSpriteFrames(frames, frameDelay);
                animation->setRestoreOriginalFrame(false);
                cache->addAnimation(animation, folder);
            }
            
            if (animation) {
                Action* action(nullptr);
                auto animate = Animate::create(animation);
                if (loop) {
                    action = RepeatForever::create(animate);
                } else {
                    action = Sequence::create(animate, CallFuncN::create(callback), nullptr) ;
                }
                
                node->runAction(action);
                
                return animation->getDuration();
            }
        }
    }
    
    return 0.0f;
}

Node* CocosUtils::playAnimation(const string& file,
                                float frameDelay,
                                bool loop,
                                int startIdx,
                                int endIdx,
                                const function<void(Node*)>& callback)
{
    Node* node(nullptr);
    if (".plist" == FileUtils::getInstance()->getFileExtension(file)) {
        CC_UNUSED_PARAM(frameDelay);
        CC_UNUSED_PARAM(startIdx);
        CC_UNUSED_PARAM(endIdx);
        CC_UNUSED_PARAM(callback);
        node = ParticleSystemQuad::create(file);
        auto particle = dynamic_cast<ParticleSystemQuad*>(node);
        assert(particle);
        particle->setAutoRemoveOnFinish(!loop);
    } else {
        node = getAnimationNode(file, startIdx);
        playAnimation(node, file, frameDelay, loop, startIdx, endIdx, callback);
    }
    
    return node;
}

static Button* createExitButton(Node* parent, const function<void()>& callback, const string& file)
{
    const auto& psize(parent->getContentSize());
    auto button = Button::create(file, file);
    button->addClickEventListener([callback](Ref*) {
        if (callback) {
            callback();
        }
    });
    parent->addChild(button);
    
    static const Vec2 offset(8.0f, 8.0f);
    const auto& size = button->getContentSize();
    button->setPosition(Point(psize.width - size.width / 2, psize.height - size.height / 2) - offset);
    
    return button;
}

Button* CocosUtils::createGrayExitButton(Node* parent, const function<void()>& callback)
{
    return createExitButton(parent, callback, getResourcePath("ui_guanbi.png"));
}

Button* CocosUtils::createRedExitButton(Node* parent, const function<void()>& callback)
{
    return createExitButton(parent, callback, getResourcePath("button_hongse.png"));
}

void CocosUtils::fixWidgetTouchEvent(Widget* widget, bool& flag, const Widget::ccWidgetClickCallback& clickedCallback)
{
    if (widget) {
        CC_ASSERT(widget->isTouchEnabled());
        widget->setSwallowTouches(false);
        widget->addTouchEventListener([=, &flag](Ref* pSender, Widget::TouchEventType type) {
            auto widget = dynamic_cast<Widget*>(pSender);
            if (type == Widget::TouchEventType::BEGAN) {
                flag = false;
            } else if (type == Widget::TouchEventType::MOVED) {
                if (!flag) {
                    const auto& mp(widget->getTouchMovePosition());
                    const auto& bp(widget->getTouchBeganPosition());
                    if (abs(mp.x - bp.x) >= TOUCH_CANCEL_BY_MOVING_DISTANCE ||
                        abs(mp.y - bp.y) >= TOUCH_CANCEL_BY_MOVING_DISTANCE) {
                        flag = true;
                    }
                }
            } else if (type == Widget::TouchEventType::ENDED) {
                if (!flag && clickedCallback) {
                    clickedCallback(pSender);
                }
            }
        });
    }
}

#pragma mark - notifications
void CocosUtils::postNotification(const string& notification)
{
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(notification);
}

bool CocosUtils::sort_by_tag(Node* a, Node* b)
{
    return a->getTag() < b->getTag();
}
