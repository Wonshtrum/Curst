#include "nl.h"



#define PTR(T) typeof(T*)
#define ARR(T, N) typeof(T[N])
#define VEC(T) typeof(T*)
#define T2(T0, T1) struct { T0 _0; T1 _1; }
#define T3(T0, T1, T2) struct { T0 _0; T1 _1; T2 _2; T3 _3; }
#define T4(T0, T1, T2, T3) struct { T0 _0; T1 _1; T2 _2; T3 _3; }



# define EMPTY(...)
# define DEFER(...) __VA_ARGS__ EMPTY()
# define EXPAND(...) __VA_ARGS__

#define ENUM_() ENUM
#define NAME(N) N,
#define V1() V2
#define V2(...) V3(__VA_ARGS__)
#define V3(V, T, N, ...) __ ## N ## _ ## V ## _Tag,
#define VARIANT(N, T) DEFER(V1)()(N, T, DEFER(ENUM_)())

#define D1() D2
#define D2(N, ...) typedef enum N ## _Tag N ## _Tag; enum N ## _Tag { __VA_ARGS__

EXPAND(DEFER(D1)()(EXPAND(ENUM))) };

#undef V3
#undef D2
#define V3(V, T, N, ...) typedef T __ ## N ## _ ## V ## _Type NL_COLON
#define D2(N, ...) __VA_ARGS__

EXPAND(DEFER(D1)()(EXPAND(ENUM)))

#undef NAME
#undef VARIANT
#undef V3
#undef D2



#define NAME(N) typedef struct N N; struct N { N ## _Tag  _tag; union {
#define VARIANT(N, T, ...) T N NL_COLON

ENUM } as; };

#undef NAME
#undef VARIANT
#undef PTR
#undef ARR
#undef VEC

#define NAME(N) N,
#define VARIANT(N, T) DEFER(V1)()(N, T, DEFER(ENUM_)())

//#define V3(V, T, N, ...) N N ## _ ## V(T x) { typeof((N){0}.as.V) tmp = *(typeof((N){0}.as.V*))&x; return (N) { ._tag = __ ## N ## _ ## V ## _Tag, .as = { .V = tmp } } NL_COLON }
#define V3(V, T, N, ...) N N ## _ ## V(T x) {  return (N) { ._tag = __ ## N ## _ ## V ## _Tag, .as = { .V = x } } NL_COLON }
#define D2(N, ...) __VA_ARGS__

EXPAND(DEFER(D1)()(EXPAND(ENUM)))

#undef T2
#undef T3
#undef T4


//MATCH(x)
//ARM(Option_Some, { x, y=2, z=Option_Some {a} })
//ARM(Option_None)
//ARM(_)
