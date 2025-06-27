#ifndef __CURST
#define __CURST

#include "stdint.h"
#include "stdio.h"
#include "do.h"

//================================================
// Std
//================================================

//int printf(const char*, ...);
//int fprintf(void*, const char*, ...);
void exit(int);
void __invalid_enum_tag(const char* file, int line, const char* base, const char* mangle, int tag) {
    fflush(stdout);
    fprintf(stderr, "\nPanic at %s:%d: tried to access enum %s (%s) with invalid variant %d\n", file, line, base, mangle, tag);
    exit(-1);
}
void __panic_at(const char* file, const char* line, const char* message) {
    fflush(stdout);
    fprintf(stderr, "\nPanic at %s:%d: %s\n", message);
    exit(-1);
}

//================================================
// Generic helpers
//================================================

#define __head(H, ...) H
#define __tail(H, ...) __VA_ARGS__
#define __apply(M, ...) M(__VA_ARGS__)
#define __split(...) __VA_ARGS__
#define __stmt(...) do { __VA_ARGS__; } while(0)

#define __str(...) __str_(__VA_ARGS__)
#define __str_(...) #__VA_ARGS__
#define  __cat2(A, B) __cat2_(A, B)
#define __cat2_(A, B) A##B
#define __cat3(A, B, C) __cat3_(A, B, C)
#define __cat3_(A, B, C) A##B##C
#define __cat4(A, B, C, D) __cat4_(A, B, C, D)
#define __cat4_(A, B, C, D) A##B##C##D
#if 1
//#define __qp(P, X) __cat4(P, ꜒, X, ˥)
//#define __qp(P, X) __cat4(P, ꜖, X, ˩)
//#define __qp(P, X) __cat4(P, ʿ, X, ʾ)
//#define __qp(P, X) __cat4(P, ʻ, X, ʼ)
#define __qp(P, X) __cat4_(P, ꜀, X, ꜆)
#else
#define __qp(P, X) __cat4(P, q, X, p)
#endif

//================================================
// Type bundle getters
//================================================

// TODO: what to do with S exactly?
// TODO: naming is hard, do something about sealed/derived, consider monomorph?
#define   __pretty(P, S, M, T, V) P
#define   __mangle(P, S, M, T, V) M
#define     __type(P, S, M, T, V) __split T
#define    __type_(P, S, M, T, V) T
#define  __visitor(P, S, M, T, V) __split V
#define __visitor_(P, S, M, T, V) V

//================================================
// User API
//================================================

#define auto(V, x) typeof(x) V = x
#define ty(T) __type T
#define _pretty(T) __pretty T
#define _mangle(T) __mangle T
#define _mangle_str(T) __str(__mangle T)
#define _declare(T) typedef __type T __mangle T
#define _visit(T, x, ...) __stmt(__cat2(__visitor_init_, VISITOR)(__type T, __VA_ARGS__); __follow(T, x))
#define _call(T, V, ...) __visit_derived(__mangle T, V)(__VA_ARGS__)
// #define _sealed
// #define _struct
// #define _field
// #define _rec
// #define _enum
// #define _namespace
// #define _variant

//================================================
// Visitor helpers
//================================================

#undef VISITOR
#define __visit(T) __cat3(T, _, VISITOR)
#define __visit_derived(T, V) __cat4(__derived_, T, _, V)
#define __derive(T) __derive_ T
#define __derive_(P, S, M, T, V)\
    __visitor_of(ret, __split T) __visit_derived(M, VISITOR)(__visitor_of(args, __split T));\
    __visitor_of(ret, __split T) __visit_derived(M, VISITOR)(__visitor_of(args, __split T)) { __split V; }

#define __visitor_of(E, T) __visitor_of_(E, VISITOR, T)
#define __visitor_of_(E, V, T) __visitor_##E(V, T)
#define __visitor_sig(V, T) __visitor_sig_##V(T)
#define __visitor_ret(V, T) __apply(__head, __visitor_sig_##V(T))
#define __visitor_args(V, T) __apply(__tail, __visitor_sig_##V(T))
#define __visitor_names(V, T) __visitor_names_##V()
#define __visitor_ret_name(V, T) __apply(__head, __visitor_names_##V())
#define __visitor_arg_names(V, T) __apply(__tail, __visitor_names_##V())
#define __visitor_typeof(V, T) typeof(__visitor_ret(V, T)(*)(__visitor_args(V, T)))

#define __shadow(T, var, x) __shadow_with(T, var, __##var, x)
#define __shadow_with(T, var, __var, x) T __var = x; typeof(__var) var = __var
#define __follow(T, path) __shadow(__split __type_ T*, this, path); __visitor T

