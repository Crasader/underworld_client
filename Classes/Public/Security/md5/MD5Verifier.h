//
//  MD5Verifier.h
//  Underworld_Client
//
//  Created by burst on 14-11-15.
//
//

#ifndef MD5Verifier__
#define MD5Verifier__

#include <iostream>
#include "md5.h"

class MD5Verifier
{
private:
    MD5Verifier();
    ~MD5Verifier();
    std::string mKey;
public:
    static MD5Verifier * getInstance();
    bool verify(const std::string &input, std::string &output);
    std::string digest(const std::string &input, const std::string &key);
    std::string encrypt(const std::string &input);
};
#endif /* defined(MD5Verifier__) */
