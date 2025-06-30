#include <stdarg.h>
int vsnprintf(char*, size_t, const char*, va_list);
void *memcpy(void*, const void*, size_t);

//================================================
// Traits definition
//================================================

// write: write a 0 terminated string
#define __visitor_sig_write(T) int, T *this, const char* fmt
#define __visitor_names_write() res, this, fmt

// writef: interpolate all valid printf formatters
#define __visitor_sig_writef(T) int, T *this, const char *fmt, va_list args
#define __visitor_names_writef() res, this, fmt, args

// fmt: write the representation of an instance into the given Formatter
#define __visitor_sig_fmt(T) int, T *this, Formatter f
#define __visitor_names_fmt() res, this, f, fmt
#define __visitor_init_fmt(T, _f, _fmt) int res = 0; Formatter f = (_f); const char* fmt = (_fmt)

//================================================
// Formatter definition and implementations
//================================================

typedef struct Formatter {
    void *data;
    int (*writef)(void*, const char*, va_list);
    int (*write)(void*, const char*);
    int flags;
    int depth;
} Formatter;

// Create Formatter from anything that implements write and writef
#define Formatter_new(T, x) (Formatter) {\
    .data = __type_check(Ptr(T), x),\
    .flags = 0, .depth = 0,\
    __dyn_fill_item(_, write, __mangle T)\
    __dyn_fill_item(_, writef, __mangle T) }

int _write(Formatter f, const char *fmt) {
    return Dyn_call(write, f, fmt);
}
int _writef(Formatter f, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = Dyn_call(writef, f, fmt, args);
    va_end(args);
    return n;
}

int _write_vfmt(Formatter f, const char *fmt, va_list args) {
    int len = strlen(fmt);
    char *p = malloc(len);
    memcpy(p, fmt, len);
    fmt = p;
    int n = 0;
    while (*p) {
        if (p[0] == '%' && p[1] == '%') {
            p += 2;
            continue;
        }
        if (p[0] == '%' && p[1] == '?') {
            *p = 0;
            n += Dyn_call(writef, f, fmt, args);
            fmt = ++p + 1;
            auto(l, va_arg(args, ty(Lmd)));
            n += Lmd_call(fmt, l, f);
        }
        p++;
    }
    n += Dyn_call(writef, f, fmt, args);
    return n;
}
// like _writef but interpolates %? with the given fmt implementation
int _write_fmt(Formatter f, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = _write_vfmt(f, fmt, args);
    va_end(args);
    return n;
}

//================================================
// fmt implementations
//================================================

#define unit_fmt() _write(f, "()")
#define chr_fmt( ) _writef(f, "%c", *this)
#define  i8_fmt( ) _writef(f, "%d", *this)
#define i16_fmt( ) _writef(f, "%d", *this)
#define i32_fmt( ) _writef(f, "%d", *this)
#define i64_fmt( ) _writef(f, "%d", *this)
#define  u8_fmt( ) _writef(f, "%u", *this)
#define u16_fmt( ) _writef(f, "%u", *this)
#define u32_fmt( ) _writef(f, "%u", *this)
#define u64_fmt( ) _writef(f, "%u", *this)
#define Nil_fmt( ) _write(f, "(nil)")
#define Ptr_fmt(V) V

//#define Iter_fmt(V) _write(f, "[\n"); f.depth++; V; f.depth--; _writef(f, "%*.s]", f.depth*4, "")
//#define Iter_item_fmt(i, V) _write(f, "%*.s", f.depth*4, ""); V; _write(f, ",\n")
#define Iter_fmt(V) _write(f, "[ "); f.depth++; V; f.depth--; _write(f, "]")
#define Iter_item_fmt(i, V) V; _write(f, ", ")
#define Arr_fmt(N, V) _write(f, "(" #N ")"); Iter_fmt(V)
#define Arr_item_fmt(i, V) Iter_item_fmt(i, V)
#define Slc_fmt(V) Iter_fmt(V)
#define Slc_item_fmt(i, V) Iter_item_fmt(i, V)
#define Vec_fmt(V) Iter_fmt(V)
#define Vec_item_fmt(i, V) Iter_item_fmt(i, V)
#define Str_fmt() _writef(f, "%.*s", this->len, this->ptr)
#define String_fmt() Str_fmt()
#define Tuple_fmt(V) _write(f, "( "); V; _write(f, ")")
#define Tuple_item_fmt(i, V) Iter_item_fmt(i, V)

#define Struct_fmt(M, V) _write(f, #M " {\n"); f.depth++; V; f.depth--; _writef(f, "%*.s}", f.depth*4, "")
#define Struct_field_fmt(N, V) _writef(f, "%*.s" #N ": ", f.depth*4, ""); V; _write(f, ",\n")
#define Enum_fmt(M, V) _write(f, #M "::"); V;
#define Enum_variant_fmt(N, V) _write(f, #N "("); V; _write(f, ")")

//================================================
// write/writef implementations
//================================================
// TODO: refactor trait defintions and implementations:
// - custom implementation for monomorphed type should be easier
// - monomorphic macros should be called like monomorphic functions

// String
int __visit_derived(__mangle String, write)(ty(String) *this, const char *fmt) {
    ty(Str) str = Str_newz(fmt);
    Vec_extend(*this, str);
    return str.len;
}
int __visit_derived(__mangle String, writef)(ty(String) *this, const char *fmt, va_list args) {
    va_list o_args;
    va_copy(o_args, args);
    int n = vsnprintf(NULL, 0, fmt, args);
    Vec_reserve(*this, this->len + n);
    n = vsnprintf(this->ptr + this->len, this->cap - this->len, fmt, o_args);
    this->len += n;
    va_end(o_args);
    return n;
}

// File
int __visit_derived(__mangle File, write)(ty(File) *this, const char *fmt) {
    return fprintf(this, fmt);
}
int __visit_derived(__mangle File, writef)(ty(File) *this, const char *fmt, va_list args) {
    return vfprintf(this, fmt, args);
}

//================================================
// User API
//================================================

// String
int String_write_vfmt(ty(String) *this, bool nl, const char *fmt, va_list args) {
    Formatter f = Formatter_new(String, this);
    int n = _write_vfmt(f, fmt, args);
    if (nl) {
        n += Dyn_call(write, f, "\n");
    }
    return n;
}
int String_writeln_fmt(ty(String) *this, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = String_write_vfmt(this, true, fmt, args);
    va_end(args);
    return n;
}
int String_write_fmt(ty(String) *this, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = String_write_vfmt(this, false, fmt, args);
    va_end(args);
    return n;
}

// File
int vfprint(FILE *this, bool nl, const char *fmt, va_list args) {
    Formatter f = Formatter_new(File, this);
    int n = _write_vfmt(f, fmt, args);
    if (nl) {
        n += fprintf(this, "\n");
    }
    return n;
}
int println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vfprint(stdout, true, fmt, args);
    va_end(args);
    return n;
}
int print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprint(stdout, false, fmt, args);
    va_end(args);
}
int eprintln(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprint(stderr, true, fmt, args);
    va_end(args);
}
int eprint(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprint(stderr, false, fmt, args);
    va_end(args);
}
int fprintln(FILE *this, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprint(this, true, fmt, args);
    va_end(args);
}
int fprint(FILE *this, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprint(this, false, fmt, args);
    va_end(args);
}
