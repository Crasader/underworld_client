//
//  Scheduler.h
//  Underworld_Client
//
//  Created by Andy on 16/3/17.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef Scheduler_h
#define Scheduler_h

#include <set>
#include <string>

class Scheduler;

#define REPEAT_FOREVER (UINT_MAX -1)

typedef std::function<void(float)> schedulerFunc;

#pragma mark - Timer
class Timer
{
public:
    Timer(::Scheduler* scheduler, const schedulerFunc& callback, void *target, const std::string& key, float interval, unsigned int repeat, float delay);
    
    inline void setInterval(float interval) { _interval = interval; }
    inline const std::string& getKey() const { return _key; }
    
    void trigger(float dt);
    void cancel();
    
    void update(float dt);
    
protected:
    ::Scheduler* _scheduler;
    float _elapsed;
    bool _runForever;
    bool _useDelay;
    unsigned int _timesExecuted;
    unsigned int _repeat;
    float _delay;
    float _interval;
    void* _target;
    schedulerFunc _callback;
    std::string _key;
};

class _hashSelectorEntry;

#pragma mark - Scheduler
class Scheduler
{
public:
    static const int PRIORITY_SYSTEM;
    static const int PRIORITY_NON_SYSTEM_MIN;
    
    static Scheduler * getInstance();
    static void purge();
    
    inline float getTimeScale() { return _timeScale; }
    inline void setTimeScale(float timeScale) { _timeScale = timeScale; }
    
    void update(float dt);
    void schedule(const schedulerFunc& callback, void *target, float interval, unsigned int repeat, float delay, bool paused, const std::string& key);
    void schedule(const schedulerFunc& callback, void *target, float interval, bool paused, const std::string& key);
    
    void unschedule(const std::string& key, void *target);
    void unscheduleAllForTarget(void *target);
    void unscheduleAll();
    void unscheduleAllWithMinPriority(int minPriority);
    
    bool isScheduled(const std::string& key, void *target);
    void pauseTarget(void *target);
    void resumeTarget(void *target);
    bool isTargetPaused(void *target);
    
    std::set<void*> pauseAllTargets();
    std::set<void*> pauseAllTargetsWithMinPriority(int minPriority);
    
    void resumeTargets(const std::set<void*>& targetsToResume);
    
protected:
    Scheduler();
    virtual ~Scheduler();
    
    void loop();
    
    void removeHashElement(struct ::_hashSelectorEntry *element);
    
protected:
    float _timeScale;
    
    struct ::_hashSelectorEntry *_hashForTimers;
    struct ::_hashSelectorEntry *_currentTarget;
    bool _currentTargetSalvaged;
};

#endif /* Scheduler_h */
