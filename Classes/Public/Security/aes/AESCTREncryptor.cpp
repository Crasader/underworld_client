//
//  AESCTREncryptor.cpp
//  AESCTREncryptor
//
//  Created by burst on 15-5-12.
//  Copyright (c) 2015 mofish. All rights reserved.
//

#include "AESCTREncryptor.h"
#include <string.h>
#include "LocalHelper.h"

static const char* KEY = "U#O(*&RRI:P$%Z|{";

AESCTREncryptor::AESCTREncryptor()
{
    init_key();
}

AESCTREncryptor::~AESCTREncryptor()
{
    
}

AESCTREncryptor* AESCTREncryptor::getInstance()
{
    static AESCTREncryptor instance;
    return &instance;
}

void AESCTREncryptor::init_key()
{
    unsigned char userKey[AES_BLOCK_SIZE];
    int i=0;
    for (i = 0; i < AES_BLOCK_SIZE; ++i) {
        userKey[i] = KEY[i] ^ i;
    }
    private_AES_set_encrypt_key_fixed(userKey, AES_KEY_SIZE, &AESEncryptKey);
    private_AES_set_decrypt_key_fixed(userKey, AES_KEY_SIZE, &AESDecryptKey);
}

void AESCTREncryptor::init_ctr(ctr_state &state)
{
    /* aes_ctr128_encrypt requires 'num' and 'ecount' set to zero on the
     * first call. */
    state.num = 0;
    memset(state.ecount, 0, AES_BLOCK_SIZE);
    
    /* Copy IV into 'ivec' */
    //RtlCopyMemory(state->ivec, iv, AES_BLOCK_SIZE);
    memset(state.ivec, 0, AES_BLOCK_SIZE);
}

void AESCTREncryptor::Encrypt(const char* input, char* output, unsigned long len)
{
    struct ctr_state state;
    init_ctr(state);
    
    AES_ctr128_encrypt(
                       (const unsigned char*)input,
                       (unsigned char*)output,
                       len,
                       &AESEncryptKey,
                       state.ivec,
                       state.ecount,
                       &state.num);
}

void AESCTREncryptor::Decrypt(const char* input, char* output, unsigned long len)
{
    struct ctr_state state;
    init_ctr(state);
    
    AES_ctr128_decrypt((const unsigned char*)input,
                       (unsigned char*)output,
                       len,
                       &AESDecryptKey,
                       state.ivec,
                       state.ecount,
                       &state.num);
}

std::string AESCTREncryptor::Encrypt(const std::string& input)
{
    struct ctr_state state;
    init_ctr(state);
    size_t len = input.size();
    char* tmp = new char[len];
    AES_ctr128_encrypt(
                       (const unsigned char*)input.c_str(),
                       (unsigned char*)tmp,
                       len,
                       &AESEncryptKey,
                       state.ivec,
                       state.ecount,
                       &state.num);
    std::string output;
    output.assign(tmp, len);
    delete[] tmp;
    return output;
}

std::string AESCTREncryptor::Decrypt(const std::string& input)
{
    struct ctr_state state;
    init_ctr(state);
    size_t len = input.size();
    char* tmp = new char[len];
    AES_ctr128_decrypt((const unsigned char*)input.c_str(),
                       (unsigned char*)tmp,
                       len,
                       &AESDecryptKey,
                       state.ivec,
                       state.ecount,
                       &state.num);
    std::string output;
    output.assign(tmp, len);
    delete[] tmp;
    return output;
}

std::string AESCTREncryptor::getFileData(const std::string& filename)
{
    std::string ret = LocalHelper::loadFileContentString(filename);
    if (ret.size() > 0)
    {
        return Encrypt(ret);
    }
    return "";
}