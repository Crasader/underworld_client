//
//  Utils.cpp
//  Underworld_Client
//
//  Created by Andy on 15/10/14.
//  Copyright (c) 2015 Mofish Studio. All rights reserved.
//

#include "Utils.h"
#include "Global.h"
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <sys/time.h>
#include <chrono>

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

#pragma mark - string
void Utils::stringReplace(string& src, const string& raw, const string& replaced)
{
    string::size_type pos(0);
    const auto rawSize(raw.size());
    const auto replacedSize(replaced.size());
    
    while (string::npos != (pos = src.find(raw, pos))) {
        src.replace(pos, rawSize, replaced);
        pos += replacedSize;
    }
}

string Utils::formatWithParams(const char *format, ...)
{
    string ret;
    string raw(format);
    
    va_list ap;
    va_start(ap, format);
    
    int idx(1);
    auto arg = va_arg(ap, const char*);
    while (arg) {
        ostringstream os;
        os << "{" << idx << "}";
        stringReplace(raw, os.str(), arg);
        
        ++ idx;
        arg = va_arg(ap, const char*);
    }
    
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
    if (buf != nullptr) {
        vsnprintf(buf, MAX_TEXT_LENGTH, format, args);
        ret = buf;
        free(buf);
    }
    va_end(args);
    return ret;
}

void Utils::split(vector<string> &v, const string& src, const string& delimit, const string& null_subst, int max)
{
    if (src.empty() || delimit.empty()) { return; }
    
    string strTmp = src;
    string::size_type cutAt = 0;
    v.clear();
    while((cutAt = strTmp.find_first_of(delimit)) != strTmp.npos ) {
        if (max > 0 && v.size() == max - 1) break;
        v.push_back((cutAt > 0) ? strTmp.substr(0, cutAt) : null_subst);
        strTmp = strTmp.substr(cutAt + 1);
    }
    
    v.push_back(strTmp.empty() ? null_subst : strTmp);
}

void Utils::trimLeft(string &s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
}

void Utils::trimRight(string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
}

void Utils::trim(string &s)
{
    trimLeft(s);
    trimRight(s);
}

int Utils::stoi(const string& str)
{
#if defined(__cplusplus) && __cplusplus >= 201103L
    return std::stoi(str);
#else
    return std::atoi(str.c_str());
#endif
}

float Utils::stof(const string& str)
{
#if defined(__cplusplus) && __cplusplus >= 201103L
    return std::stof(str);
#else
    return std::atof(str.c_str());
#endif
}

#pragma mark - security
static const char *tohex = "0123456789ABCDEF";

int Utils::hexCharToInt(char c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}

void Utils::charToHex(const unsigned char *input, int inputLen, unsigned char *output)
{
    int i = 0;
    while(i < inputLen) {
        *output++ = tohex[input[i] >> 4];
        *output++ = tohex[input[i] & 0x0F];
        ++i;
    }
}

void Utils::hexToChar(const unsigned char *input, int inputLen, unsigned char *output)
{
    int i = 0;
    while(i < inputLen) {
        *output++ = (unsigned char)(hexCharToInt(input[i]) << 4 | hexCharToInt(input[i+1]));
        i+=2;
    }
}

string Utils::charToHex(const string &input)
{
    int i = 0;
    size_t inputLen = input.length();
    string output(inputLen * 2, 0);
    while(i < inputLen) {
        output[i * 2] = tohex[input[i] >> 4];
        output[i * 2 + 1] = tohex[input[i] & 0x0F];
        ++i;
    }
    return output;
}

string Utils::hexToChar(const string &input)
{
    int i = 0;
    size_t inputLen = input.length();
    string output(inputLen / 2, 0);
    output.resize(inputLen / 2);
    while(i < inputLen) {
        output[i / 2] = (unsigned char)(hexCharToInt(input[i]) << 4 | hexCharToInt(input[i+1]));
        i+=2;
    }
    return output;
}

#pragma mark - time
long Utils::getMillSecond()
{
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

string Utils::formatTime(long duration, const char* format)
{
    const time_t timeT = duration;
    struct tm tm = *localtime(&timeT);
    
#if UTILS_REQUIRES_GCC5
    return Utils::toString(put_time(&tm, format));
#else
    static const unsigned int maxSize(255);
    char szOut[maxSize];
    auto size = strftime(szOut, maxSize, format, &tm);
    return std::string(szOut, size);
#endif
}

string Utils::getTimeString(long timeMillSecond)
{
    return formatTime(timeMillSecond / 1000, "%d-%m %H-%M");
}

string Utils::getCurrentTime(const char* format)
{
    auto seconds(std::chrono::duration_cast<std::chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count());
    return formatTime(seconds, format);
}

string Utils::getFormattedTime(long timeInterval)
{
    int second = timeInterval % 60;
    long minute = timeInterval / 60;
    std::ostringstream oss;
    oss << setfill('0') << setw(2) << minute % 60 << ":" << setfill('0') << setw(2) << second;
    return oss.str();
}

string Utils::getBeautyTime(long timeInterval)
{
    int second = timeInterval % 60;
    long minute = timeInterval / 60;
    long hour = minute / 60;
    long day = hour / 24;
    
    std::ostringstream oss;
    if (day >= 1){
        oss << setfill('0') << setw(2) << day << " days " << setfill('0') << setw(2) << hour % 24 << " hours";
    } else if(hour >= 1){
        oss << setfill('0') << setw(2) << hour << " hours " << setfill('0') << setw(2) << minute % 60 << " minutes";
    } else {
        oss << setfill('0') << setw(2) << minute << " minutes " << setfill('0') << setw(2) << second << " seconds";
    }
    return oss.str();
}

string Utils::urlEncode(const string &value)
{
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;
    
    for (auto i = begin(value), n = end(value); i != n; ++i) {
        auto c = (*i);
        
        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        
        // Any other characters are percent-encoded
        escaped << '%' << uppercase << setw(2) << int((unsigned char) c);
    }
    
    return escaped.str();
}
