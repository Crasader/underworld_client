//
//  NSString+Extended.h
//  Underworld_Client
//
//  Created by Andy on 16/3/15.
//  Copyright (c) 2016 Mofish Studio. All rights reserved.
//
//

#include <string>

@interface NSString (Extended)

- (std::string)stdString;
- (NSString *)toAppleProductId;
- (NSString *)hashedValueForAccount;

@end
