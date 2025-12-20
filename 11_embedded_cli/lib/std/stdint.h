#ifndef STDINT_H
#define STDINT_H

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;

typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;

#define INT8_MIN   (-128)
#define INT8_MAX   127
#define UINT8_MAX  255

#define INT16_MIN  (-32768)
#define INT16_MAX  32767
#define UINT16_MAX 65535

#define INT32_MIN  (-2147483648L)
#define INT32_MAX  2147483647L
#define UINT32_MAX 4294967295UL

#define INT64_MIN  (-9223372036854775808LL)
#define INT64_MAX  9223372036854775807LL
#define UINT64_MAX 18446744073709551615ULL

#define UINTPTR_MAX UINT16_MAX

#endif // STDINT_H

