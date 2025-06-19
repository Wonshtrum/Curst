#define ELEMENT_0(T, ...) typeof(&(*T) __VA_ARGS__)
#define ELEMENT_1(T, ...) typeof(T __VA_ARGS__)
#define ELEMENT_2(T, ...) typeof(T __VA_ARGS__)
#define DO_0(T) T
#define DO_1(T) LEAF(T)
#define DO_2(T) T

#define VEC_LEN(T) (T ? 42 : 0)


#define FIELD(N, T) FIELD_(N, T, 1)
#define FIELD_(N, T, E, ...) { \
    ROOT(N) NL_COLON \
    ELEMENT_ ## E(this.N) _this = this.N NL_COLON \
    typeof(_this) this = _this NL_COLON \
    DO_ ## E(T); }


#define PTR(T) PTR_(T, 1), 0
#define PTR_(T, E, ...) if (this) { \
    ELEMENT_ ## E(*this) _this = *this; typeof(_this) this = _this NL_COLON \
    DO_ ## E(T); } else { NL_COLON \
    LEAF(null); }

#define ARR(T, N) ARR_(N, T, 1), 0
#define ARR_(N, T, E, ...) for (int i=0; i<N; i++) { NL_COLON \
    ELEMENT_ ## E(this, [0]) _this = this[i] NL_COLON \
    typeof(_this) this = _this NL_COLON \
    DO_ ## E(T); }

#define VEC(T) VEC_(T, 1), 0
#define VEC_(T, E, ...) for (int i=0; i<VEC_LEN(this); i++) { NL_COLON \
    ELEMENT_ ## E(this, [0]) _this = this[i] NL_COLON \
    typeof(_this) this = _this NL_COLON \
    DO_ ## E(T); }

#define T2(T0, T1) TUP_(_0, T0, 1) TUP_(_1, T1, 1), 2
#define T3(T0, T1, T2) TUP_(_0, T0, 1) TUP_(_1, T1, 1) TUP_(_2, T2, 1), 2
#define T4(T0, T1, T2, T3) TUP_(_0, T0, 1) TUP_(_1, T1, 1) TUP_(_2, T2, 1) TUP_(_3, T3, 1), 2
#define TUP_(F, T, E, ...) { \
    ELEMENT_ ## E(this.F) _this = this.F NL_COLON \
    typeof(_this) this = _this NL_COLON \
    DO_ ## E(T); }


CLASS

#undef NAME
#undef FIELD
#undef PTR
#undef ARR
#undef VEC

#undef T2
#undef T3
#undef T4
