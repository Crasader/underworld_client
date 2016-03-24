//
//  Scheduler.cpp
//  Underworld_Client
//
//  Created by Andy on 16/3/17.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#include "Scheduler.h"
#include <sys/time.h>
#include <thread>
#include <vector>
#include <cassert>
#include <unistd.h>
#include "uthash.h"

// Hash Element used for "selectors with interval"
typedef struct _hashSelectorEntry
{
    std::vector<Timer*> timers;
    void                *target;
    int                 timerIndex;
    Timer               *currentTimer;
    bool                currentTimerSalvaged;
    bool                paused;
    UT_hash_handle      hh;
} tHashTimerEntry;

#pragma mark - Timer
Timer::Timer(Scheduler* scheduler, const schedulerFunc& callback, void *target, const std::string& key, float interval, unsigned int repeat, float delay)
: _scheduler(scheduler)
, _elapsed(-1)
, _runForever(false)
, _useDelay(false)
, _timesExecuted(0)
, _repeat(repeat)
, _delay(delay)
, _interval(interval)
, _target(target)
, _callback(callback)
, _key(key)
{
    _runForever = (_repeat == REPEAT_FOREVER) ? true : false;
    _useDelay = (_delay > 0.0f) ? true : false;
}

void Timer::trigger(float dt)
{
    if (_callback) {
        _callback(dt);
    }
}

void Timer::cancel()
{
    _scheduler->unschedule(_key, _target);
}

void Timer::update(float dt)
{
    if (_elapsed == -1) {
        _elapsed = 0;
        _timesExecuted = 0;
        return;
    }
    
    // accumulate elapsed time
    _elapsed += dt;
    
    // deal with delay
    if (_useDelay) {
        if (_elapsed < _delay) {
            return;
        }
        trigger(_delay);
        _elapsed = _elapsed - _delay;
        _timesExecuted += 1;
        _useDelay = false;
        // after delay, the rest time should compare with interval
        if (!_runForever && _timesExecuted > _repeat) {    //unschedule timer
            cancel();
            return;
        }
    }
    
    // if _interval == 0, should trigger once every frame
    float interval = (_interval > 0) ? _interval : _elapsed;
    while (_elapsed >= interval) {
        trigger(interval);
        _elapsed -= interval;
        _timesExecuted += 1;
        
        if (!_runForever && _timesExecuted > _repeat) {
            cancel();
            break;
        }
        
        if (_elapsed <= 0.f) {
            break;
        }
    }
}

#pragma mark - Scheduler
const int Scheduler::PRIORITY_SYSTEM = INT_MIN;
const int Scheduler::PRIORITY_NON_SYSTEM_MIN = PRIORITY_SYSTEM + 1;

static long getCurrentMilliSecond() {
    struct timeval ct;
    gettimeofday(&ct,NULL);
    const long ret = ct.tv_sec * 1000 + ct.tv_usec * 0.001;
    return ret;
}

static Scheduler* s_pSharedInstance = nullptr;

Scheduler * Scheduler::getInstance()
{
    if (nullptr == s_pSharedInstance) {
        s_pSharedInstance = new (std::nothrow) Scheduler();
        
        // init
        auto t = std::thread(&Scheduler::loop, s_pSharedInstance);
        t.detach();
    }
    return s_pSharedInstance;
}

void Scheduler::purge()
{
    if (s_pSharedInstance) {
        delete s_pSharedInstance;
        s_pSharedInstance = nullptr;
    }
}

// main loop
void Scheduler::update(float dt)
{
    if (_timeScale != 1.0f) {
        dt *= _timeScale;
    }
    
    //
    // Selector callbacks
    //
    
    // Iterate over all the custom selectors
    for (tHashTimerEntry *elt = _hashForTimers; elt != nullptr; ) {
        _currentTarget = elt;
        _currentTargetSalvaged = false;
        
        if (! _currentTarget->paused) {
            // The 'timers' array may change while inside this loop
            for (elt->timerIndex = 0; elt->timerIndex < elt->timers.size(); ++(elt->timerIndex)) {
                elt->currentTimer = elt->timers.at(elt->timerIndex);
                elt->currentTimerSalvaged = false;
                elt->currentTimer->update(dt);
                
                if (elt->currentTimerSalvaged) {
                    // The currentTimer told the remove itself. To prevent the timer from
                    // accidentally deallocating itself before finishing its step, we retained
                    // it. Now that step is done, it's safe to release it.
                    delete elt->currentTimer;
                }
                
                elt->currentTimer = nullptr;
            }
        }
        
        // elt, at this moment, is still valid
        // so it is safe to ask this here (issue #490)
        elt = (tHashTimerEntry *)elt->hh.next;
        
        // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
        if (_currentTargetSalvaged && _currentTarget->timers.size() == 0) {
            removeHashElement(_currentTarget);
        }
    }
    
    _currentTarget = nullptr;
}

void Scheduler::schedule(const schedulerFunc& callback, void *target, float interval, bool paused, const std::string& key)
{
    schedule(callback, target, interval, REPEAT_FOREVER, 0.0f, paused, key);
}

