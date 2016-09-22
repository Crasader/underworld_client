//
//  Utils.h
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#ifndef Utils_h
#define Utils_h

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>

namespace Utils
{
    unsigned int bkdrHash(const char *str);
    template <typename T> std::string toString(const T& n);
    std::string formatWithParams(const std::string& raw, ...);
    std::string format(const char *format, ...);
    void split(std::vector<std::string> &v, const std::string& src, const std::string& delimit, const std::string& null_subst = "", int max = 0);
    
    /** template operations */
    template<typename T> void clearVector(std::vector<T> &vec);
    template<typename K, typename V> void clearMap(std::map<K, V> &m);
    template<typename K, typename V> void clearMap(std::unordered_map<K, V> &m);
};

#pragma mark - template operations
template <typename T> std::string Utils::toString(const T& n)
{
    std::ostringstream stm; stm << n; return stm.str();
}

template<typename T> void Utils::clearVector(std::vector<T> &vec)
{
    for(auto iter = begin(vec); iter != end(vec); ++iter) {
        T p = *iter;
        if (p) { delete p; }
    }
    vec.clear();
}

template<typename K, typename V> void Utils::clearMap(std::map<K, V> &m)
{
    for(auto iter = begin(m); iter != end(m); ++iter) {
        V p = iter->second;
        if (p) { delete p; }
    }
    m.clear();
}

template<typename K, typename V> void Utils::clearMap(std::unordered_map<K, V> &m)
{
    for(auto iter = begin(m); iter != end(m); ++iter) {
        V p = iter->second;
        if (p) { delete p; }
    }
    m.clear();
}

#endif /* Utils_h */
