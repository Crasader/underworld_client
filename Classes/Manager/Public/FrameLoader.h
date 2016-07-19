//
//  FrameLoader.h
//  Underworld_Client
//
//  Created by Andy on 16/7/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//

#ifndef FrameLoader_h
#define FrameLoader_h

#include <string>
#include <vector>
#include <unordered_set>
#include <functional>

class FrameLoader
{
public:
    static FrameLoader* getInstance();
    static void purge();
    
    // synchronous
    void addAllFrames();
    void add(const std::vector<std::string>& files);
    void add(const std::string& file);
    
    // asynchronous
    void addAllFramesAsync(const std::function<void()>& callback);
    void addAsync(const std::vector<std::string>& files, const std::function<void()>& callback);
    void addAsync(const std::string& file, const std::function<void(std::string)>& callback);
    
    void removeCachedFrames();
    
private:
    FrameLoader();
    virtual ~FrameLoader();
    void universalAdd(const std::string& file, const std::function<void(std::string)>& callback);
    void recursiveAddAsync();
    
private:
    std::unordered_set<std::string> _resources;
    
    bool _isLoading;
    std::vector<std::string> _files;
    std::function<void()> _callback;
};

#endif /* FrameLoader_h */