void Scheduler::schedule(const schedulerFunc& callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key)
{
    assert(target);
    assert(!key.empty());
    
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    
    if (!element) {
        element = (tHashTimerEntry *)calloc(sizeof(*element), 1);
        element->target = target;
        
        HASH_ADD_PTR(_hashForTimers, target, element);
        
        // Is this the 1st element ? Then set the pause level to all the selectors of this target
        element->paused = paused;
    }
    else {
        assert(element->paused == paused);
    }
    
    for (Timer *timer : element->timers) {
        if (timer && key == timer->getKey()) {
            timer->setInterval(interval);
            return;
        }
    }
    
    Timer *timer = new (std::nothrow) Timer(this, callback, target, key, interval, repeat, delay);
    element->timers.push_back(timer);
}

void Scheduler::unschedule(const std::string &key, void *target)
{
    if (target == nullptr || key.empty()) {
        return;
    }
    
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    
    if (element) {
        std::vector<Timer*>& timers = element->timers;
        for (int i = 0; i < timers.size(); ++i) {
            auto timer = timers.at(i);
            if (timer && key == timer->getKey()) {
                if (timer == element->currentTimer && (!element->currentTimerSalvaged)) {
                    element->currentTimerSalvaged = true;
                }
                
                delete timer;
                timers.erase(timers.begin() + i);
                
                if (element->timerIndex >= i) {
                    element->timerIndex--;
                }
                
                if (timers.size() == 0) {
                    if (_currentTarget == element)
                    {
                        _currentTargetSalvaged = true;
                    }
                    else
                    {
                        removeHashElement(element);
                    }
                }
                
                return;
            }
        }
    }
}

void Scheduler::unscheduleAllForTarget(void *target)
{
    // explicit nullptr handling
    if (target == nullptr) {
        return;
    }
    
    // Custom Selectors
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    
    if (element) {
        bool contain(false);
        for (Timer *timer : element->timers) {
            if (timer && timer == element->currentTimer) {
                contain = true;
                break;
            }
        }
        
        if (contain && (!element->currentTimerSalvaged)) {
            element->currentTimerSalvaged = true;
        }
        
        for (Timer *timer : element->timers) {
            if (timer) {
                delete timer;
            }
        }
        
        element->timers.clear();
        
        if (_currentTarget == element) {
            _currentTargetSalvaged = true;
        }
        else {
            removeHashElement(element);
        }
    }
}

void Scheduler::unscheduleAll(void)
{
    unscheduleAllWithMinPriority(PRIORITY_SYSTEM);
}

void Scheduler::unscheduleAllWithMinPriority(int minPriority)
{
    // Custom Selectors
    tHashTimerEntry *element = nullptr;
    tHashTimerEntry *nextElement = nullptr;
    for (element = _hashForTimers; element != nullptr;) {
        // element may be removed in unscheduleAllSelectorsForTarget
        nextElement = (tHashTimerEntry *)element->hh.next;
        unscheduleAllForTarget(element->target);
        
        element = nextElement;
    }
}

bool Scheduler::isScheduled(const std::string& key, void *target)
{
    assert(!key.empty());
    assert(target);
    
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    
    if (!element) {
        return false;
    }
    
    if (element->timers.size() == 0) {
        return false;
    }
    else {
        for (Timer *timer : element->timers) {
            if (timer && key == timer->getKey()) {
                return true;
            }
        }
        
        return false;
    }
    
    return false;  // should never get here
}

void Scheduler::pauseTarget(void *target)
{
    assert(target);
    
    // custom selectors
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    if (element) {
        element->paused = true;
    }
}

void Scheduler::resumeTarget(void *target)
{
    assert(target);
    
    // custom selectors
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    if (element) {
        element->paused = false;
    }
}

bool Scheduler::isTargetPaused(void *target)
{
    assert(target);
    
    // Custom selectors
    tHashTimerEntry *element = nullptr;
    HASH_FIND_PTR(_hashForTimers, &target, element);
    if( element ) {
        return element->paused;
    }
    
    return false;  // should never get here
}

std::set<void*> Scheduler::pauseAllTargets()
{
    return pauseAllTargetsWithMinPriority(PRIORITY_SYSTEM);
}

std::set<void*> Scheduler::pauseAllTargetsWithMinPriority(int minPriority)
{
    std::set<void*> idsWithSelectors;
    
    // Custom Selectors
    for(tHashTimerEntry *element = _hashForTimers; element != nullptr;
        element = (tHashTimerEntry*)element->hh.next) {
        element->paused = true;
        idsWithSelectors.insert(element->target);
    }
    
    return idsWithSelectors;
}

void Scheduler::resumeTargets(const std::set<void*>& targetsToResume)
{
    for(const auto &obj : targetsToResume) {
        resumeTarget(obj);
    }
}

#pragma mark - protected
Scheduler::Scheduler(void)
: _timeScale(1.0f)
, _hashForTimers(nullptr)
, _currentTarget(nullptr)
, _currentTargetSalvaged(false)
{
    
}

Scheduler::~Scheduler(void)
{
    unscheduleAll();
}

void Scheduler::loop()
{
    static const long interval(1000);   // milliseconds
    while (true) {
        const long lastTime = getCurrentMilliSecond();
        
        update(interval / 1000);
        
        const long curTime = getCurrentMilliSecond();
        
        if (curTime - lastTime < interval) {
            const long time = (interval - (curTime - lastTime)) * 1000;
            usleep(static_cast<useconds_t>(time));
        }
    }
}

void Scheduler::removeHashElement(_hashSelectorEntry *element)
{
    for (auto timer : element->timers) {
        delete timer;
    }
    
    HASH_DEL(_hashForTimers, element);
    free(element);
}
