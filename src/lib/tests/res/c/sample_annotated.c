/* Golden sample: Contains C language features and constructs */

/**
 * block comment
 */

// line comment

#include <stdio.h> // +1 (preproc include)
#include <stdlib.h> // +1 (preproc include)
#include <stdint.h> // +1 (preproc include)
#include <stdbool.h> // +1 (preproc include)
#include <stddef.h> // +1 (preproc include)
#include <stdarg.h> // +1 (preproc include)
#include <string.h> // +1 (preproc include)
#include <assert.h> // +1 (preproc include)
#include <stdalign.h> // +1 (preproc include)
#include <stdatomic.h> // +1 (preproc include)

#if defined(__STDC__) // +1 (preproc if)
#  if defined(__STDC_VERSION__) // +1 (preproc if)
#    if (__STDC_VERSION__ >= 201112L) // +1 (preproc if)
#      define SAMPLE_C_C11 1 // +1 (preproc def)
#    else // +1 (preproc else)
#      define SAMPLE_C_C11 0 // +1 (preproc def)
#    endif
#  else // +1 (preproc else)
#    define SAMPLE_C_C11 0 // +1 (preproc def)
#  endif
#else // +1 (preproc else)
#  define SAMPLE_C_C11 0 // +1 (preproc def)
#endif

/* Pragmas */
#pragma STDC FENV_ACCESS OFF // +1 (preproc directive)

/* Preprocessor macros */
#define SAMPLE_VERSION 1 // +1 (preproc def)
#define STR(x) #x // +1 (preproc function def)
#define XSTR(x) STR(x) // +1 (preproc function def)
#define CONCAT(a, b) a##b // +1 (preproc function def)

#define TYPE_NAME(x) _Generic((x), int: "int", long: "long", unsigned: "unsigned", float: "float", double: "double", const char*: "const char*", default: "other") // +1 (preproc function def)

#define DO_ONCE(stmt) do { stmt; } while (0) // +1 (preproc function def)

#define TRACEF(fmt, ...) do { fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); } while (0) // +1 (preproc function def)

#define UNUSED(x) ((void)(x)) // +1 (preproc function def)

/* Conditional compilation blocks */
#if 0 // +1 (preproc if)
#error "disabled" // +1 (preproc directive)
#endif

/* Types: enum, struct, union, typedef, bit-fields, arrays */
typedef uint32_t u32; // +1 (type definition)

typedef int (*BinaryOp)(int, int); // +1 (type definition)

enum Color { // +1 (enum specifier)
    COLOR_RED = 1, // +1 (enumerator)
    COLOR_GREEN, // +1 (enumerator)
    COLOR_BLUE = 4 // +1 (enumerator)
};

struct Bits { // +1 (struct specifier)
    unsigned a : 1; // +1 (field declaration)
    unsigned b : 2; // +1 (field declaration)
    unsigned c : 3; // +1 (field declaration)
};

union IntFloat { // +1 (union specifier)
    int i; // +1 (field declaration)
    float f; // +1 (field declaration)
    unsigned char bytes[sizeof(int)]; // +1 (field declaration)
};

struct Pair { // +1 (struct specifier)
    int x; // +1 (field declaration)
    int y; // +1 (field declaration)
};

typedef struct Flex { // +1 (type definition)
    size_t n; // +1 (field declaration)
    double data[]; // +1 (field declaration)
} Flex;

/* Alignment */
struct Aligned16 { // +1 (struct specifier)
    alignas(16) unsigned char bytes[16]; // +1 (field declaration)
};

/* Assertions */
_Static_assert( // +1 (expression statement)
    sizeof(u32) == 4 || sizeof(u32) == 8,
    "unexpected u32 size"
);
static_assert(alignof(struct Aligned16) >= 1, "alignof sanity"); // +1 (expression statement)

/* Storage class + qualifiers */
static _Atomic int gCounter = 0; // +1 (declaration)
static volatile int gVolatileSink = 0; // +1 (declaration)
extern int gExternDecl; // +1 (declaration)

/* Function prototypes */
static int add(int a, int b); // +1 (declaration)
static inline int mul(int a, int b) { // +1 (function definition)
    return a * b; // +1 (return statement)
}
_Noreturn static void die(const char *msg); // +1 (declaration)
static int sumInts(size_t n, ...); // +1 (declaration)
static void scale( // +1 (declaration)
    size_t n,
    double *restrict dst,
    const double *restrict src,
    double k
);

