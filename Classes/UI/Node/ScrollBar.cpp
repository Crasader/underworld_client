//
//  ScrollBar.cpp
//  Underworld_Client
//
//  Created by Andy on 15/6/23.
//
//

#include "ScrollBar.h"

ScrollBar::ScrollBar()
:m_pTarget(nullptr)
,m_pBg(nullptr)
,m_pSlider(nullptr)
,m_direction(extension::ScrollView::Direction::NONE)
,m_preContentSize(Size::ZERO)
,m_viewSize(Size::ZERO)
,m_sliderTouched(false)
,m_firstTouch(Vec2::ZERO)
,m_sliderCurPos(Vec2::ZERO)
,m_targetCurPos(Vec2::ZERO)
{
    
}

ScrollBar::~ScrollBar()
{
    
}

ScrollBar * ScrollBar::create(const char * bar_bgFile, const char * bar_sliderFile, extension::TableView * tableView, extension::ScrollView::Direction dir)
{
    ui::Scale9Sprite * pBg = ui::Scale9Sprite::create(bar_bgFile);
    ui::Scale9Sprite * pSlider = ui::Scale9Sprite::create(bar_sliderFile);
    
    return create(pBg, pSlider, tableView, dir);
}

ScrollBar * ScrollBar::create(ui::Scale9Sprite * bar_bg, ui::Scale9Sprite * bar_slider, extension::TableView * tableView, extension::ScrollView::Direction dir)
{
    ScrollBar * pRet = new ScrollBar();
    if (pRet && pRet->init(bar_bg, bar_slider, tableView, dir))
    {
        pRet->autorelease();
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool ScrollBar::init(ui::Scale9Sprite * bar_bg, ui::Scale9Sprite * bar_slider, extension::TableView * tableView, extension::ScrollView::Direction dir)
{
    if (Layer::init())
    {
        m_pBg = bar_bg;
        m_pSlider = bar_slider;
        m_pTarget = tableView;
        m_direction = (dir == extension::ScrollView::Direction::BOTH) ? extension::ScrollView::Direction::VERTICAL : dir;
        m_preContentSize = m_pTarget->getContainer()->getContentSize();
        m_viewSize = m_pTarget->getViewSize();
        
        if (m_direction == extension::ScrollView::Direction::VERTICAL)
        {
            m_pBg->setContentSize(Size(m_pBg->getContentSize().width, m_viewSize.height));
            m_pBg->setPosition(Vec2(m_pBg->getContentSize().width / 2, 0));
            m_pSlider->setPositionX(m_pBg->getContentSize().width / 2);
        }
        else if (m_direction == extension::ScrollView::Direction::HORIZONTAL)
        {
            m_pBg->setContentSize(Size(m_viewSize.width, m_pBg->getContentSize().height));
            m_pBg->setPosition(Vec2(0, -m_pBg->getContentSize().height / 2));
            m_pSlider->setPositionY(-m_pBg->getContentSize().height / 2);
        }
        
        addChild(m_pBg, 0);
        
        updateSlider();
        
        addChild(m_pSlider, 1);
        
        scheduleUpdate();
        
        auto eventListener = EventListenerTouchOneByOne::create();
        eventListener->onTouchBegan = CC_CALLBACK_2(ScrollBar::onTouchBegan, this);
        eventListener->onTouchMoved = CC_CALLBACK_2(ScrollBar::onTouchMoved, this);
        eventListener->onTouchEnded = CC_CALLBACK_2(ScrollBar::onTouchEnded, this);
        eventListener->setSwallowTouches(false);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
        
        return true;
    }
    
    return false;
}

void ScrollBar::updateSlider()
{
    float ratio = 0.0;
    if (m_direction == extension::ScrollView::Direction::VERTICAL)
    {
        ratio = m_viewSize.height / m_preContentSize.height;
        m_pSlider->setContentSize(Size(m_pSlider->getContentSize().width, m_viewSize.height * ratio));
    }
    else if (m_direction == extension::ScrollView::Direction::HORIZONTAL)
    {
        ratio = m_viewSize.width / m_preContentSize.width;
        m_pSlider->setContentSize(Size(m_viewSize.width * ratio, m_pSlider->getContentSize().height));
    }
    
    setVisible( !(ratio >= 1) );
}

void ScrollBar::update(float dt)
{    
    auto curContentSize = m_pTarget->getContainer()->getContentSize();
    if ( !(fabsf(curContentSize.height - m_preContentSize.height) <= 0.00001)  ||
        !(fabsf(curContentSize.width - m_preContentSize.width) <= 0.00001) )
    {
        m_preContentSize = curContentSize;
        updateSlider();
    }
    
    if (m_direction == extension::ScrollView::Direction::VERTICAL)
    {
        auto curOffset = m_pTarget->getContentOffset() + (m_preContentSize - m_viewSize) / 2;
        auto sliderOffset = curOffset.y / (m_viewSize.height - curContentSize.height) *
        (m_viewSize.height - m_pSlider->getContentSize().height);
        if (fabsf(sliderOffset) > (m_viewSize.height - m_pSlider->getContentSize().height) / 2)
        {
            return ;
        }
        m_pSlider->setPositionY(sliderOffset);
    }
    else if (m_direction == extension::ScrollView::Direction::HORIZONTAL)
    {
        auto curOffset = m_pTarget->getContentOffset() + (m_preContentSize - m_viewSize) / 2;
        auto sliderOffset = curOffset.x / (m_viewSize.width - curContentSize.width) *
        (m_viewSize.width - m_pSlider->getContentSize().width);
        if (fabsf(sliderOffset) > (m_viewSize.width - m_pSlider->getContentSize().width) / 2)
        {
            return ;
        }
        m_pSlider->setPositionX(sliderOffset);
    }
}

bool ScrollBar::onTouchBegan(Touch *touch, Event *unused_event)
{
    if (!_visible)
    {
        return false;
    }
    
    m_sliderCurPos = m_pSlider->getPosition();
    m_targetCurPos = m_pTarget->getContentOffset();
    auto touchPoint = touch->getLocation();
    m_firstTouch = touchPoint;
    touchPoint = convertToNodeSpace(touchPoint);
    if (!m_pBg->getBoundingBox().containsPoint(touchPoint))
    {
        return false;
    }
    
    if (m_pSlider->getBoundingBox().containsPoint(touchPoint))
    {
        m_sliderTouched = true;
    }
    else
    {
        if (m_direction == extension::ScrollView::Direction::VERTICAL)
        {
            auto offset = touchPoint.y - m_sliderCurPos.y;
            if (touchPoint.y <= 0)
            {
                offset += m_pSlider->getContentSize().height / 2;
            }
            else
            {
                offset -= m_pSlider->getContentSize().height / 2;
            }
            auto newOff = m_targetCurPos.y + offset / (m_pSlider->getContentSize().height - m_viewSize.height)
            * (m_preContentSize.height - m_viewSize.height);
            m_pTarget->setContentOffset(Vec2(0,newOff));
        }
        else if (m_direction == extension::ScrollView::Direction::HORIZONTAL)
        {
            auto offset = touchPoint.x - m_sliderCurPos.x;
            if (touchPoint.x <= 0)
            {
                offset += m_pSlider->getContentSize().width / 2;
            }
            else
            {
                offset -= m_pSlider->getContentSize().width / 2;
            }
            auto newOff = m_targetCurPos.x + offset / (m_pSlider->getContentSize().width - m_viewSize.width)
            * (m_preContentSize.width - m_viewSize.width);
            m_pTarget->setContentOffset(Vec2(newOff,0));
        }
    }
    return true;
}
void ScrollBar::onTouchMoved(Touch *touch, Event *unused_event)
{
    if (m_sliderTouched)
    {
        auto offPos = touch->getLocation() - m_firstTouch;
        if (m_direction == extension::ScrollView::Direction::VERTICAL)
        {
            auto newOff = m_sliderCurPos.y + offPos.y;
            if (fabsf(newOff) > (m_viewSize.height - m_pSlider->getContentSize().height) / 2)
            {
                (newOff < 0 ? (newOff = (m_pSlider->getContentSize().height - m_viewSize.height) / 2) :
                 (newOff = (m_viewSize.height - m_pSlider->getContentSize().height) / 2));
            }
            newOff -= m_sliderCurPos.y;
            m_pTarget->setContentOffset(Vec2(0,
                                             m_targetCurPos.y + newOff / (m_pSlider->getContentSize().height - m_viewSize.height) 
                                             * (m_preContentSize.height - m_viewSize.height)));
        }
        else if (m_direction == extension::ScrollView::Direction::HORIZONTAL)
        {
            auto newOff = m_sliderCurPos.x + offPos.x;
            if (fabsf(newOff) > (m_viewSize.width - m_pSlider->getContentSize().width) / 2)
            {
                (newOff < 0 ? (newOff = (m_pSlider->getContentSize().width - m_viewSize.width) / 2) : 
                 (newOff = (m_viewSize.width - m_pSlider->getContentSize().width) / 2));
            }
            newOff -= m_sliderCurPos.x;
            m_pTarget->setContentOffset(Vec2(m_targetCurPos.x + newOff / (m_pSlider->getContentSize().width - m_viewSize.width)
                                             * (m_preContentSize.width - m_viewSize.width),0));
        }
    }
}

void ScrollBar::onTouchEnded(Touch *touch, Event *unused_event)
{
    m_sliderTouched = false;
}