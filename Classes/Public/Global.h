//
//  Global.h
//  Underworld_Client
//
//  Created by Andy on 15/10/13.
//
//

#ifndef Global_h
#define Global_h

#define INVALID_VALUE   (-1)

/** public macro */
#ifndef M_SAFE_DELETE
#define M_SAFE_DELETE(p) do { delete (p); (p) = nullptr; } while(0)
#endif // SAFE DELETE

#ifndef M_BREAK_IF
#define M_BREAK_IF(cond)    if(cond) break
#endif //IF BREAK

#ifndef M_RETURN_IF
#define M_RETURN_IF(cond)   if(cond) return
#endif //IF RETURN

#endif /* Global_h */
