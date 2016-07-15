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

class FrameLoader
{
public:
    static FrameLoader* getInstance();
    static void purge();
    
    void addAllFramesAsync(const std::function<void()>& callback);
    void addAsync(const std::vector<std::string>& files, const std::function<void()>& callback);
    void addAsync(const std::string& file, const std::function<void(std::string)>& callback);
    
private:
    FrameLoader();
    virtual ~FrameLoader();
    void recursiveAddFile();
    
private:
    std::unordered_set<std::string> _resources;
    
    bool _isLoading;
    std::vector<std::string> _files;
    std::function<void()> _callback;
};

#endif /* FrameLoader_h */
