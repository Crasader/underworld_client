//
//  AESEncryptor.h
//  Underworld_Client
//
//  Created by burst on 14-11-14.
//
//

#ifndef AESEncryptor__
#define AESEncryptor__

#include <stdio.h>
#include <string>
#include "aes.h"

class AESEncryptor
{
private:
    AESEncryptor();
    ~AESEncryptor();
private:
    AES_KEY AESEncryptKey;
    AES_KEY AESDecryptKey;
public:
    static AESEncryptor * getInstance();
    #pragma deprecated
    unsigned long AESOutputLen(unsigned long inputLen);
    void AESEncrypt(const unsigned char *in, unsigned long inputLen, unsigned char *out);
    void AESDecrypt(const unsigned char *in, unsigned long inputLen, unsigned char *out);
    unsigned char* getAESFileData(const std::string& filename);
    
    void AESEncrypt(const std::string& input, std::string &output);
    void AESDecrypt(const std::string& input, std::string &output);
    void getAESFileData(const std::string& filename, std::string &output);
};
#endif /* defined(AESEncryptor__) */
