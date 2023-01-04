#pragma once

#define _STR(...) #__VA_ARGS__
#define STR(...) _STR(__VA_ARGS__)
#define MSG(...) __pragma(message(STR(__VA_ARGS__)))
#define LOG(...) MSG(__VA_ARGS__)__VA_ARGS__

#define STD (__cplusplus / 100 - 2000)

#include "std-headers.h"

#if STD < 11
#define SINCE_11(...)
#define UNTIL_11(...) __VA_ARGS__
#else
#define SINCE_11(...) __VA_ARGS__
#define UNTIL_11(...)
#endif

#if STD < 14
#define SINCE_14(...)
#define UNTIL_14(...) __VA_ARGS__
#else
#define SINCE_14(...) __VA_ARGS__
#define UNTIL_14(...)
#endif


#if STD < 17
#define SINCE_17(...)
#define UNTIL_17(...) __VA_ARGS__
#else
#define SINCE_17(...) __VA_ARGS__
#define UNTIL_17(...)
#endif

#if STD < 20
#define constexpr
#define SINCE_20(...)
#define UNTIL_20(...) __VA_ARGS__
#else
#define SINCE_20(...) __VA_ARGS__
#define UNTIL_20(...)
#endif

#define CALL(f, ...) { auto a = f(__VA_ARGS__); (void)a;}
#define VCALL(f, ...) { f(__VA_ARGS__); }
#define ECALL(f, ...) CALL(f, __VA_ARGS__) CALL(f, expo, __VA_ARGS__)
#define VECALL(f, ...) VCALL(f, __VA_ARGS__) VCALL(f, expo, __VA_ARGS__)
