//
//  MD5Verifier.cpp
//  Underworld_Client
//
//  Created by burst on 14-11-15.
//
//

#include "MD5Verifier.h"

using namespace std;

static const char* KEY = "U#O(*&RRI:P$%Z|{";//maybe use imei or uuid
static const int CHECK_BITS = 4;

MD5Verifier::MD5Verifier(){
    size_t bufLen = strlen(KEY);
    char buf[bufLen+1];
    int i=0;
    for (i = 0; i < strlen(KEY); ++i) {
        buf[i] = KEY[i] ^ i;
    }
    buf[bufLen] = 0;
    mKey = string(buf);
}
MD5Verifier::~MD5Verifier(){
    
}
MD5Verifier* MD5Verifier::getInstance()
{
    static MD5Verifier instance;
    return &instance;
}
bool MD5Verifier::verify(const string &input, string &output)
{
    if(input.size() < CHECK_BITS){
        return false;
    }
    string code = input.substr(0, CHECK_BITS);
    string src = input.substr(CHECK_BITS);
    MD5 md5(src + mKey);
    string result = md5.md5();
    if(result.compare(0, CHECK_BITS, code) == 0){
        output = src;
        return true;
    }
    return false;
}

string MD5Verifier::digest(const string &input, const string &key)
{
    MD5 md5(input + mKey + key);
    return md5.md5();
}

string MD5Verifier::encrypt(const string &input)
{
    MD5 md5(input + mKey);
    string result = md5.md5();
    return result.substr(0, CHECK_BITS) + input;
}

