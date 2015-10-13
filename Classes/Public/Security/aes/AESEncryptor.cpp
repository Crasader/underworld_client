//
//  AESEncryptor.cpp
//  Underworld_Client
//
//  Created by burst on 14-11-14.
//
//

#include "AESEncryptor.h"
#include <stdlib.h>
#include <string.h>
#include "errno.h"
#include "LocalHelper.h"

static const char* KEY = "U#O(*&RRI:P$%Z|{";

AESEncryptor::AESEncryptor()
{
    unsigned char userKey[AES_BLOCK_SIZE];
    int i=0;
    for (i = 0; i < AES_BLOCK_SIZE; ++i) {
        userKey[i] = KEY[i] ^ i;
    }
    private_AES_set_encrypt_key_fixed(userKey, AES_KEY_SIZE, &AESEncryptKey);
    private_AES_set_decrypt_key_fixed(userKey, AES_KEY_SIZE, &AESDecryptKey);
}

AESEncryptor::~AESEncryptor()
{
}

AESEncryptor * AESEncryptor::getInstance()
{
    static AESEncryptor instance;
    return &instance;
}

#pragma deprecated
unsigned long AESEncryptor::AESOutputLen(unsigned long inputLen)
{
    long outputLen = 0;
    if (inputLen % AES_BLOCK_SIZE == 0) {
        outputLen = inputLen;
    } else {
        outputLen = (inputLen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }
    return outputLen;
}

void AESEncryptor::AESEncrypt(const unsigned char *in, unsigned long inputLen, unsigned char *out)
{
    long enLen = 0;
    while (enLen < inputLen) {
        AES_encrypt_fixed(in, out, &AESEncryptKey);
        in += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
        enLen += AES_BLOCK_SIZE;
    }
}

void AESEncryptor::AESDecrypt(const unsigned char *in, unsigned long inputLen, unsigned char *out)
{
    long deLen = 0;
    while (deLen < inputLen) {
        AES_decrypt_fixed(in, out, &AESDecryptKey);
        in += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
        deLen += AES_BLOCK_SIZE;
    }
}

unsigned char* AESEncryptor::getAESFileData(const std::string& filename)
{
    unsigned char * pBuffer = NULL;
    unsigned long pSize = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    std::string data = LocalHelper::loadFileContentString(filename);
    if(!data.empty()){
        pBuffer = (unsigned char *)data.c_str();
        pSize = data.size();
        AESDecrypt(pBuffer, pSize, pBuffer);
    }
#else //read and decryptor at once, android toooo
    do
    {
        FILE *fp = fopen(filename.c_str(), "rb");
        if (!fp) break;
        fseek(fp,0,SEEK_END);
        pSize = ftell(fp);
        fseek(fp,0,SEEK_SET);
        pBuffer = (unsigned char*)malloc(pSize);
        unsigned char *tmp = pBuffer;
        int deLen = 0;
        while (deLen < pSize) {
            fread(tmp, sizeof(unsigned char), AES_BLOCK_SIZE, fp);
            AES_decrypt_fixed(tmp, tmp, &AESDecryptKey);
            tmp += AES_BLOCK_SIZE;
            deLen += AES_BLOCK_SIZE;
        }
        fclose(fp);
    } while (0);
#endif
    return pBuffer;
}

void AESEncryptor::AESEncrypt(const std::string& input, std::string &output)
{
    size_t inputLen = input.size();
    unsigned long times = inputLen / AES_BLOCK_SIZE + 1;
    unsigned long buffSize = times * AES_BLOCK_SIZE;
    char* pBuffer = new char[times * AES_BLOCK_SIZE];
    char* tmp = pBuffer;
    memset(tmp, 0, sizeof(char) * buffSize);
    memcpy(tmp, input.c_str(), inputLen);
    for(int i = 0; i < times; ++i){
        AES_encrypt_fixed((const unsigned char *)tmp, (unsigned char*)tmp, &AESEncryptKey);
        tmp += AES_BLOCK_SIZE;
    }
    output.assign(pBuffer, buffSize);
    delete[] pBuffer;
}

void AESEncryptor::AESDecrypt(const std::string& input, std::string &output)
{
    size_t inputLen = input.size();
    unsigned long times = inputLen / AES_BLOCK_SIZE;
    unsigned long buffSize = times * AES_BLOCK_SIZE;
    char* pBuffer = new char[times * AES_BLOCK_SIZE];
    char* tmp = pBuffer;
    memset(tmp, 0, sizeof(char) * buffSize);
    memcpy(tmp, input.c_str(), inputLen);
    for(int i = 0; i < times; ++i){
        AES_decrypt_fixed((const unsigned char *)tmp, (unsigned char*)tmp, &AESDecryptKey);
        tmp += AES_BLOCK_SIZE;
    }
    output = pBuffer; //must be plain text
    delete[] pBuffer;
}

void AESEncryptor::getAESFileData(const std::string& filename, std::string &output)
{
    unsigned char * pBuffer = NULL;
    unsigned long pSize = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    std::string data = LocalHelper::loadFileContentString(filename);
    if(!data.empty()){
        pBuffer = (unsigned char *)data.c_str();
        pSize = data.size();
        std::string input;
        input.assign((char*)pBuffer, pSize);
        AESDecrypt(input, output);
    }
#else //read and decryptor at once, android toooo
    do
    {
        FILE *fp = fopen(filename.c_str(), "rb");
        if (!fp) break;
        fseek(fp,0,SEEK_END);
        pSize = ftell(fp);
        fseek(fp,0,SEEK_SET);
        pBuffer = (unsigned char*)malloc(pSize);
        unsigned char *tmp = pBuffer;
        int deLen = 0;
        while (deLen < pSize) {
            fread(tmp, sizeof(unsigned char), AES_BLOCK_SIZE, fp);
            AES_decrypt_fixed(tmp, tmp, &AESDecryptKey);
            tmp += AES_BLOCK_SIZE;
            deLen += AES_BLOCK_SIZE;
        }
        fclose(fp);
    } while (0);
    output = (char*)pBuffer;
    free(pBuffer);
#endif
}
