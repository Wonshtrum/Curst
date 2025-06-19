#define Debug_null(X) printf("(nil)")
#define Debug_int(X) printf("%d", X)
#define Debug_char(X) printf("%c", X)

#define NAME(N) void Debug_ ## N (N this) { printf(#N " {\n");

#define ROOT(N) printf(" " #N ": ");
#define LEAF(T) Debug_ ## T(this); printf("\n");
#include "visitor.h"
#undef Visitor

printf("}\n"); }
