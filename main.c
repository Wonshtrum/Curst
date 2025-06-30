#include <stdlib.h>
#include "curst.h"
#include "fmt.h"

#define Test(A, B) (\
        _default_pretty(Test, A, B)\
        _sealed(true)\
        _struct(\
            _default_mangle(Test, A, B)\
            _field(a, A)\
            _field(b, B)))

#define Tree(A, T) (\
        _default_pretty_with_const(Tree, (A), T)\
        _sealed(true)\
        _struct(\
            _default_mangle_with_const(Tree, (A), T)\
            _field(val, T)\
            _field(children, Arr(_rec(struct, _default_mangle_with_const(Tree, (A), T)), A))))
#define Tree_new(A, T, x, left, right) ({\
        typedef ty(Tree(A, T)) tree_t;\
        tree_t* this = malloc(sizeof(tree_t));\
        this->val = (x);\
        this->children[0] = left;\
        this->children[1] = right;\
        this;})

#define Option(T) (\
        _default_pretty(Option, T)\
        _sealed(false)\
        _enum(\
            _namespace(Option)\
            _default_mangle(_o, T)\
            _variant(Some, T)\
            _variant(None, unit)))

#define Result(T, E) (\
        _default_pretty(Result, T)\
        _sealed(false)\
        _enum(\
            _namespace(Result)\
            _default_mangle(_r, T, E)\
            _variant(Ok, T)\
            _variant(Err, E)))

#define DoubleOption(T) (\
        _default_pretty(DoubleOption, T)\
        _sealed(false)\
        _enum(\
            _namespace(DoubleOption)\
            _default_mangle(_2o, T)\
            _variant(Some, (\
                    _default_pretty(DoubleOptionInner, T)\
                    _sealed(false)\
                    _enum(\
                        _namespace(DoubleOptionInner)\
                        _default_mangle(_2oi, T)\
                        _variant(Some, T)\
                        _variant(None, unit))))\
            _variant(None, unit)))

#define MANY_VISITED_FOR debug
#define VISITED Test(i32, i32)
#include "derive.h"
#define VISITED Test(i16, i8)
#include "derive.h"
#define VISITED DoubleOption(i16)
#include "derive.h"
#define VISITED Tree(2, i32)
#include "derive.h"
#undef MANY_VISITED_FOR

#define MANY_VISITOR_FOR Vec(Vec(i32))
#define VISITOR debug
#include "derive.h"
#define VISITOR fmt
#include "derive.h"
#define VISITOR clone
#include "derive.h"
#define VISITOR eq
#include "derive.h"
#undef MANY_VISITOR_FOR

_declare(Opt(i32));
_declare(Res(i32, i32));
_declare(Opt(i32));
_declare(Opt(Opt(i32)));

_declare(Option(i32));
_declare(Result(Opt(i32), Opt(i32)));
_declare(DoubleOption(i32));

// TODO: support Enum in arguments
// ty(Res(Opt(i32), i32)) swap(ty(Res(i32, Opt(i32))) arg) {
//     ty(Res(Opt(i32), i32)) res;
//     if (arg.tag == Res_Tag_Ok) {
//         res.tag = Res_Tag_Err;
//         res.as.Err = arg.as.Ok;
//     } else {
//         res.tag = Res_Tag_Ok;
//         res.as.Ok = arg.as.Err;
//     }
//     return res;
// }

