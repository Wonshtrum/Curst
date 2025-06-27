#include "curst.h"

#ifdef MANY_VISITED_FOR
#define VISITOR MANY_VISITED_FOR
#endif

#ifdef MANY_VISITOR_FOR
#define VISITED MANY_VISITOR_FOR
#endif

_declare(VISITED);

#undef  __struct
#define __struct(N, F) N, (struct N), (__visit(Struct)(N, F))
#undef  _field
#define _field(N, T) { __visit(Struct_field)(N, __follow(T, &this->N)); }

__derive(VISITED);

#undef  __struct
#define __struct(N, F) N, (struct N { F }), (__visit(N)(__visitor_of(arg_names, void)))
#undef  _field
#define _field(N, T) __type T N;

#undef VISITOR
#undef VISITED
