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

typedef enum
{
    kRaceType_Vampire,
    kRaceType_Werewolf,
}RaceType;

typedef enum
{
    kUnit,
    kHero,
    kTower,
}UType;

// Unit
enum
{
    // Vampire
    kVampireUnit_0,
    kVampireUnit_1,
    kVampireUnit_2,
    
    // Werewolf
    kWerewolfUnit_0,
    kWerewolfUnit_1,
    kWerewolfUnit_2,
};

// Hero
enum
{
    // Vampire
    kVampireHero_0,
    kVampireHero_1,
    
    // Werewolf
    kWerewolfHero_0,
    kWerewolfHero_1,
    
    
};

// Tower
enum
{
    kVampireTower_0,
};

/** public macro */
#ifndef M_MIN
#define M_MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef M_MAX
#define M_MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

#ifndef M_SAFE_DELETE
#define M_SAFE_DELETE(p) do { delete (p); (p) = nullptr; } while(0)
#endif // SAFE DELETE

#ifndef M_DISALLOW_COPY_AND_ASSIGN
#define M_DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName &) = delete; \
TypeName &operator =(const TypeName &) = delete;
#endif // DISALLOW COPY

#ifndef M_BREAK_IF
#define M_BREAK_IF(cond)           if(cond) break
#endif //IF BREAK

#ifndef M_RETURN_IF
#define M_RETURN_IF(cond)           if(cond) return
#endif //IF RETURN

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define M_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
#define M_DEPRECATED_ATTRIBUTE __declspec(deprecated)
#else
#define M_DEPRECATED_ATTRIBUTE
#endif

#endif /* Global_h */
