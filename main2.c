#include <stdint.h>
#include "do.h"

#define auto(V, x) typeof(x) V = x

#define DELAY(...) __VA_ARGS__
#define name(N, ...) N
#define ty(...) _ty(__VA_ARGS__, __VA_ARGS__)
#define _ty(N, T, ...) T
#define tyy(T, ...) ty(T(DELAY(__VA_ARGS__)))

#define fn(ret, args) typeof(ret(*)args)

#define t0() _qp_, struct _qp_ {}
#define t1(A) CAT(_q, name(A), p_), struct CAT(_q, name(A), p_) { ty(A) _0; }
#define t2(A, B) CAT(_q, name(A), name(B), p_), struct CAT(_q, name(A), name(B), p_) { ty(A) _0; ty(B) _1; }
#define t3(A, B, C) CAT(_q, name(A), name(B), name(C), p_), struct CAT(_q, name(A), name(B), name(C), p_) { ty(A) _0; ty(B) _1; ty(C) _2; }
#define t4(A, B, C, D) CAT(_q, name(A), name(B), name(C), name(D), p_), struct CAT(_q, name(A), name(B), name(C), name(D), p_) { ty(A) _0; ty(B) _1; ty(C) _2; ty(D) _3; }

#define Ptr(T) CAT(_p, name(T)), typeof(ty(T)*)
#define Arr(T, N) CAT(_a, N, name(T)), typeof(ty(T)[N])

#define Vec(T) CAT(_v, name(T)), struct CAT(_v, name(T)) { u32 len; u32 cap; ty(T)* data;  }
#define Vec_new(T) (tyy(Vec,T)){ .len = 0, .cap = 0, .data = NULL }

enum Option_Tag { Option_Some, Option_None };
#define Option(T) CAT(_o, name(T)), struct CAT(_o, name(T)) { enum Option_Tag tag; union CAT(_o_as, name(T)){ ty(T) Some; unit None; } as; }
#define Option_Some(T, ...) (tyy(Option, T)){ .tag = Option_Some, .as = { .Some = __VA_ARGS__ } }
#define Option_None(T)      (tyy(Option, T)){ .tag = Option_None, .as = { .None = {} } }
#define Option_debug(T, x) if ((x)->tag == Option_None) printf("None"); else { printf("Some("); CAT(name(T), _debug)(&(x)->as.Some); printf(")"); }

#define NULL 0
typedef ty(t0()) unit;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define int_debug(x) printf("%d", *(x))

ty(Option(int)) first(ty(Vec(int)) v) {
    if (v.len > 0) {
        return Option_Some(int, v.data[0]);
    }
    return Option_None(int);
}

ty(Option(t2(int, char))) find(ty(Vec(char)) v, fn(bool, (char)) p) {
    for (int i = 0; i < v.len; i++) {
        if (p(v.data[i])) {
            ty(Option(t2(int, char))) o = { i, v.data[i] };
            return Option_Some(t2(int, char), { i, v.data[i] });
        }
    }
    return Option_None(t2(int, char));
}

int printf(const char*, ...);
void main() {
    auto(v, Vec_new(int));
    auto(f, first(v));
    printf("first: ");
    Option_debug(int, &f);
    printf("\n");
}
