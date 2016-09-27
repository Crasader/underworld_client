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

#if !defined(UTILS_REQUIRES_NULL_TERMINATION)
    #if defined(__APPLE_CC__) && (__APPLE_CC__ >= 5549)
        #define UTILS_REQUIRES_NULL_TERMINATION __attribute__((sentinel(0,1)))
    #elif defined(__GNUC__)
        #define UTILS_REQUIRES_NULL_TERMINATION __attribute__((sentinel))
    #else
        #define UTILS_REQUIRES_NULL_TERMINATION
    #endif
#endif

#if !defined(UTILS_REQUIRES_CPP11)
    #if defined(__cplusplus) && __cplusplus >= 201103L
        #define UTILS_REQUIRES_CPP11 (1)
    #endif
#endif

#if !defined(UTILS_REQUIRES_GCC5)
    #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 5))
        #define UTILS_REQUIRES_GCC5 (1)
    #endif
#endif

namespace Utils
{
    unsigned int bkdrHash(const char *str);
    
#pragma mark - string
    template <typename T> std::string toString(const T& n);
    void stringReplace(std::string& src, const std::string& raw, const std::string& replaced);
    std::string formatWithParams(const char *format, ...) UTILS_REQUIRES_NULL_TERMINATION;
    std::string format(const char *format, ...);
    void split(std::vector<std::string> &v, const std::string& src, const std::string& delimit, const std::string& null_subst = "", int max = 0);
    
    void trimLeft(std::string &s);
    void trimRight(std::string &s);
    void trim(std::string &s);
    
    int stoi(const std::string& str);
    float stof (const std::string& str);
    
#pragma mark - security
    int hexCharToInt(char c);
    void charToHex(const unsigned char *input, int inputLen, unsigned char *output);
    void hexToChar(const unsigned char *input, int inputLen, unsigned char *output);
    std::string charToHex(const std::string &input);
    std::string hexToChar(const std::string &input);
    std::string urlEncode(const std::string &value);
    
#pragma mark - time
    long getMillSecond();
    std::string formatTime(long duration, const char* format);
    std::string getTimeString(long timeInterval);
    std::string getCurrentTime(const char* format);
    std::string getFormattedTime(long timeInterval);
    std::string getBeautyTime(long timeInterval);
    
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
