//
//  AESCTREncryptor.h
//  AESCTREncryptor
//
//  Created by burst on 15-5-12.
//  Copyright (c) 2015 mofish. All rights reserved.
//

#ifndef __AESCTREncryptor__AESCTREncryptor__
#define __AESCTREncryptor__AESCTREncryptor__

#include <stdio.h>
#include <string>
#include "aes.h"

struct ctr_state {
    unsigned char ivec[16];  /* ivec[0..7] is the IV, ivec[8..15] is the big-endian counter */
    unsigned int num;
    unsigned char ecount[16];
};

class AESCTREncryptor
{
private:
    unsigned char iv[AES_BLOCK_SIZE];// = {0/*'U','T','Y','[','^','F','B',']','U','T','Y','[','^','F','B',']'*/};
    AES_KEY AESEncryptKey;
    AES_KEY AESDecryptKey;
private:
    void init_ctr(ctr_state &state);
    void init_key();
    AESCTREncryptor();
    ~AESCTREncryptor();
public:
    static AESCTREncryptor* getInstance();
    void Encrypt(const char* input, char* output, unsigned long len);
    void Decrypt(const char* input, char* output, unsigned long len);
    
    std::string Encrypt(const std::string& input);
    std::string Decrypt(const std::string& input);
    
    std::string getFileData(const std::string& filename);
};

#endif /* defined(__AESCTREncryptor__AESCTREncryptor__) */
