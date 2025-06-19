#include "nl.h"

#define NAME(N) typedef struct N N; struct N {
#define FIELD(N, T, ...) T N NL_COLON
#define PTR(T) typeof(T*)
#define ARR(T, N) typeof(T[N])
#define VEC(T) typeof(T*)

#define T2(T0, T1) struct { T0 _0; T1 _1; }
#define T3(T0, T1, T2) struct { T0 _0; T1 _1; T2 _2; T3 _3; }
#define T4(T0, T1, T2, T3) struct { T0 _0; T1 _1; T2 _2; T3 _3; }

CLASS };

#undef NAME
#undef FIELD
#undef PTR
#undef ARR
#undef VEC

#undef T2
#undef T3
#undef T4
