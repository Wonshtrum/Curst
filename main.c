#define NULL 0
int printf(const char *, ...);
typedef struct {} unit;


#define CLASS NAME(Test) \
    FIELD(a, int)                   \
    FIELD(b, PTR(int))              \
    FIELD(c, VEC(int))              \
    FIELD(d, ARR(int, 2))           \
    FIELD(e, PTR(PTR(int)))         \
    FIELD(f, PTR(ARR(int, 2)))      \
    FIELD(g, ARR(PTR(int), 2))      \
    FIELD(h, ARR(ARR(int, 2), 4))   \
    FIELD(i, PTR(ARR(PTR(int), 2))) \
    FIELD(j, T2(int, T4(char, char, char, int)))
#include "define_class.h"
#include "derive_debug.h"
#undef CLASS

#define CLASS NAME(Point) \
    FIELD(x, int) \
    FIELD(y, int)
#include "define_class.h"
#include "derive_debug.h"
#undef CLASS

#define ENUM NAME(Option) \
    VARIANT(Some, int) \
    VARIANT(None, unit)
#include "define_enum.h"
#undef ENUM

#define ASSERT(P, ...) typedef char assertion_failed[2*!!(P)-1]

int main() {
    struct { int a; int b; int c; } tt = { .a = 1, .b = 2, .c = 3 };

    Point p = { .x = 1, .y = 2 };
    Debug_Point(p);
    Test t = {
        .a = 42,
        .b = &p.x,
        .c = NULL,
        .d = { 8, 9 },
        .e = NULL,
        .f = NULL,
        .g = { &p.y, NULL },
        .h = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } },
        .i = NULL,
        .j = { 100, { 'H', 'I', '!', -1 } },
    };
    Debug_Test(t);
}