/* Designated initializers, compound literals */
static const struct Pair kPair = { // +1 (declaration)
    .x = 1,
    .y = 2
};

static int add(int a, int b) { // +1 (function definition)
    return a + b; // +1 (return statement)
}

_Noreturn static void die(const char *msg) { // +1 (function definition)
    fputs(msg, stderr); // +1 (expression statement)
    fputc('\n', stderr); // +1 (expression statement)
    abort(); // +1 (expression statement)
}

static int sumInts(size_t n, ...) { // +1 (function definition)
    va_list ap; // +1 (declaration)
    va_start(ap, n); // +1 (expression statement)
    int acc = 0; // +1 (declaration)
    for (size_t i = 0; i < n; ++i) { // +1 (for statement)
        acc += va_arg(ap, int); // +1 (expression statement)
    }
    va_end(ap); // +1 (expression statement)
    return acc; // +1 (return statement)
}

static void scale( // +1 (function definition)
    size_t n,
    double *restrict dst,
    const double *restrict src,
    double k
) {
    for (size_t i = 0; i < n; i++) dst[i] = src[i] * k; // +2 (for statement, expression statement)
}

/* Example K&R-style */
static int oldstyle(a, b) // +1 (function definition)
int a; // +1 (declaration)
int b; // +1 (declaration)
{
    return a - b; // +1 (return statement)
}

/* Function pointer usage */
static int apply(BinaryOp op, int a, int b) { // +1 (function definition)
    return op ? op(a, b) : 0; // +1 (return statement)
}