ty(i32) main() {
    auto(hello, Str_new("hello"));
    ty(String) s = String_new();
    Vec_extend(s, hello);
    Vec_extend(s, Str_new(" world"));
    Vec_push(s, '!');
    Vec_push(s, 0);
    printf("String { len: %d, cap: %d, ptr: \"%s\" }\n", s.len, s.cap, s.ptr);

    auto(vvi, Vec_new(Vec(i32)));
    Vec_reserve(vvi, 1);
    printf("len: %d, cap: %d, ptr: %d\n", vvi.len, vvi.cap, vvi.ptr);
    Vec_reserve(vvi, 1);
    Vec_push(vvi, Vec_new(i32));
    printf("len: %d, cap: %d, ptr: %d\n", vvi.len, vvi.cap, vvi.ptr);
    auto(v, vvi.ptr[0]);
    printf("len: %d, cap: %d, ptr: %d\n", v.len, v.cap, v.ptr);
    Vec_push(v, 32);
    printf("len: %d, cap: %d, ptr: %d\n", v.len, v.cap, v.ptr);

    ty(Arr(i32, 4)) a4i = { 1, 2, 3, 4 };
    ty(Arr(Vec(i32), 2)) a2vi = {
        { .len = 3, .cap = 4, .ptr = a4i },
        { .len = 4, .cap = 4, .ptr = a4i },
    };
    vvi.len = 2;
    vvi.ptr = a2vi;
    ty(i32) i = 4;
    ty(Ptr(i32)) pi = &i;
    printf("\n%s: %s", _pretty(Ptr(Ptr(Ptr(i32)))), _mangle_str(Ptr(Ptr(Ptr(i32)))));
    printf("\n%s: %s", _pretty(Vec(Arr(Ptr(i32), 2))), _mangle_str(Vec(Arr(Ptr(i32), 2))));
    #define VISITOR debug
    pi = 0;
    printf("\n%s: ", _mangle_str(Ptr(i32)));
    _visit(Ptr(i32), &pi, 0);
    printf("\n%s: ", _mangle_str(Vec(i32)));
    _visit(Vec(i32), &vvi.ptr[0], 0);
    printf("\n%s: ", _mangle_str(Vec(Vec(i32))));
    _visit(Vec(Vec(i32)), &vvi, 0);
    printf("\n");

    ty(Test(i32, i32)) x = { .a = 42, .b = 1337 };
    _call(Test(i32, i32), debug, &x, 0);
    printf("\nTest as dyn debug: ");
    ty(Dyn(debug)) dy = Dyn_new(Test(i32, i32), (debug), &x);
    dy.debug(dy.data, 0);
    // ty(Dyn(debug)) dy2 = Dyn_downgrade(debug, clone); // fails because Dyn(debug) doesn't implement clone

    printf("\nVec as dyn debug+clone+eq: ");
    ty(Dyn(debug, clone, eq)) dy2 = Dyn_new(Vec(Vec(i32)), (debug, clone, eq), &vvi);
    dy2.debug(dy2.data, 0);

    printf("\nImplicit downgrade to debug: ");
    Dyn_call(debug, dy2, 0);

    printf("\nExplicit downgrade to debug: ");
    ty(Dyn(debug)) dy3 = Dyn_down((debug), dy2);
    dy3.debug(dy3.data, 0);

    printf("\nDerive call debug on fat pointer: ");
    auto(l1, Dyn_into(debug, dy2));
    _call(Lmd, debug, l1, 0);

    printf("\nDirect call debug on fat pointer: ");
    Lmd_call(debug, l1, 0);
    
    printf("\nVec as debug lambda: ");
    ty(Lmd) l2 = Lmd_new(Vec(Vec(i32)), debug, &vvi);
    Lmd_call(debug, l2, 0);

    printf("\nCall through formatter with Stdout sink: ");
    ty(Lmd) l3 = Lmd_new(Vec(Vec(i32)), fmt, &vvi);
    println("--%?--", l3);
    printf("Call through formatter with String sink: ");
    //String_clear(s);
    s.len--;
    auto(f, Formatter_new(String, &s));
    _write_fmt(f, "--%?--", l3);
    String_write_fmt(&s, "--%?--", l3);
    Vec_push(s, 0);
    printf("{ len: %d, cap: %d, ptr: %s }", s.len, s.cap, s.ptr);

    printf("\nCall through nested visitor: ");
    auto(res, Res_Ok(Vec(Vec(i32)), i32, vvi));
    _visit(Res(Vec(Vec(i32)), i32), &res, 0);

    printf("\nCall recursive Tree: ");
    ty(Tree(2, i32))* tree = Tree_new(2, i32, 0,
            Tree_new(2, i32, 1,
                NULL,
                NULL),
            Tree_new(2, i32, 2,
                NULL,
                Tree_new(2, i32, 3, NULL, NULL)));
    _call(Tree(2, i32), debug, tree, 0);
    printf("\n");

    ty(_t(_t(chr, i32, _t(i32, i16, i16), i32, DoubleOption(i16)), _t(chr))) tt = { { 'A', 10, { 2, 3, 4 }, 50, 60 }, 'B' };
    _visit(DoubleOption(i16), &tt._0._4, 0); // pancis on invalid variant
}