//================================================
// Type definition helpers
//================================================

// Common
#define _sealed(V) V,
#define _namespace(N) N,
#define _default_pretty(N, T, ...) #N "<" __pretty T DO(__default_pretty_item, ##__VA_ARGS__) ">",
#define _default_mangle(N, ...) __qp(N, CAT(DO(__default_mangle_item, ##__VA_ARGS__))),
#define _default_pretty_with_const(N, C, ...) #N "<" DO(__default_pretty_const) DO(__default_pretty_item, ##__VA_ARGS__) ">",
#define _default_mangle_with_const(N, C, ...) __qp(N, CAT(DO(__default_mangle_const, __split C) DO(__default_mangle_item, ##__VA_ARGS__))),
#define __default_pretty_item(i, T) ", " __pretty T
#define __default_mangle_item(i, T) ,__mangle T
#define __default_pretty_const(i, C) "," #C
#define __default_mangle_const(i, C) C##_,
#define _rec(T, M, ...) __rec(T, M)
#define __rec(T, M, ...) ("...", true, M, (T M*),\
        (__visit(Ptr)(if (*this) { typeof(*this) __this = *this; typeof(__this) this = __this;  __visit_derived(M, VISITOR)(__visitor_of(arg_names, void)); } else { __visit(Nil)(); })))

// Struct
#define __inline_struct(N, F) N, (struct N __split F)
#define __struct(N, F) N, (struct N { F }), (__visit(N)(__visitor_of(arg_names, void)))
#define _struct(...) __struct(__VA_ARGS__)
#define _field(N, T) __type T N;

// Tagged Enum
#define _enum(...) __enum(__VA_ARGS__)
#define __enum(B, M, _, ...) M, (struct M {\
        typeof(enum B##_Tag { DO_WITH(__enum_tag, B, __VA_ARGS__) }) tag;\
        union __cat2(M, _as) { DO(__enum_variant, __VA_ARGS__) } as; }),\
        (__visit(Enum)(M, if(false){} DO(__enum_visit, __VA_ARGS__) else { __invalid_enum_tag(__FILE__, __LINE__, #B, #M, this->tag); }))
#define _variant(N, T) ,__variant(N, T)
#define __enum_tag(i, V, B) __cat2(B##_Tag_, __cat2(__name, V)),
#define __enum_variant(i, V) __def##V
#define __enum_visit(i, T) else if (this->tag == i) { __cat2(__visit, T); }
#define  __name__variant(N, T) N
#define   __def__variant(N, T) __type T N;
#define __visit__variant(N, T) __visit(Enum_variant)(N, __follow(T, &this->as.N))

//================================================
// Types and Visitor definition
//================================================

// Intrinsics
typedef char     _chr;
typedef int8_t   _i8;
typedef int16_t  _i16;
typedef int32_t  _i32;
typedef int64_t  _i64;
typedef uint8_t  _u8;
typedef uint16_t _u16;
typedef uint32_t _u32;
typedef uint64_t _u64;
typedef struct __qp(,){} _unit;

#define chr  ("char", false, c,   (char),     (__visit(chr)()))
#define i8   ("i8",   false, i8,  (int8_t),   (__visit(i8)()))
#define i16  ("i16",  false, i16, (int16_t),  (__visit(i16)()))
#define i32  ("i32",  false, i32, (int32_t),  (__visit(i32)()))
#define i64  ("i64",  false, i64, (int64_t),  (__visit(i64)()))
#define u8   ("u8",   false, u8,  (uint8_t),  (__visit(u8)()))
#define u16  ("u16",  false, u16, (uint16_t), (__visit(u16)()))
#define u32  ("u32",  false, u32, (uint32_t), (__visit(u32)()))
#define u64  ("u64",  false, u64, (uint64_t), (__visit(u64)()))
#define unit ("()",   false, _unit, (_unit),  (__visit(unit)()))

// Wrapper around a C type
#define Raw(T)

// Pointer
#define Ptr(T) ("&" __pretty T, false, __qp(p, __mangle T),\
        (typeof(__type T*)),\
        (__visit(Ptr)(if (*this) { __follow(T, *this); } else { __visit(Nil)(); })))

// C Array
#define Arr(T, N) ("[" __pretty T "; " #N "]", false, __qp(a##N, __mangle T),\
        (typeof(__type T[N])),\
        (__visit(Arr)(N, for (int i=0; i<N; i++) { __visit(Arr_item)(i, __follow(T, *this+i)); })))

// Slice
#define Slc(T) ("&[" __pretty T "]", false, __inline_struct(__qp(s, __mangle T),\
        ({ int len; __type T *data; })),\
        (__visit(Slc)(for (int i=0; i<this->len; i++) { __visit(Slc_item)(i, __follow(T, this->data+i)); })))