int main(void) { // +1 (function definition)
    /* Literals */
    int dec = 42; // +1 (declaration)
    int hex = 0x2A; // +1 (declaration)
    int oct = 052; // +1 (declaration)
    unsigned long long ull = 18446744073709551615ULL; // +1 (declaration)
    double fp = 3.14e0; // +1 (declaration)
    float ff = 1.0f; // +1 (declaration)
    char ch = 'A'; // +1 (declaration)
    const char* s = "hello\t" "world\n"; // +1 (declaration)
    const wchar_t* ws = L"wide"; // +1 (declaration)

    UNUSED(ws); // +1 (expression statement)

    /* Declarations */
    int arr[3] = { 1, 2, 3 }; // +1 (declaration)
    int* p = arr; // +1 (declaration)
    int (*parr)[3] = &arr; // +1 (declaration)
    const char* strings[] = { // +1 (declaration)
        "a",
        "b",
        "c",
        NULL
    };

    int n = 5; // +1 (declaration)
    int vla[n]; // +1 (declaration)
    for (int i = 0; i < n; ++i) { // +1 (for statement)
        vla[i] = i; // +1 (expression statement)
    }

    /* Operators */
    size_t sz_i = sizeof(int); // +1 (declaration)
    size_t al_i = _Alignof(int); // +1 (declaration)
    int cond = ( // +1 (declaration)
        (dec > 0)
        ? dec
        : -dec
    );
    int comma = (dec += 1, dec += 1, dec); // +1 (declaration)
    int bits = (hex & 0xf) | ((oct << 1) ^ 0x3); // +1 (declaration)
    int sh = (1 << 3) >> 1; // +1 (declaration)

    UNUSED(sz_i); // +1 (expression statement)
    UNUSED(al_i); // +1 (expression statement)
    UNUSED(cond); // +1 (expression statement)
    UNUSED(comma); // +1 (expression statement)
    UNUSED(bits); // +1 (expression statement)
    UNUSED(sh); // +1 (expression statement)

    /* Struct/union/enum usage + designated init + compound literal */
    enum Color color = COLOR_GREEN; // +1 (declaration)
    struct Bits bf = { // +1 (declaration)
        .a = 1u,
        .b = 2u,
        .c = 5u
    };
    union IntFloat u = { // +1 (declaration)
        .i = 0x3f800000
    };
    struct Pair local = (struct Pair){ // +1 (declaration)
        .x = arr[0],
        .y = arr[1]
    };

    UNUSED(bf); // +1 (expression statement)
    UNUSED(u); // +1 (expression statement)
    UNUSED(local); // +1 (expression statement)

    /* Flexible array allocation */
    Flex *fx = (Flex *) malloc( // +1 (declaration)
        sizeof(*fx) + 3 * sizeof(double)
    );
    if (!fx) // +1 (if statement)
        die("malloc failed"); // +1 (expression statement)

    fx->n = 3; // +1 (expression statement)
    fx->data[0] = 1.0; // +1 (expression statement)
    fx->data[1] = 2.0; // +1 (expression statement)
    fx->data[2] = 3.0; // +1 (expression statement)

    /* Atomic ops */
    atomic_fetch_add(&gCounter, 1); // +1 (expression statement)

    /* Volatile read/write */
    gVolatileSink = gVolatileSink + 1; // +1 (expression statement)

    /* _Generic usage */
    TRACEF("TYPE_NAME(dec)=%s, TYPE_NAME(fp)=%s, version=%d", // +1 (expression statement)
           TYPE_NAME(dec), TYPE_NAME(fp), SAMPLE_VERSION);

    /* Control flow */
    if (color == COLOR_RED) { // +1 (if statement)
        puts("red"); // +1 (expression statement)
    } else if (color == COLOR_GREEN) { // +1 (else clause)
        puts("green"); // +1 (expression statement)
    } else { // +1 (else clause)
        puts("blue/other"); // +1 (expression statement)
    }

    for (int i = 0; i < 3; ++i) { // +1 (for statement)
        if (i == 1) { // +1 (if statement)
            continue; // +1 (continue statement)
        }
        if (i == 2) { // +1 (if statement)
            break; // +1 (break statement)
        }
        printf("i=%d\n", i); // +1 (expression statement)
    }

    int w = 0; // +1 (declaration)
    while (w < 2) { // +1 (while statement)
        ++w; // +1 (expression statement)
    }

    int d = 0; // +1 (declaration)
    do { // +2 (do statement)
        d++; // +1 (expression statement)
    } while (d < 2);

    switch (arr[0]) { // +1 (switch statement)
        case 0: // +1 (case statement)
            puts("zero"); // +1 (expression statement)
            break; // +1 (break statement)
        case 1: // +1 (case statement)
            puts("one"); // +1 (expression statement)
            /* fallthrough */
        default: // +1 (case statement)
            puts("default"); // +1 (expression statement)
            break; // +1 (break statement)
    }

    goto done; // +1 (goto statement)
intermediate: // +1 (labeled statement)
    puts("unreachable in this run"); // +1 (expression statement)
done: // +1 (labeled statement)
    if (0) { // +1 (if statement)
        goto intermediate; // +1 (goto statement)
    }

    /* Function pointers */
    BinaryOp op = &add; // +1 (declaration)
    int r1 = apply(op, 2, 3); // +1 (declaration)
    int r2 = mul(4, 5); // +1 (declaration)
    int r3 = sumInts(4, 1, 2, 3, 4); // +1 (declaration)
    int r4 = oldstyle(9, 2); // +1 (declaration)

    /* Pointer arithmetic + array access via pointer */
    int sum = 0; // +1 (declaration)
    for (int i = 0; i < 3; i++) { // +1 (for statement)
        sum += *(p + i); // +1 (expression statement)
    }
    printf( // +1 (expression statement)
        "r1=%d r2=%d r3=%d r4=%d sum=%d bf.a=%u s=%s ch=%c ff=%.1f ull=%llu parr0=%d\n",
        r1, r2, r3, r4, sum, bf.a, s, ch, ff, ull, (*parr)[0]
    );

    /* Strings array iteration */
    for (size_t i = 0; strings[i] != NULL; ++i) { // +1 (for statement)
        puts(strings[i]); // +1 (expression statement)
    }

    struct Pair copy = {0}; // +1 (declaration)
    memcpy(&copy, &kPair, sizeof(copy)); // +1 (expression statement)
    memset(&copy, 0, sizeof(copy)); // +1 (expression statement)
    free(fx); // +1 (expression statement)

    /* Assertions */
    assert(arr[0] == 1); // +1 (expression statement)

    return 0; // +1 (return statement)
}

/* Definition for the extern declaration */
int gExternDecl = 123; // +1 (declaration)
