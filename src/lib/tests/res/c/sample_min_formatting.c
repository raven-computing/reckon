/* Golden sample: Contains C language features and constructs */

/**
 * block comment
 */

// line comment

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stdalign.h>
#include <stdatomic.h>

#if defined(__STDC__)
#  if defined(__STDC_VERSION__)
#    if (__STDC_VERSION__ >= 201112L)
#      define SAMPLE_C_C11 1
#    else
#      define SAMPLE_C_C11 0
#    endif
#  else
#    define SAMPLE_C_C11 0
#  endif
#else
#  define SAMPLE_C_C11 0
#endif

/* Pragmas */
#pragma STDC FENV_ACCESS OFF

/* Preprocessor macros */
#define SAMPLE_VERSION 1
#define STR(x) #x
#define XSTR(x) STR(x)
#define CONCAT(a, b) a##b

#define TYPE_NAME(x) _Generic((x), int: "int", long: "long", unsigned: "unsigned", float: "float", double: "double", const char*: "const char*", default: "other")

#define DO_ONCE(stmt) do { stmt; } while (0)

#define TRACEF(fmt, ...) do { fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); } while (0)

#define UNUSED(x) ((void)(x))

/* Conditional compilation blocks */
#if 0
#error "disabled"
#endif

/* Types: enum, struct, union, typedef, bit-fields, arrays */
typedef uint32_t u32;

typedef int (*BinaryOp)(int, int);

enum Color { COLOR_RED = 1, COLOR_GREEN, COLOR_BLUE = 4 };

struct Bits { unsigned a : 1; unsigned b : 2; unsigned c : 3; };

union IntFloat { int i; float f; unsigned char bytes[sizeof(int)]; };

struct Pair { int x; int y; };

typedef struct Flex { size_t n; double data[]; } Flex;

/* Alignment */
struct Aligned16 { alignas(16) unsigned char bytes[16]; };

/* Assertions */
_Static_assert(sizeof(u32) == 4 || sizeof(u32) == 8, "unexpected u32 size");
static_assert(alignof(struct Aligned16) >= 1, "alignof sanity");

/* Storage class + qualifiers */
static _Atomic int gCounter = 0;
static volatile int gVolatileSink = 0;
extern int gExternDecl;

/* Function prototypes */
static int add(int a, int b);
static inline int mul(int a, int b) { return a * b; }
_Noreturn static void die(const char *msg);
static int sumInts(size_t n, ...);
static void scale(size_t n, double *restrict dst, const double *restrict src, double k);

/* Designated initializers, compound literals */
static const struct Pair kPair = { .x = 1, .y = 2 };

static int add(int a, int b) { return a + b; }

_Noreturn static void die(const char *msg) {
    fputs(msg, stderr); fputc('\n', stderr); abort();
}

static int sumInts(size_t n, ...) {
    va_list ap;
    va_start(ap, n);
    int acc = 0;
    for (size_t i = 0; i < n; ++i) { acc += va_arg(ap, int); }
    va_end(ap);
    return acc;
}

static void scale(size_t n, double *restrict dst, const double *restrict src, double k) {
    for (size_t i = 0; i < n; i++) dst[i] = src[i] * k;
}

/* Example K&R-style */
static int oldstyle(a, b)
int a; int b;
{
    return a - b;
}

/* Function pointer usage */
static int apply(BinaryOp op, int a, int b) { return op ? op(a, b) : 0; }

int main(void) {
    /* Literals */
    int dec = 42;
    int hex = 0x2A;
    int oct = 052;
    unsigned long long ull = 18446744073709551615ULL;
    double fp = 3.14e0;
    float ff = 1.0f;
    char ch = 'A';
    const char* s = "hello\t" "world\n";
    const wchar_t* ws = L"wide";

    UNUSED(ws);

    /* Declarations */
    int arr[3] = { 1, 2, 3 };
    int* p = arr;
    int (*parr)[3] = &arr;
    const char* strings[] = { "a", "b", "c", NULL };

    int n = 5;
    int vla[n];
    for (int i = 0; i < n; ++i) { vla[i] = i; }

    /* Operators */
    size_t sz_i = sizeof(int);
    size_t al_i = _Alignof(int);
    int cond = ( (dec > 0) ? dec : -dec );
    int comma = (dec += 1, dec += 1, dec);
    int bits = (hex & 0xf) | ((oct << 1) ^ 0x3);
    int sh = (1 << 3) >> 1;

    UNUSED(sz_i);
    UNUSED(al_i);
    UNUSED(cond);
    UNUSED(comma);
    UNUSED(bits);
    UNUSED(sh);

    /* Struct/union/enum usage + designated init + compound literal */
    enum Color color = COLOR_GREEN;
    struct Bits bf = { .a = 1u, .b = 2u, .c = 5u };
    union IntFloat u = { .i = 0x3f800000 };
    struct Pair local = (struct Pair){ .x = arr[0], .y = arr[1] };

    UNUSED(bf);
    UNUSED(u);
    UNUSED(local);

    /* Flexible array allocation */
    Flex *fx = (Flex *) malloc(sizeof(*fx) + 3 * sizeof(double));
    if (!fx) die("malloc failed");

    fx->n = 3;
    fx->data[0] = 1.0;
    fx->data[1] = 2.0;
    fx->data[2] = 3.0;

    /* Atomic ops */
    atomic_fetch_add(&gCounter, 1);

    /* Volatile read/write */
    gVolatileSink = gVolatileSink + 1;

    /* _Generic usage */
    TRACEF("TYPE_NAME(dec)=%s, TYPE_NAME(fp)=%s, version=%d", TYPE_NAME(dec), TYPE_NAME(fp), SAMPLE_VERSION);

    /* Control flow */
    if (color == COLOR_RED) { puts("red");
    } else if (color == COLOR_GREEN) { puts("green");
    } else { puts("blue/other");
    }

    for (int i = 0; i < 3; ++i) {
        if (i == 1) { continue; }
        if (i == 2) { break; }
        printf("i=%d\n", i);
    }

    int w = 0;
    while (w < 2) {++w;}

    int d = 0;
    do {d++;} while (d < 2);

    switch (arr[0]) {
        case 0:
            puts("zero");
            break;
        case 1:
            puts("one");
            /* fallthrough */
        default:
            puts("default");
            break;
    }

    goto done;
intermediate:
    puts("unreachable in this run");
done:
    if (0) { goto intermediate; }

    /* Function pointers */
    BinaryOp op = &add;
    int r1 = apply(op, 2, 3);
    int r2 = mul(4, 5);
    int r3 = sumInts(4, 1, 2, 3, 4);
    int r4 = oldstyle(9, 2);

    /* Pointer arithmetic + array access via pointer */
    int sum = 0;
    for (int i = 0; i < 3; i++) { sum += *(p + i); }
    printf("r1=%d r2=%d r3=%d r4=%d sum=%d bf.a=%u s=%s ch=%c ff=%.1f ull=%llu parr0=%d\n", r1, r2, r3, r4, sum, bf.a, s, ch, ff, ull, (*parr)[0]);

    /* Strings array iteration */
    for (size_t i = 0; strings[i] != NULL; ++i) { puts(strings[i]); }

    struct Pair copy = {0};
    memcpy(&copy, &kPair, sizeof(copy));
    memset(&copy, 0, sizeof(copy));
    free(fx);

    /* Assertions */
    assert(arr[0] == 1);

    return 0;
}

/* Definition for the extern declaration */
int gExternDecl = 123;