// Vector
// TODO: struct mVecT { int cap; union { struct { int len; mT* data; }; struct mSlcT { int len; mT* data; } as_slice; } };
#define Vec(T) ("Vec<" __pretty T ">", false, __inline_struct(__qp(v, __mangle T),\
        ({ int len; int cap; __type T *data; })),\
        (__visit(Vec)(for (int i=0; i<this->len; i++) { __visit(Vec_item)(i, __follow(T, this->data+i)); })))

// Option
#define Opt(T) ("Opt<" __pretty T ">", false, __inline_struct(__qp(o, __mangle T),\
        ({ typeof(enum Opt_Tag { Opt_Tag_Some, Opt_Tag_None }) tag; union __cat2(__qp(o, __mangle T), _as) { __type T Some; _unit None; } as; })),\
        (__visit(Enum)(Opt, if (this->tag == Opt_Tag_Some) { __visit(Enum_variant)(Some, __follow(T, &this->as.Some)); } else { __visit(Enum_variant)(None, {}); })))

// Result
#define Res(T, E) ("Res<" __pretty T "," __pretty E ">", false, __inline_struct(__qp(r, __cat3(__mangle T, _, __mangle E)),\
        ({ typeof(enum Res_Tag { Res_Tag_Ok, Res_Tag_Err }) tag; union __cat2(__qp(r, __cat2(__mangle T, __mangle E)), _as) { __type T Ok; __type E Err; } as; })),\
        (__visit(Enum)(Res, if (this->tag == Res_Tag_Ok) { __visit(Enum_variant)(Ok, __follow(T, &this->as.Ok)); } else { __visit(Enum_variant)(Err, __follow(E, &this->as.Err)); })))

// Tuple
#define _t(...) ("(" DO(__tuple_pretty_item, __VA_ARGS__) ")", false, __inline_struct(__qp(,CAT(DO(__default_mangle_item, __VA_ARGS__))),\
        ({ DO(__tuple_def_item, __VA_ARGS__) })),\
        (__visit(Tuple)(DO(__tuple_visit_item, __VA_ARGS__))))
        #define __tuple_pretty_item(i, T) __pretty T ","
        #define    __tuple_def_item(i, T) ty(T) __cat2(_, i);
        #define  __tuple_visit_item(i, T) { __visit(Tuple_item)(i, __follow(T, &this->__cat2(_, i))); }

// Dynamic Trait (V-Table)
#define Dyn(...) ("Dyn<" __str_(__VA_ARGS__) ">", false, __inline_struct(__qp(d, CAT(DO(__dyn_mangle_item, __VA_ARGS__))),\
        ({ void* data; DO(__dyn_def_item, __VA_ARGS__) })),\
        (this->__cat2(__, VISITOR)(__visitor_of(arg_names, void))))
        #define __dyn_mangle_item(i, V) _##V,
        #define    __dyn_def_item(i, V) __visitor_of_(typeof, V, void) V;
        #define   __dyn_fill_item(i, V, M) .V = (void*)__visit_derived(M, V),
        #define   __dyn_down_item(i, V, x) .V = x.V,

// Lambda/Fat-Pointer
#define Lmd ("???", false, __inline_struct(_l, ({ void* data; void* method; })), (__visit(Lmd)()))

//================================================
// Types methods
//================================================

#define __type_check(T, x) (struct{ __type T __type_checked; }){ .__type_checked = (x) }.__type_checked

#define Vec_new(T) (ty(Vec(T))){ .len = 0, .cap = 0, .data = 0 }
#define Vec_slice(T, x, a, b) (ty(Slc(T))){ .len = (b-a), .data = (x).data+a }
#define Slc_slice(x, a, b) (typeof(x)){ .len = (b-a), .data = (x).data+a }

#define Opt_Some(T, x) (ty(Opt(T))){ .tag = Opt_Tag_Some, .as = { .Some = (x) } }
#define Opt_None(T, x) (ty(Opt(T))){ .tag = Opt_Tag_None }

#define  Res_Ok(T, E, x) (ty(Res(T, E))){ .tag = Res_Tag_Ok,  .as = { .Ok = (x) } }
#define Res_Err(T, E, x) (ty(Res(T, E))){ .tag = Res_Tag_Err, .as = { .Err = (x) } }

