//
//  Utils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "Utils.h"
#include "Global.h"

using namespace std;

unsigned int Utils::bkdrHash(const char *str)
{
    static const unsigned int seed(131); // 31 131 1313 13131 131313 etc..
    unsigned int hash(0);
    
    while (*str) {
        hash = hash * seed + (*str++);
    }
    
    return (hash & 0x7FFFFFFF);
}

string Utils::formatWithParams(const string& raw, ...)
{
    string ret;
    
    va_list ap;
    va_end(ap);
    
    return ret;
}

string Utils::format(const char *format, ...)
{
    static const unsigned int MAX_TEXT_LENGTH = 1024 * 100;
    string ret;
    va_list args;
    va_start(args, format);
    char* buf = (char*)calloc(MAX_TEXT_LENGTH, sizeof(char));
    if (buf != nullptr)
    {
        vsnprintf(buf, MAX_TEXT_LENGTH, format, args);
        ret = buf;
        free(buf);
    }
    va_end(args);
    return ret;
}

void Utils::split(vector<string> &v, const string& src, const string& delimit, const string& null_subst, int max)
{
    if (src.empty() || delimit.empty())
    {
        return;
    }
    
    string strTmp = src;
    string::size_type cutAt = 0;
    v.clear();
    while((cutAt = strTmp.find_first_of(delimit)) != strTmp.npos )
    {
        if (max > 0 && v.size() == max - 1) break;
        v.push_back((cutAt > 0) ? strTmp.substr(0, cutAt) : null_subst);
        strTmp = strTmp.substr(cutAt + 1);
    }
    
    v.push_back(strTmp.empty() ? null_subst : strTmp);
}
