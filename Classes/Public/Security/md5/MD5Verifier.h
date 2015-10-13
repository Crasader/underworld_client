//
//  MD5Verifier.h
//  Underworld_Client
//
//  Created by burst on 14-11-15.
//
//

#ifndef MD5Verifier__
#define MD5Verifier__

#include <stdio.h>
#include <string.h>
#include "md5.h"

using namespace std;

class MD5Verifier
{
private:
    MD5Verifier();
    ~MD5Verifier();
    string mKey;
public:
    static MD5Verifier * getInstance();
    bool verify(const string &input, string &output);
    string digest(const string &input, const string &key);
    string encrypt(const string &input);
};
#endif /* defined(MD5Verifier__) */
