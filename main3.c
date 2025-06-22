#include "nl.h"

int printf(const char*, ...);


#define ty(T) __ty(T)
#define __ty(N, T, V) T
#define __name(N, T, V) N
#define __visitor(N, T, V) V
#define __bind(N, T, V) N, T, V
#define __with(T, V) __name(T), __ty(T), V

#define follow(T, path, V) T *_this = &(path); typeof(_this) this = _this; V


#define i32 i, int, _visitor(int)()

#define Ptr(T) Ptr_(T)
#define Ptr_(N, T, V) _p##N, typeof(T*), \
    _visitor(Ptr)( follow(T, *this, V); )

#define Arr(n, T) Arr_(n, T)
#define Arr_(n, N, T, V) _a##n##N, typeof(T[n]), \
    _visitor(Arr)(n, \
    for (int i=0; i<n; i++) NL_RCURLY \
        _visitor(Arr_item)(i, follow(T, (*this)[i], V)); \
    })

#define Vec(T) Vec_(T)
#define Vec_(N, T, V) _v##N, struct _v##N { int len; int cap; T* data; }, \
    _visitor(Vec)( \
    for (int i=0; i<this->len; i++) NL_RCURLY \
        _visitor(Vec_item)(i, follow(T, this->data[i], V)); \
    })
#define Vec_new(T) (ty(Vec_(T))){ .len = 0, .cap = 0, .data = 0 }

#define    int_debug() printf("%d", *this)
#define struct_debug(C, V) printf("struct " #C " {"); V; printf("\n}")
#define  field_debug(F, V) printf("\n " #F ": "); V; printf(",")

#define Ptr_debug(V) if (this) { V; } else { printf("(nil)") }
#define Arr_debug(n, V) printf("(" #n ")[ "); V; printf("]")
#define Arr_item_debug(i, V) /*printf("%d: ", i);*/ V; printf(", ")
#define Vec_debug(V) printf("[ "); V; printf("]")
#define Vec_item_debug(i, V) /*printf("%d: ", i);*/ V; printf(", ")

#define Test(A, B) Test_(A, B)
#define Test_(NA, TA, VA, NB, TB, VB) struct(Test_##NA##NB, field(a, __bind(NA, TA, VA)) field(b, Vec(Vec(__bind(NB, TB, VB)))))
#define CLASS(A, B) Test_(A, B)

#define struct(C, fields) typedef struct C C; struct C { fields }
#define field(F, T) field_(F, T)
#define field_(F, N, T, V) T F;
CLASS(i32, i32);
#undef struct
#undef field_

#define __visitor_ret(T) void
#define __visitor_args(T) T *this, int fmt
#define _visitor(T) T##_debug

#define struct(C, fields) __visitor_ret(C) _visitor(C) (__visitor_args(C)) { _visitor(struct)(C, fields); }
#define field_(F, N, T, V) NL_RCURLY _visitor(field)(F, follow(T, this->F, V)); }
CLASS(i32, i32);
#undef struct
#undef field_

#undef __visitor_ret
#undef __visitor_args
#undef _visitor
#define _visitor(T)

#define struct(C, fields) C, C, _visitor(C)()

#define method(C, M) method_(C, M)
#define method_(N, T, V, M) N##_##M

void main() {
    //Vec_new(Vec(Test(i32, i32)));
    int data[4] = { 2, 3, 4, 5 };
    ty(Vec(i32)) v[2] = {
        { .len = 4, .cap = 5, .data = data },
        { .len = 3, .cap = 3, .data = data },
    };
    ty(Test(i32, i32)) t = {
        .a = 42,
        .b = { .len = 2, .cap = 3, .data = v },
    };
    method(Test(i32, i32), debug)(&t, 0);
    printf("\n");
}