#define Dyn_new(T, V, x) (ty(Dyn V)){ .data = __type_check(Ptr(T), x), DO_WITH(__dyn_fill_item, __mangle T, __split V) };
#define Dyn_down(V, x) (ty(Dyn V)){ .data = (x).data, DO_WITH(__dyn_down_item, (x), __split V) }
#define Dyn_into(V, x) (ty(Lmd)){ .data = (x).data, .method = (x).V }
#define Dyn_call(V, x, ...) (x).V((x).data, ##__VA_ARGS__)

#define Lmd_new(T, V, x) (ty(Lmd)){ .data = (x), .method = __visit_derived(__mangle T, V) }
#define Lmd_call(V, x, ...) ((__visitor_of_(typeof, V, void))(x).method)((x).data, ##__VA_ARGS__)
#define __derived__l_debug(x, depth) Lmd_call(debug, x, depth)
#define __derived__l_clone(x) Lmd_call(debug, x)
#define __derived__l_eq(x, other) Lmd_call(debug, x, other)

//================================================
// Traits definition
//================================================

// Trait debug
#define __visitor_sig_debug(T) void, T *this, int depth
#define __visitor_names_debug() , this, depth
#define __visitor_init_debug(T, d) int depth = d;

// Trait clone
#define __visitor_sig_clone(T) T, T *this
#define __visitor_names_clone() res, this

// Trait eq
#define __visitor_sig_eq(T) bool, T *this, T *other
#define __visitor_names_eq() res, this, other

//================================================
// Traits Implementation
//================================================

// No trait short-circuit
#define Ptr_VISITOR(...)
#define Arr_VISITOR(...)
#define Slc_VISITOR(...)
#define Vec_VISITOR(...)
#define Tuple_VISITOR(...)
#define Struct_VISITOR(...)
#define Enum_VISITOR(...)

// Trait debug
#define unit_debug() printf("()")
#define chr_debug( ) printf("%c", *this)
#define  i8_debug( ) printf("%d", *this)
#define i16_debug( ) printf("%d", *this)
#define i32_debug( ) printf("%d", *this)
#define i64_debug( ) printf("%d", *this)
#define  u8_debug( ) printf("%u", *this)
#define u16_debug( ) printf("%u", *this)
#define u32_debug( ) printf("%u", *this)
#define u64_debug( ) printf("%u", *this)
#define Nil_debug( ) printf("(nil)")
#define Ptr_debug(V) if (*this) { printf("Ptr("); V; printf(")"); } else { Nil_debug(); }

#define Iter_debug(V) printf("[\n"); depth++; V; depth--; printf("%*.s]", depth*4, "")
#define Iter_item_debug(i, V) printf("%*.s", depth*4, ""); V; printf(",\n")
#define Arr_debug(N, V) printf("(" #N ")"); Iter_debug(V)
#define Arr_item_debug(i, V) Iter_item_debug(i, V)
#define Slc_debug(V) Iter_debug(V)
#define Slc_item_debug(i, V) Iter_item_debug(i, V)
#define Vec_debug(V) Iter_debug(V)
#define Vec_item_debug(i, V) Iter_item_debug(i, V)
#define Tuple_debug(V) printf("( "); V; printf(")")
#define Tuple_item_debug(i, V) Iter_item_debug(i, V)

#define Struct_debug(M, V) printf(#M " {\n"); depth++; V; depth--; printf("%*.s}", depth*4, "")
#define Struct_field_debug(N, V) printf("%*.s" #N ": ", depth*4, ""); V; printf(",\n")
#define Enum_debug(M, V) printf(#M "::"); V;
#define Enum_variant_debug(N, V) printf(#N "("); V; printf(")")

// trait clone
#define unit_clone() ret = this
#define i32_clone( ) ret = this
#define Nil_clone( ) ret = NULL
#define Ptr_clone(V) ret = this
#define Vec_clone(V) //ret.len = this->len; ret.cap = this->len; ret.data = malloc(sizeof(*ret.data)*ret.len); V
#define Vec_item_clone(i, V) typeof(ret) _ret = ret; typeof(*ret.data) ret; V; _ret.data[i] = ret;
#define Tuple_clone(V)
#define Tuple_item_clone(i, V)

// trait eq
#define unit_eq() ret = true
#define i32_eq( ) ret = *this == *other
#define Nil_eq( ) ret = true
#define Ptr_eq(V) V
#define Arr_eq(N, V) V
#define Arr_item_eq(i, V) V
#define Vec_eq(V) //ret = true; if (this->len == other->len) { V; } else { ret = false; }
#define Vec_item_eq(i, V) V
#define Tuple_eq(V) ret = true; V
#define Tuple_item_eq(i, V) V


#endif
