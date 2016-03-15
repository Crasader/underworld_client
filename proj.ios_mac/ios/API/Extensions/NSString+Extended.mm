//
//  NSString+Extended.m
//  Underworld_Client
//
//  Created by Andy on 16/3/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//
//

#import "NSString+Extended.h"
#import <CommonCrypto/CommonCrypto.h>

@implementation NSString (Extended)

- (std::string)stdString
{
    const char* utf8 = self.UTF8String;
    if (utf8) {
        return utf8;
    }
    
    return "";
}

- (NSString *)toAppleProductId
{
    NSString * productIdPrefix = [NSString stringWithFormat:@"%@.gem.", [NSBundle mainBundle].bundleIdentifier];
    return [NSString stringWithFormat:@"%@%@", productIdPrefix, self];
}

- (NSString *)hashedValueForAccount
{
    static const int HASH_SIZE = 32;
    unsigned char hashedChars[HASH_SIZE];
    const char *accountName = [self UTF8String];
    size_t accountNameLen = strlen(accountName);
    
    // Confirm that the length of the user name is small enough
    // to be recast when calling the hash function.
    if (accountNameLen > UINT32_MAX) {
        return nil;
    }
    CC_SHA256(accountName, (CC_LONG)accountNameLen, hashedChars);
    
    // Convert the array of bytes into a string showing its hex representation.
    NSMutableString *userAccountHash = [[NSMutableString alloc] init];
    for (int i = 0; i < HASH_SIZE; i++) {
        // Add a dash every four bytes, for readability.
        if (i != 0 && i % 4 == 0) {
            [userAccountHash appendString:@"-"];
        }
        [userAccountHash appendFormat:@"%02x", hashedChars[i]];
    }
    
    return userAccountHash;
}

@end
