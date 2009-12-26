/* S7, a Scheme interpreter
 *
 *    derived from:
 */

/* T I N Y S C H E M E    1 . 3 9
 *   Dimitrios Souflis (dsouflis@acm.org)
 *   Based on MiniScheme (original credits follow)
 * (MINISCM)               coded by Atsushi Moriwaki (11/5/1989)
 * (MINISCM)           E-MAIL :  moriwaki@kurims.kurims.kyoto-u.ac.jp
 * (MINISCM) This version has been modified by R.C. Secrist.
 * (MINISCM)
 * (MINISCM) Mini-Scheme is now maintained by Akira KIDA.
 * (MINISCM)
 * (MINISCM) This is a revised and modified version by Akira KIDA.
 * (MINISCM)	current version is 0.85k4 (15 May 1994)
 */


/* s7, Bill Schottstaedt, Aug-08
 *
 *   major changes from tinyScheme:
 *        just two files: s7.c and s7.h, source-level embeddable (no library, no run-time init files)
 *        full continuations, call-with-exit for goto or return, dynamic-wind
 *        ratios and complex numbers (and ints are 64-bit by default)
 *          optional multiprecision arithmetic for all numeric types and functions
 *        generalized set!, procedure-with-setter, applicable objects
 *        defmacro and define-macro
 *        keywords, hash tables, block comments, define*
 *        format
 *        error handling using error and catch
 *        in sndlib, the run macro works giving s7 a (somewhat limited) byte compiler
 *        no invidious distinction between built-in and "foreign"
 *          (this makes it easy to extend built-in operators like "+" -- see s7.html for a simple example)
 *        lists, strings, vectors, and hash-tables are (set-)applicable objects
 *        true multiple-values, multiple-value-bind, multiple-value-set!
 *        threads (optional)
 *        multidimensional vectors (optional)
 *        encapsulation (optional)
 *
 *   many minor changes!
 *
 *   deliberate omission from r5rs: 
 *        no syntax-rules or any of its friends
 *        no force and delay unless WITH_FORCE is 1 (default is 0)
 *        no inexact integer or ratio (so, for example, truncate returns an exact integer), no exact complex or exact real
 *           (exact? has no obvious meaning in regard to complex numbers anyway -- are we referring to the polar or
 *            the rectangular form, and are both real and imaginary parts included? -- why can't they be separate?)
 *           In s7, exact? is a synonym for rational?, inexact->exact is a synonym for rationalize.
 *           Also, why isn't 1e2 considered exact?  The e2 business is 10^2 -- not a float!
 *        '#' does not stand for an unknown digit, and the '@' complex number notation is ignored
 *           I also choose not to include numbers such as +i (= 0+i) -- include the real part!
 *
 *   deliberate difference from r5rs:
 *        modulo, remainder, and quotient take integer, ratio, or real args 
 *        lcm and gcd can take integer or ratio args
 *        continuation? function to distinguish a continuation from a procedure
 *        log takes an optional 2nd arg (base)
 *        '.' and an exponent can occur in a number in any base -- they do not mean "base 10"!  
 *           However, the exponent itself is always in base 10 (this follows gmp usage).
 *
 *   other additions: 
 *        random for any numeric type and any numeric argument, including 0 ferchrissake!
 *        sinh, cosh, tanh, asinh, acosh, atanh
 *        read-line, read-byte, write-byte
 *        logior, logxor, logand, lognot, ash, integer-length
 *        procedure-source, procedure-arity, procedure-documentation, help
 *          if the initial expression in a function body is a string constant, it is assumed to be a documentation string
 *        symbol-table, symbol->value, global-environment, current-environment, stack
 *        provide, provided?, defined?
 *        port-line-number, port-filename
 *        object->string, eval-string
 *        reverse!, list-set!, sort!, make-list
 *        gc, quit, *load-hook*, *error-hook*, *error-info*
 *        *features*, *load-path*, *vector-print-length*
 *        define-constant, pi, most-positive-fixnum, most-negative-fixnum, constant?
 *            a constant is really constant -- it can't be bound or set.
 *        symbol-calls if profiling is enabled
 *        stacktrace, trace and untrace, *trace-hook*, __func__, macroexpand
 *            as in C, __func__ is the name of the function currently being defined.
 *        length, copy, fill!, map, for-each are generic
 *        vector-for-each, vector-map, string-for-each, hash-table-for-each, for-each of any applicable object
 *
 *
 * Mike Scholz provided the FreeBSD support (complex trig funcs, etc)
 * Rick Taube and Andrew Burnson provided the MS Visual C++ support
 *
 *
 * Documentation is in s7.h and s7.html.  s7test.scm is a regression test.
 *
 *
 * ---------------- compile time switches ---------------- 
 */

#include "config.h"

/* 
 * Your config file goes here, or just replace that #include line with the defines you need.
 * The compile-time switches involve booleans, threads, complex numbers, multiprecision arithmetic, profiling, and sort!.
 * Currently we assume we have setjmp.h (used by the error handlers).
 *
 * If pthreads are available:
 *
 *   #define HAVE_PTHREADS 1
 *
 * s7.h includes stdbool.h if HAVE_STDBOOL_H is 1 and we're not in C++.
 *
 *
 * Complex number support (which is problematic in C++, Solaris, and netBSD)
 *   is on the WITH_COMPLEX switch. On a Mac, or in Linux, if you're not using C++,
 *   you can use:
 *
 *   #define WITH_COMPLEX 1
 *   #define HAVE_COMPLEX_TRIG 1
 *
 *   Define the first if your compiler has any support for complex numbers.
 *   Define the second if functions like csin are defined in the math library.
 *
 *   In C++ use:
 *
 *   #define WITH_COMPLEX 1
 *   #define HAVE_COMPLEX_TRIG 0
 *
 *   Some systems (freeBSD) have complex.h, but not the trig funcs, so
 *   WITH_COMPLEX means we can find
 *
 *      cimag creal cabs csqrt carg conj
 *
 *   and HAVE_COMPLEX_TRIG means we have
 *
 *      cacos cacosh casin casinh catan catanh ccos ccosh cexp clog cpow csin csinh ctan ctanh
 *
 * When WITH_COMPLEX is 0 or undefined, the complex functions are stubs that simply return their
 *   argument -- this will be very confusing for the s7 user because, for example, (sqrt -2)
 *   will return something bogus (it will not signal an error).
 *
 * Snd's configure.ac has m4 code to handle WITH_COMPLEX and HAVE_COMPLEX_TRIG.
 *
 *
 * To get multiprecision arithmetic, set WITH_GMP to 1.
 *   You'll also need libgmp, libmpfr, and libmpc (version 0.8.0 or later)
 *
 * If your C compiler supports nested functions, define HAVE_NESTED_FUNCTIONS.
 */


/* ---------------- initial sizes ---------------- */

#define INITIAL_HEAP_SIZE 128000
/* the heap grows as needed, this is its initial size. */

#define SYMBOL_TABLE_SIZE 9601
/* names are hashed into the symbol table (a vector) and collisions are chained as lists. */

#define INITIAL_STACK_SIZE 4000            
/* the stack grows as needed, each frame takes 4 entries, this is its initial size */

#define INITIAL_PROTECTED_OBJECTS_SIZE 16  
/* a vector of objects that are (semi-permanently) protected from the GC, grows as needed */

#define GC_TEMPS_SIZE 128
/* the number of recent objects that are temporarily gc-protected; 8 works for s7test and snd-test */

#define INITIAL_TRACE_LIST_SIZE 2
/* a list of currently-traced functions */



/* ---------------- scheme choices ---------------- */

#ifndef WITH_GMP
  #define WITH_GMP 0
  /* this includes multiprecision arithmetic for all numeric types and functions, using gmp, mpfr, and mpc
   * WITH_GMP adds the following functions: bignum, bignum?, bignum-precision
   */
#endif

#ifndef WITH_MULTIDIMENSIONAL_VECTORS
  #define WITH_MULTIDIMENSIONAL_VECTORS 1
  /* this includes the multidimension vector support
   *   added function: vector-dimensions returns a list of dimensions
   */
#endif

#ifndef WITH_PROFILING
  #define WITH_PROFILING 0
  /* this includes a simple profiler -- see the profile function in Snd's extensions.scm 
   *   added function: symbol-calls
   */
#endif

#ifndef WITH_ENCAPSULATION
  #define WITH_ENCAPSULATION 0
  /* this include open-encapsulator, close-encapsulator, encapsulator?, and encapsulator-bindings
   */
#endif

#ifndef WITH_FORCE
  #define WITH_FORCE 0
  /* this includes the slib versions of force and delay.  The name "delay" collides with CLM,
   *    so this is not compatible with sndlib.
   */
#endif



/* -------------------------------------------------------------------------------- */

/* s7.c is organized as follows:
 *    structs and type flags
 *    constants
 *    GC
 *    stacks
 *    symbols
 *    environments
 *    keywords
 *    continuations
 *    numbers
 *    characters
 *    strings
 *    ports
 *    lists
 *    vectors and hash-tables
 *    objects and functions
 *    eq?
 *    generic length, copy, fill!
 *    encapsulators
 *    format
 *    error handlers, stacktrace, trace
 *    sundry leftovers
 *    multiple-values, quasiquote
 *    eval
 *    threads
 *    multiprecision arithmetic
 *    s7 init
 */

#if __cplusplus
  #ifndef WITH_COMPLEX
    #define WITH_COMPLEX 1
  #endif
  #ifndef HAVE_COMPLEX_TRIG
    #define HAVE_COMPLEX_TRIG 0
  #endif
#endif


#ifndef _MSC_VER
  #include <unistd.h>
#endif
#include <limits.h>
#include <float.h>
#include <ctype.h>
#ifndef _MSC_VER
  #include <strings.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

#if __cplusplus
  #include <cmath>
  #include <cstdio>
#else
  #include <math.h>
  #include <stdio.h>
#endif

#if WITH_COMPLEX
  #if __cplusplus
    #include <complex>
  #else
    #include <complex.h>
  #endif
#endif

#include <setjmp.h>

#if HAVE_PTHREADS
  #include <pthread.h>
#endif

#include "s7.h"

#ifndef M_PI
  #define M_PI 3.1415926535897932384626433832795029L
#endif

typedef enum {OP_READ_INTERNAL, OP_EVAL, OP_EVAL_ARGS0, OP_EVAL_ARGS1, OP_APPLY, OP_EVAL_MACRO, OP_LAMBDA, OP_QUOTE, 
	      OP_DEFINE0, OP_DEFINE1, OP_BEGIN, OP_IF0, OP_IF1, OP_SET0, OP_SET1, OP_SET2, 
	      OP_LET0, OP_LET1, OP_LET2, OP_LET_STAR0, OP_LET_STAR1, 
	      OP_LETREC0, OP_LETREC1, OP_LETREC2, OP_COND0, OP_COND1, 
	      OP_AND0, OP_AND1, OP_OR0, OP_OR1, OP_DEFMACRO, OP_DEFMACRO_STAR, OP_MACRO0, OP_MACRO1, 
	      OP_DEFINE_MACRO, OP_DEFINE_MACRO_STAR, OP_DEFINE_EXPANSION, OP_EXPANSION,
	      OP_CASE0, OP_CASE1, OP_CASE2, OP_READ_LIST, OP_READ_DOT, OP_READ_QUOTE, 
	      OP_READ_QUASIQUOTE, OP_READ_QUASIQUOTE_VECTOR, OP_READ_UNQUOTE, OP_READ_UNQUOTE_SPLICING, 
	      OP_READ_VECTOR, OP_READ_RETURN_EXPRESSION, OP_READ_POP_AND_RETURN_EXPRESSION, 
	      OP_LOAD_RETURN_IF_EOF, OP_LOAD_CLOSE_AND_POP_IF_EOF, OP_EVAL_STRING, OP_EVAL_STRING_DONE, OP_EVAL_DONE,
	      OP_QUIT, OP_CATCH, OP_DYNAMIC_WIND, OP_LIST_FOR_EACH, OP_LIST_MAP, OP_DEFINE_CONSTANT0, OP_DEFINE_CONSTANT1, 
	      OP_DO, OP_DO_END0, OP_DO_END1, OP_DO_STEP0, OP_DO_STEP1, OP_DO_STEP2, OP_DO_INIT,
	      OP_DEFINE_STAR, OP_LAMBDA_STAR, OP_ERROR_QUIT, OP_UNWIND_INPUT, OP_UNWIND_OUTPUT, 
	      OP_TRACE_RETURN, OP_ERROR_HOOK_QUIT, OP_TRACE_HOOK_QUIT, OP_WITH_ENV0, OP_WITH_ENV1, OP_WITH_ENV2,
	      OP_VECTOR_FOR_EACH, OP_VECTOR_MAP0, OP_VECTOR_MAP1, OP_STRING_FOR_EACH, OP_OBJECT_FOR_EACH,
	      OP_HASH_TABLE_FOR_EACH, 
	      OP_MAX_DEFINED} opcode_t;

#define NUM_SMALL_INTS 256
/* this needs to be at least OP_MAX_DEFINED = 87 */

typedef enum {TOKEN_EOF, TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_DOT, TOKEN_ATOM, TOKEN_QUOTE, TOKEN_DOUBLE_QUOTE, 
	      TOKEN_BACK_QUOTE, TOKEN_COMMA, TOKEN_AT_MARK, TOKEN_SHARP_CONST, TOKEN_VECTOR} token_t;


typedef struct s7_num_t {
  char type;
  union {
    
    s7_Int integer_value;
    
    s7_Double real_value;
    
    struct {
      s7_Int numerator;
      s7_Int denominator;
    } fraction_value;
    
    struct {
      s7_Double real;
      s7_Double imag;
    } complex_value;
    
    unsigned long ul_value; /* these two are not used by s7 in any way */
    unsigned long long ull_value;

  } value;
} s7_num_t;


typedef enum {FILE_PORT, STRING_PORT, FUNCTION_PORT} port_type_t;

typedef struct s7_port_t {
  bool is_closed;
  port_type_t type;
  bool needs_free;
  FILE *file;
  int line_number;
  int file_number;
  char *filename;
  char *value;
  int size, point; 
  s7_pointer (*input_function)(s7_scheme *sc, s7_read_t read_choice, s7_pointer port);
  void (*output_function)(s7_scheme *sc, char c, s7_pointer port);
  void *data;
} s7_port_t;


typedef struct s7_func_t {
  s7_function ff;
  const char *name;
  char *doc;
  int required_args, optional_args, all_args;
  bool rest_arg;
} s7_func_t;


#if WITH_MULTIDIMENSIONAL_VECTORS
typedef struct s7_vdims_t {
  int ndims;
  s7_Int *dims, *offsets;
} s7_vdims_t;
#endif


/* cell structure */
typedef struct s7_cell {
  unsigned int flag;
  int hloc;
#if WITH_PROFILING
  long long int calls;
#endif
  union {
    
    struct {
      int  length;
      char *svalue;
      s7_pointer global_slot; /* for strings that represent symbol names, this is the current global environment (symbol value) cons */
    } string;
    
    s7_num_t number;
    
    s7_port_t *port;
    
    unsigned char cvalue;
    
    opcode_t proc_num;

    void *c_pointer;
    
    struct {
      s7_Int length;
      s7_pointer *elements;
#if WITH_MULTIDIMENSIONAL_VECTORS
      s7_vdims_t *dim_info;
#endif
    } vector;
    
    s7_func_t *ffptr;
    
    struct {
      struct s7_cell *car;
      struct s7_cell *cdr;
      int line;
    } cons;
    
    struct {
      int type;
      void *value;
    } fobj;
    
    struct {
      int stack_size, stack_top;
      s7_pointer stack;
    } continuation;
    
    int goto_loc;
    
    struct {
      int goto_loc;
      s7_pointer tag;
      s7_pointer handler;
    } rcatch; /* C++ reserves "catch" I guess */
    
    struct {
      s7_pointer in, out, body;
    } winder;

  } object;
} s7_cell;


#if HAVE_PTHREADS
typedef struct {
  s7_scheme *sc;
  s7_pointer func;
  pthread_t *thread;
} thred;
#endif


struct s7_scheme {  
  s7_cell **heap, **free_heap;
  unsigned int heap_size, free_heap_top;

  /* "int" or "unsigned int" seems safe here:
   *      sizeof(s7_cell) = 28 in 32-bit machines, 32 in 64
   *      so to get more than 2^32 actual objects would require ca 140 GBytes RAM
   *      vectors might be full of the same object (sc->NIL for example), so there
   *      we need ca 38 GBytes RAM (8 bytes per pointer).
   */
  
  s7_pointer args;                    /* arguments of current function */
  s7_pointer envir;                   /* current environment */
  s7_pointer code, cur_code;          /* current code */
  
  s7_pointer stack;                   /* stack is a vector */
  int stack_size, stack_top, stack_size2;
  s7_pointer *small_ints;             /* permanent numbers for opcode entries in the stack */
  s7_pointer real_zero, real_one;
  
  s7_pointer protected_objects;       /* a vector of gc-protected objects */
  int *protected_objects_size, *protected_objects_loc; /* pointers so they're global across threads */

  struct s7_cell _NIL;
  s7_pointer NIL;                     /* empty list */
  
  struct s7_cell _T;
  s7_pointer T;                       /* #t */
  
  struct s7_cell _F;
  s7_pointer F;                       /* #f */
  
  struct s7_cell _EOF_OBJECT;
  s7_pointer EOF_OBJECT;              /* end-of-file object */
  
  struct s7_cell _UNDEFINED;  
  s7_pointer UNDEFINED;               /* unset or undefined object */
  
  struct s7_cell _UNSPECIFIED;
  s7_pointer UNSPECIFIED;             /* the unspecified value */
  
  s7_pointer symbol_table;            /* symbol table */
  s7_pointer global_env;              /* global environment */
  
  s7_pointer LAMBDA, LAMBDA_STAR, QUOTE, QUASIQUOTE, UNQUOTE, UNQUOTE_SPLICING, MACROEXPAND;
  s7_pointer APPLY, VECTOR, CONS, APPEND, CDR, VECTOR_FUNCTION, VALUES, ELSE, SET;
  s7_pointer ERROR, WRONG_TYPE_ARG, OUT_OF_RANGE, FORMAT_ERROR, WRONG_NUMBER_OF_ARGS;
  s7_pointer KEY_KEY, KEY_OPTIONAL, KEY_REST, __FUNC__, ERROR_HOOK, TRACE_HOOK;
  s7_pointer FEED_TO;                 /* => */
  s7_pointer OBJECT_SET;              /* applicable object set method */
  s7_pointer VECTOR_SET, STRING_SET, LIST_SET, HASH_TABLE_SET;
  
  s7_pointer input_port;              /* current-input-port (nil = stdin) */
  s7_pointer input_port_stack;        /*   input port stack (load and read internally) */
  s7_pointer output_port;             /* current-output-port (nil = stderr) */
  s7_pointer error_port;              /* current-error-port (nil = stderr) */
  s7_pointer error_info;              /* the vector bound to *error-info* */
  
  /* these 6 are pointers so that all thread refs are to the same thing */
  bool *gc_off;                       /* if true, the GC won't run */
  bool *tracing, *trace_all;          /* if tracing, each function on the *trace* list prints its args upon application */
  long *gensym_counter;

#if WITH_ENCAPSULATION
  s7_pointer encapsulators;
#endif
  
  #define INITIAL_STRBUF_SIZE 1024
  int strbuf_size;
  char *strbuf;
  
  char *read_line_buf;
  int read_line_buf_size;

  token_t tok;
  s7_pointer value;
  opcode_t op;
  s7_pointer x, y, z;            /* evaluator local vars */

  s7_pointer *temps;             /* short-term gc protection */
  int temps_ctr, temps_size;

  #define CIRCULAR_REFS_SIZE 8
  s7_pointer *circular_refs;     /* printer circular list/vector checks */

  jmp_buf goto_start, goto_qsort_end;
  bool longjmp_ok;
  void (*error_exiter)(void);
  
#if HAVE_PTHREADS
  struct s7_scheme *orig_sc;
  s7_pointer key_values;
  int thread_id;
  int *thread_ids;               /* global current top thread_id */
#endif

  s7_pointer *trace_list;
  int trace_list_size, trace_top, trace_depth;

  #define BLOCK_VECTOR_SIZE 100
  s7_pointer *nil_vector, *unspecified_vector;

  void *default_rng;
#if WITH_GMP
  void *default_big_rng;
#endif
};


#define T_UNTYPED       0
#define T_NIL           1
#define T_STRING        2
#define T_NUMBER        3
#define T_SYMBOL        4
#define T_PAIR          5
#define T_CLOSURE       6
#define T_CLOSURE_STAR  7
#define T_CONTINUATION  8
#define T_C_FUNCTION    9
#define T_CHARACTER    10
#define T_INPUT_PORT   11
#define T_VECTOR       12
#define T_MACRO        13
#define T_C_OBJECT     14
#define T_GOTO         15
#define T_OUTPUT_PORT  16
#define T_CATCH        17
#define T_DYNAMIC_WIND 18
#define T_HASH_TABLE   19
#define T_BOOLEAN      20
#define T_C_MACRO      21
#define T_C_POINTER    22
#define BUILT_IN_TYPES 23

#define TYPE_BITS                     8
#define T_MASKTYPE                    0xff

#define typeflag(p)                   ((p)->flag)
#define type(p)                       (typeflag(p) & T_MASKTYPE)
/* set_type below -- needs to maintain mark setting */

#define T_SYNTAX                      (1 << (TYPE_BITS + 1))
#define is_syntax(p)                  ((typeflag(p) & T_SYNTAX) != 0) /* the silly != 0 business is for MS C++'s benefit */
#define syntax_opcode(x)              ((x)->hloc)

#define T_IMMUTABLE                   (1 << (TYPE_BITS + 2))
#define is_immutable(p)               ((typeflag(p) & T_IMMUTABLE) != 0)
#define set_immutable(p)              typeflag(p) |= T_IMMUTABLE
/* immutable means the object's value can't be changed via set! */

#define T_ATOM                        (1 << (TYPE_BITS + 3))
#define is_atom(p)                    ((typeflag(p) & T_ATOM) != 0)
/* is_atom means there's no car/cdr to be marked */

#define T_GC_MARK                     (1 << (TYPE_BITS + 4))
#define is_marked(p)                  ((typeflag(p) &  T_GC_MARK) != 0)
#define set_mark(p)                   typeflag(p)  |= T_GC_MARK
#define clear_mark(p)                 typeflag(p)  &= (~T_GC_MARK)
/* making this a separate bool field in the cell struct slightly speeds up the mark function,
 *   but at the cost of 4 bytes per object. Since the speedup was about 2% overall, I think
 *   the size increase is more important.
 */

#define T_SETTER                      (1 << (TYPE_BITS + 5))
#define is_setter(p)                  ((typeflag(p) & T_SETTER) != 0)
/* this marks a function that sets something (for the encapsulator) */

#define T_OBJECT                      (1 << (TYPE_BITS + 6))
/* debugging sanity check */

#define T_FINALIZABLE                 (1 << (TYPE_BITS + 7))
#define is_finalizable(p)             ((typeflag(p) & T_FINALIZABLE) != 0)
#define clear_finalizable(p)          typeflag(p) &= (~T_FINALIZABLE)
/* finalizable means some action may need to be taken when the cell is GC'd */

#define T_SIMPLE                      (1 << (TYPE_BITS + 8))
#define is_simple(p)                  ((typeflag(p) & T_SIMPLE) != 0)
/* a simple object has no markable subfields and no special mark actions */

#define T_DONT_COPY                   (1 << (TYPE_BITS + 9))
#define dont_copy(p)                  ((typeflag(p) & T_DONT_COPY) != 0)
/* dont_copy means the object is not copied when saved in a continuation */

#define T_PROCEDURE                   (1 << (TYPE_BITS + 10))
#define is_procedure(p)               ((typeflag(p) & T_PROCEDURE) != 0)

#define T_ETERNAL                     (1 << (TYPE_BITS + 11))
#define is_eternal(p)                 ((typeflag(p) & T_ETERNAL) != 0)
/* an eternal object is a cons whose "line_number" is actually the global environment vector location (i.e. a symbol table entry) */

#define T_ANY_MACRO                   (1 << (TYPE_BITS + 12))
#define is_any_macro(p)               ((typeflag(p) & T_ANY_MACRO) != 0)
/* this marks scheme and C-defined macros */

#define T_EXPANSION                   (1 << (TYPE_BITS + 13))
#define is_expansion(p)               ((typeflag(p) & T_EXPANSION) != 0)
/* this marks macros from define-expansion */

#define T_LOCAL                       (1 << (TYPE_BITS + 14))
#define is_not_local(p)               ((typeflag(p) & T_LOCAL) == 0)
#define set_local(p)                  typeflag(p) |= T_LOCAL
/* this marks a symbol that has been used at some time as a local variable */

#define T_DWIND_INIT                  (1 << (TYPE_BITS + 15))
#define T_DWIND_BODY                  (1 << (TYPE_BITS + 16))
#define T_DWIND_FINISH                (T_DWIND_INIT | T_DWIND_BODY)
#define DWIND_STATE(p)                (typeflag(p) & T_DWIND_FINISH)
#define DWIND_SET_STATE(p, n)         typeflag(p) = ((typeflag(p) & (~T_DWIND_FINISH)) | n)

#define UNUSED_BITS                   0xfe000000

#if HAVE_PTHREADS
#define set_type(p, f)                typeflag(p) = ((typeflag(p) & T_GC_MARK) | (f) | T_OBJECT)
/* the gc call can be interrupted, leaving mark bits set -- we better not clear those bits */
#else
#define set_type(p, f)                typeflag(p) = ((f) | T_OBJECT)
#endif

#define is_true(Sc, p)                ((p) != Sc->F)
#define is_false(Sc, p)               ((p) == Sc->F)
#ifdef _MSC_VER
  #define make_boolean(sc, Val)       (((Val) &0xff) ? sc->T : sc->F)
#else
  #define make_boolean(sc, Val)       ((Val) ? sc->T : sc->F)
#endif

#define is_pair(p)                    (type(p) == T_PAIR)
#define car(p)                        ((p)->object.cons.car)
#define cdr(p)                        ((p)->object.cons.cdr)
#define caar(p)                       car(car(p))
#define cadr(p)                       car(cdr(p))
#define cdar(p)                       cdr(car(p))
#define cddr(p)                       cdr(cdr(p))
#define caaar(p)                      car(car(car(p)))
#define cadar(p)                      car(cdr(car(p)))
#define cdadr(p)                      cdr(car(cdr(p)))
#define caddr(p)                      car(cdr(cdr(p)))
#define caadr(p)                      car(car(cdr(p)))
#define cdaar(p)                      cdr(car(car(p)))
#define cdddr(p)                      cdr(cdr(cdr(p)))
#define cddar(p)                      cdr(cdr(car(p)))
#define caaadr(p)                     car(car(car(cdr(p))))
#define cadaar(p)                     car(cdr(car(car(p))))
#define cadddr(p)                     car(cdr(cdr(cdr(p))))
#define caaddr(p)                     car(car(cdr(cdr(p))))
#define cddddr(p)                     cdr(cdr(cdr(cdr(p))))
#define caddar(p)                     car(cdr(cdr(car(p))))
#define pair_line_number(p)           (p)->object.cons.line
#define port_file_number(p)           (p)->object.port->file_number

#define string_value(p)               ((p)->object.string.svalue)
#define string_length(p)              ((p)->object.string.length)
#define character(p)                  ((p)->object.cvalue)

#define symbol_location(p)            (p)->object.cons.line
  /* presumably the symbol table size will fit in an int, so this is safe */
#define symbol_name(p)                string_value(car(p))
#define symbol_name_length(p)         string_length(car(p))
#define symbol_value(Sym)             cdr(Sym)
#define set_symbol_value(Sym, Val)    cdr(Sym) = (Val)
#if WITH_PROFILING
  #define symbol_calls(p)             (p)->calls
#endif
#define symbol_global_slot(p)         (car(p))->object.string.global_slot

#define vector_length(p)              ((p)->object.vector.length)
#define vector_element(p, i)          ((p)->object.vector.elements[i])
#define vector_elements(p)            (p)->object.vector.elements

#if WITH_MULTIDIMENSIONAL_VECTORS
  #define vector_dimension(p, i)      ((p)->object.vector.dim_info->dims[i])
  #define vector_ndims(p)             ((p)->object.vector.dim_info->ndims)
  #define vector_offset(p, i)         ((p)->object.vector.dim_info->offsets[i])
  #define vector_is_multidimensional(p) ((p)->object.vector.dim_info)
  #define VECTOR_REST_ARGS            true
#else
  #define VECTOR_REST_ARGS            false
#endif

#define small_int(Sc, Val)            (Sc)->small_ints[Val]

#define is_input_port(p)              (type(p) == T_INPUT_PORT) 
#define is_output_port(p)             (type(p) == T_OUTPUT_PORT)
#define is_string_port(p)             ((p)->object.port->type == STRING_PORT)
#define is_file_port(p)               ((p)->object.port->type == FILE_PORT)
#define is_function_port(p)           ((p)->object.port->type == FUNCTION_PORT)
#define port_type(p)                  (p)->object.port->type
#define port_line_number(p)           (p)->object.port->line_number
#define port_filename(p)              (p)->object.port->filename
#define port_file(p)                  (p)->object.port->file
#define port_is_closed(p)             (p)->object.port->is_closed
#define port_string(p)                (p)->object.port->value
#define port_string_length(p)         (p)->object.port->size
#define port_string_point(p)          (p)->object.port->point
#define port_needs_free(p)            (p)->object.port->needs_free
#define port_output_function(p)       (p)->object.port->output_function
#define port_input_function(p)        (p)->object.port->input_function
#define port_data(p)                  (p)->object.port->data

#define is_c_function(f)              (type(f) == T_C_FUNCTION)
#define c_function(f)                 (f)->object.ffptr
#define c_function_call(f)            (f)->object.ffptr->ff
#define c_function_name(f)            (f)->object.ffptr->name
#define c_function_documentation(f)   (f)->object.ffptr->doc
#define c_function_required_args(f)   (f)->object.ffptr->required_args
#define c_function_optional_args(f)   (f)->object.ffptr->optional_args
#define c_function_has_rest_arg(f)    (f)->object.ffptr->rest_arg
#define c_function_all_args(f)        (f)->object.ffptr->all_args

#define is_c_macro(p)                 (type(p) == T_C_MACRO)
#define c_macro_call(f)               (f)->object.ffptr->ff
#define c_macro_name(f)               (f)->object.ffptr->name
#define c_macro_required_args(f)      (f)->object.ffptr->required_args
#define c_macro_all_args(f)           (f)->object.ffptr->all_args

#define continuation_stack(p)         (p)->object.continuation.stack
#define continuation_stack_top(p)     (p)->object.continuation.stack_top
#define continuation_stack_size(p)    (p)->object.continuation.stack_size

#define is_goto(p)                    (type(p) == T_GOTO)
#define is_macro(p)                   (type(p) == T_MACRO)

#define is_closure(p)                 (type(p) == T_CLOSURE)
#define is_closure_star(p)            (type(p) == T_CLOSURE_STAR)
#define closure_source(Obj)           car(Obj)
#define closure_args(Obj)             car(closure_source(Obj))
#define closure_body(Obj)             cdr(closure_source(Obj))
#define closure_environment(Obj)      cdr(Obj)

#define is_catch(p)                   (type(p) == T_CATCH)
#define catch_tag(p)                  (p)->object.rcatch.tag
#define catch_goto_loc(p)             (p)->object.rcatch.goto_loc
#define catch_handler(p)              (p)->object.rcatch.handler

#define is_dynamic_wind(p)            (type(p) == T_DYNAMIC_WIND)
#define dynamic_wind_state(p)         DWIND_STATE(p)
#define dynamic_wind_set_state(p, n)  DWIND_SET_STATE(p, n)
#define dynamic_wind_in(p)            (p)->object.winder.in
#define dynamic_wind_out(p)           (p)->object.winder.out
#define dynamic_wind_body(p)          (p)->object.winder.body

#define is_c_object(p)                (type(p) == T_C_OBJECT)
#define c_object_type(p)              (p)->object.fobj.type
#define c_object_value(p)             (p)->object.fobj.value

#if WITH_ENCAPSULATION
#define is_encapsulating(Sc)          ((Sc)->encapsulators != sc->NIL)
#endif

#define NUM_INT      0
#define NUM_RATIO    1
#define NUM_REAL     2
#define NUM_REAL2    3
#define NUM_COMPLEX  4
#define NO_NUM       8
#define NO_NUM_SHIFT 3
#define IS_NUM(n)    (n < NO_NUM)

#define number(p)                     (p)->object.number
#define number_type(p)                (p)->object.number.type
#define num_type(n)                   (n.type)

#define numerator(n)                  n.value.fraction_value.numerator
#define denominator(n)                n.value.fraction_value.denominator
#define fraction(n)                   (((s7_Double)numerator(n)) / ((s7_Double)denominator(n)))

#define real_part(n)                  n.value.complex_value.real
#define imag_part(n)                  n.value.complex_value.imag
#define integer(n)                    n.value.integer_value

#ifndef LLONG_MAX
  #define LLONG_MAX 9223372036854775807LL
  #define LLONG_MIN (-LLONG_MAX - 1LL)
#endif

#if __cplusplus
  using namespace std;
  typedef complex<s7_Double> s7_Complex;
  static s7_Double Real(complex<s7_Double> x) {return(real(x));} /* protect the C++ name */
  static s7_Double Imag(complex<s7_Double> x) {return(imag(x));}
#endif

#define real(n)                       n.value.real_value


static int safe_strlen(const char *str)
{
  if ((str) && (*str))
    return(strlen(str));
  return(0);
}


static char *s7_strdup_with_len(const char *str, int len)
{
  char *newstr;
  if (!str) return(NULL);
  newstr = (char *)malloc((len + 1) * sizeof(char));
  if (!newstr) return(NULL);
  memcpy((void *)newstr, (void *)str, len);
  newstr[len] = 0;
  return(newstr);
}


static char *s7_strdup(const char *str)
{
  return(s7_strdup_with_len(str, safe_strlen(str)));
}


static bool is_proper_list(s7_scheme *sc, s7_pointer lst);
static void s7_mark_embedded_objects(s7_pointer a); /* called by gc, calls fobj's mark func */
static s7_pointer eval(s7_scheme *sc, opcode_t first_op);
static s7_pointer s7_division_by_zero_error(s7_scheme *sc, const char *caller, s7_pointer arg);
static s7_pointer s7_file_error(s7_scheme *sc, const char *caller, const char *descr, const char *name);
static void s7_free_function(s7_pointer a);
static s7_pointer safe_reverse_in_place(s7_scheme *sc, s7_pointer list);
static s7_pointer s7_immutable_cons(s7_scheme *sc, s7_pointer a, s7_pointer b);
static void s7_free_object(s7_pointer a);
static char *s7_describe_object(s7_scheme *sc, s7_pointer a);
static s7_pointer make_atom(s7_scheme *sc, char *q, int radix, bool want_symbol);
static bool s7_is_applicable_object(s7_pointer x);
static s7_pointer make_list_1(s7_scheme *sc, s7_pointer a);
static s7_pointer make_list_2(s7_scheme *sc, s7_pointer a, s7_pointer b);
static s7_pointer s7_permanent_cons(s7_pointer a, s7_pointer b, int type);
static void write_string(s7_scheme *sc, const char *s, s7_pointer pt);
static s7_pointer eval_symbol(s7_scheme *sc, s7_pointer sym);
static s7_pointer eval_error(s7_scheme *sc, const char *errmsg, s7_pointer obj);
static bool is_thunk(s7_scheme *sc, s7_pointer x);
static int remember_file_name(const char *file);
static s7_pointer splice_in_values(s7_scheme *sc, s7_pointer args);
static const char *s7_type_name(s7_pointer arg);

#if WITH_ENCAPSULATION
  static void encapsulate(s7_scheme *sc, s7_pointer sym);
#endif
#if HAVE_PTHREADS
  static s7_pointer g_is_thread(s7_scheme *sc, s7_pointer args);
#endif



/* -------------------------------- constants -------------------------------- */

s7_pointer s7_f(s7_scheme *sc) 
{
  return(sc->F);
}


s7_pointer s7_t(s7_scheme *sc) 
{
  return(sc->T);
}


s7_pointer s7_nil(s7_scheme *sc) 
{
  return(sc->NIL);
}


s7_pointer s7_undefined(s7_scheme *sc) 
{
  return(sc->UNDEFINED);
}


s7_pointer s7_unspecified(s7_scheme *sc) 
{
  return(sc->UNSPECIFIED);
}


bool s7_is_unspecified(s7_scheme *sc, s7_pointer val)
{
  return(val == sc->UNSPECIFIED);
}


s7_pointer s7_eof_object(s7_scheme *sc) 
{
  return(sc->EOF_OBJECT);
}


static s7_pointer g_not(s7_scheme *sc, s7_pointer args)
{
  #define H_not "(not obj) returns #t if obj is #f, otherwise #t"
  return(make_boolean(sc, is_false(sc, car(args))));
}


bool s7_boolean(s7_scheme *sc, s7_pointer x)
{
  return(x != sc->F);
}


bool s7_is_boolean(s7_pointer x)
{
  return(type(x) == T_BOOLEAN);
}


s7_pointer s7_make_boolean(s7_scheme *sc, bool x)
{
  return(make_boolean(sc, x));
}


static s7_pointer g_is_boolean(s7_scheme *sc, s7_pointer args)
{
  #define H_is_boolean "(boolean? obj) returns #t if obj is #f or #t"
  return(s7_make_boolean(sc, s7_is_boolean(car(args))));
}


static bool s7_is_immutable(s7_pointer p) 
{ 
  return((typeflag(p) & T_IMMUTABLE) != 0);
}


static s7_pointer s7_set_immutable(s7_pointer p) 
{ 
  typeflag(p) |= T_IMMUTABLE;
  return(p);
}


static s7_pointer set_pair_line_number(s7_pointer p, int n)
{
  if ((!is_eternal(p)) &&
      (is_pair(p)))
    pair_line_number(p) = n;
  return(p);
}


bool s7_is_constant(s7_pointer p) 
{ 
  /* this means "not settable": numbers, characters, strings, keywords, #f #t pi etc */
  /*   so to be non-constant, it has to be a non-keyword symbol with the immutable bit not set, I think */
  
  return((type(p) != T_SYMBOL) ||
	 ((typeflag(p) & T_IMMUTABLE) != 0) ||
	 (s7_is_keyword(p)));
}


static s7_pointer g_is_constant(s7_scheme *sc, s7_pointer args)
{
  #define H_is_constant "(constant? obj) returns #t if obj is a constant (unsettable)"
  return(s7_make_boolean(sc, s7_is_constant(car(args))));
}




/* -------------------------------- GC -------------------------------- */

#if HAVE_PTHREADS
static pthread_mutex_t protected_objects_lock = PTHREAD_MUTEX_INITIALIZER;
#endif


int s7_gc_protect(s7_scheme *sc, s7_pointer x)
{
  int i, loc, new_size;
  
#if HAVE_PTHREADS
  pthread_mutex_lock(&protected_objects_lock);
#endif

  if (vector_element(sc->protected_objects, (*(sc->protected_objects_loc))) == sc->NIL)
    {
      vector_element(sc->protected_objects, (*(sc->protected_objects_loc))) = x;
      loc = (*(sc->protected_objects_loc))++;
      if ((*(sc->protected_objects_loc)) >= (*(sc->protected_objects_size)))
	(*(sc->protected_objects_loc)) = 0;
      {
#if HAVE_PTHREADS
	pthread_mutex_unlock(&protected_objects_lock);
#endif
	return(loc);
      }
    }
  
  for (i = 0; i < (*(sc->protected_objects_size)); i++)
    if (vector_element(sc->protected_objects, i) == sc->NIL)
      {
	vector_element(sc->protected_objects, i) = x;
#if HAVE_PTHREADS
	pthread_mutex_unlock(&protected_objects_lock);
#endif
	return(i);
      }
  
  loc = (*(sc->protected_objects_size));
  new_size = 2 * (*(sc->protected_objects_size));
  vector_elements(sc->protected_objects) = (s7_pointer *)realloc(vector_elements(sc->protected_objects), new_size * sizeof(s7_pointer));
  for (i = (*(sc->protected_objects_size)) + 1; i < new_size; i++)
    vector_element(sc->protected_objects, i) = sc->NIL;
  vector_length(sc->protected_objects) = new_size;
  (*(sc->protected_objects_size)) = new_size;
  vector_element(sc->protected_objects, loc) = x;
  
#if HAVE_PTHREADS
  pthread_mutex_unlock(&protected_objects_lock);
#endif
  return(loc);
}


void s7_gc_unprotect(s7_scheme *sc, s7_pointer x)
{
  int i;

#if HAVE_PTHREADS
  pthread_mutex_lock(&protected_objects_lock);
#endif

  for (i = 0; i < (*(sc->protected_objects_size)); i++)
    if (vector_element(sc->protected_objects, i) == x)
      {
	vector_element(sc->protected_objects, i) = sc->NIL;
	(*(sc->protected_objects_loc)) = i;
#if HAVE_PTHREADS
	pthread_mutex_unlock(&protected_objects_lock);
#endif
	return;
      }

#if HAVE_PTHREADS
  pthread_mutex_unlock(&protected_objects_lock);
#endif
}


void s7_gc_unprotect_at(s7_scheme *sc, int loc)
{
#if HAVE_PTHREADS
  pthread_mutex_lock(&protected_objects_lock);
#endif

  vector_element(sc->protected_objects, loc) = sc->NIL;
  (*(sc->protected_objects_loc)) = loc;

#if HAVE_PTHREADS
  pthread_mutex_unlock(&protected_objects_lock);
#endif
}


s7_pointer s7_gc_protected_at(s7_scheme *sc, int loc)
{
  s7_pointer obj;

#if HAVE_PTHREADS
  pthread_mutex_lock(&protected_objects_lock);
#endif

  obj = vector_element(sc->protected_objects, loc);

#if HAVE_PTHREADS
  pthread_mutex_unlock(&protected_objects_lock);
#endif

  return(obj);
}


static void finalize_s7_cell(s7_scheme *sc, s7_pointer a) 
{
  switch (type(a))
    {
    case T_STRING:
      free(string_value(a)); /* calloc'd in make-*-string */
      break;
      
    case T_INPUT_PORT:
      if (port_needs_free(a))
	{
	  if (port_string(a))
	    {
	      free(port_string(a));
	      port_string(a) = NULL;
	    }
	  port_needs_free(a) = false;
	}
      if (port_filename(a))
	{
	  free(port_filename(a));
	  port_filename(a) = NULL;
	}
      free(a->object.port);
      break;
      
    case T_OUTPUT_PORT:
      s7_close_output_port(sc, a);
      if ((is_file_port(a)) && 
	  (port_filename(a)))
	{
	  free(port_filename(a));
	  port_filename(a) = NULL;
	}
      free(a->object.port);
      break;
      
    case T_C_OBJECT:
      s7_free_object(a);
      break;
      
    case T_C_FUNCTION:
    case T_C_MACRO:
      s7_free_function(a);
      break;
      
    case T_VECTOR:
    case T_HASH_TABLE:
      if (vector_length(a) > 0)
	{
	  free(vector_elements(a));
#if WITH_MULTIDIMENSIONAL_VECTORS
	  if (vector_is_multidimensional(a))
	    {
	      free(a->object.vector.dim_info->dims);
	      free(a->object.vector.dim_info->offsets);
	      free(a->object.vector.dim_info);
	    }
#endif
	}
      break;
      
    default:
      break;
    }
}


static void s7_mark_object_1(s7_pointer p);

#if defined(__GNUC__) && (!(defined(__cplusplus)))
  #define S7_MARK(Obj) ({ s7_pointer _p_; _p_ = Obj; if (!is_marked(_p_)) s7_mark_object_1(_p_); })
#else
  #define S7_MARK(Obj) if (!is_marked(Obj)) s7_mark_object_1(Obj)  
#endif
/* this is slightly faster than if we first call s7_mark_object, then check the mark bit */


static void mark_vector(s7_pointer p, s7_Int top)
{
  s7_pointer *tp;
  set_mark(p); /* might be called outside s7_mark_object */
  tp = (s7_pointer *)(vector_elements(p));

  if (top < INT_MAX)
    {
      int j, jlen;
      jlen = top;
      for (j = 0; j < jlen; j++) 
	S7_MARK(tp[j]);
    }
  else
    {
      s7_Int i;
      for (i = 0; i < top; i++) 
	S7_MARK(tp[i]);
    }
}


static void s7_mark_object_1(s7_pointer p)
{
  set_mark(p);
  
  if (is_simple(p)) return;
  
  switch (type(p))
    {
    case T_UNTYPED: /* 0 actually -- a cell still being set up when the GC was triggered */
      return;

    case T_VECTOR:
    case T_HASH_TABLE:
      mark_vector(p, vector_length(p));
      return;
      
    case T_C_OBJECT:
      s7_mark_embedded_objects(p);
      return;

    case T_CONTINUATION:
      mark_vector(continuation_stack(p), continuation_stack_top(p));
      return;

    case T_CATCH:
      S7_MARK(catch_tag(p));
      S7_MARK(catch_handler(p));
      return;

    case T_DYNAMIC_WIND:
      S7_MARK(dynamic_wind_in(p));
      S7_MARK(dynamic_wind_out(p));
      S7_MARK(dynamic_wind_body(p));
      return;

    default:
      break;
    }
  
  /* this should follow s7_is_object -- the latter is an atom, but we have to run the object's internal mark function */
  if (is_atom(p))
    return;
  
  S7_MARK(car(p));
  S7_MARK(cdr(p));
}


void s7_mark_object(s7_pointer p)
{
  S7_MARK(p);
}


static int gc(s7_scheme *sc)
{
  int i;
  unsigned int old_free_heap_top;
  
  /* mark all live objects (the symbol table is in permanent memory, not the heap) */
  S7_MARK(sc->global_env);
  S7_MARK(sc->args);
  S7_MARK(sc->envir);
  S7_MARK(sc->code);
  S7_MARK(sc->cur_code);
  mark_vector(sc->stack, sc->stack_top);
  S7_MARK(sc->x);
  S7_MARK(sc->y);
  S7_MARK(sc->z);
  S7_MARK(sc->value);  

  S7_MARK(sc->input_port);
  S7_MARK(sc->input_port_stack);
  S7_MARK(sc->output_port);
  S7_MARK(sc->error_port);

#if WITH_ENCAPSULATION
  S7_MARK(sc->encapsulators);
#endif
  
  S7_MARK(sc->protected_objects);
  {
    s7_pointer *tmps;
    tmps = sc->temps;
    for (i = 0; i < sc->temps_size; i++)
      S7_MARK(tmps[i]);
  }

  /* free up all other objects */
  old_free_heap_top = sc->free_heap_top;

  {
    s7_pointer *fp, *tp;
    fp = sc->free_heap;

    for (tp = sc->heap; (*tp); tp++) /* this form of the loop is slightly faster than counting i with sc->heap[i] */
      {
	s7_pointer p;
	p = (*tp);

	if (typeflag(p) != 0) /* an already-free object? */
	  {
	    if (is_marked(p)) /* it's marginally faster to switch the order of these checks */
	      clear_mark(p);
	    else 
	      {
		if (is_finalizable(p))
		  finalize_s7_cell(sc, p); 
		typeflag(p) = 0;
		fp[sc->free_heap_top++] = p;
	      }
	  }
      }
  }

  /* fprintf(stderr, "gc: %d of %u\n", sc->free_heap_top - old_free_heap_top, sc->heap_size); */
  
  return(sc->free_heap_top - old_free_heap_top); /* needed by cell allocator to decide when to increase heap size */
}


#if 0
static s7_pointer g_dump_heap(s7_scheme *sc, s7_pointer args)
{
  FILE *fd;
  s7_pointer *tp;
  int i;

  gc(sc);

  fd = fopen("heap.data", "w");
  for (tp = sc->heap; (*tp); tp++)
    {
      s7_pointer p;
      p = (*tp);
      if (typeflag(p) != 0)
	fprintf(fd, "%s\n", s7_object_to_c_string(sc, p));
    }

  fprintf(fd, "-------------------------------- temps --------------------------------\n");
  for (i = 0; i < sc->temps_size; i++)
    if (typeflag(sc->temps[i]) != 0)
      fprintf(fd, "%s\n", s7_object_to_c_string(sc, sc->temps[i]));

  fclose(fd);
  return(sc->NIL);
}
#endif


#if HAVE_PTHREADS
static pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;

static s7_pointer new_cell(s7_scheme *nsc)
#else
static s7_pointer new_cell(s7_scheme *sc)
#endif
{
  s7_pointer p;
  
#if HAVE_PTHREADS
  s7_scheme *sc;
  pthread_mutex_lock(&alloc_lock);
  sc = nsc->orig_sc;
#endif

  if (sc->free_heap_top == 0)
    {
      /* no free heap */
      unsigned int k, old_size, freed_heap = 0;
      
      if (!(*(sc->gc_off)))
        freed_heap = gc(sc);
      /* when threads, the gc function can be interrupted at any point and resumed later -- mark bits need to be preserved during this interruption */
      
      if (freed_heap < sc->heap_size / 4) /* was 1000, setting it to 2 made no difference in run time */
	{
	  /* alloc more heap */
	  old_size = sc->heap_size;
	  if (sc->heap_size < 512000)
	    sc->heap_size *= 2;
	  else sc->heap_size += 512000;

	  sc->heap = (s7_cell **)realloc(sc->heap, (sc->heap_size + 1) * sizeof(s7_cell *));
	  if (!(sc->heap))
	    fprintf(stderr, "heap reallocation failed! tried to get %lu bytes\n", (unsigned long)((sc->heap_size + 1) * sizeof(s7_cell *)));

	  sc->free_heap = (s7_cell **)realloc(sc->free_heap, sc->heap_size * sizeof(s7_cell *));
	  if (!(sc->free_heap))
	    fprintf(stderr, "free heap reallocation failed! tried to get %lu bytes\n", (unsigned long)(sc->heap_size * sizeof(s7_cell *)));	  

	  { 
	    /* optimization suggested by K Matheussen */
	    s7_cell *cells = (s7_cell *)calloc(sc->heap_size - old_size, sizeof(s7_cell));
	    for (k = old_size; k < sc->heap_size; k++)
	      {
		sc->heap[k] = &cells[k - old_size];
		sc->free_heap[sc->free_heap_top++] = sc->heap[k];
		sc->heap[k]->hloc = k;
	      }
	  }
	  sc->heap[sc->heap_size] = NULL; /* end mark for GC loop */
	}
    }

  p = sc->free_heap[--(sc->free_heap_top)];

#if HAVE_PTHREADS
  set_type(p, T_SIMPLE);
  /* currently the overall allocation of an object is not locked, so we could
   *   return a new cell from new_cell without its fields set yet, set_type to
   *   something non-simple, then before setting the fields, be interrupted.
   *   The mark/gc stuff then sees a typed object (T_PAIR for example), and
   *   tries to mark its cdr (for example) which is probably 0 (not a valid
   *   scheme object).  So set the type temporarily to T_SIMPLE to avoid that.
   *   Ideally, I think, we'd set the type last in all the allocations.
   *
   * I think this is no longer needed.
   */

  nsc->temps[nsc->temps_ctr++] = p;
  if (nsc->temps_ctr >= nsc->temps_size)
    nsc->temps_ctr = 0;
  pthread_mutex_unlock(&alloc_lock);
#else
  sc->temps[sc->temps_ctr++] = p;
  if (sc->temps_ctr >= sc->temps_size)
    sc->temps_ctr = 0;
#endif
  /* originally I tried to mark each temporary value until I was done with it, but
   *   that way madness lies... By delaying GC of _every_ %$^#%@ pointer, I can dispense
   *   with hundreds of individual protections.  Using this array of temps is much faster
   *   than using a type bit to say "newly allocated" because that protects so many cells
   *   betweeen gc calls that we end up calling the gc twice as often overall.
   */

  
  return(p);
}


static s7_pointer g_gc(s7_scheme *sc, s7_pointer args)
{
  #define H_gc "(gc :optional on) runs the garbage collector.  If 'on' is supplied, it turns the GC on or off."

  if (args != sc->NIL)
    {
      (*(sc->gc_off)) = (car(args) == sc->F);
      if (*(sc->gc_off)) return(sc->F);
    }

#if HAVE_PTHREADS
  pthread_mutex_lock(&alloc_lock);
  gc(sc->orig_sc);
  pthread_mutex_unlock(&alloc_lock);
#else
  {
    int i;
    for (i = 0; i < sc->temps_size; i++)
      sc->temps[i] = sc->NIL;
  }
  gc(sc);
#endif
  
  return(sc->UNSPECIFIED);
}


s7_pointer s7_gc_on(s7_scheme *sc, bool on)
{
  (*(sc->gc_off)) = !on;
  return(s7_make_boolean(sc, on));
}


#define NOT_IN_HEAP -1

void s7_remove_from_heap(s7_scheme *sc, s7_pointer x)
{
  int loc;
  /* global functions are very rarely redefined, so we can remove the function body from
   *   the heap when it is defined.  If redefined, we currently lose the memory held by the
   *   old definition.  If this memory leak becomes a problem, we could notice the redefinition 
   *   in add_to_environment, and GC the old body by hand.
   */

  switch (type(x))
    {
    case T_PAIR:
      s7_remove_from_heap(sc, car(x));
      s7_remove_from_heap(sc, cdr(x));
      break;

    case T_UNTYPED:
    case T_NIL:
    case T_BOOLEAN:
      return;

    case T_STRING:
    case T_NUMBER:
    case T_CHARACTER:
      break;

    case T_SYMBOL:
      /* here hloc is usually NOT_IN_HEAP, but in the syntax case can be the syntax op code */
      return;

    case T_CLOSURE:
    case T_CLOSURE_STAR:
    case T_MACRO:
      s7_remove_from_heap(sc, closure_source(x));
      break;

      /* not sure any of these can exist as code-level constants */
    case T_CONTINUATION:
    case T_GOTO:
    case T_INPUT_PORT:
    case T_OUTPUT_PORT:
    case T_CATCH:
    case T_DYNAMIC_WIND:
      break;

    case T_C_OBJECT:
    case T_C_FUNCTION:
    case T_C_MACRO:
    case T_C_POINTER:
      break;

    case T_HASH_TABLE:
    case T_VECTOR:
      {
	s7_Int i;
	for (i = 0; i < vector_length(x); i++)
	  s7_remove_from_heap(sc, vector_element(x, i));
      }
      break;
    }

  loc = x->hloc;
  if (loc != NOT_IN_HEAP)
    {
      x->hloc = NOT_IN_HEAP;
      sc->heap[loc] = (s7_cell *)calloc(1, sizeof(s7_cell));
      sc->free_heap[sc->free_heap_top++] = sc->heap[loc];
      sc->heap[loc]->hloc = loc;
    }
}



/* -------------------------------- stack -------------------------------- */

static void stack_reset(s7_scheme *sc) 
{ 
  sc->stack_top = 0;
} 


static void pop_stack(s7_scheme *sc) 
{ 
  /* avoid "if..then" here and in push_stack -- these 2 are called a zillion times */
  s7_pointer *vel;
  sc->stack_top -= 4;
  vel = (s7_pointer *)(vector_elements(sc->stack) + sc->stack_top);
  sc->code =  vel[0];
  sc->envir = vel[1];
  sc->args =  vel[2];
  sc->op =    (opcode_t)integer(number(vel[3]));
} 


static void push_stack(s7_scheme *sc, opcode_t op, s7_pointer args, s7_pointer code)
{ 
  s7_pointer *vel;
  vel = (s7_pointer *)(vector_elements(sc->stack) + sc->stack_top);
  vel[0] = code;
  vel[1] = sc->envir;
  vel[2] = args;
  vel[3] = sc->small_ints[(int)op];
  sc->stack_top += 4;
}


static void increase_stack_size(s7_scheme *sc)
{
  int i, new_size;
  new_size = sc->stack_size * 2;
  vector_elements(sc->stack) = (s7_pointer *)realloc(vector_elements(sc->stack), new_size * sizeof(s7_pointer));
  for (i = sc->stack_size; i < new_size; i++)
    vector_element(sc->stack, i) = sc->NIL;
  vector_length(sc->stack) = new_size;
  sc->stack_size2 = sc->stack_size;
  sc->stack_size = new_size;
} 


static s7_pointer g_stack(s7_scheme *sc, s7_pointer args)
{
  #define H_stack "(stack :optional continuation-or-thread) returns a list containing the current stack top (an integer) and the stack itself (a vector).\
Each stack frame has 4 entries, the function, the current environment, the function arguments, and an op code used \
internally by the evaluator.  If a continuation is passed, its stack and stack-top are returned."

  if (args != sc->NIL)
    {
      if (s7_is_continuation(car(args)))
	return(make_list_2(sc, s7_make_integer(sc, continuation_stack_top(car(args))), continuation_stack(car(args))));

#if HAVE_PTHREADS
      if (g_is_thread(sc, args) != sc->F)
	{
	  thred *f;
	  f = (thred *)s7_object_value(car(args));
	  return(make_list_2(sc, s7_make_integer(sc, f->sc->stack_top), f->sc->stack));
	}
#endif	 
    }

  return(make_list_2(sc, s7_make_integer(sc, sc->stack_top), sc->stack));
}


#if 0
static void show_stack(s7_scheme *sc)
{
  const char *ops[OP_MAX_DEFINED] = 
    {"read_internal", "eval", "eval_args0", "eval_args1", "apply", "eval_macro", "lambda", "quote", 
     "define0", "define1", "begin", "if0", "if1", "set0", "set1", "set2", "let0", "let1", "let2", 
     "let_star0", "let_star1", "letrec0", "letrec1", "letrec2", "cond0", "cond1", 
     "and0", "and1", "or0", "or1", "defmacro", "defmacro_star", "macro0", "macro1", 
     "define_macro", "define_macro_star", "define_expansion", "expansion", 
     "case0", "case1", "case2", "read_list", "read_dot", "read_quote", "read_quasiquote", "read_quasiquote_vector", 
     "read_unquote", "read_unquote_splicing", "read_vector", "read_return_expression", 
     "read_and_return_expression", "load_return_if_eof", "load_close_and_pop_if_eof", "eval_string", 
     "eval_string_done", "eval_done", "quit", "catch", "dynamic_wind", "for_list_each", "list_map", "define_constant0", 
     "define_constant1", "do", "do_end0", "do_end1", "do_step0", "do_step1", "do_step2", "do_init", "define_star", 
     "lambda_star", "error_quit", "unwind_input", "unwind_output", "trace_return", "error_hook_quit", "trace_hook_quit",
     "with_env0", "with_env1", "with_env2", "vector_for_each", "vector_map0", "vector_map1", "string_for_each", 
     "object_for_each", "hash-table-for-each"};

  int i;
  for (i = sc->stack_top - 4; i >= 0; i -= 4)
    fprintf(stderr, "[%s: (%s %s)]\n", 
	    ops[(int)integer(number(vector_element(sc->stack, i + 3)))],
	    s7_object_to_c_string(sc, vector_element(sc->stack, i + 0)),
	    s7_object_to_c_string(sc, vector_element(sc->stack, i + 2)));
}
#endif	    




/* -------------------------------- symbols -------------------------------- */

static s7_Int hash_table_hash(const char *key, s7_Int table_size) 
{ 
  /* I tried several other hash functions, but they gave about the same incidence of collisions */
  unsigned long long int hashed = 0; 
  const char *c; 
  for (c = key; *c; c++) 
    hashed = *c + hashed * 37;
  return(hashed % table_size); 
} 


static int symbol_table_hash(const char *key, int table_size) 
{ 
  unsigned int hashed = 0;
  const char *c; 
  for (c = key; *c; c++) 
    hashed = *c + hashed * 37;
  return(hashed % table_size); 
  /* using ints here is much faster, and the symbol table will not be enormous, so it's worth splitting out this case */
} 


#if HAVE_PTHREADS
static pthread_mutex_t symtab_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static s7_pointer symbol_table_add_by_name_at_location(s7_scheme *sc, const char *name, int location) 
{ 
  s7_pointer x, str; 
  
  str = s7_make_permanent_string(name);
  x = s7_permanent_cons(str, sc->NIL, T_SYMBOL | T_ATOM | T_SIMPLE | T_DONT_COPY | T_ETERNAL);
  symbol_location(x) = location;
  symbol_global_slot(x) = sc->NIL;

#if HAVE_PTHREADS
  pthread_mutex_lock(&symtab_lock);
#endif

  vector_element(sc->symbol_table, location) = s7_permanent_cons(x, 
								 vector_element(sc->symbol_table, location), 
								 T_PAIR | T_ATOM | T_SIMPLE | T_IMMUTABLE | T_DONT_COPY);
#if HAVE_PTHREADS
  pthread_mutex_unlock(&symtab_lock);
#endif
  
  return(x); 
} 


static s7_pointer symbol_table_add_by_name(s7_scheme *sc, const char *name) 
{
  return(symbol_table_add_by_name_at_location(sc, name, symbol_table_hash(name, vector_length(sc->symbol_table)))); 
}


static  s7_pointer symbol_table_find_by_name(s7_scheme *sc, const char *name, int location) 
{ 
  s7_pointer x; 

#if HAVE_PTHREADS
  pthread_mutex_lock(&symtab_lock);
#endif

  for (x = vector_element(sc->symbol_table, location); x != sc->NIL; x = cdr(x)) 
    { 
      const char *s; 
      s = s7_symbol_name(car(x)); 
      if ((s) && (strcmp(name, s) == 0))
	{
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&symtab_lock);
#endif
	  return(car(x)); 
	}
    } 
#if HAVE_PTHREADS
  pthread_mutex_unlock(&symtab_lock);
#endif

  return(sc->NIL); 
} 


static s7_pointer g_symbol_table(s7_scheme *sc, s7_pointer args)
{
  #define H_symbol_table "(symbol-table) returns the s7 symbol table (a vector)"
  return(sc->symbol_table);
}


void s7_for_each_symbol_name(s7_scheme *sc, bool (*symbol_func)(const char *symbol_name, void *data), void *data)
{
  int i; 
  s7_pointer x; 

#if HAVE_PTHREADS
  pthread_mutex_lock(&symtab_lock);
#endif

  for (i = 0; i < vector_length(sc->symbol_table); i++) 
    for (x  = vector_element(sc->symbol_table, i); x != sc->NIL; x = cdr(x)) 
      if (symbol_func(s7_symbol_name(car(x)), data))
	{
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&symtab_lock);
#endif
	  return;
	}

#if HAVE_PTHREADS
  pthread_mutex_unlock(&symtab_lock);
#endif
}


void s7_for_each_symbol(s7_scheme *sc, bool (*symbol_func)(const char *symbol_name, s7_pointer value, void *data), void *data)
{
  int i; 
  s7_pointer x; 

#if HAVE_PTHREADS
  pthread_mutex_lock(&symtab_lock);
#endif

  for (i = 0; i < vector_length(sc->symbol_table); i++) 
    for (x  = vector_element(sc->symbol_table, i); x != sc->NIL; x = cdr(x)) 
      if (symbol_func(s7_symbol_name(car(x)), cdr(x), data))
	{
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&symtab_lock);
#endif
	  return;
	}

#if HAVE_PTHREADS
  pthread_mutex_unlock(&symtab_lock);
#endif
}


s7_pointer s7_make_symbol(s7_scheme *sc, const char *name) 
{ 
  s7_pointer x; 
  int location;
  location = symbol_table_hash(name, vector_length(sc->symbol_table)); 
  x = symbol_table_find_by_name(sc, name, location); 
  if (x != sc->NIL) 
    return(x); 
  return(symbol_table_add_by_name_at_location(sc, name, location)); 
} 


s7_pointer s7_gensym(s7_scheme *sc, const char *prefix)
{ 
  char *name;
  int len, location;
  s7_pointer x;
  
  len = safe_strlen(prefix) + 32;
  name = (char *)calloc(len, sizeof(char));
  
  for (; (*(sc->gensym_counter)) < LONG_MAX; ) 
    { 
      snprintf(name, len, "%s-%ld", prefix, (*(sc->gensym_counter))++); 
      location = symbol_table_hash(name, vector_length(sc->symbol_table)); 
      x = symbol_table_find_by_name(sc, name, location); 
      if (x != sc->NIL)
	{
	  if (s7_symbol_value(sc, x) != sc->UNDEFINED)
	    continue; 
	  free(name);
	  return(x); 
	}
      
      x = symbol_table_add_by_name_at_location(sc, name, location); 
      free(name);
      return(x); 
    } 
  free(name);
  return(sc->NIL); 
} 


static s7_pointer g_gensym(s7_scheme *sc, s7_pointer args) 
{
  #define H_gensym "(gensym :optional prefix) returns a new (or at least an un-used) symbol"
  if (args != sc->NIL)
    {
      if (!s7_is_string(car(args)))
	return(s7_wrong_type_arg_error(sc, "gensym", 0, car(args), "a string"));
      return(s7_gensym(sc, string_value(car(args))));
    }
  return(s7_gensym(sc, "gensym"));
}


s7_pointer s7_name_to_value(s7_scheme *sc, const char *name)
{
  return(s7_symbol_value(sc, s7_make_symbol(sc, name)));
}


bool s7_is_symbol(s7_pointer p)   
{ 
  return(type(p) == T_SYMBOL);
}


static s7_pointer g_is_symbol(s7_scheme *sc, s7_pointer args)
{
  #define H_is_symbol "(symbol? obj) returns #t if obj is a symbol"
  return(make_boolean(sc, s7_is_symbol(car(args))));
}


const char *s7_symbol_name(s7_pointer p)   
{ 
  return(symbol_name(p));
}


static s7_pointer g_symbol_to_string(s7_scheme *sc, s7_pointer args)
{
  #define H_symbol_to_string "(symbol->string sym) returns the symbol sym converted to a string"
  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "symbol->string", 0, car(args), "a symbol"));
  return(s7_set_immutable(s7_make_string(sc, s7_symbol_name(car(args)))));
}


static s7_pointer g_string_to_symbol(s7_scheme *sc, s7_pointer args)
{
  #define H_string_to_symbol "(string->symbol str) returns the string str converted to a symbol"
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "string->symbol", 0, car(args), "a string"));
  return(s7_make_symbol(sc, string_value(car(args))));
}


void s7_provide(s7_scheme *sc, const char *feature)
{
  char *expr;
  int len;
  len = safe_strlen(feature) + 64;
  expr = (char *)calloc(len, sizeof(char));
  snprintf(expr, len, "(set! *features* (cons '%s *features*))", feature);
  s7_eval_c_string(sc, expr);
  free(expr);
}


#if WITH_PROFILING
static s7_pointer g_symbol_calls(s7_scheme *sc, s7_pointer args)
{
  #define H_symbol_calls "(symbol-calls sym) returns the number of times sym was called (applied)"
  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "symbol-calls", 0, car(args), "a symbol"));    
  return(s7_make_integer(sc, symbol_calls(eval_symbol(sc, car(args)))));
}
#endif



/* -------------------------------- environments -------------------------------- */

static s7_pointer new_frame_in_env(s7_scheme *sc, s7_pointer old_env) 
{ 
  return(s7_cons(sc, sc->NIL, old_env));
} 


static s7_pointer add_to_environment(s7_scheme *sc, s7_pointer env, s7_pointer variable, s7_pointer value) 
{ 
  s7_pointer slot, e;

  e = car(env);
  slot = s7_immutable_cons(sc, variable, value);

  if (s7_is_vector(e))
    {
      int loc;

      if (is_c_function(value))
	s7_remove_from_heap(sc, value);
      else
	{
	  if ((is_closure(value)) ||
	      (is_closure_star(value)) ||
	      (is_macro(value)))
	    s7_remove_from_heap(sc, closure_source(value));
	}

      loc = symbol_location(variable);
      vector_element(e, loc) = s7_cons(sc, slot, vector_element(e, loc));
      symbol_global_slot(variable) = slot;
    }
  else
    {
      s7_pointer x;
#if HAVE_PTHREADS
      x = new_cell(sc); 
#else
      if (sc->free_heap_top > 0)
	x = sc->free_heap[--(sc->free_heap_top)];
      else x = new_cell(sc);
#endif
      car(x) = slot;
      cdr(x) = e;
      set_type(x, T_PAIR);
      car(env) = x;
      set_local(variable);
    }

  return(slot);
} 


static s7_pointer add_to_current_environment(s7_scheme *sc, s7_pointer variable, s7_pointer value) 
{ 
  if (is_immutable(variable))
    return(s7_error(sc, sc->ERROR, make_list_2(sc, s7_make_string(sc, "can't bind or set an immutable object: ~S"), variable)));

  return(add_to_environment(sc, sc->envir, variable, value)); 
} 


static s7_pointer add_to_local_environment(s7_scheme *sc, s7_pointer variable, s7_pointer value) 
{ 
  /* this is called when it is guaranteed that there is a local environment */
  s7_pointer x;

  if (is_immutable(variable))
    return(s7_error(sc, sc->ERROR, make_list_2(sc, s7_make_string(sc, "can't bind or set an immutable object: ~S"), variable)));

#if HAVE_PTHREADS
  x = new_cell(sc); 
#else
  if (sc->free_heap_top > 0)
    x = sc->free_heap[--(sc->free_heap_top)];
  else x = new_cell(sc);
#endif
  car(x) = s7_immutable_cons(sc, variable, value);
  cdr(x) = car(sc->envir);
  set_type(x, T_PAIR);
  car(sc->envir) = x;
  set_local(variable);
  return(car(x));
} 


static s7_pointer s7_find_symbol_in_environment(s7_scheme *sc, s7_pointer env, s7_pointer hdl, bool all_envirs) 
{ 
  s7_pointer x, y;
  /* this is a list (of alists, each representing a frame) ending with a vector (the global environment) */

  for (x = env; is_pair(x); x = cdr(x)) 
    { 
      if (s7_is_vector(car(x)))
	return(symbol_global_slot(hdl));
      
      for (y = car(x); is_pair(y); y = cdr(y))
	if (caar(y) == hdl)
	  return(car(y));

      if (!all_envirs) 
	return(sc->NIL); 
    }
  return(sc->NIL); 
} 


s7_pointer s7_symbol_value(s7_scheme *sc, s7_pointer sym) /* was searching just the global environment? */
{
  s7_pointer x;
  x = s7_find_symbol_in_environment(sc, sc->envir, sym, true);
  if (x != sc->NIL)
    return(symbol_value(x));
  if (s7_is_keyword(sym))
    return(sym);
  return(sc->UNDEFINED);
}


s7_pointer s7_symbol_local_value(s7_scheme *sc, s7_pointer sym, s7_pointer local_env)
{
  s7_pointer x;
  x = s7_find_symbol_in_environment(sc, local_env, sym, true);
  if (x != sc->NIL)
    return(symbol_value(x));
  return(s7_symbol_value(sc, sym)); /* try sc->envir */
}


static s7_pointer g_symbol_to_value(s7_scheme *sc, s7_pointer args)
{
  #define H_symbol_to_value "(symbol->value sym :optional env) returns the binding of (the value associated with) the symbol sym in the given environment"
  s7_pointer local_env;

  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "symbol->value", 1, car(args), "a symbol"));

  if (cdr(args) != sc->NIL)
    {
      if (!is_pair(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "symbol->value", 2, cadr(args), "an environment"));
      local_env = cadr(args);
    }
  else local_env = sc->envir;

  return(s7_symbol_local_value(sc, car(args), local_env));
}


s7_pointer s7_symbol_set_value(s7_scheme *sc, s7_pointer sym, s7_pointer val)
{
  s7_pointer x;
  x = s7_find_symbol_in_environment(sc, sc->envir, sym, true);
  if (x != sc->NIL)
    set_symbol_value(x, val);
  return(val);
}


static bool lambda_star_argument_set_value(s7_scheme *sc, s7_pointer sym, s7_pointer val)
{
  s7_pointer x;
  for (x = car(sc->envir) /* presumably the arglist */; is_pair(x); x = cdr(x))
    if (caar(x) == sym)
      {
	/* car(x) is our binding (symbol value) */
	cdar(x) = val;
	return(true);
      }
  return(false);
}


static s7_pointer lambda_star_argument_default_value(s7_scheme *sc, s7_pointer val)
{
  s7_pointer x;
  if (s7_is_symbol(val))
    {
      x = s7_find_symbol_in_environment(sc, sc->envir, val, true);
      if (x != sc->NIL) 
	return(symbol_value(x));
    }
  if ((is_pair(val)) &&
      (car(val) == sc->QUOTE))
    return(cadr(val));
  return(val);
}


static s7_pointer g_global_environment(s7_scheme *sc, s7_pointer ignore)
{
  #define H_global_environment "(global-environment) returns the current s7 top-level definitions (symbol bindings)"
  return(sc->global_env);
}


#if HAVE_PTHREADS
static s7_pointer thread_environment(s7_scheme *sc, s7_pointer obj);
#define CURRENT_ENVIRONMENT_OPTARGS 1
#else
#define CURRENT_ENVIRONMENT_OPTARGS 0
#endif

static s7_pointer g_current_environment(s7_scheme *sc, s7_pointer args)
{
#if HAVE_PTHREADS
  #define H_current_environment "(current-environment :optional thread) returns the current definitions (symbol bindings)"
  if (args != sc->NIL)
    {
      if (g_is_thread(sc, args) == sc->F)
	return(s7_wrong_type_arg_error(sc, "current-environment", 0, car(args), "a thread object"));
      return(thread_environment(sc, car(args)));
    }
#else
  #define H_current_environment "(current-environment) returns the current definitions (symbol bindings)"
#endif
  return(sc->envir);
}


static s7_pointer make_closure(s7_scheme *sc, s7_pointer c, s7_pointer e, int type) 
{
  /* c is code. e is environment */
  /* this is called every time a lambda form is evaluated, or during letrec, etc */

  s7_pointer x;
  x = new_cell(sc); /* expansion here doesn't save much */
  car(x) = c;
  cdr(x) = e;
  set_type(x, type | T_PROCEDURE);
  return(x);
}


s7_pointer s7_make_closure(s7_scheme *sc, s7_pointer c, s7_pointer e)
{
  /* c is a list: args code, so 
   *   (define (proc a b) (+ a b)) becomes
   *   make_closure ((a b) (+ a b)) e
   */
  return(make_closure(sc, c, e, T_CLOSURE));
}


s7_pointer s7_global_environment(s7_scheme *sc) 
{
  return(sc->global_env);
}


s7_pointer s7_current_environment(s7_scheme *sc) 
{
  return(sc->envir);
}


static s7_pointer g_is_defined(s7_scheme *sc, s7_pointer args)
{
  #define H_is_defined "(defined? obj :optional env) returns #t if obj has a binding (a value) in the environment env"
  s7_pointer x;

  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "defined?", 0, car(args), "a symbol"));
  
  if (is_syntax(car(args)))
    return(sc->T);
  
  if (cdr(args) != sc->NIL)
    {
      if (!is_pair(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "defined?", 2, cadr(args), "an environment"));
      x = cadr(args);
    }
  else x = sc->envir;
  
  x = s7_find_symbol_in_environment(sc, x, car(args), true);
  return(make_boolean(sc, (x != sc->NIL) && (x != sc->UNDEFINED)));
}


void s7_define(s7_scheme *sc, s7_pointer envir, s7_pointer symbol, s7_pointer value) 
{
  s7_pointer x;
  x = s7_find_symbol_in_environment(sc, envir, symbol, false);
  if (x != sc->NIL) 
    set_symbol_value(x, value); 
  else add_to_environment(sc, envir, symbol, value); /* I think this means C code can override "constant" defs */
}


void s7_define_variable(s7_scheme *sc, const char *name, s7_pointer value)
{
  s7_pointer sym;
  
  sym = s7_make_symbol(sc, name);
  s7_define(sc, s7_global_environment(sc), sym, value);
}


void s7_define_constant(s7_scheme *sc, const char *name, s7_pointer value)
{
  s7_pointer x, sym;
  
  sym = s7_make_symbol(sc, name);
  s7_define(sc, s7_global_environment(sc), sym, value);

  x = s7_find_symbol_in_environment(sc, s7_global_environment(sc), sym, false);
  s7_set_immutable(car(x));
}

/*        (define (func a) (let ((cvar (+ a 1))) cvar))
 *        (define-constant cvar 23)
 *        (func 1)
 *        ;can't bind or set an immutable object: cvar
 */


static s7_pointer s7_find_value_in_environment(s7_scheme *sc, s7_pointer val)
{ 
  s7_pointer x, y, vec; 
  for (x = sc->envir; (x != sc->NIL) && (!s7_is_vector(car(x))); x = cdr(x)) 
    for (y = car(x); y != sc->NIL; y = cdr(y)) 
      if (cdr(car(y)) == val)
	return(car(y));
  
  if (s7_is_vector(car(x)))
    {
      int i, len;
      vec = car(x);
      len = vector_length(vec);
      for (i = 0; i < len; i++)
	if (vector_element(vec, i) != sc->NIL)
	  for (y = vector_element(vec, i); y != sc->NIL; y = cdr(y)) 
	    if (cdr(car(y)) == val)
	      return(car(y));
    }
  return(sc->F); 
} 



/* -------- keywords -------- */

bool s7_keyword_eq_p(s7_pointer obj1, s7_pointer obj2)
{
  return(obj1 == obj2);
}


bool s7_is_keyword(s7_pointer obj)
{
  return((s7_is_symbol(obj)) &&
	 ((symbol_name(obj)[0] == ':') ||
	  (symbol_name(obj)[symbol_name_length(obj) - 1] == ':')));
}


static s7_pointer g_is_keyword(s7_scheme *sc, s7_pointer args)
{
  #define H_is_keyword "(keyword? obj) returns #t if obj is a keyword"
  return(make_boolean(sc, s7_is_keyword(car(args))));
}


s7_pointer s7_make_keyword(s7_scheme *sc, const char *key)
{
  s7_pointer sym, x;
  char *name;
  
  name = (char *)malloc((safe_strlen(key) + 2) * sizeof(char));
  sprintf(name, ":%s", key);                     /* prepend ":" */
  sym = s7_make_symbol(sc, name);
  typeflag(sym) |= (T_IMMUTABLE | T_DONT_COPY); 
  free(name);
  
  x = s7_find_symbol_in_environment(sc, sc->envir, sym, true); /* is it already defined? */
  if (x == sc->NIL) 
    add_to_environment(sc, sc->envir, sym, sym); /* its value is itself, skip the immutable check in add_to_current_environment */
  
  return(sym);
}


static s7_pointer g_make_keyword(s7_scheme *sc, s7_pointer args)
{
  #define H_make_keyword "(make-keyword str) prepends ':' to str and defines that as a keyword"
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "make-keyword", 0, car(args), "a string"));
  return(s7_make_keyword(sc, string_value(car(args))));
}


static s7_pointer g_keyword_to_symbol(s7_scheme *sc, s7_pointer args)
{
  #define H_keyword_to_symbol "(keyword->symbol key) returns a symbol with the same name as key but no prepended colon"
  const char *name;

  if (!s7_is_keyword(car(args)))
    return(s7_wrong_type_arg_error(sc, "keyword->symbol", 0, car(args), "a keyword"));

  name = s7_symbol_name(car(args));
  if (name[0] == ':')
    return(s7_make_symbol(sc, (const char *)(name + 1)));

  /* else it ends in ":", (keyword->symbol foo:) */
  {
    char *temp;
    s7_pointer res;
    temp = s7_strdup(name);
    temp[strlen(temp) - 1] = '\0';
    res = s7_make_symbol(sc, (const char *)temp);
    free(temp);
    return(res);
  }
}


static s7_pointer g_symbol_to_keyword(s7_scheme *sc, s7_pointer args)
{
  #define H_symbol_to_keyword "(symbol->keyword sym) returns a keyword with the same name as sym, but with a colon prepended"
  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "symbol->keyword", 0, car(args), "a symbol"));
  return(s7_make_keyword(sc, s7_symbol_name(car(args))));
}


/* for uninterpreted pointers */

bool s7_is_c_pointer(s7_pointer arg) 
{
  return(type(arg) == T_C_POINTER);
}


void *s7_c_pointer(s7_pointer p) 
{
  if ((type(p) == T_NUMBER) && (s7_integer(p) == 0))
    return(NULL); /* special case where the null pointer has been cons'd up by hand */

  if (type(p) != T_C_POINTER)
    fprintf(stderr, "s7_c_pointer argument is a %s\n", s7_type_name(p));

  return(p->object.c_pointer);
}


s7_pointer s7_make_c_pointer(s7_scheme *sc, void *ptr)
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_C_POINTER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  x->object.c_pointer = ptr;
  return(x);
}




/* -------------------------------- continuations and gotos -------------------------------- */

bool s7_is_continuation(s7_pointer p)    
{ 
  return(type(p) == T_CONTINUATION);
}


static s7_pointer g_is_continuation(s7_scheme *sc, s7_pointer args)
{
  #define H_is_continuation "(continuation? obj) returns #t if obj is a continuation"
  return(make_boolean(sc, (s7_is_continuation(car(args))) ||
		          (is_goto(car(args)))));
}


static s7_pointer copy_list(s7_scheme *sc, s7_pointer lst)
{
  if (lst == sc->NIL)
    return(sc->NIL);
  return(s7_cons(sc, car(lst), copy_list(sc, cdr(lst))));
}


static s7_pointer copy_object(s7_scheme *sc, s7_pointer obj)
{
  s7_pointer nobj;

  {
    int nloc;
    nobj = new_cell(sc);
    nloc = nobj->hloc;
    memcpy((void *)nobj, (void *)obj, sizeof(s7_cell));
    nobj->hloc = nloc;
  }
  
  if (dont_copy(car(obj)))
    car(nobj) = car(obj);
  else car(nobj) = copy_object(sc, car(obj));

  if ((dont_copy(cdr(obj))) ||
      (is_closure(obj)) ||
      (is_closure_star(obj)) ||
      (is_macro(obj)) || 
      (s7_is_function(obj)))
    cdr(nobj) = cdr(obj); /* closure_environment in func cases */
  else cdr(nobj) = copy_object(sc, cdr(obj));
  
  return(nobj);
}


static s7_pointer copy_stack(s7_scheme *sc, s7_pointer old_v, int top)
{
  int i;
  s7_pointer new_v;
  s7_pointer *nv, *ov;

  new_v = s7_make_vector(sc, vector_length(old_v));
  /* we can't leave the upper stuff simply malloc-garbage because this object is in the
   *   temps array for a nanosecond or two, and we're sure to call the GC at that moment.
   *   We also can't just copy the vector since that seems to confuse the gc mark process.
   */

  nv = vector_elements(new_v);
  ov = vector_elements(old_v);
  
  s7_gc_on(sc, false);

  for (i = 0; i < top; i += 4)
    {
      if (dont_copy(ov[i]))
	nv[i] = ov[i];
      else nv[i] = copy_object(sc, ov[i]);    /* code */
      nv[i + 1] = ov[i + 1];                  /* environment pointer */
      if (is_pair(ov[i + 2]))                 /* args need not be a list (it can be a port or #f, etc) */
	nv[i + 2] = copy_list(sc, ov[i + 2]); /* args (copy is needed -- see s7test.scm) */
      else nv[i + 2] = ov[i + 2];             /* is this a safe assumption? */
      nv[i + 3] = ov[i + 3];                  /* op (constant int) */
    }
  
  s7_gc_on(sc, true);
  return(new_v);
}


static s7_pointer s7_make_goto(s7_scheme *sc) 
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_ATOM | T_GOTO | T_SIMPLE | T_DONT_COPY | T_PROCEDURE);
  x->object.goto_loc = sc->stack_top;
  return(x);
}


s7_pointer s7_make_continuation(s7_scheme *sc) 
{
  s7_pointer x;

  if (sc->free_heap_top < sc->heap_size / 4)
    gc(sc);
  /* this gc call is needed if there are lots of call/cc's -- by pure bad luck
   *   we can end up hitting the end of the gc free list time after time while
   *   in successive copy_stack's below, causing s7 to core up until it runs out of memory.
   */

  x = new_cell(sc);
  continuation_stack_top(x) = sc->stack_top;
  continuation_stack_size(x) = sc->stack_size;
  continuation_stack(x) = copy_stack(sc, sc->stack, sc->stack_top);
  set_type(x, T_CONTINUATION | T_DONT_COPY | T_PROCEDURE);
  return(x);
}


static void check_for_dynamic_winds(s7_scheme *sc, s7_pointer c)
{
  int i, s_base = 0, c_base = -1;
  
  for (i = sc->stack_top - 1; i > 0; i -= 4)
    {
      s7_pointer x;
      opcode_t op;
      op = (opcode_t)s7_integer(vector_element(sc->stack, i));
      if (op == OP_DYNAMIC_WIND)
	{
	  int j;
	  x = vector_element(sc->stack, i - 3);
	  for (j = 3; j < continuation_stack_top(c); j += 4)
	    if (((opcode_t)s7_integer(vector_element(continuation_stack(c), j)) == OP_DYNAMIC_WIND) &&
		(x == vector_element(continuation_stack(c), j - 3)))
	      {
		s_base = i;
		c_base = j;
		break;
	      }
	  
	  if (s_base != 0)
	    break;	  
	  
	  if (dynamic_wind_state(x) == T_DWIND_BODY)
	    {
	      push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
	      sc->args = sc->NIL;
	      sc->code = dynamic_wind_out(x);
	      eval(sc, OP_APPLY);
	    }
	}
      else
	{
	  if (op == OP_TRACE_RETURN)
	    sc->trace_depth--;
	}
    }
  
  for (i = c_base + 4; i < continuation_stack_top(c); i += 4)
    if ((opcode_t)s7_integer(vector_element(continuation_stack(c), i)) == OP_DYNAMIC_WIND)
      {
	s7_pointer x;
	x = vector_element(continuation_stack(c), i - 3);
	push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
	sc->args = sc->NIL;
	sc->code = dynamic_wind_in(x);
	eval(sc, OP_APPLY);
	dynamic_wind_set_state(x, T_DWIND_BODY);
      }
}


static s7_pointer g_call_cc(s7_scheme *sc, s7_pointer args)
{
  #define H_call_cc "(call-with-current-continuation ...) needs more than a one sentence explanation"
  s7_pointer proc_args;

  /* car(args) is the procedure passed to call/cc */
  
  if (!is_procedure(car(args)))                      /* this includes continuations */
    return(s7_wrong_type_arg_error(sc, "call/cc", 1, car(args), "a procedure"));

  proc_args = s7_procedure_arity(sc, car(args));
  if ((s7_integer(car(proc_args)) > 1) ||
      ((s7_integer(car(proc_args)) == 0) &&
       (s7_integer(cadr(proc_args)) == 0) &&
       (caddr(proc_args) == sc->F)))
    return(s7_error(sc, sc->ERROR, make_list_2(sc, s7_make_string(sc, "call/cc procedure should take one argument"), car(args))));

  sc->code = car(args);
  sc->args = make_list_1(sc, s7_make_continuation(sc));
  push_stack(sc, OP_APPLY, sc->args, sc->code);
  return(sc->NIL);
}


static s7_pointer g_call_with_exit(s7_scheme *sc, s7_pointer args)
{
  #define H_call_with_exit "(call-with-exit ...) is a simplified call/cc"
  
  /* (call-with-exit (lambda (return) ...)) */
  /* perhaps "call/exit"? */
  
  if (!is_procedure(car(args)))                      /* this includes continuations */
    return(s7_wrong_type_arg_error(sc, "call-with-exit", 1, car(args), "a procedure"));

  sc->code = car(args);                              /* the lambda form */
  sc->args = make_list_1(sc, s7_make_goto(sc));      /*   the argument to the lambda (the goto = "return" above) */
  push_stack(sc, OP_APPLY, sc->args, sc->code);      /* apply looks at sc->code to decide what to do (it will see the lambda) */
  
  /* if the lambda body calls the argument as a function, 
   *   it is applied to its arguments, apply notices that it is a goto, and...
   *   
   *      sc->stack_top = (sc->code)->object.goto_loc;           
   *      s_pop(sc, sc->args != sc->NIL ? car(sc->args) : sc->NIL);
   * 
   *   which jumps to the point of the goto returning car(args)
   */
  
  return(sc->NIL);
}



/* -------------------------------- numbers -------------------------------- */

#if WITH_GMP
  static char *big_number_to_string_with_radix(s7_pointer p, int radix);
  static bool big_numbers_are_eqv(s7_pointer a, s7_pointer b);
  static s7_pointer string_to_either_integer(s7_scheme *sc, const char *str, int radix);
  static s7_pointer string_to_either_ratio(s7_scheme *sc, const char *nstr, const char *dstr, int radix);
  static s7_pointer string_to_either_real(s7_scheme *sc, const char *str, int radix);
  static s7_pointer string_to_either_complex(s7_scheme *sc,
					     char *q, char *slash1, char *ex1, bool has_dec_point1, 
					     char *plus, char *slash2, char *ex2, bool has_dec_point2, 
					     int radix, int has_plus_or_minus);
  static s7_pointer big_add(s7_scheme *sc, s7_pointer args);
  static s7_pointer big_subtract(s7_scheme *sc, s7_pointer args);
  static s7_pointer big_multiply(s7_scheme *sc, s7_pointer args);
  static s7_pointer big_divide(s7_scheme *sc, s7_pointer args);
  static s7_pointer big_lcm(s7_scheme *sc, s7_pointer args);
  static s7_pointer big_random(s7_scheme *sc, s7_pointer args);
  static s7_pointer s7_Int_to_big_integer(s7_scheme *sc, s7_Int val);
  static s7_pointer s7_ratio_to_big_ratio(s7_scheme *sc, s7_Int num, s7_Int den);
#endif


#define s7_Int_abs(x) (x >= 0 ? x : -x)
/* can't use abs even in gcc -- it doesn't work with long long ints! */
#define s7_Double_abs(x) fabs(x)
#define s7_fabsl(x) ((x < 0.0) ? -x : x)
/* fabsl doesn't exist in netBSD! */


#ifdef _MSC_VER
/* need to provide inverse hyperbolic trig funcs and cbrt */

double asinh(double x);
double asinh(double x) 
{ 
  return(log(x + sqrt(1.0 + x * x))); 
} 


double acosh(double x);
double acosh(double x)
{ 
  return(log(x + sqrt(x * x - 1.0))); 
  /* perhaps less prone to numerical troubles (untested):
   *   2.0 * log(sqrt(0.5 * (x + 1.0)) + sqrt(0.5 * (x - 1.0)))
   */
} 


double atanh(double x);
double atanh(double x)
{ 
  return(log((1.0 + x) / (1.0 - x)) / 2.0); 
} 


double cbrt(double x);
double cbrt(double x)
{
  if (x >= 0.0)
    return(pow(x, 1.0 / 3.0));
  return(-pow(-x, 1.0 / 3.0));
}
#endif

#if WITH_COMPLEX

#if __cplusplus
  #define _Complex_I (complex<s7_Double>(0.0, 1.0))
  #define creal(x) Real(x)
  #define cimag(x) Imag(x)
  #define carg(x) arg(x)
  #define cabs(x) abs(x)
  #define csqrt(x) sqrt(x)
  #define cpow(x, y) pow(x, y)
  #define clog(x) log(x)
  #define cexp(x) exp(x)
  #define csin(x) sin(x)
  #define ccos(x) cos(x)
  #define csinh(x) sinh(x)
  #define ccosh(x) cosh(x)
#else
  typedef double complex s7_Complex;
#endif

/* Trigonometric functions. FreeBSD's math library does not include the complex form of the trig funcs. */ 

/* FreeBSD supplies cabs carg cimag creal conj csqrt, so can we assume those exist if complex.h exists?
 */

#if 0
static s7_Double carg(s7_Complex z)
{ 
  return(atan2(cimag(z), creal(z))); 
} 


static s7_Double cabs(s7_Complex z) 
{ 
  return(hypot(creal(z), cimag(z))); 
} 


static s7_Complex conj(s7_Complex z) 
{ 
  return(~z); 
} 


static s7_Complex csqrt(s7_Complex z) 
{ 
  if (cimag(z) < 0.0) 
    return(conj(csqrt(conj(z)))); 
  else 
    { 
      s7_Double r = cabs(z); 
      s7_Double x = creal(z); 
      
      return(sqrt((r + x) / 2.0) + sqrt((r - x) / 2.0) * _Complex_I); 
    } 
} 
#endif


#if (!HAVE_COMPLEX_TRIG)

#if (!__cplusplus)
static s7_Complex csin(s7_Complex z) 
{ 
  return(sin(creal(z)) * cosh(cimag(z)) + (cos(creal(z)) * sinh(cimag(z))) * _Complex_I); 
} 


static s7_Complex ccos(s7_Complex z) 
{ 
  return(cos(creal(z)) * cosh(cimag(z)) + (-sin(creal(z)) * sinh(cimag(z))) * _Complex_I); 
} 


static s7_Complex csinh(s7_Complex z) 
{ 
  return(sinh(creal(z)) * cos(cimag(z)) + (cosh(creal(z)) * sin(cimag(z))) * _Complex_I); 
} 


static s7_Complex ccosh(s7_Complex z) 
{ 
  return(cosh(creal(z)) * cos(cimag(z)) + (sinh(creal(z)) * sin(cimag(z))) * _Complex_I); 
} 
#endif


static s7_Complex ctan(s7_Complex z) 
{ 
  return(csin(z) / ccos(z)); 
} 


static s7_Complex ctanh(s7_Complex z) 
{ 
  return(csinh(z) / ccosh(z)); 
} 


#if (!__cplusplus)
static s7_Complex cexp(s7_Complex z) 
{ 
  return(exp(creal(z)) * cos(cimag(z)) + (exp(creal(z)) * sin(cimag(z))) * _Complex_I); 
} 


static s7_Complex clog(s7_Complex z) 
{ 
  return(log(s7_Double_abs(cabs(z))) + carg(z) * _Complex_I); 
} 


static s7_Complex cpow(s7_Complex x, s7_Complex y) 
{ 
  s7_Double r = cabs(x); 
  s7_Double theta = carg(x); 
  s7_Double yre = creal(y); 
  s7_Double yim = cimag(y); 
  s7_Double nr = exp(yre * log(r) - yim * theta); 
  s7_Double ntheta = yre * theta + yim * log(r); 
  
  return(nr * cos(ntheta) + (nr * sin(ntheta)) * _Complex_I); /* make-polar */ 
} 
#endif


static s7_Complex casin(s7_Complex z) 
{ 
  return(-_Complex_I * clog(_Complex_I * z + csqrt(1.0 - z * z))); 
} 


static s7_Complex cacos(s7_Complex z) 
{ 
  return(-_Complex_I * clog(z + _Complex_I * csqrt(1.0 - z * z))); 
} 


static s7_Complex catan(s7_Complex z) 
{ 
  return(_Complex_I * clog((_Complex_I + z) / (_Complex_I - z)) / 2.0); 
} 


static s7_Complex casinh(s7_Complex z) 
{ 
  return(clog(z + csqrt(1.0 + z * z))); 
} 


static s7_Complex cacosh(s7_Complex z) 
{ 
  return(clog(z + csqrt(z * z - 1.0))); 
  /* perhaps less prone to numerical troubles (untested):
   *   2.0 * clog(csqrt(0.5 * (z + 1.0)) + csqrt(0.5 * (z - 1.0)))
   */
} 


static s7_Complex catanh(s7_Complex z) 
{ 
  return(clog((1.0 + z) / (1.0 - z)) / 2.0); 
} 
#endif

#else
/* not WITH_COMPLEX */
  typedef double s7_Complex;
  #define _Complex_I 1
  #define creal(x) x
  #define cimag(x) x
  #define csin(x) sin(x)
  #define casin(x) x
  #define ccos(x) cos(x)
  #define cacos(x) x
  #define ctan(x) x
  #define catan(x) x
  #define csinh(x) x
  #define casinh(x) x
  #define ccosh(x) x
  #define cacosh(x) x
  #define ctanh(x) x
  #define catanh(x) x
  #define cexp(x) exp(x)
  #define cpow(x, y) pow(x, y)
  #define clog(x) log(x)
  #define csqrt(x) sqrt(x)
  #define conj(x) x
#endif


#if (!WITH_GMP)

bool s7_is_number(s7_pointer p)
{
  return(type(p) == T_NUMBER);
}


bool s7_is_integer(s7_pointer p) 
{ 
  if (!(s7_is_number(p)))
    return(false);
  
  return(number_type(p) == NUM_INT);
}


bool s7_is_real(s7_pointer p) 
{ 
  if (!(s7_is_number(p)))
    return(false);
  
  return(number_type(p) < NUM_COMPLEX);
}


bool s7_is_rational(s7_pointer p)
{
  if (!(s7_is_number(p)))
    return(false);
  
  return(number_type(p) <= NUM_RATIO);
}


bool s7_is_ratio(s7_pointer p)
{
  if (!(s7_is_number(p)))
    return(false);
  
  return(number_type(p) == NUM_RATIO);
}


bool s7_is_complex(s7_pointer p)
{
  return(s7_is_number(p));
}

#endif 
/* WITH_GMP */


bool s7_is_exact(s7_pointer p)
{
  return(s7_is_rational(p));
}


bool s7_is_inexact(s7_pointer p)
{
  return(!s7_is_rational(p));
}


static s7_Int c_mod(s7_Int x, s7_Int y)
{
  s7_Int z;
  if (y == 0) return(x); /* else arithmetic exception */
  z = x % y;
  if (((y < 0) && (z > 0)) ||
      ((y > 0) && (z < 0)))
    return(z + y);
  return(z);
}


static s7_Int c_gcd(s7_Int u, s7_Int v)
{
  s7_Int a, b, temp;
  
  a = s7_Int_abs(u);
  b = s7_Int_abs(v);
  while (b != 0)
    {
      temp = a % b;
      a = b;
      b = temp;
    }
  if (a < 0)
    return(-a);
  return(a);
}


static s7_Int c_lcm(s7_Int a, s7_Int b)
{
  if ((a == 0) || (b == 0)) return(0);
  if (a < 0) a = -a;
  if (b < 0) b = -b;
  return((a / c_gcd(a, b)) * b);
}


static bool c_rationalize(s7_Double ux, s7_Double error, s7_Int *numer, s7_Int *denom)
{
  /*
    (define* (rat ux (err 0.0000001))
      ;; translated from CL code in Canny, Donald, Ressler, "A Rational Rotation Method for Robust Geometric Algorithms"
      (let ((x0 (- ux error))
	    (x1 (+ ux error)))
        (let ((i (ceiling x0))
	      (i0 (floor x0))
	      (i1 (ceiling x1))
	      (r 0))
          (if (>= x1 i)
	      i
	      (do ((p0 i0 (+ p1 (* r p0)))
	           (q0 1 (+ q1 (* r q0)))
	           (p1 i1 p0)
	           (q1 1 q0)
	           (e0 (- i1 x0) e1p)
	           (e1 (- x0 i0) (- e0p (* r e1p)))
	           (e0p (- i1 x1) e1)
	           (e1p (- x1 i0) (- e0 (* r e1))))
	          ((<= x0 (/ p0 q0) x1)
	           (/ p0 q0))
	        (set! r (min (floor (/ e0 e1))
			     (ceiling (/ e0p e1p)))))))))
  */
  
  s7_Double x0, x1, val;
  s7_Int i, i0, i1, r, r1, p0, q0, p1, q1;
  s7_Double e0, e1, e0p, e1p;
  s7_Int old_p1, old_q1;
  s7_Double old_e0, old_e1, old_e0p;

  if (error < 0.0) error = -error;

  x0 = ux - error;
  x1 = ux + error;
  i = (s7_Int)ceil(x0);
  
  if (error >= 1.0) /* aw good grief! */
    {
      if (x0 < 0)
	{
	  if (x1 < 0)
	    (*numer) = (s7_Int)floor(x1);
	  else (*numer) = 0;
	}
      else
	{
	  (*numer) = i;
	}
      (*denom) = 1;
      return(true);
    }
  
  if (x1 >= i)
    {
      if (i >= 0)
	(*numer) = i;
      else (*numer) = (s7_Int)floor(x1);
      (*denom) = 1;
      return(true);
    }

  i0 = (s7_Int)floor(x0);
  i1 = (s7_Int)ceil(x1);

  p0 = i0; 
  q0 = 1;
  p1 = i1; 
  q1 = 1; 
  e0 = i1 - x0;
  e1 = x0 - i0;
  e0p = i1 - x1;
  e1p = x1 - i0;

  while (true)
    {
      val = (double)p0 / (double)q0;
      
      if ((x0 <= val) && (val <= x1))
	{
	  (*numer) = p0;
	  (*denom) = q0;
	  return(true);
	}

      r = (s7_Int)floor(e0 / e1);
      r1 = (s7_Int)ceil(e0p / e1p);
      if (r1 < r) r = r1;

      /* Scheme "do" handles all step vars in parallel */
      old_p1 = p1;
      p1 = p0;
      old_q1 = q1;
      q1 = q0;
      old_e0 = e0;
      e0 = e1p;
      old_e0p = e0p;
      e0p = e1;
      old_e1 = e1;

      p0 = old_p1 + r * p0;
      q0 = old_q1 + r * q0;
      e1 = old_e0p - r * e1p;
      e1p = old_e0 - r * old_e1;
    }
  return(false);
}

#if 0
/* there is another way to rationalize.  Here is a scheme version of
 *   Bill Gosper's farint:

(define* (farint x (err 1/1000000))
  
  (define* (farint-1 x nhi dhi (ln 0) (ld 1) (hn 1) (hd 0))
    (if (> (+ ln hn) (* (+ ld hd) x))
	(let* ((m (min (if (= 0 ln) 
			   nhi 
			 (floor (/ (- nhi hn) ln)))
		       (floor (/ (- dhi hd) ld))))
	       (d (- (* x ld) ln))
	       (k (if (= 0 d) 
		      m 
		    (ceiling (/ (- hn (* x hd)) d)))))
	  (if (< k m)
	      (let ((hn1 (+ (* k ln) hn))
		    (hd1 (+ (* k ld) hd)))
		(farint-1 x nhi dhi hn1 hd1 (- hn1 ln) (- hd1 ld)))

	    (let* ((n (+ (* m ln) hn)) (d (+ (* m ld) hd)))
	      (if (< (* 2 d ld x) (+ (* ld n) (* ln d)))
		  (/ ln ld) 
		(/ n d)))))

      (let* ((m (min (floor (/ (- nhi ln) hn))
		     (if (= 0 hd) 
			 dhi 
		       (floor (/ (- dhi ld) hd)))))
	     (d (- hn (* x hd)))
	     (k (if (= 0 d) 
		    m 
		  (ceiling (/ (- (* x ld) ln) d)))))
	(if (< k m)
	    (let ((ln1 (+ (* k hn) ln))
		  (ld1 (+ (* k hd) ld)))
	    (farint-1 x nhi dhi (- ln1 hn) (- ld1 hd) ln1 ld1))
	  (let* ((n (+ (* m hn) ln)) (d (+ (* m hd) ld)))
	    (if (< (* 2 d hd x) (+ (* hd n) (* hn d)))
		(/ n d) 
	      (/ hn hd)))))))

  (farint-1 x (/ err) (/ err)))
*/
#endif


s7_pointer s7_rationalize(s7_scheme *sc, s7_Double x, s7_Double error)
{
  s7_Int numer = 0, denom = 1;
  if (c_rationalize(x, error, &numer, &denom))
    return(s7_make_ratio(sc, numer, denom));
  return(s7_make_real(sc, x));
}


static s7_Double num_to_real(s7_num_t n)
{
  if (n.type >= NUM_REAL)
    return(real(n));
  if (n.type == NUM_INT)
    return((s7_Double)integer(n));
  return(fraction(n));
}


static s7_Int num_to_numerator(s7_num_t n)
{
  if (n.type == NUM_RATIO)
    return(numerator(n));
  return(integer(n));
}


static s7_Int num_to_denominator(s7_num_t n)
{
  if (n.type == NUM_RATIO)
    return(denominator(n));
  return(1);
}


static s7_Double num_to_real_part(s7_num_t n)
{
  switch (n.type)
    {
    case NUM_INT:   return((s7_Double)integer(n));
    case NUM_RATIO: return(fraction(n));
    case NUM_REAL:
    case NUM_REAL2: return(real(n));
    default:        return(real_part(n));
    }
}


static s7_Double num_to_imag_part(s7_num_t n)
{
  if (n.type >= NUM_COMPLEX)
    return(imag_part(n));
  return(0.0);
}


static s7_num_t make_ratio(s7_Int numer, s7_Int denom)
{
  s7_num_t ret;
  s7_Int divisor;

  if (numer == 0)
    {
      ret.type = NUM_INT;
      integer(ret) = 0;
      return(ret);
    }
  
  if (denom < 0)
    {
      numer = -numer;
      denom = -denom;
    }
  
  divisor = c_gcd(numer, denom);
  if (divisor != 1)
    {
      numer /= divisor;
      denom /= divisor;
    }
  
  if (denom == 1)
    {
      ret.type = NUM_INT;
      integer(ret) = numer;
    }
  else
    {
      ret.type = NUM_RATIO;
      numerator(ret) = numer;
      denominator(ret) = denom;
    }
  return(ret);
}


static s7_num_t make_real(s7_Double rl)
{
  s7_num_t ret;
  ret.type = NUM_REAL;
  real(ret) = rl;
  return(ret);
}


static s7_num_t make_complex(s7_Double rl, s7_Double im)
{
  s7_num_t ret;
  if (im == 0.0)
    return(make_real(rl));

  ret.type = NUM_COMPLEX;
  real_part(ret) = rl;
  imag_part(ret) = im;
  return(ret);
}

s7_pointer s7_make_integer(s7_scheme *sc, s7_Int n) 
{
  s7_pointer x;
  if ((n >= 0) && (n < NUM_SMALL_INTS))
    return(small_int(sc, n));
  /* there are ca 6300 -1's in s7test (14500 small negative ints) -- if like sc->real_zero (64000), this would gain us .2% overall speed */
  /*   similarly, between 256 and 512, 8200 or so */

  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  
  number_type(x) = NUM_INT;
  integer(number(x)) = n;
  
  return(x);
}


static s7_pointer make_mutable_integer(s7_scheme *sc, s7_Int n)
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  number_type(x) = NUM_INT;
  integer(number(x)) = n;
  return(x);
}


s7_pointer s7_make_real(s7_scheme *sc, s7_Double n) 
{
  s7_pointer x;
  if (n == 0.0)
    return(sc->real_zero);
  if (n == 1.0)
    return(sc->real_one);

  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  
  number_type(x) = NUM_REAL;
  real(number(x)) = n;
  
  return(x);
}


s7_pointer s7_make_complex(s7_scheme *sc, s7_Double a, s7_Double b)
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  if (b == 0.0)
    {
      number_type(x) = NUM_REAL;
      real(number(x)) = a;
    }
  else
    {
      number_type(x) = NUM_COMPLEX;
      real_part(number(x)) = a;
      imag_part(number(x)) = b;
    }
  return(x);
}


s7_pointer s7_make_ratio(s7_scheme *sc, s7_Int a, s7_Int b)
{
  /* make_number calls us, so we can't call it as a convenience! */
  
  s7_num_t ret;
  s7_pointer x;

  if (b == 0)
    return(s7_division_by_zero_error(sc, "make-ratio", make_list_2(sc, s7_make_integer(sc, a), s7_make_integer(sc, b))));

  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);

  ret = make_ratio(a, b);
  
  number_type(x) = ret.type;
  if (ret.type == NUM_INT)
    integer(number(x)) = numerator(ret);
  else
    {
      numerator(number(x)) = numerator(ret);
      denominator(number(x)) = denominator(ret);
    }
  return(x);
}


static s7_pointer make_number(s7_scheme *sc, s7_num_t n) 
{
  /* using pointers rather than (presumably copied) structs here slowed us down? */
  switch (num_type(n))
    {
    case NUM_INT:     return(s7_make_integer(sc, integer(n)));
    case NUM_RATIO:   return(s7_make_ratio(sc, numerator(n), denominator(n)));
    case NUM_REAL2:
    case NUM_REAL:    return(s7_make_real(sc, real(n)));
    default:          return(s7_make_complex(sc, real_part(n), imag_part(n)));
    }
}


static s7_pointer exact_to_inexact(s7_scheme *sc, s7_pointer x)
{
  if (s7_is_rational(x))
    return(s7_make_real(sc, s7_number_to_real(x)));
  return(x);
}


static double default_rationalize_error = 1.0e-12;

static s7_pointer inexact_to_exact(s7_scheme *sc, s7_pointer x)
{
  if ((s7_is_real(x)) && 
      (!s7_is_rational(x)))
    {
      s7_Int numer = 0, denom = 1;
      if (c_rationalize(s7_real_part(x), default_rationalize_error, &numer, &denom))
	return(s7_make_ratio(sc, numer, denom));
    }
  return(x);
}


#if (!WITH_GMP)
s7_Double s7_number_to_real(s7_pointer x)
{
  switch (number_type(x))
    {
    case NUM_INT:   return((s7_Double)s7_integer(x));
    case NUM_RATIO: return((s7_Double)s7_numerator(x) / (s7_Double)s7_denominator(x));
    case NUM_REAL:
    case NUM_REAL2: return(s7_real(x));
    default:        return(s7_real_part(x));
    }
}


s7_Int s7_numerator(s7_pointer x)
{
  if (number_type(x) == NUM_RATIO)
    return(numerator(number(x)));
  return(integer(number(x)));
}


s7_Int s7_denominator(s7_pointer x)
{
  if (number_type(x) == NUM_RATIO)
    return(denominator(number(x)));
  return(1);
}


s7_Double s7_real_part(s7_pointer x)
{
  return(num_to_real_part(number(x)));
}


s7_Double s7_imag_part(s7_pointer x)
{
  return(num_to_imag_part(number(x)));
}


s7_Int s7_integer(s7_pointer p)
{
  return(integer(number(p)));
}


s7_Double s7_real(s7_pointer p)
{
  return(real(number(p)));
}
#endif


static s7_Complex s7_complex(s7_pointer p)
{
  return(num_to_real_part(number(p)) + num_to_imag_part(number(p)) * _Complex_I);
}


static s7_pointer s7_from_c_complex(s7_scheme *sc, s7_Complex z)
{
  return(s7_make_complex(sc, creal(z), cimag(z)));
}


static s7_num_t num_max(s7_num_t a, s7_num_t b) 
{
  s7_num_t ret;
  ret.type = a.type | b.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      if (integer(a) >= integer(b))
	integer(ret) = integer(a);
      else integer(ret) = integer(b);
      break;
      
    case NUM_RATIO:
      {
	s7_Int vala, valb;
	vala = num_to_numerator(a) / num_to_denominator(a); 
	valb = num_to_numerator(b) / num_to_denominator(b);

	if ((vala > valb) ||
	    ((vala == valb) && (b.type == NUM_INT)))
	  return(a);

	if ((valb > vala) ||
	    ((vala == valb) && (a.type == NUM_INT)))
	  return(b);

	/* sigh -- both are ratios and the int parts are equal */
	if (((double)(num_to_numerator(a) % num_to_denominator(a)) / (double)num_to_denominator(a)) >
	    ((double)(num_to_numerator(b) % num_to_denominator(b)) / (double)num_to_denominator(b)))
	  return(a);
	return(b);
      }
      break;
      
    default:
      if (num_to_real(a) >= num_to_real(b))
	real(ret) = num_to_real(a);
      else real(ret) = num_to_real(b);
      break;
    }
  return(ret);
}


static s7_num_t num_min(s7_num_t a, s7_num_t b) 
{
  s7_num_t ret;
  ret.type = a.type | b.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      if (integer(a) < integer(b))
	integer(ret) = integer(a);
      else integer(ret) = integer(b);
      break;
      
    case NUM_RATIO:
      {
	s7_Int vala, valb;
	vala = num_to_numerator(a) / num_to_denominator(a); 
	valb = num_to_numerator(b) / num_to_denominator(b);

	if ((vala < valb) ||
	    ((vala == valb) && (a.type == NUM_INT)))
	  return(a);

	if ((valb < vala) ||
	    ((vala == valb) && (b.type == NUM_INT)))
	  return(b);

	/* sigh -- both are ratios and the int parts are equal */
	if (((double)(num_to_numerator(a) % num_to_denominator(a)) / (double)num_to_denominator(a)) <
	    ((double)(num_to_numerator(b) % num_to_denominator(b)) / (double)num_to_denominator(b)))
	  return(a);
	return(b);
      }
      break;
      
    default:
      if (num_to_real(a) < num_to_real(b))
	real(ret) = num_to_real(a);
      else real(ret) = num_to_real(b);
      break;
    }
  return(ret);
}


static int integer_length(s7_Int a)
{
  static int bits[256] =
    {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

  #define I_8 256LL
  #define I_16 65536LL
  #define I_24 16777216LL
  #define I_32 4294967296LL
  #define I_40 1099511627776LL
  #define I_48 281474976710656LL
  #define I_56 72057594037927936LL

  if (a < 0) a = -a;
  if (a < I_8) return(bits[a]);
  if (a < I_16) return(8 + bits[a >> 8]);
  if (a < I_24) return(16 + bits[a >> 16]);
  if (a < I_32) return(24 + bits[a >> 24]);
  if (a < I_40) return(32 + bits[a >> 32]);
  if (a < I_48) return(40 + bits[a >> 40]);
  if (a < I_56) return(48 + bits[a >> 48]);
  return(56 + bits[a >> 56]);
}


static int s7_int_max = 0, s7_int_min = 0, s7_int_bits = 0, s7_int_digits = 0; /* initialized later */

static s7_num_t num_add(s7_num_t a, s7_num_t b) 
{
  s7_num_t ret;
  ret.type = a.type | b.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      integer(ret) = integer(a) + integer(b);
      break;
      
    case NUM_RATIO:
      {
	s7_Int d1, d2, n1, n2;
	d1 = num_to_denominator(a);
	n1 = num_to_numerator(a);
	d2 = num_to_denominator(b);
	n2 = num_to_numerator(b);
	if (d1 == d2)                                     /* the easy case -- if overflow here, it matches the int case */
	  return(make_ratio(n1 + n2, d1));                /* d1 can't be zero */

#if (!WITH_GMP)
	if ((d1 > s7_int_max) || (d2 > s7_int_max) ||     /* before counting bits, check that overflow is possible */
	    (n1 > s7_int_max) || (n2 > s7_int_max) ||
	    (n1 < s7_int_min) || (n2 < s7_int_min))
	  {
	    int d1bits, d2bits;
	    d1bits = integer_length(d1);
	    d2bits = integer_length(d2);
	    if (((d1bits + d2bits) > s7_int_bits) ||
		((d1bits + integer_length(n2)) > (s7_int_bits - 1)) ||
		((d2bits + integer_length(n1)) > (s7_int_bits - 1)))
	      return(make_real(((long double)n1 / (long double)d1) + ((long double)n2 / (long double)d2)));
	  }
#endif
	return(make_ratio(n1 * d2 + n2 * d1, d1 * d2));
      }
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      real(ret) = num_to_real(a) + num_to_real(b);
      break;
      
    default:
      /* NUM_COMPLEX is 4 separate types */
      return(make_complex(num_to_real_part(a) + num_to_real_part(b),
			  num_to_imag_part(a) + num_to_imag_part(b)));
      break;
    }
  return(ret);
}


static s7_num_t num_negate(s7_num_t a)
{
  s7_num_t ret;
  ret.type = a.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      integer(ret) = -integer(a);
      break;
      
    case NUM_RATIO:
      numerator(ret) = -numerator(a);
      denominator(ret) = denominator(a);
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      real(ret) = -real(a);
      break;
      
    default:
      real_part(ret) = -real_part(a);
      imag_part(ret) = -imag_part(a);
      break;
    }
  return(ret);
}


static s7_num_t num_sub(s7_num_t a, s7_num_t b) 
{
  s7_num_t ret;
  ret.type = a.type | b.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      integer(ret) = integer(a) - integer(b);
      break;
      
    case NUM_RATIO:
      {
	s7_Int d1, d2, n1, n2;
	d1 = num_to_denominator(a);
	n1 = num_to_numerator(a);
	d2 = num_to_denominator(b);
	n2 = num_to_numerator(b);

	if (d1 == d2)                                     /* the easy case -- if overflow here, it matches the int case */
	  return(make_ratio(n1 - n2, d1));

#if (!WITH_GMP)
	if ((d1 > s7_int_max) || (d2 > s7_int_max) ||     /* before counting bits, check that overflow is possible */
	    (n1 > s7_int_max) || (n2 > s7_int_max) ||
	    (n1 < s7_int_min) || (n2 < s7_int_min))
	  {
	    int d1bits, d2bits;
	    d1bits = integer_length(d1);
	    d2bits = integer_length(d2);
	    if (((d1bits + d2bits) > s7_int_bits) ||
		((d1bits + integer_length(n2)) > (s7_int_bits - 1)) ||
		((d2bits + integer_length(n1)) > (s7_int_bits - 1)))
	      return(make_real(((long double)n1 / (long double)d1) - ((long double)n2 / (long double)d2)));
	  }
#endif
	return(make_ratio(n1 * d2 - n2 * d1, d1 * d2));
      }
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      real(ret) = num_to_real(a) - num_to_real(b);
      break;
      
    default:
      return(make_complex(num_to_real_part(a) - num_to_real_part(b),
			  num_to_imag_part(a) - num_to_imag_part(b)));
      break;
    }
  return(ret);
}


static s7_num_t num_mul(s7_num_t a, s7_num_t b) 
{
  s7_num_t ret;
  ret.type = a.type | b.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      integer(ret) = integer(a) * integer(b);
      break;
      
    case NUM_RATIO:
      {
	s7_Int d1, d2, n1, n2;
	d1 = num_to_denominator(a);
	n1 = num_to_numerator(a);
	d2 = num_to_denominator(b);
	n2 = num_to_numerator(b);
#if (!WITH_GMP)
	if ((d1 > s7_int_max) || (d2 > s7_int_max) ||     /* before counting bits, check that overflow is possible */
	    (n1 > s7_int_max) || (n2 > s7_int_max) ||
	    (n1 < s7_int_min) || (n2 < s7_int_min))
	  {
	    if ((integer_length(d1) + integer_length(d2) > s7_int_bits) ||
		(integer_length(n1) + integer_length(n2) > s7_int_bits))
	      return(make_real(((long double)n1 / (long double)d1) * ((long double)n2 / (long double)d2)));
	  }
#endif
	return(make_ratio(n1 * n2, d1 * d2));
      }
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      real(ret) = num_to_real(a) * num_to_real(b);
      break;
      
    default:
      {
	s7_Double r1, r2, i1, i2;
	r1 = num_to_real_part(a);
	r2 = num_to_real_part(b);
	i1 = num_to_imag_part(a);
	i2 = num_to_imag_part(b);
	return(make_complex(r1 * r2 - i1 * i2, 
			    r1 * i2 + r2 * i1));
      }
      break;
    }
  return(ret);
}

static s7_num_t num_invert(s7_num_t a)
{
  switch (num_type(a))
    {
    case NUM_INT:
      /* a already checked, not 0 */
      return(make_ratio(1, integer(a)));
      
    case NUM_RATIO:
      return(make_ratio(denominator(a), numerator(a)));

    case NUM_REAL:
    case NUM_REAL2:
      return(make_real(1.0 / real(a)));

    default:
      {
	s7_Double r2, i2, den;
	r2 = num_to_real_part(a);
	i2 = num_to_imag_part(a);
	den = (r2 * r2 + i2 * i2);
	return(make_complex(r2 / den, -i2 / den));
      }
    }
}


static s7_num_t num_div(s7_num_t a, s7_num_t b) 
{
  s7_num_t ret;
  ret.type = a.type | b.type;
  
  switch (num_type(ret))
    {
    case NUM_INT: 
      /* b checked for 0 below */
      return(make_ratio(integer(a), integer(b)));
      
    case NUM_RATIO:
      {
	s7_Int d1, d2, n1, n2;
	d1 = num_to_denominator(a);
	n1 = num_to_numerator(a);
	d2 = num_to_denominator(b);
	n2 = num_to_numerator(b);

	if (d1 == d2)
	  return(make_ratio(n1, n2));
#if (!WITH_GMP)
	if ((d1 > s7_int_max) || (d2 > s7_int_max) ||     /* before counting bits, check that overflow is possible */
	    (n1 > s7_int_max) || (n2 > s7_int_max) ||
	    (n1 < s7_int_min) || (n2 < s7_int_min))
	  {
	    if ((integer_length(d1) + integer_length(n2) > s7_int_bits) ||
		(integer_length(d2) + integer_length(n1) > s7_int_bits))
	      return(make_real(((long double)n1 / (long double)d1) / ((long double)n2 / (long double)d2)));
	  }
#endif
	return(make_ratio(n1 * d2, d1 * n2));
      }
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      {
	s7_Double rb;
	rb = num_to_real(b);
	real(ret) = num_to_real(a) / rb;
      }
      break;
      
    default:
      {
	s7_Double r1, r2, i1, i2, den;
	r1 = num_to_real_part(a);
	r2 = num_to_real_part(b);
	i1 = num_to_imag_part(a);
	i2 = num_to_imag_part(b);
	den = (r2 * r2 + i2 * i2);
	return(make_complex((r1 * r2 + i1 * i2) / den, (r2 * i1 - r1 * i2) / den));
      }
      break;
    }
  
  return(ret);
}


static s7_Int s7_truncate(s7_Double xf)
{
  if (xf > 0.0)
    return((s7_Int)floor(xf));
  return((s7_Int)ceil(xf));
}


static s7_num_t num_quotient(s7_num_t a, s7_num_t b) 
{
  /* (define (quo x1 x2) (truncate (/ x1 x2))) ; slib */
  s7_num_t ret;
  ret.type = NUM_INT;
  if ((a.type | b.type) == NUM_INT)
    integer(ret) = integer(a) / integer(b);
  else integer(ret) = s7_truncate(num_to_real(a) / num_to_real(b));
  return(ret);
}


static s7_num_t num_rem(s7_num_t a, s7_num_t b) 
{
  /* (define (rem x1 x2) (- x1 (* x2 (quo x1 x2)))) ; slib */
  s7_num_t ret;
  ret.type = a.type | b.type;
  switch (ret.type)
    {
    case NUM_INT: 
      integer(ret) = integer(a) % integer(b);
      break;

    case NUM_RATIO: 
      return(make_ratio(num_to_numerator(a) * num_to_denominator(b) - 
			num_to_numerator(b) * num_to_denominator(a) * integer(num_quotient(a, b)),
			num_to_denominator(a) * num_to_denominator(b)));
      break;

    default:
      real(ret) = num_to_real(a) - num_to_real(b) * integer(num_quotient(a, b));
      break;
    }
  return(ret);
}


static s7_num_t num_mod(s7_num_t a, s7_num_t b) 
{
  /* (define (mod x1 x2) (- x1 (* x2 (floor (/ x1 x2))))) ; slib */
  s7_num_t ret;
  ret.type = a.type | b.type;
  switch (ret.type)
    {
    case NUM_INT:
      integer(ret) = c_mod(integer(a), integer(b));
      break;

    case NUM_RATIO:
      return(make_ratio(num_to_numerator(a) * num_to_denominator(b) - 
			num_to_numerator(b) * num_to_denominator(a) * (s7_Int)floor(num_to_real(a) / num_to_real(b)),
			num_to_denominator(a) * num_to_denominator(b)));
      break;

    default:
      real(ret) = num_to_real(a) - num_to_real(b) * (s7_Int)floor(num_to_real(a) / num_to_real(b));
      break;
    }
  return(ret);
}


static bool num_eq(s7_num_t a, s7_num_t b) 
{
  switch (num_type(a))
    {
    case NUM_INT:
      switch (num_type(b))
	{
	case NUM_INT: 
	  return(integer(a) == integer(b));
	case NUM_RATIO:
	  return(false);
	case NUM_REAL:
	case NUM_REAL2:
	  return(integer(a) == real(b));
	default: 
	  return((real_part(b) == integer(a)) &&
		 (imag_part(b) == 0.0));
	}
      break;
      
    case NUM_RATIO:  
      switch (num_type(b))
	{
	case NUM_RATIO:
	  return((numerator(a) == numerator(b)) &&
		 (denominator(a) == denominator(b)));
	case NUM_REAL:
	case NUM_REAL2:
	  return(num_to_real(a) == real(b));

	  /* this gives some possibly confusing results:

              :(= 245850922/78256779 (angle -1))
              #t
	      :(= 884279719003555/281474976710656 (angle -1))
              #t
              :(= 245850922/78256779 884279719003555/281474976710656)
              #f
	      
	     should I rationalize the float before the comparison?  These results are not incorrect (pace r6rs) --
	     if you use a float, it infects the rest of the numbers, as anywhere else.
	  */
	default:
	  return(false);
	}
      break;
      
    case NUM_REAL2:
    case NUM_REAL:    
      switch (num_type(b))
	{
	case NUM_INT:
	  return(real(a) == integer(b));
	case NUM_RATIO:
	  return(real(a) == num_to_real(b));
	case NUM_REAL:
	case NUM_REAL2:
	  return(real(a) == real(b));
	default:
	  return((real_part(b) == real(a)) &&
		 (imag_part(b) == 0.0));
	}
      break;
      
    default:
      switch (num_type(b))
	{
	case NUM_INT:
	  return((real_part(a) == integer(b)) &&
		 (imag_part(a) == 0.0));
	case NUM_RATIO:
	  return((real_part(a) == num_to_real(b)) &&
		 (imag_part(a) == 0.0));
	case NUM_REAL:
	case NUM_REAL2:
	  return((real_part(a) == real(b)) &&
		 (imag_part(a) == 0.0));
	default:
	  return((real_part(a) == real_part(b)) &&
		 (imag_part(a) == imag_part(b)));
	}
      break;
    }
  return(false);
}


static bool num_gt(s7_num_t a, s7_num_t b) 
{
  /* the ">" operator here is a problem.
   *   we get different results depending on the gcc optimization level for cases like (< 1234/11 1234/11)
   *   so, to keep ratios honest, we'll use num_sub and compare against 0.  But that can cause problems:
   *   :(> 0 most-negative-fixnum)
   *   #f
   */
  s7_num_t val;
  if ((num_type(a) == NUM_INT) && 
      (num_type(b) == NUM_INT))
    return(integer(a) > integer(b));

  val = num_sub(a, b);
  switch (num_type(val))
    {
    case NUM_INT:   return(integer(val) > 0);
    case NUM_RATIO: return(numerator(val) > 0);
    default:        return(real(val) > 0.0);
    }

#if 0  
  /* this is the flakey version */
  if ((num_type(a) == NUM_INT) &&
      (num_type(b) == NUM_INT))
    return(integer(a) > integer(b));
  return(num_to_real(a) > num_to_real(b));
#endif
}


static bool num_lt(s7_num_t a, s7_num_t b) 
{
  s7_num_t val;
  if ((num_type(a) == NUM_INT) && 
      (num_type(b) == NUM_INT))
    return(integer(a) < integer(b));

  val = num_sub(a, b);
  switch (num_type(val))
    {
    case NUM_INT:   return(integer(val) < 0);
    case NUM_RATIO: return(numerator(val) < 0);
    default:        return(real(val) < 0.0);
    }
}


static bool num_ge(s7_num_t a, s7_num_t b) 
{
  return(!num_lt(a, b));
}


static bool num_le(s7_num_t a, s7_num_t b) 
{
  return(!num_gt(a, b));
}


static s7_Double round_per_R5RS(s7_Double x) 
{
  s7_Double fl = floor(x);
  s7_Double ce = ceil(x);
  s7_Double dfl = x - fl;
  s7_Double dce = ce - x;
  
  if (dfl > dce) return(ce);
  if (dfl < dce) return(fl);
  if (fmod(fl, 2.0) == 0.0) return(fl);
  return(ce);
}


static bool s7_is_negative(s7_pointer obj)
{
  switch (number_type(obj))
    {
    case NUM_INT:   return(s7_integer(obj) < 0);
    case NUM_RATIO: return(s7_numerator(obj) < 0);
    default:        return(s7_real(obj) < 0);
    }
}


static bool s7_is_positive(s7_pointer x)
{
  switch (number_type(x))
    {
    case NUM_INT:   return(s7_integer(x) > 0);
    case NUM_RATIO: return(s7_numerator(x) > 0);
    default:        return(s7_real(x) > 0);
    }
}


static bool s7_is_zero(s7_pointer x)
{
  switch (number_type(x))
    {
    case NUM_INT:   return(s7_integer(x) == 0);
    case NUM_REAL2:
    case NUM_REAL:  return(s7_real(x) == 0.0);
    default:        return(false); /* ratios and complex numbers here are already collapsed into integers and reals */
    }
}


static bool s7_is_one(s7_pointer x)
{
  switch (number_type(x))
    {
    case NUM_INT:   return(s7_integer(x) == 1);
    case NUM_REAL2:
    case NUM_REAL:  return(s7_real(x) == 1.0);
    default:        return(false);
    }
}


static void s7_Int_to_string(char *p, s7_Int n, int radix, int width)
{
  static char dignum[] = "0123456789abcdef";
  int i = 2, len, start = 0, end = 0;
  s7_Int pown = (s7_Int)1;
  bool sign;

  if ((radix < 2) || (radix > 16))
    return;
  if (n == 0)
    {
      p[0] = '0';
      p[1] = '\0';
      return;
    }

  sign = (n < 0);
  n = s7_Int_abs(n); /* most-negative-fixnum loses... */

  /* the previous version that counted up to n, rather than dividing down below n, as here,
   *   could be confused by large ints on 64 bit machines
   */
  pown = n;
  for (i = 1; i < 100; i++)
    {
      if (pown < radix)
	break;
      pown /= (s7_Int)radix;
    }
  len = i - 1;

  if (sign) len++;

  if (width > len) /* (format #f "~10B" 123) */
    {
      start = width - len - 1;
      end += start;
      for (i = 0; i < start; i++) 
	p[i] = ' ';
    }

  if (sign)
    {
      p[start] = '-';
      end++;
    }

  for (i = start + len; i >= end; i--)
    {
      p[i] = dignum[n % (s7_Int)radix];
      n /= (s7_Int)radix;
    }
  p[len + start + 1] = '\0';
}


static char *s7_number_to_string_base_10(s7_pointer obj, int width, int precision, char float_choice)
{
  char *p;

#if WITH_GMP
  if (is_c_object(obj))
    return(big_number_to_string_with_radix(obj, 10));
#endif

  switch (number_type(obj))
    {
    case NUM_INT:
      p = (char *)malloc(64 * sizeof(char));
      snprintf(p, 64, 
	       (sizeof(int) >= sizeof(s7_Int)) ? "%*d" : "%*lld",
	       width, s7_integer(obj));
      break;
      
    case NUM_RATIO:
      p = (char *)malloc(128 * sizeof(char));
      snprintf(p, 128,
	       (sizeof(int) >= sizeof(s7_Int)) ? "%d/%d" : "%lld/%lld", 
	       s7_numerator(obj), s7_denominator(obj));
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      {
	int i, loc = -1, len;
	const char *frmt;
	p = (char *)malloc(256 * sizeof(char));

	if (sizeof(double) >= sizeof(s7_Double))
	  frmt = (float_choice == 'g') ? "%*.*g" : ((float_choice == 'f') ? "%*.*f" : "%*.*e");
	else frmt = (float_choice == 'g') ? "%*.*Lg" : ((float_choice == 'f') ? "%*.*Lf" : "%*.*Le");

	len = snprintf(p, 256, frmt, width, precision, s7_real(obj));
	for (i = 0; i < len; i++) /* does it have an exponent (if so, it's already a float) */
	  if (p[i] == 'e')
	    {
	      loc = i;
	      break;
	    }
	if (loc == -1)            /* no, so make it explicitly a float! */
	  {
	    for (i = 0; i < len; i++)  
	      if (p[i] == '.') break;
	    if (i == len)
	      {
		p[i]='.';
		p[i+1]='0';
		p[i+2]='\0';
	      }
	  }
      }
      break;
      
    default:
      {
	const char *frmt;
	p = (char *)malloc(256 * sizeof(char));

	if (sizeof(double) >= sizeof(s7_Double))
	  {
	    if (s7_imag_part(obj) >= 0.0)
	      frmt = (float_choice == 'g') ? "%.*g+%.*gi" : ((float_choice == 'f') ? "%.*f+%.*fi" : "%.*e+%.*ei"); 
	    else frmt = (float_choice == 'g') ? "%.*g%.*gi" : ((float_choice == 'f') ? "%.*f-%.*fi" :"%.*e-%.*ei");
	  }
	else 
	  {
	    if (s7_imag_part(obj) >= 0.0)
	      frmt = (float_choice == 'g') ? "%.*Lg+%.*Lgi" : ((float_choice == 'f') ? "%.*Lf+%.*Lfi" : "%.*Le+%.*Lei");
	    else frmt = (float_choice == 'g') ? "%.*Lg%.*Lgi" : ((float_choice == 'f') ? "%.*Lf-%.*Lfi" : "%.*Le-%.*Lei");
	  }

	snprintf(p, 256, frmt, precision, s7_real_part(obj), precision, s7_imag_part(obj));
      }
      break;
    }
  return(p);
}


static char *s7_number_to_string_with_radix(s7_scheme *sc, s7_pointer obj, int radix, int width, int precision, char float_choice)
{
  char *p, *n, *d;

#if WITH_GMP
  if (is_c_object(obj))
    return(big_number_to_string_with_radix(obj, radix));
#endif

  if (radix == 10)
    return(s7_number_to_string_base_10(obj, width, precision, float_choice));

  switch (number_type(obj))
    {
    case NUM_INT:
      p = (char *)malloc(128 * sizeof(char));
      s7_Int_to_string(p, s7_integer(obj), radix, width);
      break;
      
    case NUM_RATIO:
      {
	n = (char *)malloc(128 * sizeof(char));
	d = (char *)malloc(128 * sizeof(char));
	s7_Int_to_string(n, s7_numerator(obj), radix, 0);
	s7_Int_to_string(d, s7_denominator(obj), radix, 0);
	p = (char *)malloc(256 * sizeof(char));
	snprintf(p, 256, "%s/%s", n, d);
	free(n);
	free(d);
      }
      break;
      
    case NUM_REAL2:
    case NUM_REAL:
      {
	char *n, *d;
	int i;
	s7_Int int_part;
	s7_Double x, frac_part, min_frac, base;
	bool sign = false;
	x = s7_real(obj);
	if (x < 0.0)
	  {
	    sign = true;
	    x = -x;
	  }
	int_part = (s7_Int)floor(x);
	frac_part = x - int_part;
	n = (char *)malloc(128 * sizeof(char));
	s7_Int_to_string(n, int_part, radix, 0);
	d = (char *)malloc((precision + 1) * sizeof(char));

	min_frac = (s7_Double)pow((s7_Double)radix, (s7_Double)(-precision));
	for (i = 0, base = radix; (i < precision) && (frac_part > min_frac); i++, base *= radix)
	  {
	    int_part = (int)(frac_part * base);
	    frac_part -= (int_part / base);
	    if (int_part < 10)
	      d[i] = (char)('0' + int_part);
	    else d[i] = (char)('a' + int_part -  10);
	  }
	if (i == 0)
	  d[i++] = '0';
	d[i] = '\0';
	p = (char *)malloc(256 * sizeof(char));
	snprintf(p, 256, "%s%s.%s", (sign) ? "-" : "", n, d);
	free(n);
	free(d);
      }
      break;

    default:
      p = (char *)malloc(512 * sizeof(char));
      n = s7_number_to_string_with_radix(sc, s7_make_real(sc, s7_real_part(obj)), radix, width, precision, float_choice);
      d = s7_number_to_string_with_radix(sc, s7_make_real(sc, s7_imag_part(obj)), radix, width, precision, float_choice);
      snprintf(p, 512, "%s%s%si", n, (s7_imag_part(obj) < 0.0) ? "" : "+", d);
      free(n);
      free(d);
      break;
      
    }
  return(p);
}


char *s7_number_to_string(s7_scheme *sc, s7_pointer obj, int radix)
{
  return(s7_number_to_string_with_radix(sc, obj, radix, 0, 20, 'g')); /* (log top 10) so we get all the digits */
}


static s7_pointer g_number_to_string(s7_scheme *sc, s7_pointer args)
{
  #define H_number_to_string "(number->string num :optional (radix 10)) converts the number num into a string"
  s7_Int radix = 0;
  int size = 20;
  char *res;
  s7_pointer x;

  x = car(args);

  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "number->string", 1, x, "a number"));

  if (number_type(x) > NUM_RATIO)
    {
      /* if size = 20, (number->string .1) gives "0.10000000000000000555", but if it's less than 20,
       *    large numbers (or very small numbers) mess up the less significant digits.
       */
      if (number_type(x) < NUM_COMPLEX)
	{
	  s7_Double val;
	  val = s7_Double_abs(s7_real(x));
	  if ((val < (LONG_MAX / 4)) && (val > 1.0e-6))
	    size = 14;
	}
      else
	{
	  s7_Double rl, im;
	  rl = s7_Double_abs(s7_real_part(x));
	  if ((rl < (LONG_MAX / 4)) && (rl > 1.0e-6))
	    {
	      im = s7_Double_abs(s7_imag_part(x));
	      if ((im < (LONG_MAX/4)) && (im > 1.0e-6))
		size = 14;
	    }
	}
    }

  if (is_pair(cdr(args)))
    {
      if (s7_is_integer(cadr(args)))
	radix = s7_integer(cadr(args));
      if ((radix < 2) || (radix > 16))
	return(s7_out_of_range_error(sc, "number->string", 2, cadr(args), "radix should be between 2 and 16"));

      res = s7_number_to_string_with_radix(sc, x, radix, 0, (radix == 10) ? size : 20, 'g');
    }
  else res = s7_number_to_string_base_10(x, 0, size, 'g');
  
  {
    s7_pointer y;
    y = s7_make_string(sc, res);
    free(res);
    return(y);
  }
}


#define CTABLE_SIZE 128
static bool *dot_table, *exponent_table, *slashify_table, *string_delimiter_table;
static int *digits, *char_nums;

static void init_ctables(void)
{
  int i;

  dot_table = (bool *)calloc(CTABLE_SIZE, sizeof(bool));
  exponent_table = (bool *)calloc(CTABLE_SIZE, sizeof(bool));
  slashify_table = (bool *)calloc(CTABLE_SIZE, sizeof(bool));
  string_delimiter_table = (bool *)calloc(CTABLE_SIZE, sizeof(bool));
  
  dot_table['\n'] = true;
  dot_table['\t'] = true;
  dot_table[' '] = true;
  dot_table[')'] = true; /* see note under TOKEN_DOT in the token function */
  dot_table['('] = true;
  dot_table['#'] = true;
  dot_table['\\'] = true;
  dot_table['\''] = true;
  
  exponent_table['e'] = true; exponent_table['E'] = true;
  exponent_table['s'] = true; exponent_table['S'] = true; 
  exponent_table['f'] = true; exponent_table['F'] = true;
  exponent_table['d'] = true; exponent_table['D'] = true;
  exponent_table['l'] = true; exponent_table['L'] = true;

  for (i = 0; i < ' '; i++)
    slashify_table[i] = true;
  slashify_table['\n'] = false;
  slashify_table['\\'] = true;
  slashify_table['"'] = true;

  for (i = 1; i < CTABLE_SIZE; i++)
    string_delimiter_table[i] = true;
  string_delimiter_table[0] = false;
  string_delimiter_table['('] = false;
  string_delimiter_table[')'] = false;
  string_delimiter_table[';'] = false;
  string_delimiter_table['\t'] = false;
  string_delimiter_table['\n'] = false;
  string_delimiter_table['\r'] = false;
  string_delimiter_table[' '] = false;

  digits = (int *)malloc(CTABLE_SIZE * sizeof(int));
  char_nums = (int *)calloc(CTABLE_SIZE, sizeof(int));
  for (i = 0; i < CTABLE_SIZE; i++)
    digits[i] = 256;

  digits['0'] = 1; digits['1'] = 1; digits['2'] = 2; digits['3'] = 3; digits['4'] = 4;
  digits['5'] = 5; digits['6'] = 6; digits['7'] = 7; digits['8'] = 8; digits['9'] = 9;
  digits['a'] = 10; digits['A'] = 10;
  digits['b'] = 11; digits['B'] = 11;
  digits['c'] = 12; digits['C'] = 12;
  digits['d'] = 13; digits['D'] = 13;
  digits['e'] = 14; digits['E'] = 14;
  digits['f'] = 15; digits['F'] = 15;

  char_nums['0'] = 0; char_nums['1'] = 1; char_nums['2'] = 2; char_nums['3'] = 3; char_nums['4'] = 4;
  char_nums['5'] = 5; char_nums['6'] = 6; char_nums['7'] = 7; char_nums['8'] = 8; char_nums['9'] = 9;
  char_nums['a'] = 10; char_nums['A'] = 10;
  char_nums['b'] = 11; char_nums['B'] = 11;
  char_nums['c'] = 12; char_nums['C'] = 12;
  char_nums['d'] = 13; char_nums['D'] = 13;
  char_nums['e'] = 14; char_nums['E'] = 14;
  char_nums['f'] = 15; char_nums['F'] = 15;
}


static bool is_radix_prefix(char prefix)
{ /* perhaps include caps here */
  return((prefix == 'b') ||
	 (prefix == 'd') ||
	 (prefix == 'x') ||
	 (prefix == 'o'));
}


static s7_pointer make_sharp_constant(s7_scheme *sc, char *name, bool at_top) 
{
  /* name is the stuff after the '#', return sc->NIL if not a recognized #... entity */
  int len;
  s7_pointer x;

  if (strcmp(name, "t") == 0)
    return(sc->T);
  
  if (strcmp(name, "f") == 0)
    return(sc->F);
  
  len = strlen(name);
  if (len == 0)
    return(sc->NIL);

  if (len < 2)          /* #<any other char> (except ':', sigh) is an error in this scheme */
    return(sc->NIL);
      
  switch (name[0])
    {
    case 'o':   /* #o (octal) */
    case 'd':   /* #d (decimal) */
    case 'x':   /* #x (hex) */
    case 'b':   /* #b (binary) */
      {
	bool to_inexact = false, to_exact = false;
	int num_at = 1;
  
	if (name[1] == '#')
	  {
	    if (!at_top)
	      return(sc->NIL);
	    if ((len > 2) && ((name[2] == 'e') || (name[2] == 'i'))) /* r6rs includes caps here */
	      {
		if ((len > 3) && (name[3] == '#'))
		  return(sc->NIL);
		to_inexact = (name[2] == 'i');
		to_exact = (name[2] == 'e');
		num_at = 3;
	      }
	    else return(sc->NIL);
	  }
	x = make_atom(sc, (char *)(name + num_at), (name[0] == 'o') ? 8 : ((name[0] == 'x') ? 16 : ((name[0] == 'b') ? 2 : 10)), false);

	/* #x#i1 apparently makes sense, so #x1.0 should also be accepted */
	if (!s7_is_number(x))
	  return(sc->NIL);

	if ((s7_imag_part(x) != 0.0) ||
	    ((!to_exact) && (!to_inexact)))
	  return(x);

	if (to_exact)
	  return(inexact_to_exact(sc, x));
	return(exact_to_inexact(sc, x));
      }

    case 'i':   /* #i<num> = ->inexact (see token for table of choices here) */
      if (name[1] == '#')
	{
	  if (is_radix_prefix(name[2]))
	    {
	      s7_pointer i_arg;
	      i_arg = make_sharp_constant(sc, (char *)(name + 2), false);
	      if (!s7_is_number(i_arg))
		return(sc->NIL);
	      return(exact_to_inexact(sc, i_arg));
	    }
	  return(sc->NIL);
	}
      x = make_atom(sc, (char *)(name + 1), 10, false);
      if (s7_is_number(x))
	return(exact_to_inexact(sc, x));
      return(sc->NIL);
  
    case 'e':   /* #e<num> = ->exact */
      if (name[1] == '#')
	{
	  if (is_radix_prefix(name[2]))
	    {
	      s7_pointer i_arg;
	      i_arg = make_sharp_constant(sc, (char *)(name + 2), false);
	      if (!s7_is_number(i_arg))
		return(sc->NIL);
	      return(inexact_to_exact(sc, i_arg));
	    }
	  return(sc->NIL);
	}
      x = make_atom(sc, (char *)(name + 1), 10, false);
      if (s7_is_number(x))
	return(inexact_to_exact(sc, x));
      return(sc->NIL);

    case '\\':
      /* #\space or whatever (named character) */
      { 
	int c = 0;
	if (strcmp(name + 1, "space") == 0) 
	  c =' ';
	else 
	  {
	    if ((strcmp(name + 1, "newline") == 0) || (strcmp(name + 1, "linefeed") == 0))
	      c ='\n';
	    else 
	      {
		if (strcmp(name + 1, "return") == 0) 
		  c ='\r';
		else 
		  {
		    if (strcmp(name + 1, "tab") == 0) 
		      c ='\t';
		    else 
		      {
			if ((strcmp(name + 1, "null") == 0) || (strcmp(name + 1, "nul") == 0))
			  c ='\0';
			else 
			  {
			    if ((name[1] == 'x') && 
				(name[2] != 0))          /* #\x is just x, but apparently #\x<num> is int->char? #\x65 -> #\e */
			      {
				int c1 = 0;
				if ((sscanf(name + 2, "%x", &c1) == 1) && 
				    (c1 < 256))
				  c = c1;
				else return(sc->NIL);    /* #\xx -> "undefined sharp expression" */
			      }
			    else 
			      {
				if (name[2] == 0) 
				  c = name[1];
				else return(sc->NIL);
			      }}}}}}
	return(s7_make_character(sc, c));
      }
    }
  return(sc->NIL);
}


static s7_Double string_to_double_with_radix(char *str, int radix)
{
  /* (do ((i 2 (+ i 1))) ((= i 17)) (display (inexact->exact (floor (log (expt 2 62) i)))) (display ", ")) */
  static int digits[] = {0, 0, 62, 39, 31, 26, 23, 22, 20, 19, 18, 17, 17, 16, 16, 15, 15};

  int i, len, iloc = 0, floc = -1, eloc = -1, sign = 1, flen = 0;
  long long int int_part = 0, frac_part = 0, exponent = 0;

  /* there's an ambiguity in number notation here if we allow "1e1" or "1.e1" in base 16 (or 15) -- is e a digit or an exponent marker?
   *   but 1e+1, for example disambiguates it -- kind of messy! -- the scheme spec says "e" can only occur in base 10. 
   *   mpfr says "e" as exponent only in bases <= 10 -- else use '@' which works in any base.  This can only cause confusion
   *   in scheme, unfortunately, due to the idiotic scheme polar notation.  So... here the exponent can occur only in
   *   bases <= 10.
   */

  if ((str[0] == '+') || (str[0] == '-'))
    {
      iloc = 1;
      if (str[0] == '-') sign = -1;
    }

  len = safe_strlen(str);
  for (i = iloc; i < len; i++)
    if (str[i] == '.')
      floc = i;
    else
      if ((str[i] == 'e') && (radix <= 10))
	eloc = i;

  if ((floc == -1) || (floc > iloc))
    {
      errno = 0;
      int_part = strtoll((const char *)(str + iloc), (char **)NULL, radix);
      if (errno == ERANGE)
	return(0.0);
    }

  if ((floc != -1) &&          /* has a "." */
      (floc < len - 1) &&      /* doesn't end in "." */
      (floc != eloc - 1))      /* "1.e1" for example */
    {
      flen = ((eloc == -1) ? (len - floc - 1) : (eloc - floc - 1));
      if (flen <= digits[radix])
	{
	  errno = 0;
	  frac_part = strtoll((const char *)(str + floc + 1), (char **)NULL, radix);
	  if (errno == ERANGE)
	    return(0.0);
	}
      else
	{
	  char old_c;
	  /* we need to ignore extra trailing digits here to avoid later overflow */
	  /* (string->number "3.1415926535897932384626433832795029" 11) */
	  flen = digits[radix];
	  old_c = str[floc + flen + 1];
	  str[floc + flen + 1] = '\0';
	  errno = 0;
	  frac_part = strtoll((const char *)(str + floc + 1), (char **)NULL, radix);
	  if (errno == ERANGE)
	    return(0.0);
	  str[floc + flen + 1] = old_c;
	}
    }

  if (eloc != -1)
    {
      errno = 0;
      exponent = strtoll((const char *)(str + eloc + 1), (char **)NULL, 10); /* the exponent is in  base 10 */
      if ((exponent > 100) || (exponent < -100))
	errno = ERANGE;
      if (errno == ERANGE)
	return(0.0);
    }

  /* (string->number "1.1e1" 2) */
  if (frac_part > 0)
    return(sign * (int_part + ((s7_Double)frac_part / pow((s7_Double)radix, (s7_Double)flen))) * pow((s7_Double)radix, (s7_Double)exponent));
  if (exponent != 0)
    return(sign * (int_part * pow((s7_Double)radix, (s7_Double)exponent)));
  return(sign * int_part);
}


/* make symbol or number atom from string */

static s7_pointer make_atom(s7_scheme *sc, char *q, int radix, bool want_symbol) 
{
  #define ISDIGIT(Chr, Rad) (digits[(int)Chr] < Rad)

  char c, *p, *slash1 = NULL, *slash2 = NULL, *plus = NULL, *ex1 = NULL, *ex2 = NULL;
  bool has_dec_point1 = false, has_i = false, has_dec_point2 = false;
  int has_plus_or_minus = 0, current_radix;

#if (!WITH_GMP)
  #define ATOLL(x) strtoll(x, (char **)NULL, radix)
  #define ATOF(x) ((radix == 10) ? atof(x) : string_to_double_with_radix(x, radix))
#endif

  current_radix = radix;
  p = q;
  c = *p++; 
  
  /* a number starts with + - . or digit, but so does 1+ for example */
  
  if (c == '#')
    return(make_sharp_constant(sc, p, true)); /* make_sharp_constant expects the '#' to be removed */
  
  if ((c == '+') || (c == '-')) 
    { 
      c = *p++; 
      if (c == '.') 
	{ 
	  has_dec_point1 = true; 
	  c = *p++; 
	} 
      if (!ISDIGIT(c, current_radix))
	return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);
    } 
  else 
    {
      if (c == '.')         /* .0 */
	{ 
	  has_dec_point1 = true; 
	  c = *p++; 

	  if (!ISDIGIT(c, current_radix))
	    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 
	} 
      else 
	{
	  if (!ISDIGIT(c, current_radix))
	    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 
	}
    }
  
  for ( ; (c = *p) != 0; ++p) 
    {
      if (!ISDIGIT(c, current_radix)) 
	{
	  current_radix = radix;

	  if (c =='.') 
	    {
	      if (((has_dec_point1) ||
		   (slash1)) &&
		  (has_plus_or_minus == 0)) /* 1.. or 1/2. */
		return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 

	      if (((has_dec_point2) ||
		   (slash2)) &&
		  (has_plus_or_minus != 0)) /* 1+1.. or 1+1/2. */
		return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 

	      if ((!ISDIGIT(p[1], current_radix)) &&
		  (!ISDIGIT(p[-1], current_radix))) 
		return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 
	      
	      if (has_plus_or_minus == 0)
		has_dec_point1 = true;
	      else has_dec_point2 = true;
	      continue;
	    }
	  else 
	    {
	      if (exponent_table[(int)c])
		{
		  if (current_radix > 10)
		    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 
		  /* see note above */

		  current_radix = 10;

		  if (((ex1) ||
		       (slash1)) &&
		      (has_plus_or_minus == 0)) /* ee */
		    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 

		  if (((ex2) ||
		       (slash2)) &&
		      (has_plus_or_minus != 0)) /* 1+1.0ee */
		    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 

		  if ((!ISDIGIT(p[-1], current_radix)) &&
		      (p[-1] != '.'))
		    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F); 

		  if (has_plus_or_minus == 0)
		    {
		      ex1 = p;
		      has_dec_point1 = true; /* decimal point illegal from now on */
		    }
		  else 
		    {
		      ex2 = p;
		      has_dec_point2 = true;
		    }
		  p++;
		  if ((*p == '-') || (*p == '+')) p++;
		  if (ISDIGIT(*p, current_radix))
		    continue;
		}
	      else
		{
		  if ((c == '+') || (c == '-'))
		    {
		      if (has_plus_or_minus != 0) /* already have the separator */
			return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);
		      
		      if (c == '+') has_plus_or_minus = 1; else has_plus_or_minus = -1;
		      plus = (char *)(p + 1);
		      continue;
		    }
		  else
		    {
		      if (c == '/')
			{
			  if ((has_plus_or_minus == 0) &&
			      ((ex1) ||
			       (slash1) ||
			       (has_dec_point1)))
			    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);

			  if ((has_plus_or_minus != 0) &&
			      ((ex2) ||
			       (slash2) ||
			       (has_dec_point2)))
			    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);
			  
			  if (has_plus_or_minus == 0)
			    slash1 = (char *)(p + 1);
			  else slash2 = (char *)(p + 1);

			  if ((!ISDIGIT(p[1], current_radix)) ||
			      (!ISDIGIT(p[-1], current_radix)))
			    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);

			  continue;
			}
		      else
			{
			  if ((has_plus_or_minus != 0) && 
			      (!has_i) && 
			      (c == 'i'))
			    {
			      has_i = true;
			      continue;
			    }
			}
		    }
		}
	    }
	  return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);
	}
    }
  
  if ((has_plus_or_minus != 0) &&
      (!has_i))
    return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);

  if (has_i)
    {
#if (!WITH_GMP)
      s7_Double rl = 0.0, im = 0.0;
#endif
      s7_pointer result;
      int len;
      char *saved_q;
      len = strlen(q);
      
      if (q[len - 1] != 'i')
	return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);

      saved_q = s7_strdup_with_len(q, len);

      /* look for cases like 1+i */
      if ((q[len - 2] == '+') || (q[len - 2] == '-'))
	q[len - 1] = '1';
      else q[len - 1] = '\0'; /* remove 'i' */
      
      if (ex1) (*ex1) = 'e';
      if (ex2) (*ex2) = 'e';
      (*((char *)(plus - 1))) = '\0';

#if (!WITH_GMP)
      if ((has_dec_point1) ||
	  (ex1))
	{
	  /* (string->number "1100.1+0.11i" 2) -- need to split into 2 honest reals before passing to non-base-10 ATOF */
	  rl = ATOF(q);
	}
      else
	{
	  if (slash1)
	    rl = (s7_Double)ATOLL(q) / (s7_Double)ATOLL(slash1);
	  else rl = (s7_Double)ATOLL(q);
	}
      if (rl == -0.0) rl = 0.0;

      if ((has_dec_point2) ||
	  (ex2))
	im = ATOF(plus);
      else
	{
	  if (slash2)
	    im = (s7_Double)ATOLL(plus) / (s7_Double)ATOLL(slash2);
	  else im = (s7_Double)ATOLL(plus);
	}
      if ((has_plus_or_minus == -1) && 
	  (im != 0.0))
	im = -im;
      result = s7_make_complex(sc, rl, im);
#else
      result = string_to_either_complex(sc, q, slash1, ex1, has_dec_point1, plus, slash2, ex2, has_dec_point2, radix, has_plus_or_minus);
#endif

      memcpy((void *)q, (void *)saved_q, len);
      free(saved_q);
      return(result);
    }

  
  if ((has_dec_point1) ||
      (ex1))
    {
      s7_pointer result;
      char old_e = 0;

      if (slash1)  /* not complex, so slash and "." is not a number */
	return((want_symbol) ? s7_make_symbol(sc, q) : sc->F);

      if (ex1)
	{
	  old_e = (*ex1);
	  (*ex1) = 'e';
	}

#if (!WITH_GMP)
      result = s7_make_real(sc, ATOF(q));
#else
      result = string_to_either_real(sc, q, radix);
#endif
      
      if (ex1)
	(*ex1) = old_e;

      return(result);
    }
  
  if (slash1)
#if (!WITH_GMP)
    return(s7_make_ratio(sc, ATOLL(q), ATOLL(slash1)));
#else
    return(string_to_either_ratio(sc, q, slash1, radix));
#endif
  
#if (!WITH_GMP)
  return(s7_make_integer(sc, ATOLL(q)));
#else
  return(string_to_either_integer(sc, q, radix));
#endif
}


static s7_pointer s7_string_to_number(s7_scheme *sc, char *str, int radix)
{
  s7_pointer x;
  x = make_atom(sc, str, radix, true);
  if (s7_is_number(x))
    return(x);
  return(sc->F);
}


static s7_pointer g_string_to_number(s7_scheme *sc, s7_pointer args)
{
  #define H_string_to_number "(string->number str :optional (radix 10)) converts str into a number"
  s7_Int radix = 0;

  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "string->number", 1, car(args), "a string"));

  if (!(string_value(car(args))))
    return(sc->F);

  if (is_pair(cdr(args)))
    {
      if (!s7_is_integer(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "string->number", 2, cadr(args), "an integer"));

      if (s7_is_integer(cadr(args)))
	radix = s7_integer(cadr(args));
      if ((radix < 2) ||              /* what about negative int as base (Knuth), reals such as phi, and some complex like -1+i */
	  (radix > 36))               /* the only problem here is printing the number; perhaps put each digit in "()" in base 10: (123)(0)(34) */
	return(s7_out_of_range_error(sc, "string->number", 2, cadr(args), "radix should be between 2 and 36"));
    }
  else radix = 10;

  return(s7_string_to_number(sc, string_value(car(args)), radix));
}


static bool numbers_are_eqv(s7_pointer a, s7_pointer b)
{
  if (number_type(a) != number_type(b)) /* (eqv? 1 1.0) -> #f! */
    return(false);

  switch (number_type(a))
    {
    case NUM_INT:
      return((integer(number(a)) == integer(number(b))));
  
    case NUM_RATIO:
      return((numerator(number(a)) == numerator(number(b))) &&
	     (denominator(number(a)) == denominator(number(b))));

    case NUM_REAL:
    case NUM_REAL2:
      return(real(number(a)) == real(number(b)));
  
    default:
      return((real_part(number(a)) == real_part(number(b))) &&
	     (imag_part(number(a)) == imag_part(number(b))));
    }
  
  return(false);
}


static s7_pointer g_make_polar(s7_scheme *sc, s7_pointer args)
{
  s7_Double ang, mag;
  #define H_make_polar "(make-polar mag ang) returns a complex number with magnitude mag and angle ang"
  
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "make-polar", 1, car(args), "a real"));
  if (!s7_is_real(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "make-polar", 2, cadr(args), "a real"));
  
  mag = num_to_real(number(car(args)));
  ang = num_to_real(number(cadr(args)));
  if (ang == 0.0)
    return(s7_make_real(sc, mag));
  if (ang == M_PI)
    return(s7_make_real(sc, -mag));
  return(s7_make_complex(sc, mag * cos(ang), mag * sin(ang)));
}


static s7_pointer g_make_rectangular(s7_scheme *sc, s7_pointer args)
{
  #define H_make_rectangular "(make-rectangular x1 x2) returns a complex number with real-part x1 and imaginary-part x2"
  
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "make-rectangular", 1, car(args), "a real"));
  if (!s7_is_real(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "make-rectangular", 2, cadr(args), "a real"));
  
  return(s7_make_complex(sc, 
			 num_to_real(number(car(args))), 
			 num_to_real(number(cadr(args)))));
}


static s7_pointer g_abs(s7_scheme *sc, s7_pointer args)
{
  #define H_abs "(abs x) returns the absolute value of the real number x"
  s7_num_t n;

  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "abs", 0, car(args), "a real"));

  n = number(car(args));
  if (num_type(n) >= NUM_REAL)
    real(n) = s7_Double_abs(real(n));
  else
    {
      if (num_type(n) == NUM_INT)
	integer(n) = s7_Int_abs(integer(n));
      else numerator(n) = s7_Int_abs(numerator(n));
    }
  return(make_number(sc, n));      
}


static s7_pointer g_magnitude(s7_scheme *sc, s7_pointer args)
{
  #define H_magnitude "(magnitude z) returns the magnitude of z"
  s7_num_t n;

  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "magnitude", 0, car(args), "a number"));

  n = number(car(args));
  if (num_type(n) < NUM_COMPLEX)
    return(g_abs(sc, args));
  
  return(s7_make_real(sc, hypot(imag_part(n), real_part(n))));
}


static s7_pointer g_angle(s7_scheme *sc, s7_pointer args)
{
  #define H_angle "(angle z) returns the angle of z"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "angle", 0, x, "a number"));

  if (!s7_is_real(x))
    return(s7_make_real(sc, atan2(s7_imag_part(x), s7_real_part(x))));
  if (num_to_real(number(x)) < 0.0)
    return(s7_make_real(sc, M_PI));
  if (number_type(x) <= NUM_RATIO)
    return(small_int(sc, 0));
  return(sc->real_zero);
}


static s7_pointer g_rationalize(s7_scheme *sc, s7_pointer args)
{
  #define H_rationalize "(rationalize x err) returns the ratio with lowest denominator within err of x"
  s7_Double err;
  s7_Int numer = 0, denom = 1;
  s7_pointer x;

  x = car(args);
  if (!s7_is_real(x))
    return(s7_wrong_type_arg_error(sc, "rationalize", 1, x, "a real"));

  if (cdr(args) != sc->NIL)
    {
      if (!s7_is_real(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "rationalize", 2, cadr(args), "a real"));
      err = s7_number_to_real(cadr(args));
    }
  else err = default_rationalize_error;

  if (c_rationalize(s7_number_to_real(x), err, &numer, &denom))
    return(s7_make_ratio(sc, numer, denom));
  return(sc->F);
}


static s7_pointer g_exp(s7_scheme *sc, s7_pointer args)
{
  #define H_exp "(exp z) returns e^z, (exp 1) is 2.718281828459"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "exp", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, exp(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, cexp(s7_complex(x))));
}


static s7_pointer g_log(s7_scheme *sc, s7_pointer args)
{
  #define H_log "(log z1 :optional z2) returns log(z1) / log(z2) where z2 (the base) defaults to e: (log 8 2) = 3"
  s7_pointer x;
  
  x = car(args);

  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "log", 1, x, "a number"));

  if ((is_pair(cdr(args))) &&
      (!(s7_is_number(cadr(args)))))
    return(s7_wrong_type_arg_error(sc, "log", 2, cadr(args), "a number"));
  
  if (is_pair(cdr(args)))
    {
      s7_pointer y;

      y = cadr(args);
      if ((s7_is_zero(y)) || (s7_is_one(y)))
	return(s7_out_of_range_error(sc, "log", 2, y, "base can't be 0.0 or 1.0"));
      
      if ((s7_is_real(x)) &&
	  (s7_is_real(y)) &&
	  (s7_is_positive(x)) &&
	  (s7_is_positive(y)))
	{
	  if ((s7_is_integer(x)) &&
	      (s7_is_integer(y)))
	    {
	      s7_Double res;
	      s7_Int ires;
	      res = log(num_to_real(number(x))) / log(num_to_real(number(y)));
	      ires = (s7_Int)res;
	      if (res - ires == 0.0)
		return(s7_make_integer(sc, ires));
	      return(s7_make_real(sc, res));
	    }
	  return(s7_make_real(sc, log(num_to_real(number(x))) / log(num_to_real(number(y)))));
	}
      return(s7_from_c_complex(sc, clog(s7_complex(x)) / clog(s7_complex(y))));
    }

  if (s7_is_real(x))
    {
      if (s7_is_positive(x))
	return(s7_make_real(sc, log(num_to_real(number(x)))));
      return(s7_make_complex(sc, log(-num_to_real(number(x))), M_PI));
    }

  return(s7_from_c_complex(sc, clog(s7_complex(x))));
}


static s7_pointer g_sin(s7_scheme *sc, s7_pointer args)
{
  #define H_sin "(sin z) returns sin(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "sin", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, sin(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, csin(s7_complex(x))));
}


static s7_pointer g_cos(s7_scheme *sc, s7_pointer args)
{
  #define H_cos "(cos z) returns cos(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "cos", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, cos(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, ccos(s7_complex(x))));
}


static s7_pointer g_tan(s7_scheme *sc, s7_pointer args)
{
  #define H_tan "(tan z) returns tan(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "tan", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, tan(num_to_real(number(x)))));

  if (s7_imag_part(x) > 350.0)
    return(s7_make_complex(sc, 0.0, 1.0));
  if (s7_imag_part(x) < -350.0)
    return(s7_make_complex(sc, 0.0, -1.0));

  return(s7_from_c_complex(sc, ctan(s7_complex(x))));
}


static s7_pointer g_asin(s7_scheme *sc, s7_pointer args)
{
  #define H_asin "(asin z) returns asin(z); (sin (asin 1)) = 1"
  s7_pointer n;

  n = car(args);
  if (!s7_is_number(n))
    return(s7_wrong_type_arg_error(sc, "asin", 0, n, "a number"));

  if (s7_is_real(n))
    {
      s7_Double x, absx, recip;
      s7_Complex result;
      x = num_to_real(number(n));
      absx = s7_Double_abs(x);
      if (absx <= 1.0)
	return(s7_make_real(sc, asin(x)));
      
      /* otherwise use maxima code: */
      recip = 1.0 / absx;
      result = (M_PI / 2.0) - (_Complex_I * clog(absx * (1.0 + (sqrt(1.0 + recip) * csqrt(1.0 - recip)))));
      if (x < 0.0)
	return(s7_from_c_complex(sc, -result));
      return(s7_from_c_complex(sc, result));
    }

#if WITH_COMPLEX
  /* if either real or imag part is very large, use explicit formula, not casin */
  /*   this code taken from sbcl's src/code/irrat.lisp */
  /* break is around x+70000000i */

  if ((s7_Double_abs(s7_real_part(n)) > 1.0e7) ||
      (s7_Double_abs(s7_imag_part(n)) > 1.0e7))
    {
      s7_Complex sq1mz, sq1pz, z;

      z = s7_complex(n);
      sq1mz = csqrt(1.0 - z);
      sq1pz = csqrt(1.0 + z);
      return(s7_make_complex(sc, 
			     atan(s7_real_part(n) / creal(sq1mz * sq1pz)),
			     asinh(cimag(sq1pz * conj(sq1mz)))));
    }
#endif

  return(s7_from_c_complex(sc, casin(s7_complex(n))));
}


static s7_pointer g_acos(s7_scheme *sc, s7_pointer args)
{
  #define H_acos "(acos z) returns acos(z); (cos (acos 1)) = 1"
  s7_pointer n;

  n = car(args);
  if (!s7_is_number(n))
    return(s7_wrong_type_arg_error(sc, "acos", 0, n, "a number"));

  if (s7_is_real(n))
    {
      s7_Double x, absx, recip;
      s7_Complex result;
      x = num_to_real(number(n));
      absx = s7_Double_abs(x);
      if (absx <= 1.0)
	return(s7_make_real(sc, acos(x)));
      
      /* else follow maxima again: */
      recip = 1.0 / absx;
      if (x > 0.0)
	result = _Complex_I * clog(absx * (1.0 + (sqrt(1.0 + recip) * csqrt(1.0 - recip))));
      else result = M_PI - _Complex_I * clog(absx * (1.0 + (sqrt(1.0 + recip) * csqrt(1.0 - recip))));
      return(s7_from_c_complex(sc, result));
    }

#if WITH_COMPLEX
  /* if either real or imag part is very large, use explicit formula, not cacos */
  /*   this code taken from sbcl's src/code/irrat.lisp */

  if ((s7_Double_abs(s7_real_part(n)) > 1.0e7) ||
      (s7_Double_abs(s7_imag_part(n)) > 1.0e7))
    {
      s7_Complex sq1mz, sq1pz, z;

      z = s7_complex(n);
      sq1mz = csqrt(1.0 - z);
      sq1pz = csqrt(1.0 + z);
      return(s7_make_complex(sc, 
			     2.0 * atan(creal(sq1mz) / creal(sq1pz)),
			     asinh(cimag(sq1mz * conj(sq1pz)))));
    }
#endif

  return(s7_from_c_complex(sc, cacos(s7_complex(n))));
}


static s7_pointer g_atan(s7_scheme *sc, s7_pointer args)
{
  #define H_atan "(atan z) returns atan(z)"
  s7_pointer x, y;

  x = car(args);
  if (!is_pair(cdr(args)))
    {
      if (!s7_is_number(x))
	return(s7_wrong_type_arg_error(sc, "atan", 1, x, "a number"));
      if (s7_is_real(x))
	return(s7_make_real(sc, atan(num_to_real(number(x)))));
      return(s7_from_c_complex(sc, catan(s7_complex(x))));
    } 

  y = cadr(args);
  if (!s7_is_real(x))
    return(s7_wrong_type_arg_error(sc, "atan", 1, x, "a real"));
  if (!s7_is_real(y))
    return(s7_wrong_type_arg_error(sc, "atan", 2, y, "a real"));

  return(s7_make_real(sc, atan2(num_to_real(number(x)), 
				num_to_real(number(y)))));
}  


static s7_pointer g_sinh(s7_scheme *sc, s7_pointer args)
{
  #define H_sinh "(sinh z) returns sinh(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "sinh", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, sinh(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, csinh(s7_complex(x))));
}


static s7_pointer g_cosh(s7_scheme *sc, s7_pointer args)
{
  #define H_cosh "(cosh z) returns cosh(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "cosh", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, cosh(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, ccosh(s7_complex(x))));
}


static s7_pointer g_tanh(s7_scheme *sc, s7_pointer args)
{
  #define H_tanh "(tanh z) returns tanh(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "tanh", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, tanh(num_to_real(number(x)))));
  if (s7_real_part(x) > 350.0)
    return(sc->real_one);           /* closer than 0.0 which is what ctanh is about to return! */
  if (s7_real_part(x) < -350.0)
    return(s7_make_real(sc, -1.0)); /* closer than -0.0 which is what ctanh is about to return! */

  return(s7_from_c_complex(sc, ctanh(s7_complex(x))));
}


static s7_pointer g_asinh(s7_scheme *sc, s7_pointer args)
{
  #define H_asinh "(asinh z) returns asinh(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "asinh", 0, x, "a number"));

  if (s7_is_real(x))
    return(s7_make_real(sc, asinh(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, casinh(s7_complex(x))));
}


static s7_pointer g_acosh(s7_scheme *sc, s7_pointer args)
{
  #define H_acosh "(acosh z) returns acosh(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "acosh", 0, x, "a number"));

  if ((s7_is_real(x)) &&
      (num_to_real(number(x)) >= 1.0))
    return(s7_make_real(sc, acosh(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, cacosh(s7_complex(x))));
}


static s7_pointer g_atanh(s7_scheme *sc, s7_pointer args)
{
  #define H_atanh "(atanh z) returns atanh(z)"
  s7_pointer x;

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "atanh", 0, x, "a number"));

  if ((s7_is_real(x)) &&
      (s7_Double_abs(num_to_real(number(x))) < 1.0))
    return(s7_make_real(sc, atanh(num_to_real(number(x)))));
  return(s7_from_c_complex(sc, catanh(s7_complex(x))));
}


static s7_pointer g_sqrt(s7_scheme *sc, s7_pointer args)
{
  #define H_sqrt "(sqrt z) returns sqrt(z)"
  s7_pointer n;

  n = car(args);
  if (!s7_is_number(n))
    return(s7_wrong_type_arg_error(sc, "sqrt", 0, n, "a number"));

  if (s7_is_real(n))
    {
      s7_Double x, sqx;
      x = num_to_real(number(n));
      if (x >= 0.0)
	{
	  sqx = sqrt(x);
	  if (s7_is_integer(n))
	    {
	      s7_Int ix;
	      ix = (s7_Int)sqx;
	      if ((ix * ix) == integer(number(n)))
		return(s7_make_integer(sc, ix));
	    }
	  if (s7_is_ratio(n))
	    {
	      s7_Int nm = 0, dn = 1;
	      if (c_rationalize(sqx, 1.0e-12, &nm, &dn))
		{
		  if ((nm * nm == s7_numerator(n)) &&
		      (dn * dn == s7_denominator(n)))
		    return(s7_make_ratio(sc, nm, dn));
		}
	    }
	  return(s7_make_real(sc, sqx));
	}
    }
  return(s7_from_c_complex(sc, csqrt(s7_complex(n))));
}


static s7_Int int_to_int(s7_Int x, s7_Int n)
{
  /* from GSL */
  s7_Int value = 1;
  do {
    if (n & 1) value *= x;
    n >>= 1;
    x *= x;
  } while (n);
  return(value);
}


static long long int nth_roots[63] = {
  LLONG_MAX, LLONG_MAX, 3037000499LL, 2097151, 55108, 6208, 1448, 511, 234, 127, 78, 52, 38, 28, 22, 
  18, 15, 13, 11, 9, 8, 7, 7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

static long int_nth_roots[31] = {
  LONG_MAX, LONG_MAX, 46340, 1290, 215, 73, 35, 21, 14, 10, 8, 7, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

static bool int_pow_ok(s7_Int x, s7_Int y)
{
  if (s7_int_bits > 31)
    return((y < 63) &&
	   (nth_roots[y] >= s7_Int_abs(x)));
  return((y < 31) &&
	 (int_nth_roots[y] >= s7_Int_abs(x)));
}


static s7_pointer g_expt(s7_scheme *sc, s7_pointer args)
{
  #define H_expt "(expt z1 z2) returns z1^z2"
  s7_pointer n, pw;
  
  n = car(args);
  pw = cadr(args);

  if (!s7_is_number(n))
    return(s7_wrong_type_arg_error(sc, "expt", 1, n, "a number"));
  if (!s7_is_number(pw))
    return(s7_wrong_type_arg_error(sc, "expt", 2, pw, "a number"));

  /* this provides more than 2 args to expt:
   *  if (cddr(args) != sc->NIL)
   *    return(g_expt(sc, make_list_2(sc, car(args), g_expt(sc, cdr(args)))));
   *
   * but it's unusual in scheme to process args in reverse order, and the
   * syntax by itself is ambiguous (does (expt 2 2 3) = 256 or 64?)
   */

  if (s7_is_zero(n))
    {
      if (s7_is_zero(pw))
	{
	  if ((s7_is_integer(n)) && (s7_is_integer(pw)))       /* (expt 0 0) -> 1 */
	    return(s7_make_integer(sc, 1));
	  return(sc->real_zero);                               /* (expt 0.0 0) -> 0.0 */
	}

      if ((s7_is_real(pw)) && (s7_is_negative(pw)))            /* (expt 0 -1) */
	return(s7_division_by_zero_error(sc, "expt", args));   /* what about (expt 0 -1+i)? */

      if ((s7_is_integer(n)) && (s7_is_integer(pw)))           /* pw != 0, (expt 0 2312) */
	return(s7_make_integer(sc, 0));
      return(sc->real_zero);                                   /* (expt 0.0 123123) */
    }

  if (s7_is_one(pw))
    {
      if (s7_is_integer(pw))
	return(n);
      if (number_type(n) <= NUM_RATIO)
	return(s7_make_real(sc, num_to_real(number(n))));
      return(n);
    }
  
  if (number_type(pw) == NUM_INT)
    {
      s7_Int y;
      y = s7_integer(pw);
      if (y == 0)
	{
	  if ((number_type(n) == NUM_INT) || (number_type(n) == NUM_RATIO))
	    return(small_int(sc, 1));
	  return(sc->real_one);
	}

      if (number_type(n) == NUM_INT)
	{
	  s7_Int x;
	  x = s7_integer(n);
	  if (x == 1)
	    return(n);
	  
	  if (x == -1)
	    {
	      if (s7_Int_abs(y) & 1)
		return(n);
	      return(small_int(sc, 1));
	    }

	  if (int_pow_ok(x, s7_Int_abs(y)))
	    {
	      if (y > 0)
		return(s7_make_integer(sc, int_to_int(x, y)));
	      return(s7_make_ratio(sc, 1, int_to_int(x, -y)));
	    }
	}
      else
	{
	  if (number_type(n) == NUM_RATIO)
	    {
	      s7_Int nm, dn;
	      
	      nm = numerator(number(n));
	      dn = denominator(number(n));

	      if ((int_pow_ok(nm, s7_Int_abs(y))) &&
		  (int_pow_ok(dn, s7_Int_abs(y))))
		{
		  if (y > 0)
		    return(s7_make_ratio(sc, int_to_int(nm, y), int_to_int(dn, y)));
		  return(s7_make_ratio(sc, int_to_int(dn, -y), int_to_int(nm, -y)));
		}
	    }
	  /* occasionally int^rat can be int but it happens so infrequently it's probably not worth checking */
	  /* but... it's sad that 
	   *    :(expt -8 1/3)
	   *    1+1.7320508075689i ; a real always has a real cbrt -- would be nice to stick to that
	   *                       ; expts x y [x and y rational] -> a list of values?
	   * or even
	   *    :(sqrt 1/9)
	   *    1/3
	   *    :(expt 1/9 1/2) ; see below -- this is fixed now
	   *    0.33333333333333
	   *    :(expt 1/32 1/5)
	   *    0.5
	   *    :(expt 4 -1/2)
	   *    0.5
	   *   add internal cbrt y=1/3 etc?
	   */
	}
    }

  if ((s7_is_real(n)) &&
      (s7_is_real(pw)))
    {
      s7_Double x, y;

      if ((number_type(pw) == NUM_RATIO) &&
	  (numerator(number(pw)) == 1))
	{
	  if (denominator(number(pw)) == 2)
	    return(g_sqrt(sc, args));
	  if (denominator(number(pw)) == 3)
	    return(s7_make_real(sc, cbrt(num_to_real(number(n))))); /* (expt 27 1/3) should be 3, not 3.0... */
	  /* and 4 -> sqrt(sqrt...) etc? */
	}

      x = num_to_real(number(n));
      y = num_to_real(number(pw));
      if (y == 0.0)
	return(sc->real_one);
      if ((x > 0.0) ||
	  ((y - floor(y)) < 1.0e-16))
	return(s7_make_real(sc, pow(x, y)));
    }

  return(s7_from_c_complex(sc, cpow(s7_complex(n), s7_complex(pw))));
}


static s7_pointer g_floor(s7_scheme *sc, s7_pointer args)
{
  #define H_floor "(floor x) returns the integer closest to x toward -inf"
  s7_pointer x;

  x = car(args);
  if (!s7_is_real(x))
    return(s7_wrong_type_arg_error(sc, "floor", 0, x, "a real"));

  switch (number_type(x))
    {
    case NUM_INT:   
      return(x);

    case NUM_RATIO: 
      {
	s7_Int val;
	val = numerator(number(x)) / denominator(number(x)); 
	/* C "/" truncates? -- C spec says "truncation toward 0" */
	/* we're avoiding "floor" here because the int->double conversion introduces inaccuracies for big numbers */
	if (numerator(number(x)) < 0) /* not "val" because it might be truncated to 0 */
	  return(s7_make_integer(sc, val - 1));
	return(s7_make_integer(sc, val));
      }

    default:        
      return(s7_make_integer(sc, (s7_Int)floor(real(number(x))))); 
    }
}


static s7_pointer g_ceiling(s7_scheme *sc, s7_pointer args)
{
  #define H_ceiling "(ceiling x) returns the integer closest to x toward inf"
  s7_pointer x;

  x = car(args);
  if (!s7_is_real(x))
    return(s7_wrong_type_arg_error(sc, "ceiling", 0, x, "a real"));

  switch (number_type(x))
    {
    case NUM_INT:   
      return(x);

    case NUM_RATIO:
      {
	s7_Int val;
	val = numerator(number(x)) / denominator(number(x));
	if (numerator(number(x)) < 0)
	  return(s7_make_integer(sc, val));
	return(s7_make_integer(sc, val + 1));
      }

    default:        
      return(s7_make_integer(sc, (s7_Int)ceil(real(number(x))))); 
    }
}


static s7_pointer g_truncate(s7_scheme *sc, s7_pointer args)
{
  #define H_truncate "(truncate x) returns the integer closest to x toward 0"
  s7_pointer x;

  x = car(args);
  if (!s7_is_real(x))
    return(s7_wrong_type_arg_error(sc, "truncate", 0, x, "a real"));

  switch (number_type(x))
    {
    case NUM_INT: 
      return(x);

    case NUM_RATIO: 
      return(s7_make_integer(sc, (s7_Int)(numerator(number(x)) / denominator(number(x))))); /* C "/" already truncates */

    default: 
      return(s7_make_integer(sc, s7_truncate(real(number(x))))); 
    }
}


static s7_pointer g_round(s7_scheme *sc, s7_pointer args)
{
  #define H_round "(round x) returns the integer closest to x"
  s7_pointer x;

  x = car(args);
  if (!s7_is_real(x))
    return(s7_wrong_type_arg_error(sc, "round", 0, x, "a real"));

  switch (number_type(x))
    {
    case NUM_INT: 
      return(x);

    case NUM_RATIO: 
      {
	s7_Int truncated, remains;
	long double frac;

	truncated = numerator(number(x)) / denominator(number(x));
	remains = numerator(number(x)) % denominator(number(x));
	frac = s7_fabsl((long double)remains / (long double)denominator(number(x)));

	if ((frac > 0.5) ||
	    ((frac == 0.5) &&
	     (truncated % 2 != 0)))
	  {
	    if (numerator(number(x)) < 0)
	      return(s7_make_integer(sc, truncated - 1));
	    return(s7_make_integer(sc, truncated + 1));
	  }
	return(s7_make_integer(sc, truncated));
      }

    default: 
      return(s7_make_integer(sc, (s7_Int)round_per_R5RS(num_to_real(number(x))))); 
    }
}


static s7_pointer g_lcm(s7_scheme *sc, s7_pointer args)
{
  #define H_lcm "(lcm ...) returns the least common multiple of its rational arguments"
  int i;
  s7_Int n = 1, d = 0;
  bool rats = false;
  s7_pointer x;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if (!s7_is_rational(car(x)))
      return(s7_wrong_type_arg_error(sc, "lcm", i, car(x), "an integer or ratio"));
    else rats = ((rats) || (number_type(car(x)) == NUM_RATIO));

  if (!rats)
    {
      for (x = args; x != sc->NIL; x = cdr(x)) 
	{
	  n = c_lcm(n, s7_integer(car(x)));
#if WITH_GMP
	  if ((n > LONG_MAX) || (n < LONG_MIN))
	    return(big_lcm(sc, s7_cons(sc, s7_Int_to_big_integer(sc, n), x)));
#endif
	  if (n == 0)
	    return(small_int(sc, 0));
	}
      return(s7_make_integer(sc, n));
    }
  /* from A Jaffer */
  for (x = args; x != sc->NIL; x = cdr(x)) 
    {
      n = c_lcm(n, s7_numerator(car(x)));
#if WITH_GMP
      if ((n > LONG_MAX) || (n < LONG_MIN))
	return(big_lcm(sc, s7_cons(sc, s7_ratio_to_big_ratio(sc, n, d), x)));
#endif
      if (n == 0)
	return(small_int(sc, 0));
      d = c_gcd(d, s7_denominator(car(x)));
    }
  return(s7_make_ratio(sc, n, d));
}


static s7_pointer g_gcd(s7_scheme *sc, s7_pointer args)
{
  #define H_gcd "(gcd ...) returns the greatest common divisor of its rational arguments"
  int i;
  bool rats = false;
  s7_Int n = 0, d = 1;
  s7_pointer x;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if (!s7_is_rational(car(x)))
      return(s7_wrong_type_arg_error(sc, "gcd", i, car(x), "an integer"));
    else rats = ((rats) || (number_type(car(x)) == NUM_RATIO));
  
  if (!rats)
    {
      for (x = args; x != sc->NIL; x = cdr(x)) 
	{
	  n = c_gcd(n, s7_integer(car(x)));
	  if (n == 1)
	    return(small_int(sc, 1));
	}
      return(s7_make_integer(sc, n));
    }
  /* from A Jaffer */
  for (x = args; x != sc->NIL; x = cdr(x)) 
    {
      n = c_gcd(n, s7_numerator(car(x)));
      d = c_lcm(d, s7_denominator(car(x)));
    }
  return(s7_make_ratio(sc, n, d));
}


static s7_pointer g_add_unchecked(s7_scheme *sc, s7_pointer args)
{
  s7_pointer x;
  s7_num_t n;

  n = number(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
#if WITH_GMP
      switch (n.type)
	{
	case NUM_INT:
	  if ((integer(n) > LONG_MAX) ||
	      (integer(n) < LONG_MIN))
	    return(big_add(sc, s7_cons(sc, s7_Int_to_big_integer(sc, integer(n)), x)));
	  break;

	case NUM_RATIO:
	  if ((numerator(n) > LONG_MAX) ||
	      (denominator(n) > LONG_MAX) ||
	      (numerator(n) < LONG_MIN))
	    return(big_add(sc, s7_cons(sc, s7_ratio_to_big_ratio(sc, (numerator(n)), denominator(n)), x)));
	  break;
	}
#endif
      n = num_add(n, number(car(x)));
    }
  return(make_number(sc, n));
}


static s7_pointer g_add(s7_scheme *sc, s7_pointer args)
{
  #define H_add "(+ ...) adds its arguments"
  int i;
  s7_pointer x;

  if (args == sc->NIL)
    return(small_int(sc, 0));
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if (!s7_is_number(car(x)))
      return(s7_wrong_type_arg_error(sc, "+", i, car(x), "a number"));

  return(g_add_unchecked(sc, args));
}


static s7_pointer g_subtract_unchecked(s7_scheme *sc, s7_pointer args)
{
  s7_pointer x;
  s7_num_t n;

  n = number(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
#if WITH_GMP
      switch (n.type)
	{
	case NUM_INT:
	  if ((integer(n) > LONG_MAX) ||
	      (integer(n) < LONG_MIN))
	    return(big_subtract(sc, s7_cons(sc, s7_Int_to_big_integer(sc, integer(n)), x)));
	  break;

	case NUM_RATIO:
	  if ((numerator(n) > LONG_MAX) ||
	      (denominator(n) > LONG_MAX) ||
	      (numerator(n) < LONG_MIN))
	    return(big_subtract(sc, s7_cons(sc, s7_ratio_to_big_ratio(sc, (numerator(n)), denominator(n)), x)));
	  break;
	}
#endif
      n = num_sub(n, number(car(x)));
    }
  return(make_number(sc, n));
}


static s7_pointer g_subtract(s7_scheme *sc, s7_pointer args)
{
  #define H_subtract "(- x1 ...) subtracts its trailing arguments from the first, or negates the first if only one argument is given"
  int i;
  s7_pointer x;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_number(car(x)))
      return(s7_wrong_type_arg_error(sc, "-", i, car(x), "a number"));
  
  if (cdr(args) == sc->NIL) 
    return(make_number(sc, num_negate(number(car(args)))));

  return(g_subtract_unchecked(sc, args));
}


static s7_pointer g_multiply_unchecked(s7_scheme *sc, s7_pointer args)
{
  s7_pointer x;
  s7_num_t n;

  n = number(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
#if WITH_GMP
      switch (n.type)
	{
	case NUM_INT:
	  if ((integer(n) > LONG_MAX) ||
	      (integer(n) < LONG_MIN))
	    return(big_multiply(sc, s7_cons(sc, s7_Int_to_big_integer(sc, integer(n)), x)));
	  break;

	case NUM_RATIO:
	  if ((numerator(n) > LONG_MAX) ||
	      (denominator(n) > LONG_MAX) ||
	      (numerator(n) < LONG_MIN))
	    return(big_multiply(sc, s7_cons(sc, s7_ratio_to_big_ratio(sc, (numerator(n)), denominator(n)), x)));
	  break;
	}
#endif
      n = num_mul(n, number(car(x)));
    }
  return(make_number(sc, n));
}


static s7_pointer g_multiply(s7_scheme *sc, s7_pointer args)
{
  #define H_multiply "(* ...) multiplies its arguments"
  int i;
  s7_pointer x;

  if (args == sc->NIL)
    return(small_int(sc, 1));
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if (!s7_is_number(car(x)))
      return(s7_wrong_type_arg_error(sc, "*", i, car(x), "a number"));

  return(g_multiply_unchecked(sc, args));
}


static s7_pointer g_divide_unchecked(s7_scheme *sc, s7_pointer args)
{
  /* "unchecked" => we're sure they're numbers, but the divisor needs to be checked for 0 */
  s7_pointer x;
  s7_num_t n;

  n = number(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (s7_is_zero(car(x)))
	return(s7_division_by_zero_error(sc, "/", args));
#if WITH_GMP
      switch (n.type)
	{
	case NUM_INT:
	  if ((integer(n) > LONG_MAX) ||
	      (integer(n) < LONG_MIN))
	    return(big_divide(sc, s7_cons(sc, s7_Int_to_big_integer(sc, integer(n)), x)));
	  break;

	case NUM_RATIO:
	  if ((numerator(n) > LONG_MAX) ||
	      (denominator(n) > LONG_MAX) ||
	      (numerator(n) < LONG_MIN))
	    return(big_divide(sc, s7_cons(sc, s7_ratio_to_big_ratio(sc, (numerator(n)), denominator(n)), x)));
	  break;
	}
#endif
      n = num_div(n, number(car(x)));
    }
  return(make_number(sc, n));
}


static s7_pointer g_divide(s7_scheme *sc, s7_pointer args)
{
  #define H_divide "(- x1 ...) divides its first argument by the rest, or inverts the first if there is only one argument"
  int i;
  s7_pointer x;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_number(car(x)))
      return(s7_wrong_type_arg_error(sc, "/", i, car(x), "a number"));

  if (cdr(args) == sc->NIL)
    {
      if (s7_is_zero(car(args)))
	return(s7_division_by_zero_error(sc, "/", args));
      return(make_number(sc, num_invert(number(car(args)))));
    }

  return(g_divide_unchecked(sc, args));
}


/* should (min 1 1.5) return 1? */

static s7_pointer g_max(s7_scheme *sc, s7_pointer args)
{
  #define H_max "(max ...) returns the maximum of its arguments"
  int i;
  s7_pointer x;
  s7_num_t n;

  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "max", 1, car(args), "a real"));

  n = number(car(args));
  for (i = 2, x = cdr(args); x != sc->NIL; i++, x = cdr(x)) 
    {
      if (!s7_is_real(car(x)))
	return(s7_wrong_type_arg_error(sc, "max", i, car(x), "a real"));
      n = num_max(n, number(car(x)));
    }
  return(make_number(sc, n));
}


static s7_pointer g_min(s7_scheme *sc, s7_pointer args)
{
  #define H_min "(min ...) returns the minimum of its arguments"
  int i;
  s7_pointer x;
  s7_num_t n;

  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "min", 1, car(args), "a real"));

  n = number(car(args));
  for (i = 2, x = cdr(args); x != sc->NIL; i++, x = cdr(x)) 
    {
      if (!s7_is_real(car(x)))
	return(s7_wrong_type_arg_error(sc, "min", i, car(x), "a real"));
      n = num_min(n, number(car(x)));
    }
  return(make_number(sc, n));
}


static s7_pointer g_quotient(s7_scheme *sc, s7_pointer args)
{
  #define H_quotient "(quotient x1 x2) returns the integer quotient of x1 and x2; (quotient 4 3) = 1"
  
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "quotient", 1, car(args), "a real"));
  if (!s7_is_real(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "quotient", 2, cadr(args), "a real"));
  if (s7_is_zero(cadr(args)))
    return(s7_division_by_zero_error(sc, "quotient", args));

  return(make_number(sc, num_quotient(number(car(args)), number(cadr(args)))));
}


static s7_pointer g_remainder(s7_scheme *sc, s7_pointer args)
{
  #define H_remainder "(remainder x1 x2) returns the integer remainder of x1 and x2; (remainder 10 3) = 1"
  
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "remainder", 1, car(args), "a real"));
  if (!s7_is_real(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "remainder", 2, cadr(args), "a real"));
  if (s7_is_zero(cadr(args))) 
    return(s7_division_by_zero_error(sc, "remainder", args));
 
  return(make_number(sc, num_rem(number(car(args)), number(cadr(args)))));
}


static s7_pointer g_modulo(s7_scheme *sc, s7_pointer args)
{
  #define H_modulo "(modulo x1 x2) returns x1 mod x2; (modulo 4 3) = 1.  The arguments can be real numbers."
  
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "modulo", 1, car(args), "a real"));
  if (!s7_is_real(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "modulo", 2, cadr(args), "a real"));

  if (!s7_is_zero(cadr(args)))
    return(make_number(sc, num_mod(number(car(args)), number(cadr(args)))));

  return(car(args)); /* (mod x 0) = x according to "Concrete Mathematics" */
}


static s7_pointer g_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_equal "(= z1 ...) returns #t if all its arguments are equal"
  /* return(compare_numbers(sc, N_EQUAL, args)); */
  int i;
  s7_pointer x;
  s7_num_t n;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_number(car(x)))
      return(s7_wrong_type_arg_error(sc, "=", i, car(x), "a number"));
  
  n = number(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      if (!num_eq(n, number(car(x)))) 
	return(sc->F);
      n = number(car(x));
    }
  return(sc->T);

}


typedef enum {N_LESS, N_GREATER, N_LESS_OR_EQUAL, N_GREATER_OR_EQUAL} compare_t;

static s7_pointer compare_numbers(s7_scheme *sc, compare_t op, s7_pointer args)
{
  int i;
  s7_pointer x;
  bool (*comp_func)(s7_num_t a, s7_num_t b) = NULL;
  const char *op_name = NULL;
  s7_num_t n;

  switch (op)
    {
    case N_LESS:             comp_func = num_lt; op_name = "<";  break;
    case N_GREATER:          comp_func = num_gt; op_name = ">";  break;
    case N_LESS_OR_EQUAL:    comp_func = num_le; op_name = "<="; break;
    case N_GREATER_OR_EQUAL: comp_func = num_ge; op_name = ">="; break;
    }
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_real(car(x)))
      return(s7_wrong_type_arg_error(sc, op_name, i, car(x), "a real"));
  
  n = number(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      if (!comp_func(n, number(car(x)))) 
	return(sc->F);
      n = number(car(x));
    }
  return(sc->T);
}


static s7_pointer g_less(s7_scheme *sc, s7_pointer args)
{
  #define H_less "(< x1 ...) returns #t if its arguments are in increasing order"
  return(compare_numbers(sc, N_LESS, args));
}


static s7_pointer g_greater(s7_scheme *sc, s7_pointer args)
{
  #define H_greater "(> x1 ...) returns #t if its arguments are in decreasing order"
  return(compare_numbers(sc, N_GREATER, args));
}


static s7_pointer g_less_or_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_less_or_equal "(<= x1 ...) returns #t if its arguments are in increasing order"
  return(compare_numbers(sc, N_LESS_OR_EQUAL, args));
}


static s7_pointer g_greater_or_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_greater_or_equal "(>= x1 ...) returns #t if its arguments are in decreasing order"
  return(compare_numbers(sc, N_GREATER_OR_EQUAL, args));
}


static s7_pointer g_real_part(s7_scheme *sc, s7_pointer args)
{
  #define H_real_part "(real-part num) returns the real part of num"
  s7_pointer p;
  p = car(args);
  if (!s7_is_number(p))
    return(s7_wrong_type_arg_error(sc, "real-part", 0, p, "a number"));
  if (number_type(p) < NUM_COMPLEX)
    return(p);                                      /* if num is real, real-part should return it as is (not exact->inexact) */
  return(s7_make_real(sc, real_part(number(p))));
}


static s7_pointer g_imag_part(s7_scheme *sc, s7_pointer args)
{
  #define H_imag_part "(imag-part num) returns the imaginary part of num"
  s7_pointer p;
  p = car(args);
  if (!s7_is_number(p))
    return(s7_wrong_type_arg_error(sc, "imag-part", 0, p, "a number"));
  if (number_type(p) < NUM_REAL)
    return(small_int(sc, 0));                /* try to maintain exactness throughout expressions */
  return(s7_make_real(sc, s7_imag_part(p)));
}


static s7_pointer g_numerator(s7_scheme *sc, s7_pointer args)
{
  #define H_numerator "(numerator rat) returns the numerator of the rational number rat"
  if (!s7_is_rational(car(args)))
    return(s7_wrong_type_arg_error(sc, "numerator", 0, car(args), "a rational"));
  return(s7_make_integer(sc, num_to_numerator(number(car(args)))));
}


static s7_pointer g_denominator(s7_scheme *sc, s7_pointer args)
{
  #define H_denominator "(denominator rat) returns the denominator of the rational number rat"
  if (!s7_is_rational(car(args)))
    return(s7_wrong_type_arg_error(sc, "denominator", 0, car(args), "a rational"));
  return(s7_make_integer(sc, num_to_denominator(number(car(args)))));
}


static s7_pointer g_is_number(s7_scheme *sc, s7_pointer args) 
{
  #define H_is_number "(number? obj) returns #t if obj is a number"
  return(make_boolean(sc, s7_is_number(car(args))));
}


static s7_pointer g_is_integer(s7_scheme *sc, s7_pointer args) 
{
  #define H_is_integer "(integer? obj) returns #t if obj is an integer"
  return(make_boolean(sc, s7_is_integer(car(args))));
}


static s7_pointer g_is_real(s7_scheme *sc, s7_pointer args) 
{
  #define H_is_real "(real? obj) returns #t if obj is a real number"
  return(make_boolean(sc, s7_is_real(car(args))));
}


static s7_pointer g_is_complex(s7_scheme *sc, s7_pointer args) 
{
  #define H_is_complex "(complex? obj) returns #t if obj is a complex number"
  return(make_boolean(sc, s7_is_complex(car(args))));
}


static s7_pointer g_is_rational(s7_scheme *sc, s7_pointer args) 
{
  #define H_is_rational "(rational? obj) returns #t if obj is a rational number"
  return(make_boolean(sc, s7_is_rational(car(args))));
}


static s7_pointer g_is_even(s7_scheme *sc, s7_pointer args)
{
  #define H_is_even "(even? int) returns #t if the integer int is even"
  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "even?", 0, car(args), "an integer"));
  return(make_boolean(sc, (s7_integer(car(args)) & 1) == 0));
}


static s7_pointer g_is_odd(s7_scheme *sc, s7_pointer args)
{
  #define H_is_odd "(odd? int) returns #t if the integer int is odd"
  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "odd?", 0, car(args), "an integer"));
  return(make_boolean(sc, (s7_integer(car(args)) & 1) == 1));
}


static s7_pointer g_is_zero(s7_scheme *sc, s7_pointer args)
{
  #define H_is_zero "(zero? num) returns #t if the number num is zero"
  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "zero?", 0, car(args), "a number"));
  return(make_boolean(sc, s7_is_zero(car(args))));
}


static s7_pointer g_is_positive(s7_scheme *sc, s7_pointer args)
{
  #define H_is_positive "(positive? num) returns #t if the real number num is positive (greater than 0)"
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "positive?", 0, car(args), "a real"));
  return(make_boolean(sc, s7_is_positive(car(args))));
}


static s7_pointer g_is_negative(s7_scheme *sc, s7_pointer args)
{
  #define H_is_negative "(negative? num) returns #t if the real number num is negative (less than 0)"
  if (!s7_is_real(car(args)))
    return(s7_wrong_type_arg_error(sc, "negative?", 0, car(args), "a real"));
  return(make_boolean(sc, s7_is_negative(car(args))));
}


static s7_pointer g_inexact_to_exact(s7_scheme *sc, s7_pointer args)
{
  #define H_inexact_to_exact "(inexact->exact num) converts num to an exact number; (inexact->exact 1.5) = 3/2"
  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "inexact->exact", 0, car(args), "a number"));
  return(inexact_to_exact(sc, car(args)));
}


static s7_pointer g_exact_to_inexact(s7_scheme *sc, s7_pointer args)
{
  #define H_exact_to_inexact "(exact->inexact num) converts num to an inexact number; (exact->inexact 3/2) = 1.5"
  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "exact->inexact", 0, car(args), "a number"));
  return(exact_to_inexact(sc, car(args)));
}


static s7_pointer g_is_exact(s7_scheme *sc, s7_pointer args)
{
  #define H_is_exact "(exact? num) returns #t if num is exact (an integer or a ratio)"
  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "exact?", 0, car(args), "a number"));
  return(make_boolean(sc, s7_is_exact(car(args))));
}


static s7_pointer g_is_inexact(s7_scheme *sc, s7_pointer args)
{
  #define H_is_inexact "(inexact? num) returns #t if num is inexact (not an integer or a ratio)"
  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "inexact?", 0, car(args), "a number"));
  return(make_boolean(sc, s7_is_inexact(car(args))));
}


bool s7_is_ulong(s7_pointer arg) 
{
  return(s7_is_integer(arg));
}


unsigned long s7_ulong(s7_pointer p) 
{
  return(number(p).value.ul_value);
}


s7_pointer s7_make_ulong(s7_scheme *sc, unsigned long n) 
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  
  number_type(x) = NUM_INT;
  number(x).value.ul_value = n;
  return(x);
}


bool s7_is_ulong_long(s7_pointer arg) 
{
  return(s7_is_integer(arg));
}


unsigned long long s7_ulong_long(s7_pointer p) 
{
  return(number(p).value.ull_value);
}


s7_pointer s7_make_ulong_long(s7_scheme *sc, unsigned long long n) 
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_NUMBER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  
  number_type(x) = NUM_INT;
  number(x).value.ull_value = n;
  return(x);
}


static s7_pointer g_integer_length(s7_scheme *sc, s7_pointer args)
{
  #define H_integer_length "(integer-length arg) returns the number of bits required to represent the integer 'arg': (ceiling (log (abs arg) 2))"
  s7_Int x;
  if (!s7_is_integer(car(args)))
      return(s7_wrong_type_arg_error(sc, "integer-length", 0, car(args), "an integer"));
    
  x = s7_integer(car(args));
  if (x < 0)
    return(s7_make_integer(sc, integer_length(-(x + 1))));
  return(s7_make_integer(sc, integer_length(x)));
}


static s7_pointer g_logior(s7_scheme *sc, s7_pointer args)
{
  #define H_logior "(logior i1 ...) returns the bitwise OR of its integer arguments"
  s7_Int result = 0;
  int i; 
  s7_pointer x;

  for (i = 0, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_integer(car(x)))
      return(s7_wrong_type_arg_error(sc, "logior", i, car(x), "an integer"));
    else result |= s7_integer(car(x));

  return(s7_make_integer(sc, result));
}


static s7_pointer g_logxor(s7_scheme *sc, s7_pointer args)
{
  #define H_logxor "(logxor i1 ...) returns the bitwise XOR of its integer arguments"
  s7_Int result = 0;
  int i;
  s7_pointer x;

  for (i = 0, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_integer(car(x)))
      return(s7_wrong_type_arg_error(sc, "logxor", i, car(x), "an integer"));
    else result ^= s7_integer(car(x));

  return(s7_make_integer(sc, result));
}


static s7_pointer g_logand(s7_scheme *sc, s7_pointer args)
{
  #define H_logand "(logand i1 ...) returns the bitwise AND of its integer arguments"
  s7_Int result = -1;
  int i;
  s7_pointer x;

  for (i = 0, x = args; x != sc->NIL; i++, x = cdr(x))
    if (!s7_is_integer(car(x)))
      return(s7_wrong_type_arg_error(sc, "logand", i, car(x), "an integer"));
    else result &= s7_integer(car(x));

  return(s7_make_integer(sc, result));
}


static s7_pointer g_lognot(s7_scheme *sc, s7_pointer args)
{
  #define H_lognot "(lognot i1) returns the bitwise negation of i1"
  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "lognot", 1, car(args), "an integer"));
  return(s7_make_integer(sc, ~s7_integer(car(args))));
}


static s7_pointer g_ash(s7_scheme *sc, s7_pointer args)
{
  #define H_ash "(ash i1 i2) returns i1 shifted right or left i2 times, i1 << i2"
  s7_Int arg1, arg2;

  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "ash", 1, car(args), "an integer"));
  if (!s7_is_integer(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "ash", 2, cadr(args), "an integer"));
  
  arg1 = s7_integer(car(args));
  if (arg1 == 0) return(small_int(sc, 0));

  arg2 = s7_integer(cadr(args));
  if (arg2 >= s7_int_bits)
    return(s7_out_of_range_error(sc, "ash", 2, cadr(args), "shift is too large"));
  if (arg2 < -s7_int_bits)
    return(small_int(sc, 0));

  if (arg2 >= 0)
    return(s7_make_integer(sc, arg1 << arg2));
  return(s7_make_integer(sc, arg1 >> -arg2));
}


/* random numbers.  The simple version used in clm.c is probably adequate,
 *   but here I'll use Marsaglia's MWC algorithm as an experiment.
 *     (random num) -> a number (0..num), if num == 0 return 0, use global default state
 *     (random num state) -> same but use this state
 *     (make-random-state seed) -> make a new state
 *     (make-random-state seed type) ??
 */

typedef struct {
  unsigned long long int ran_seed, ran_carry;
} s7_rng_t;

static int rng_tag = 0;

#if WITH_GMP
static int big_rng_tag = 0;
#endif

static char *print_rng(s7_scheme *sc, void *val)
{
  char *buf;
  s7_rng_t *r = (s7_rng_t *)val;
  buf = (char *)malloc(64 * sizeof(char));
  snprintf(buf, 64, "#<rng %d %d>", (unsigned int)(r->ran_seed), (unsigned int)(r->ran_carry));
  return(buf);
}


static void free_rng(void *val)
{
  free(val);
}


static bool equal_rng(void *val1, void *val2)
{
  return(val1 == val2);
}

  
static s7_pointer g_make_random_state(s7_scheme *sc, s7_pointer args)
{
  #define H_make_random_state "(make-random-state seed) returns a new random number state initialized with 'seed'"
  s7_rng_t *r;
  r = (s7_rng_t *)calloc(1, sizeof(s7_rng_t));
  r->ran_seed = s7_integer(car(args));
  r->ran_carry = 1675393560;  /* should this be dependent on the seed? */
  return(s7_make_object(sc, rng_tag, (void *)r));
}


static s7_pointer copy_random_state(s7_scheme *sc, s7_pointer obj)
{
  s7_rng_t *r;
  if (c_object_type(obj) == rng_tag)
    {
      s7_rng_t *new_r;
      r = (s7_rng_t *)s7_object_value(obj);
      new_r = (s7_rng_t *)calloc(1, sizeof(s7_rng_t));
      new_r->ran_seed = r->ran_seed;
      new_r->ran_carry = r->ran_carry;
      return(s7_make_object(sc, rng_tag, (void *)new_r));
    }
  /* I can't find a way to copy a gmp random generator */
  return(sc->F);
}


#if HAVE_PTHREADS
static pthread_mutex_t rng_lock = PTHREAD_MUTEX_INITIALIZER;
#endif
  

static double next_random(s7_rng_t *r)
{
  /* The multiply-with-carry generator for 32-bit integers: 
   *        x(n)=a*x(n-1) + carry mod 2^32 
   * Choose multiplier a from this list: 
   *   1791398085 1929682203 1683268614 1965537969 1675393560 
   *   1967773755 1517746329 1447497129 1655692410 1606218150 
   *   2051013963 1075433238 1557985959 1781943330 1893513180 
   *   1631296680 2131995753 2083801278 1873196400 1554115554 
   * ( or any 'a' for which both a*2^32-1 and a*2^31-1 are prime) 
   */
  double result;
  unsigned long long int temp;
  #define RAN_MULT 2131995753UL

#if HAVE_PTHREADS
  pthread_mutex_lock(&rng_lock);
#endif

  temp = r->ran_seed * RAN_MULT + r->ran_carry;
  r->ran_seed = (temp & 0xffffffffUL);
  r->ran_carry = (temp >> 32);
  result = (double)((unsigned int)(r->ran_seed)) / 4294967295.0;

  /* (let ((mx 0) (mn 1000)) (do ((i 0 (+ i 1))) ((= i 10000)) (let ((val (random 123))) (set! mx (max mx val)) (set! mn (min mn val)))) (list mn mx)) */

#if HAVE_PTHREADS
  pthread_mutex_unlock(&rng_lock);
#endif

  return(result);
}


static s7_rng_t *s7_default_rng(s7_scheme *sc)
{
  if (!sc->default_rng)
    {
      sc->default_rng = (s7_rng_t *)calloc(1, sizeof(s7_rng_t));
      ((s7_rng_t *)(sc->default_rng))->ran_seed = (unsigned int)time(NULL);
      ((s7_rng_t *)(sc->default_rng))->ran_carry = 1675393560;
    }
  return((s7_rng_t *)(sc->default_rng));
}


double s7_random(s7_scheme *sc)
{
  /* an experiment -- for run.c */
  return(next_random(s7_default_rng(sc)));
}


static s7_pointer g_random(s7_scheme *sc, s7_pointer args)
{
  #define H_random "(random num :optional state) returns a random number between 0 and num (0 if num=0)."
  s7_pointer num, state;
  s7_rng_t *r;
  double dnum;
  
  num = car(args);
  if (!s7_is_number(num))
    return(s7_wrong_type_arg_error(sc, "random", 1, num, "a number"));

  if (cdr(args) != sc->NIL)
    {
      state = cadr(args);
      if (!is_c_object(state))
	return(s7_wrong_type_arg_error(sc, "random", 2, state, "a random state as returned by make-random-state"));

      if (c_object_type(state) == rng_tag)
	r = (s7_rng_t *)s7_object_value(state);
      else
	{
#if WITH_GMP
	  if (c_object_type(state) == big_rng_tag)
	    return(big_random(sc, args));
#endif
	  return(s7_wrong_type_arg_error(sc, "random", 2, state, "a random state as returned by make-random-state"));
	}
    }
  else r = s7_default_rng(sc);

  dnum = s7_number_to_real(num);

  switch (number_type(num))
    {
    case NUM_INT:
      return(s7_make_integer(sc, (s7_Int)(dnum * next_random(r))));

    case NUM_RATIO:
      {
	s7_Int numer = 0, denom = 1;
	c_rationalize(dnum * next_random(r), 1e-6, &numer, &denom);
	return(s7_make_ratio(sc, numer, denom));
      }

    case NUM_REAL:
    case NUM_REAL2:
      return(s7_make_real(sc, dnum * next_random(r)));

    default: 
      return(s7_make_complex(sc, dnum * next_random(r), dnum * next_random(r)));
    }

  return(sc->F);
}




/* -------------------------------- characters -------------------------------- */

static s7_pointer g_char_to_integer(s7_scheme *sc, s7_pointer args)
{
  #define H_char_to_integer "(char->integer c) converts the character c to an integer"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char->integer", 0, car(args), "a character"));
  return(s7_make_integer(sc, character(car(args))));
}


static s7_pointer g_integer_to_char(s7_scheme *sc, s7_pointer args)
{
  #define H_integer_to_char "(integer->char i) converts the non-negative integer i to a character"
  s7_pointer x;
  s7_Int ind;                   /* not int here!  (integer->char (expt 2 32)) -> #\null */
  x = car(args);

  if (!s7_is_integer(x))
    return(s7_wrong_type_arg_error(sc, "integer->char", 0, x, "an integer"));
  ind = s7_integer(x);
  if ((ind < 0) || (ind > 255))
    return(s7_wrong_type_arg_error(sc, "integer->char", 0, x, "an integer between 0 and 255"));

  return(s7_make_character(sc, (char)ind));
}


static s7_pointer g_char_upcase(s7_scheme *sc, s7_pointer args)
{
  #define H_char_upcase "(char-upcase c) converts the character c to upper case"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-upcase", 0, car(args), "a character"));
  return(s7_make_character(sc, (char)toupper(character(car(args)))));
}


static s7_pointer g_char_downcase(s7_scheme *sc, s7_pointer args)
{
  #define H_char_downcase "(char-downcase c) converts the character c to lower case"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-downcase", 0, car(args), "a character"));
  return(s7_make_character(sc, (char)tolower(character(car(args)))));
}


static s7_pointer g_is_char_alphabetic(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char_alphabetic "(char-alphabetic? c) returns #t if the character c is alphabetic"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-alphabetic?", 0, car(args), "a character"));
  return(make_boolean(sc, isalpha(character(car(args)))));

  /* isalpha will return #t for (integer->char 226) and others in that range -- should
   *   we insist on ASCII here? (i.e. less than 128)
   */
}


static s7_pointer g_is_char_numeric(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char_numeric "(char-numeric? c) returns #t if the character c is a digit"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-numeric?", 0, car(args), "a character"));
  return(make_boolean(sc, isdigit(character(car(args)))));
}


static s7_pointer g_is_char_whitespace(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char_whitespace "(char-whitespace? c) returns #t if the character c is non-printing character"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-whitespace?", 0, car(args), "a character"));
  return(make_boolean(sc, isspace(character(car(args)))));
}


static s7_pointer g_is_char_upper_case(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char_upper_case "(char-upper-case? c) returns #t if the character c is in upper case"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-upper-case?", 0, car(args), "a character"));
  return(make_boolean(sc, isupper(character(car(args)))));
}


static s7_pointer g_is_char_lower_case(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char_lower_case "(char-lower-case? c) returns #t if the character c is in lower case"
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "char-lower-case?", 0, car(args), "a character"));
  return(make_boolean(sc, islower(character(car(args)))));
}


static s7_pointer g_is_char(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char "(char? obj) returns #t if obj is a character"
  return(make_boolean(sc, s7_is_character(car(args))));
}


s7_pointer s7_make_character(s7_scheme *sc, int c) 
{
  s7_pointer x = new_cell(sc);
  set_type(x, T_CHARACTER | T_ATOM | T_SIMPLE | T_DONT_COPY);
  character(x) = c;
  return(x);
}


bool s7_is_character(s7_pointer p) 
{ 
  return(type(p) == T_CHARACTER);
}


char s7_character(s7_pointer p)  
{ 
  return(character(p));
}


static int charcmp(unsigned char c1, unsigned char c2, bool ci)
{
  if (ci)
    return(charcmp(toupper(c1), toupper(c2), false)); 
  /* not tolower here -- the single case is apparently supposed to be upper case
   *   this matters in a case like (char-ci<? #\_ #\e) which Guile and Gauche say is #f
   *   although (char<? #\_ #\e) is #t -- the spec does not say how to interpret this!
   */
  if (c1 == c2)
    return(0);
  if (c1 < c2)
    return(-1);
  return(1);
}


static s7_pointer g_char_cmp(s7_scheme *sc, s7_pointer args, int val, const char *name, bool ci)
{
  int i;
  s7_pointer x;
  unsigned char last_chr;
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))  
    if (!s7_is_character(car(x)))
      return(s7_wrong_type_arg_error(sc, name, i, car(x), "a character"));
  
  last_chr = character(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (charcmp(last_chr, character(car(x)), ci) != val)
	return(sc->F);
      last_chr = character(car(x));
    }
  return(sc->T);
}


static s7_pointer g_char_cmp_not(s7_scheme *sc, s7_pointer args, int val, const char *name, bool ci)
{
  int i;
  s7_pointer x;
  unsigned char last_chr;
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))  
    if (!s7_is_character(car(x)))
      return(s7_wrong_type_arg_error(sc, name, i, car(x), "a character"));
  
  last_chr = character(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (charcmp(last_chr, character(car(x)), ci) == val)
	return(sc->F);
      last_chr = character(car(x));
    }
  return(sc->T);
}


static s7_pointer g_chars_are_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_equal "(char=? chr...) returns #t if all the character arguments are equal"
  return(g_char_cmp(sc, args, 0, "char=?", false));
}	


static s7_pointer g_chars_are_less(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_less "(char<? chr...) returns #t if all the character arguments are increasing"
  return(g_char_cmp(sc, args, -1, "char<?", false));
}	


static s7_pointer g_chars_are_greater(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_greater "(char>? chr...) returns #t if all the character arguments are decreasing"
  return(g_char_cmp(sc, args, 1, "char>?", false));
}


static s7_pointer g_chars_are_geq(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_geq "(char>=? chr...) returns #t if all the character arguments are equal or decreasing"
  return(g_char_cmp_not(sc, args, -1, "char>=?", false));
}	


static s7_pointer g_chars_are_leq(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_leq "(char<=? chr...) returns #t if all the character arguments are equal or increasing"
  return(g_char_cmp_not(sc, args, 1, "char<=?", false));
}


static s7_pointer g_chars_are_ci_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_ci_equal "(char-ci=? chr...) returns #t if all the character arguments are equal, ignoring case"
  return(g_char_cmp(sc, args, 0, "char-ci=?", true));
}


static s7_pointer g_chars_are_ci_less(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_ci_less "(char-ci<? chr...) returns #t if all the character arguments are increasing, ignoring case"
  return(g_char_cmp(sc, args, -1, "char-ci<?", true));
}	


static s7_pointer g_chars_are_ci_greater(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_ci_greater "(char-ci>? chr...) returns #t if all the character arguments are decreasing, ignoring case"
  return(g_char_cmp(sc, args, 1, "char-ci>?", true));
}	


static s7_pointer g_chars_are_ci_geq(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_ci_geq "(char-ci>=? chr...) returns #t if all the character arguments are equal or decreasing, ignoring case"
  return(g_char_cmp_not(sc, args, -1, "char-ci>=?", true));
}


static s7_pointer g_chars_are_ci_leq(s7_scheme *sc, s7_pointer args)
{
  #define H_chars_are_ci_leq "(char-ci<=? chr...) returns #t if all the character arguments are equal or increasing, ignoring case"
  return(g_char_cmp_not(sc, args, 1, "char-ci<=?", true));
}



/* -------------------------------- strings -------------------------------- */


s7_pointer s7_make_string_with_length(s7_scheme *sc, const char *str, int len) 
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_STRING | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  string_value(x) = s7_strdup_with_len(str, len);
  string_length(x) = len;
  return(x);
}


static s7_pointer make_empty_string(s7_scheme *sc, int len, char fill) 
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_STRING | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  string_value(x) = (char *)malloc((len + 1) * sizeof(char));
  memset((void *)(string_value(x)), fill, len);
  string_value(x)[len] = 0;

  string_length(x) = len;
  return(x);
}


s7_pointer s7_make_string(s7_scheme *sc, const char *str) 
{
  return(s7_make_string_with_length(sc, str, safe_strlen(str)));
}


s7_pointer s7_make_permanent_string(const char *str) 
{
  /* for the symbol table which is never GC'd */
  s7_pointer x;
  x = (s7_cell *)calloc(1, sizeof(s7_cell));
  x->hloc = NOT_IN_HEAP;
  set_type(x, T_STRING | T_ATOM | T_SIMPLE | T_IMMUTABLE | T_DONT_COPY);
  if (str)
    {
      string_length(x) = strlen(str);
      string_value(x) = s7_strdup_with_len(str, string_length(x));
    }
  else 
    {
      string_value(x) = NULL;
      string_length(x) = 0;
    }
  return(x);
}


bool s7_is_string(s7_pointer p)
{
  return((type(p) == T_STRING)); 
}


const char *s7_string(s7_pointer p) 
{ 
  return(string_value(p));
}


static s7_pointer g_is_string(s7_scheme *sc, s7_pointer args)
{
  #define H_is_string "(string? obj) returns #t if obj is a string"
  return(make_boolean(sc, s7_is_string(car(args))));
}


static s7_pointer g_make_string(s7_scheme *sc, s7_pointer args)
{
  #define H_make_string "(make-string len :optional val) makes a string of length len filled with the character val (default: space)"
  int len;
  char fill = ' ';
  
  if ((!s7_is_integer(car(args))) || (s7_integer(car(args)) < 0))
    return(s7_wrong_type_arg_error(sc, "make-string", 1, car(args), "a non-negative integer"));
  
  len = s7_integer(car(args));
  if (cdr(args) != sc->NIL) 
    {
      if (!s7_is_character(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "make-string", 2, cadr(args), "a character"));
      fill = s7_character(cadr(args));
    }
  return(make_empty_string(sc, len, fill));
}


static s7_pointer g_string_length(s7_scheme *sc, s7_pointer args)
{
  #define H_string_length "(string-length str) returns the length of the string str"
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "string-length", 0, car(args), "string"));
  return(s7_make_integer(sc, string_length(car(args))));
}


static s7_pointer g_string_ref(s7_scheme *sc, s7_pointer args)
{
  #define H_string_ref "(string-ref str index) returns the character at the index-th element of the string str"
  
  s7_pointer index;
  char *str;
  s7_Int ind;

  index = cadr(args);
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "string-ref", 1, car(args), "a string"));
  if (!s7_is_integer(index))
    return(s7_wrong_type_arg_error(sc, "string-ref", 2, index, "an integer"));

  ind = s7_integer(index);

  if (ind < 0)
    return(s7_wrong_type_arg_error(sc, "string-ref", 2, index, "a non-negative integer"));
  if (ind >= string_length(car(args)))
    return(s7_out_of_range_error(sc, "string-ref", 2, index, "index should be less than string length"));
  
  str = string_value(car(args));
  return(s7_make_character(sc, ((unsigned char *)str)[ind]));
}


static s7_pointer g_string_set(s7_scheme *sc, s7_pointer args)
{
  #define H_string_set "(string-set! str index chr) sets the index-th element of the string str to the character chr"
  
  s7_pointer x, index;
  char *str;
  s7_Int ind;

  x = car(args);
  index = cadr(args);
  
  if (!s7_is_string(x))
    return(s7_wrong_type_arg_error(sc, "string-set!", 1, x, "a string"));
  if (!s7_is_character(caddr(args)))
    return(s7_wrong_type_arg_error(sc, "string-set!", 3, caddr(args), "a character"));
  if (s7_is_immutable(x))
    return(s7_wrong_type_arg_error(sc, "string-set!", 1, x, "a mutable string"));
  if (!s7_is_integer(index))
    return(s7_wrong_type_arg_error(sc, "string-set!", 2, index, "an integer"));

  ind = s7_integer(index);

  if (ind < 0)
    return(s7_wrong_type_arg_error(sc, "string-set!", 2, index, "a non-negative integer"));
  if (ind >= string_length(x))
    return(s7_out_of_range_error(sc, "string-set!", 2, index, "index should be less than string length"));
  
  /* I believe this does not need a lock in the multithread case -- local vars are specific
   *   to each thread, and it should be obvious to anyone writing such code that a global
   *   variable needs its lock (caller-supplied) -- it's not s7's job to protect even the
   *   caller's (scheme) variables.
   */
  str = string_value(x);
  str[ind] = (char)s7_character(caddr(args));
  return(x);
}


static s7_pointer g_string_append_1(s7_scheme *sc, s7_pointer args, const char *name)
{
  int i, len = 0;
  s7_pointer x, newstr;
  char *pos;
  
  if (args == sc->NIL)
    return(s7_make_string_with_length(sc, "", 0));
  
  /* get length for new string */
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      if (!s7_is_string(car(x)))
	return(s7_wrong_type_arg_error(sc, name, i, car(x), "a string"));
      len += string_length(car(x));
    }
  
  /* store the contents of the argument strings into the new string */
  newstr = make_empty_string(sc, len, 0);
  for (pos = string_value(newstr), x = args; x != sc->NIL; pos += string_length(car(x)), x = cdr(x)) 
    memcpy(pos, string_value(car(x)), string_length(car(x)));
  
  return(newstr);
}


static s7_pointer g_string_append(s7_scheme *sc, s7_pointer args)
{
  #define H_string_append "(string-append str1 ...) appends all its string arguments into one string"
  return(g_string_append_1(sc, args, "string-append"));
}


static s7_pointer g_string_copy(s7_scheme *sc, s7_pointer args)
{
  #define H_string_copy "(string-copy str) returns a copy of its string argument"
  if (args == sc->NIL)
    return(s7_wrong_type_arg_error(sc, "string-copy", 0, car(args), "a string"));
  
  return(g_string_append_1(sc, args, "string-copy"));
}


static s7_pointer g_substring(s7_scheme *sc, s7_pointer args)
{
  #define H_substring "(substring str start :optional end) returns the portion of the string str between start and end"
  
  s7_pointer x, start, end, str;
  s7_Int i0, i1;
  int len;
  char *s;
  
  str = car(args);
  start = cadr(args);
  
  if (!s7_is_string(str))
    return(s7_wrong_type_arg_error(sc, "substring", 1, str, "a string"));
  
  if ((!s7_is_integer(start)) || (s7_integer(start) < 0))
    return(s7_wrong_type_arg_error(sc, "substring", 2, start, "a non-negative integer"));
  
  if (cddr(args) != sc->NIL)
    {
      end = caddr(args);
      if ((!s7_is_integer(end)) || (s7_integer(end) < 0))
	return(s7_wrong_type_arg_error(sc, "substring", 3, end, "an integer > start"));
      i1 = s7_integer(end);
    }
  else i1 = string_length(str);
  
  i0 = s7_integer(start);
  s = string_value(str);
  
  if ((i0 > i1) || 
      (i1 > string_length(str)))
    return(s7_out_of_range_error(sc, "substring", 2, start, "start <= end <= string length"));
  
  len = i1 - i0;
  x = make_empty_string(sc, len, 0);
  memcpy(string_value(x), s + i0, len);
  string_value(x)[len] = 0;
  return(x);
}


static s7_pointer g_object_to_string(s7_scheme *sc, s7_pointer args)
{
  #define H_object_to_string "(object->string obj) returns a string representation of obj"
  return(s7_object_to_string(sc, car(args)));
}


static int safe_strcmp(const char *s1, const char *s2)
{
  int val;
  if (s1 == NULL)
    {
      if (s2 == NULL)
	return(0);
      return(-1);
    }
  if (s2 == NULL)
    return(1);

  val = strcmp(s1, s2); /* strcmp can return stuff like -97, but we want -1, 0, or 1 */

  if (val <= -1)
    return(-1);
  if (val >= 1)
    return(1);
  return(val);
}


static s7_pointer g_string_cmp(s7_scheme *sc, s7_pointer args, int val, const char *name)
{
  int i;
  s7_pointer x;
  const char *last_str = NULL;
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))  
    if (!s7_is_string(car(x)))
      return(s7_wrong_type_arg_error(sc, name, i, car(x), "a string"));
  
  last_str = string_value(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (safe_strcmp(last_str, string_value(car(x))) != val)
	return(sc->F);
      last_str = string_value(car(x));
    }
  return(sc->T);
}


static s7_pointer g_string_cmp_not(s7_scheme *sc, s7_pointer args, int val, const char *name)
{
  int i;
  s7_pointer x;
  const char *last_str = NULL;
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))  
    if (!s7_is_string(car(x)))
      return(s7_wrong_type_arg_error(sc, name, i, car(x), "a string"));
  
  last_str = string_value(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (safe_strcmp(last_str, string_value(car(x))) == val)
	return(sc->F);
      last_str = string_value(car(x));
    }
  return(sc->T);
}


static s7_pointer g_strings_are_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_equal "(string=? str...) returns #t if all the string arguments are equal"
  return(g_string_cmp(sc, args, 0, "string=?"));
}	


static s7_pointer g_strings_are_less(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_less "(string<? str...) returns #t if all the string arguments are increasing"
  return(g_string_cmp(sc, args, -1, "string<?"));
}	


static s7_pointer g_strings_are_greater(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_greater "(string>? str...) returns #t if all the string arguments are decreasing"
  return(g_string_cmp(sc, args, 1, "string>?"));
}	


static s7_pointer g_strings_are_geq(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_geq "(string>=? str...) returns #t if all the string arguments are equal or decreasing"
  return(g_string_cmp_not(sc, args, -1, "string>=?"));
}	


static s7_pointer g_strings_are_leq(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_leq "(string<=? str...) returns #t if all the string arguments are equal or increasing"
  return(g_string_cmp_not(sc, args, 1, "string<=?"));
}	


static int safe_strcasecmp(const char *s1, const char *s2)
{
  int len1, len2, len;
  int i;
  if (s1 == NULL)
    {
      if (s2 == NULL)
	return(0);
      return(-1);
    }

  if (s2 == NULL)
    return(1);

  len1 = strlen(s1);
  len2 = strlen(s2);
  len = len1;
  if (len1 > len2)
    len = len2;

  for (i = 0; i < len; i++)
    if (toupper(s1[i]) < toupper(s2[i]))
      return(-1);
    else
      {
	if (toupper(s1[i]) > toupper(s2[i]))
	  return(1);
      }

  if (len1 < len2) 
    return(-1);
  if (len1 > len2)
    return(1);

  return(0);
}


static s7_pointer g_string_ci_cmp(s7_scheme *sc, s7_pointer args, int val, const char *name)
{
  int i;
  s7_pointer x;
  const char *last_str = NULL;
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))  
    if (!s7_is_string(car(x)))
      return(s7_wrong_type_arg_error(sc, name, i, car(x), "a string"));
  
  last_str = string_value(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (safe_strcasecmp(last_str, string_value(car(x))) != val)
	return(sc->F);
      last_str = string_value(car(x));
    }
  return(sc->T);
}


static s7_pointer g_string_ci_cmp_not(s7_scheme *sc, s7_pointer args, int val, const char *name)
{
  int i;
  s7_pointer x;
  const char *last_str = NULL;
  
  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x))  
    if (!s7_is_string(car(x)))
      return(s7_wrong_type_arg_error(sc, name, i, car(x), "a string"));
  
  last_str = string_value(car(args));
  for (x = cdr(args); x != sc->NIL; x = cdr(x))
    {
      if (safe_strcasecmp(last_str, string_value(car(x))) == val)
	return(sc->F);
      last_str = string_value(car(x));
    }
  return(sc->T);
}


static s7_pointer g_strings_are_ci_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_ci_equal "(string-ci=? str...) returns #t if all the string arguments are equal, ignoring case"
  return(g_string_ci_cmp(sc, args, 0, "string-ci=?"));
}	


static s7_pointer g_strings_are_ci_less(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_ci_less "(string-ci<? str...) returns #t if all the string arguments are increasing, ignoring case"
  return(g_string_ci_cmp(sc, args, -1, "string-ci<?"));
}	


static s7_pointer g_strings_are_ci_greater(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_ci_greater "(string-ci>? str...) returns #t if all the string arguments are decreasing, ignoring case"
  return(g_string_ci_cmp(sc, args, 1, "string-ci>?"));
}	


static s7_pointer g_strings_are_ci_geq(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_ci_geq "(string-ci>=? str...) returns #t if all the string arguments are equal or decreasing, ignoring case"
  return(g_string_ci_cmp_not(sc, args, -1, "string-ci>=?"));
}	


static s7_pointer g_strings_are_ci_leq(s7_scheme *sc, s7_pointer args)
{
  #define H_strings_are_ci_leq "(string-ci<=? str...) returns #t if all the string arguments are equal or increasing, ignoring case"
  return(g_string_ci_cmp_not(sc, args, 1, "string-ci<=?"));
}	


static s7_pointer g_string_fill(s7_scheme *sc, s7_pointer args)
{
  #define H_string_fill "(string-fill! str chr) fills the string str with the character chr"
  s7_pointer x;
  x = car(args);

  if (!s7_is_string(x))
    return(s7_wrong_type_arg_error(sc, "string-fill", 1, x, "a string"));
  if (s7_is_immutable(x))
    return(s7_wrong_type_arg_error(sc, "string-fill!", 1, x, "a mutable string"));
  if (!s7_is_character(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "string-fill", 2, cadr(args), "a character"));

  /* strlen and so on here is probably not right -- a scheme string has a length
   *   set when it is created, and (apparently) can contain an embedded 0, so its
   *   print length is not its length.
   *         char *str; char c; str = string_value(car(args)); c = character(cadr(args));
   *         int i, len = 0; if (str) len = strlen(str); if (len > 0) for (i = 0; i < len; i++) str[i] = c; 
   */
  memset((void *)(string_value(x)), (int)character(cadr(args)), string_length(x)); /* presumably memset can fill 0 bytes if empty string */
  return(x); /* or perhaps sc->UNSPECIFIED */
}


static s7_pointer g_string_1(s7_scheme *sc, s7_pointer args, const char *name)
{
  int i, len;
  s7_pointer x, newstr;
  
  /* get length for new string and check arg types */
  for (len = 0, x = args; x != sc->NIL; len++, x = cdr(x)) 
    if (!s7_is_character(car(x)))
      return(s7_wrong_type_arg_error(sc, name, len + 1, car(x), "a character"));
  
  newstr = make_empty_string(sc, len, 0);
  for (i = 0, x = args; x != sc->NIL; i++, x = cdr(x)) 
    string_value(newstr)[i] = character(car(x));
  
  return(newstr);
}


static s7_pointer g_string(s7_scheme *sc, s7_pointer args)
{
  #define H_string "(string chr...) appends all its character arguments into one string"
  if (args == sc->NIL)                                /* (string) but not (string '()) */
    return(s7_make_string_with_length(sc, "", 0));
  return(g_string_1(sc, args, "string"));
}


static s7_pointer g_list_to_string(s7_scheme *sc, s7_pointer args)
{
  #define H_list_to_string "(list->string lst) appends all the list's characters into one string"
  if (car(args) == sc->NIL)
    return(s7_make_string_with_length(sc, "", 0));
  
  if (!is_proper_list(sc, car(args)))
    return(s7_wrong_type_arg_error(sc, "list->string", 0, car(args), "a (proper, non-circular) list of characters"));
  
  return(g_string_1(sc, car(args), "list->string"));
}


static s7_pointer g_string_to_list(s7_scheme *sc, s7_pointer args)
{
  #define H_string_to_list "(string->list str) returns the elements of the string str in a list"
  
  int i, len = 0;
  char *str;
  s7_pointer lst;
  lst = sc->NIL;
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "string->list", 0, car(args), "a string"));
  
  str = string_value(car(args));
  if (str) len = strlen(str);
  if (len == 0)
    return(sc->NIL);
  for (i = 0; i < len; i++)
    lst = s7_cons(sc, s7_make_character(sc, str[i]), lst);

  return(safe_reverse_in_place(sc, lst));
}


static s7_pointer g_string_for_each(s7_scheme *sc, s7_pointer args)
{
  /* (string-for-each (lambda (n) (format #t "~A " n)) "hiho" "1234") */
  #define H_string_for_each "(string-for-each func string1 ...) applies func to each element of the strings traversed in parallel"
  int i, len;
  s7_pointer str, x, fargs;

  sc->code = car(args);
  if (!is_procedure(sc->code))
    return(s7_wrong_type_arg_error(sc, "string-for-each", 1, sc->code, "a procedure"));

  str = cadr(args);
  if (!s7_is_string(str))
    return(s7_wrong_type_arg_error(sc, "string-for-each", 2, str, "a string"));

  len = string_length(str);
  fargs = s7_cons(sc, sc->NIL, sc->NIL);

  if (cddr(args) != sc->NIL)
    {
      for (i = 3, x = cddr(args); x != sc->NIL; x = cdr(x), i++)
	{
	  if (!s7_is_string(car(x)))
	    return(s7_wrong_type_arg_error(sc, "string-for-each", i, car(x), "a string"));
	  if (len != string_length(car(x)))
	    return(s7_wrong_type_arg_error(sc, "string-for-each", i, car(x), "a string whose length matches the other strings"));
	  fargs = s7_cons(sc, sc->NIL, fargs);
	}
    }
  /* during the loop we need the string list (cdddr(sc->args)), the function (sc->code), 
   *   the (mutable) loop counter (car(sc->args)) and end (cadr(sc->args)), and the current function arg list (caddr(sc->args)).
   */

  sc->args = s7_cons(sc, make_mutable_integer(sc, 0), 
               s7_cons(sc, s7_make_integer(sc, len), 
                 s7_cons(sc, fargs, cdr(args))));
  push_stack(sc, OP_STRING_FOR_EACH, sc->args, sc->code);
  return(sc->UNSPECIFIED);
}



/* -------------------------------- ports -------------------------------- */

static char *describe_port(s7_scheme *sc, s7_pointer p)
{
  char *desc;
  desc = (char *)malloc(64 * sizeof(char));
  snprintf(desc, 64, "<port%s%s%s>",
  	   (is_file_port(p)) ? " file" : ((is_string_port(p)) ? " string" : " function"),
	   (is_input_port(p)) ? " input" : " output",
	   (port_is_closed(p)) ? " (closed)" : "");
  return(desc);
}


static s7_pointer g_port_line_number(s7_scheme *sc, s7_pointer args)
{
  #define H_port_line_number "(port-line-number input-file-port) returns the current read line number of port"
  s7_pointer x;
  x = car(args);

  if (!(is_input_port(x)))
    return(s7_wrong_type_arg_error(sc, "port-line-number", 1, x, "an input port"));

  if (is_file_port(x))
    return(s7_make_integer(sc, port_line_number(x)));

  return(sc->F); /* not an error! */
}


const char *s7_port_filename(s7_pointer x)
{
  if (((is_input_port(x)) || (is_output_port(x))) && /* make sure it's some kind of port */
      (is_file_port(x)))
    return(port_filename(x));
  return(NULL);
}

static s7_pointer g_port_filename(s7_scheme *sc, s7_pointer args)
{
  #define H_port_filename "(port-filename file-port) returns the filename associated with port"
  s7_pointer x;
  x = car(args);

  if ((is_input_port(x)) || (is_output_port(x)))
    {
      if (is_file_port(x))
	return(s7_make_string(sc, port_filename(x)));
      return(sc->F); /* not an error! */
    }
  return(s7_wrong_type_arg_error(sc, "port-filename", 1, x, "a port"));
}


bool s7_is_input_port(s7_scheme *sc, s7_pointer p)   
{ 
  return((p == sc->NIL) || /* stdin? */
	 (is_input_port(p)));
}


static s7_pointer g_is_input_port(s7_scheme *sc, s7_pointer args)
{
  #define H_is_input_port "(input-port? p) returns #t is p is an input port"
  return(make_boolean(sc, s7_is_input_port(sc, car(args))));
}


bool s7_is_output_port(s7_scheme *sc, s7_pointer p)     
{ 
  return((p == sc->NIL) ||  /* stderr? */
	 (is_output_port(p)));
}


static s7_pointer g_is_output_port(s7_scheme *sc, s7_pointer args)
{
  #define H_is_output_port "(output-port? p) returns #t is p is an output port"
  return(make_boolean(sc, s7_is_output_port(sc, car(args))));
}


s7_pointer s7_current_input_port(s7_scheme *sc)
{
  return(sc->input_port);
}


static s7_pointer g_current_input_port(s7_scheme *sc, s7_pointer args)
{
  #define H_current_input_port "(current-input-port) returns the current input port"
  return(sc->input_port);
}


static s7_pointer g_set_current_input_port(s7_scheme *sc, s7_pointer args)
{
  #define H_set_current_input_port "(set-current-input-port port) sets the current-input port to port and returns the previous value of the input port"
  s7_pointer old_port, port;
  old_port = sc->input_port;
  port = car(args);
  if (s7_is_input_port(sc, port))
    sc->input_port = port;
  else return(s7_wrong_type_arg_error(sc, "set-current-input-port", 0, port, "an input port or nil"));
  return(old_port);
}


s7_pointer s7_set_current_input_port(s7_scheme *sc, s7_pointer port)
{
  s7_pointer old_port;
  old_port = sc->input_port;
  sc->input_port = port;
  return(old_port);
}


s7_pointer s7_current_output_port(s7_scheme *sc)
{
  return(sc->output_port);
}


s7_pointer s7_set_current_output_port(s7_scheme *sc, s7_pointer port)
{
  s7_pointer old_port;
  old_port = sc->output_port;
  sc->output_port = port;
  return(old_port);
}


static s7_pointer g_current_output_port(s7_scheme *sc, s7_pointer args)
{
  #define H_current_output_port "(current-output-port) returns the current output port"
  return(sc->output_port);
}


static s7_pointer g_set_current_output_port(s7_scheme *sc, s7_pointer args)
{
  #define H_set_current_output_port "(set-current-output-port port) sets the current-output port to port and returns the previous value of the output port"
  s7_pointer old_port, port;
  old_port = sc->output_port;
  port = car(args);
  if (s7_is_output_port(sc, port))
    sc->output_port = port;
  else return(s7_wrong_type_arg_error(sc, "set-current-output-port", 0, port, "an output port or nil"));
  return(old_port);
}


s7_pointer s7_current_error_port(s7_scheme *sc)
{
  return(sc->error_port);
}


s7_pointer s7_set_current_error_port(s7_scheme *sc, s7_pointer port)
{
  s7_pointer old_port;
  old_port = sc->error_port;
  sc->error_port = port;
  return(old_port);
}


static s7_pointer g_current_error_port(s7_scheme *sc, s7_pointer args)
{
  #define H_current_error_port "(current-error-port) returns the current error port"
  return(sc->error_port);
}


static s7_pointer g_set_current_error_port(s7_scheme *sc, s7_pointer args)
{
  #define H_set_current_error_port "(set-current-error-port port) sets the current-error port to port and returns the previous value of the error port"
  s7_pointer old_port, port;
  old_port = sc->error_port;
  port = car(args);
  if (s7_is_output_port(sc, port))
    sc->error_port = port;
  else return(s7_wrong_type_arg_error(sc, "set-current-error-port", 0, port, "an output port or nil"));
  return(old_port);
}


static s7_pointer g_is_char_ready(s7_scheme *sc, s7_pointer args)
{
  #define H_is_char_ready "(char-ready? :optional port) returns #t if a character is ready for input on the given port"
  if (args != sc->NIL)
    {
      s7_pointer pt = car(args);
      if (!s7_is_input_port(sc, pt))
	return(s7_wrong_type_arg_error(sc, "char-ready?", 0, pt, "an input port"));

      if (is_function_port(pt))
	return((*(port_input_function(pt)))(sc, S7_IS_CHAR_READY, pt));
      return(make_boolean(sc, is_string_port(pt)));
    }
  return(make_boolean(sc, (is_input_port(sc->input_port)) && (is_string_port(sc->input_port))));
}      


static s7_pointer g_is_eof_object(s7_scheme *sc, s7_pointer args)
{
  #define H_is_eof_object "(eof-object? val) returns #t is val is the end-of-file object"
  return(make_boolean(sc, car(args) == sc->EOF_OBJECT));
}


void s7_close_input_port(s7_scheme *sc, s7_pointer p)
{
  if ((p == sc->NIL) ||
      ((is_input_port(p)) && (port_is_closed(p))))
    return;
  
  if ((is_file_port(p)) &&
      (port_file(p)))
    {
      fclose(port_file(p));
      port_file(p) = NULL;
    }
  if (port_needs_free(p))
    {
      if (port_string(p))
	{
	  free(port_string(p));
	  port_string(p) = NULL;
	}
      port_needs_free(p) = false;
    }
  /* if input string, someone else is dealing with GC */
  port_is_closed(p) = true;
}


static s7_pointer g_close_input_port(s7_scheme *sc, s7_pointer args)
{
  #define H_close_input_port "(close-input-port port) closes the port"
  s7_pointer pt = car(args);
  if (!s7_is_input_port(sc, pt))
    return(s7_wrong_type_arg_error(sc, "close-input-port", 0, pt, "an input port"));
  s7_close_input_port(sc, pt);
  return(sc->UNSPECIFIED);
}


void s7_close_output_port(s7_scheme *sc, s7_pointer p)
{
  if ((p == sc->NIL) ||
      ((is_output_port(p)) && (port_is_closed(p))))
    return;
  
  if (is_file_port(p))
    {
      if (port_file(p))
	{
	  fclose(port_file(p));
	  port_file(p) = NULL;
	}
    }
  else
    {
      if ((is_string_port(p)) && (port_string(p)))
	{
	  free(port_string(p));
	  port_string(p) = NULL;
	  port_needs_free(p) = false;
	}
    }
  port_is_closed(p) = true;
}


static s7_pointer g_close_output_port(s7_scheme *sc, s7_pointer args)
{
  #define H_close_output_port "(close-output-port port) closes the port"
  s7_pointer pt = car(args);
  if (!s7_is_output_port(sc, pt))
    return(s7_wrong_type_arg_error(sc, "close-output-port", 0, pt, "an output port"));
  s7_close_output_port(sc, pt);
  return(sc->UNSPECIFIED);
}


static s7_pointer s7_make_input_file(s7_scheme *sc, const char *name, FILE *fp)
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_INPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  /* set up the port struct */
  x->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_file(x) = fp;
  port_type(x) = FILE_PORT;
  port_is_closed(x) = false;
  port_filename(x) = s7_strdup(name);
  port_line_number(x) = 1;  /* 1st line is numbered 1 */
  port_needs_free(x) = false;
  return(x);
}


s7_pointer s7_open_input_file(s7_scheme *sc, const char *name, const char *mode)
{
  FILE *fp;
  /* see if we can open this file before allocating a port */
  
  fp = fopen(name, mode);
  if (!fp)
    return(s7_file_error(sc, "open-input-file", "can't open", name));
  
  return(s7_make_input_file(sc, name, fp));
}


static s7_pointer g_open_input_file(s7_scheme *sc, s7_pointer args)
{
  #define H_open_input_file "(open-input-file filename :optional mode) opens filename for reading"
  s7_pointer name = car(args);
  if (!s7_is_string(name))
    return(s7_wrong_type_arg_error(sc, "open-input-file", 1, name, "a string (a filename)"));
  
  if (is_pair(cdr(args)))
    {
      if (!s7_is_string(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "open-input-file", 2, cadr(args), "a string (a mode)"));
      return(s7_open_input_file(sc, s7_string(name), s7_string(cadr(args))));
    }
  return(s7_open_input_file(sc, s7_string(name), "r"));
}


s7_pointer s7_open_output_file(s7_scheme *sc, const char *name, const char *mode)
{
  FILE *fp;
  s7_pointer x;
  /* see if we can open this file before allocating a port */
  
  fp = fopen(name, mode);
  if (!fp)
    return(s7_file_error(sc, "open-output-file", "can't open", name));
  
  x = new_cell(sc);
  set_type(x, T_OUTPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  x->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_type(x) = FILE_PORT;
  port_is_closed(x) = false;
  port_filename(x) = s7_strdup(name);
  port_line_number(x) = 1;
  port_file(x) = fp;
  port_needs_free(x) = false;
  return(x);
}


static s7_pointer g_open_output_file(s7_scheme *sc, s7_pointer args)
{
  #define H_open_output_file "(open-output-file filename :optional mode) opens filename for writing"
  s7_pointer name = car(args);
  if (!s7_is_string(name))
    return(s7_wrong_type_arg_error(sc, "open-output-file", 1, name, "a string (a filename)"));
  
  if (is_pair(cdr(args)))
    {
      if (!s7_is_string(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "open-output-file", 2, cadr(args), "a string (a mode)"));
      return(s7_open_output_file(sc, s7_string(name), s7_string(cadr(args))));
    }
  return(s7_open_output_file(sc, s7_string(name), "w"));
}


s7_pointer s7_open_input_string(s7_scheme *sc, const char *input_string)
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_INPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  x->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_type(x) = STRING_PORT;
  port_is_closed(x) = false;
  port_string(x) = (char *)input_string;
  port_string_length(x) = safe_strlen(input_string);
  port_string_point(x) = 0;
  port_filename(x) = NULL;
  port_file_number(x) = -1;
  port_needs_free(x) = false;

  return(x);
}


static s7_pointer g_open_input_string(s7_scheme *sc, s7_pointer args)
{
  #define H_open_input_string "(open-input-string str) opens an input port reading str"
  s7_pointer input_string = car(args);
  if (!s7_is_string(input_string))
    return(s7_wrong_type_arg_error(sc, "open-input-string", 0, input_string, "a string"));
  
  return(s7_open_input_string(sc, s7_string(input_string))); /* presumably the caller is protecting the input string?? */
}


#define STRING_PORT_INITIAL_LENGTH 128

s7_pointer s7_open_output_string(s7_scheme *sc)
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_OUTPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  x->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_type(x) = STRING_PORT;
  port_is_closed(x) = false;
  port_string_length(x) = STRING_PORT_INITIAL_LENGTH;
  port_string(x) = (char *)calloc(STRING_PORT_INITIAL_LENGTH, sizeof(char));
  port_string_point(x) = 0;
  port_needs_free(x) = true;
  return(x);
}


static s7_pointer g_open_output_string(s7_scheme *sc, s7_pointer args)
{
  #define H_open_output_string "(open-output-string) opens an output string port"
  return(s7_open_output_string(sc));
}


const char *s7_get_output_string(s7_scheme *sc, s7_pointer p)
{
  return(port_string(p));
}


static s7_pointer g_get_output_string(s7_scheme *sc, s7_pointer args)
{
  #define H_get_output_string "(get-output-string port) returns the output accumulated in port"
  s7_pointer p = car(args);
  if ((!is_output_port(p)) ||
      (!is_string_port(p)))
    return(s7_wrong_type_arg_error(sc, "get-output-string", 0, p, "an output string port"));
  return(s7_make_string(sc, s7_get_output_string(sc, p)));
}

s7_pointer s7_open_input_function(s7_scheme *sc, s7_pointer (*function)(s7_scheme *sc, s7_read_t read_choice, s7_pointer port))
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_INPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  x->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_type(x) = FUNCTION_PORT;
  port_is_closed(x) = false;
  port_needs_free(x) = false;
  port_input_function(x) = function;
  return(x);
}


s7_pointer s7_open_output_function(s7_scheme *sc, void (*function)(s7_scheme *sc, char c, s7_pointer port))
{
  s7_pointer x;
  x = new_cell(sc);
  set_type(x, T_OUTPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  
  x->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_type(x) = FUNCTION_PORT;
  port_is_closed(x) = false;
  port_needs_free(x) = false;
  port_output_function(x) = function;
  return(x);
}


void *s7_port_data(s7_pointer port)
{
  return(port_data(port));
}


void *s7_port_set_data(s7_pointer port, void *stuff)
{
  port_data(port) = stuff;
  return(stuff);
}


static bool push_input_port(s7_scheme *sc, s7_pointer new_port)
{
  sc->input_port_stack = s7_cons(sc, sc->input_port, sc->input_port_stack);
  sc->input_port = new_port;
  return(sc->input_port != sc->NIL);
}


static s7_pointer pop_input_port(s7_scheme *sc)
{
  if (is_pair(sc->input_port_stack))
    {
      sc->input_port = car(sc->input_port_stack);
      sc->input_port_stack = cdr(sc->input_port_stack);
    }
  else sc->input_port = sc->NIL;
  return(sc->input_port);
}


/* TODO: tie in (and test/document) the caller function input port
 */

static int inchar(s7_scheme *sc, s7_pointer pt)
{
  int c;
  if (pt == sc->NIL) return(EOF);
  
  if (is_file_port(pt))
    c = fgetc(port_file(pt));
  else 
    {
      if ((!(port_string(pt))) ||
	  (port_string_length(pt) <= port_string_point(pt)))
	return(EOF);
      c = port_string(pt)[port_string_point(pt)++];
    }

  if (c == '\n')
    port_line_number(pt)++;

  return(c);
}


static void backchar(s7_scheme *sc, char c, s7_pointer pt) 
{
  if (pt == sc->NIL) return;
  
  if (c == '\n')
    port_line_number(pt)--;

  if (is_file_port(pt))
    ungetc(c, port_file(pt));
  else 
    {
      if (port_string_point(pt) > 0)
	port_string_point(pt)--;
    }
}


static char s7_read_char_1(s7_scheme *sc, s7_pointer port, s7_read_t read_choice)
{
  /* port nil -> as if read-char with no arg -> use current input port */
  int c;

  if (is_function_port(port))
    return(character((*(port_input_function(port)))(sc, read_choice, port)));

  c = inchar(sc, port);
  if ((read_choice == S7_PEEK_CHAR) && (c != EOF))
    backchar(sc, c, port);
  return(c);
}


char s7_read_char(s7_scheme *sc, s7_pointer port)
{
  return(s7_read_char_1(sc, port, S7_READ_CHAR));
}


char s7_peek_char(s7_scheme *sc, s7_pointer port)
{
  return(s7_read_char_1(sc, port, S7_PEEK_CHAR));
}


static s7_pointer g_read_char_1(s7_scheme *sc, s7_pointer args, bool peek)
{
  char c;
  s7_pointer port;

  if (args != sc->NIL)
    port = car(args);
  else port = sc->input_port;
  if (!s7_is_input_port(sc, port))
    return(s7_wrong_type_arg_error(sc, (peek) ? "peek-char" : "read-char", 0, port, "an input port"));
      
  c = s7_read_char_1(sc, port, (peek) ? S7_PEEK_CHAR : S7_READ_CHAR);
  if (c == EOF)
    return(sc->EOF_OBJECT); 

  return(s7_make_character(sc, c));
}


static s7_pointer g_read_char(s7_scheme *sc, s7_pointer args)
{
  #define H_read_char "(read-char :optional port) returns the next character in the input port"
  return(g_read_char_1(sc, args, false));
}


static s7_pointer g_peek_char(s7_scheme *sc, s7_pointer args)
{
  #define H_peek_char "(peek-char :optional port) returns the next character in the input port, but does not remove it from the input stream"
  return(g_read_char_1(sc, args, true));
}


static s7_pointer g_read_line(s7_scheme *sc, s7_pointer args)
{
  #define H_read_line "(read-line port (with-eol #f)) returns the next line from port, or #<eof> (use the function eof-object?).\
If 'with-eol' is not #f, include the trailing end-of-line character."

  s7_pointer port;
  int i;
  bool with_eol = false;

  if (args != sc->NIL)
    {
      port = car(args);
      if (!s7_is_input_port(sc, port))
	return(s7_wrong_type_arg_error(sc, "read-line", 0, port, "an input port"));

      if ((cdr(sc->args) != sc->NIL) &&
	  (cadr(sc->args) != sc->F))
	with_eol = true;
    }
  else port = sc->input_port;

  if (is_function_port(port))
    return((*(port_input_function(port)))(sc, S7_READ_LINE, port));

  if (sc->read_line_buf == NULL)
    {
      sc->read_line_buf_size = 256;
      sc->read_line_buf = (char *)malloc(sc->read_line_buf_size * sizeof(char));
    }

  for (i = 0; ; i++)
    {
      int c;
      if (i + 1 >= sc->read_line_buf_size)
	{
	  sc->read_line_buf_size *= 2;
	  sc->read_line_buf = (char *)realloc(sc->read_line_buf, sc->read_line_buf_size * sizeof(char));
	}

      c = inchar(sc, port);
      if (c == EOF)
	{
	  if (i == 0)
	    return(sc->EOF_OBJECT);
	  sc->read_line_buf[i] = 0;
	  return(s7_make_string_with_length(sc, sc->read_line_buf, i));
	}

      sc->read_line_buf[i] = (char)c;
      if (c == '\n')
	{
	  if (!with_eol) i--;
	  sc->read_line_buf[i + 1] = 0;
	  return(s7_make_string_with_length(sc, sc->read_line_buf, i + 1));
	}
    }
  return(sc->EOF_OBJECT);
}


s7_pointer s7_read(s7_scheme *sc, s7_pointer port)
{
  if (s7_is_input_port(sc, port))
    {
      bool old_longjmp;
      old_longjmp = sc->longjmp_ok;
      if (!sc->longjmp_ok)
	{
	  sc->longjmp_ok = true;
	  if (setjmp(sc->goto_start) != 0)
	    return(sc->value);
	}
      push_input_port(sc, port);
      push_stack(sc, OP_READ_RETURN_EXPRESSION, port, sc->NIL);
      eval(sc, OP_READ_INTERNAL);
      sc->longjmp_ok = old_longjmp;
      pop_input_port(sc);
      return(sc->value);
    }
  return(s7_wrong_type_arg_error(sc, "read", 0, port, "an input port"));
}


static s7_pointer g_read(s7_scheme *sc, s7_pointer args)
{
  #define H_read "(read :optional port) returns the next object in the input port"
  s7_pointer port;
  
  if (args != sc->NIL)
    port = car(args);
  else port = sc->input_port;
  
  if (!is_input_port(port)) 
    /* s7_is_input_port here lets NIL through as stdin, but that segfaults in token
     *    should stdin work in that case?
     */
    return(s7_wrong_type_arg_error(sc, "read", 0, port, "an input port"));

  if (is_function_port(port))
    return((*(port_input_function(port)))(sc, S7_READ, port));
  
  push_input_port(sc, port);
  push_stack(sc, OP_READ_POP_AND_RETURN_EXPRESSION, sc->NIL, sc->NIL); /* this stops the internal read process so we only get one form */
  push_stack(sc, OP_READ_INTERNAL, sc->NIL, sc->NIL);
  return(port);
}


static FILE *search_load_path(s7_scheme *sc, const char *name)
{
  int i, len, name_len;
  s7_pointer lst;
  lst = s7_load_path(sc);
  len = s7_list_length(sc, lst);
  name_len = strlen(name);
  for (i = 0; i < len; i++)
    {
      const char *new_dir;
      int size;
      new_dir = s7_string(s7_list_ref(sc, lst, i));
      if (new_dir)
	{
	  char *new_name;
	  FILE *fp;
	  size = name_len + strlen(new_dir) + 2;
	  new_name = (char *)malloc(size * sizeof(char));
	  snprintf(new_name, size, "%s/%s", new_dir, name);
	  fp = fopen(new_name, "r");
	  free(new_name);
	  if (fp) return(fp);
	}
    }
  return(NULL);
}


static s7_pointer load_file(s7_scheme *sc, FILE *fp, const char *name)
{
  s7_pointer port;
  long size;
  char *content = NULL;

  port = new_cell(sc);
  set_type(port, T_INPUT_PORT | T_ATOM | T_FINALIZABLE | T_SIMPLE | T_DONT_COPY);
  port->object.port = (s7_port_t *)calloc(1, sizeof(s7_port_t));
  port_type(port) = STRING_PORT;
  port_is_closed(port) = false;

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  if (size > 0)
    {
      size_t bytes;
      rewind(fp);
      content = (char *)malloc((size + 1) * sizeof(char));
      bytes = fread(content, sizeof(char), size, fp);
      if (bytes != (size_t)size)
	{
	  char *tmp;
	  tmp = (char *)calloc(256, sizeof(char));
	  snprintf(tmp, 256, "(load \"%s\") read %ld bytes of an expected %ld?", name, (long)bytes, size);
	  write_string(sc, tmp, sc->output_port);
	  free(tmp);
	}
      content[size] = '\0';
    }
  else
    {
      content = (char *)calloc(1, sizeof(char)); /* empty file in load still accesses the string (for 0=eof) */
    }
  fclose(fp);

  port_string(port) = content;
  port_string_length(port) = size;
  port_string_point(port) = 0;
  port_line_number(port) = 1;
  port_filename(port) = s7_strdup(name); /* for missing close paren error etc */
  port_needs_free(port) = true;

  return(port);
}


static void run_load_hook(s7_scheme *sc, const char *filename)
{
  /* (set! *load-hook* (lambda (name) (format #t "loading ~A~%" name))) */
  s7_pointer load_hook;
  load_hook = s7_symbol_value(sc, s7_make_symbol(sc, "*load-hook*"));
  if (is_procedure(load_hook))
    {
      push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
      sc->args = make_list_1(sc, s7_make_string(sc, filename));
      sc->code = load_hook;
      eval(sc, OP_APPLY);
    }
}


s7_pointer s7_load(s7_scheme *sc, const char *filename)
{
  bool old_longjmp;
  s7_pointer port;
  FILE *fp;
  
  fp = fopen(filename, "r");
  if (!fp)
    fp = search_load_path(sc, filename);
  if (!fp)
    return(s7_file_error(sc, "open-input-file", "can't open", filename));

  run_load_hook(sc, filename);

  port = load_file(sc, fp, filename);
  port_file_number(port) = remember_file_name(filename);
  push_input_port(sc, port);
  
  /* it's possible to call this recursively (s7_load is XEN_LOAD_FILE which can be invoked via s7_call)
   *   but in that case, we actually want it to behave like g_load and continue the evaluation upon completion
   */
  
  if (!sc->longjmp_ok)
    {
      push_stack(sc, OP_LOAD_RETURN_IF_EOF, port, sc->NIL);
      
      old_longjmp = sc->longjmp_ok;
      if (!sc->longjmp_ok)
	{
	  sc->longjmp_ok = true;
	  if (setjmp(sc->goto_start) != 0)
	    eval(sc, sc->op);
	  else eval(sc, OP_READ_INTERNAL);
	}
      sc->longjmp_ok = old_longjmp;  
      pop_input_port(sc);
      s7_close_input_port(sc, port);
    }
  else
    {
      /* caller here is assuming the load will be complete before this function returns */
      push_stack(sc, OP_LOAD_RETURN_IF_EOF, sc->args, sc->code);
      eval(sc, OP_READ_INTERNAL);
      pop_input_port(sc);
      s7_close_input_port(sc, port);
    }

  return(sc->UNSPECIFIED);
}


static s7_pointer g_load(s7_scheme *sc, s7_pointer args)
{
  #define H_load "(load file :optional env) loads the scheme file 'file'. The 'env' argument \
defaults to the global environment; to load into the current environment instead, pass (current-environment)."

  FILE *fp = NULL;
  s7_pointer name, port;
  const char *fname;
  
  name = car(args);
  if (!s7_is_string(name))
    return(s7_wrong_type_arg_error(sc, "load", 1, name, "a string (a filename)"));
  
  fname = s7_string(name);
  
  fp = fopen(fname, "r");
  if (!fp)
    fp = search_load_path(sc, fname);
  if (!fp)
    return(s7_file_error(sc, "open-input-file", "can't open", fname));
  
  run_load_hook(sc, fname);

  port = load_file(sc, fp, fname);
  port_file_number(port) = remember_file_name(fname);
  push_input_port(sc, port);

  if (cdr(args) != sc->NIL) 
    {
      if (!is_pair(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "load", 2, cadr(args), "an environment"));
      sc->envir = cadr(args);
    }
  else sc->envir = s7_global_environment(sc);

  push_stack(sc, OP_LOAD_CLOSE_AND_POP_IF_EOF, sc->NIL, sc->NIL);  /* was pushing args and code, but I don't think they're used later */
  push_stack(sc, OP_READ_INTERNAL, sc->NIL, sc->NIL);
  
  return(sc->UNSPECIFIED);
}


s7_pointer s7_load_path(s7_scheme *sc)
{
  return(s7_symbol_value(sc, s7_make_symbol(sc, "*load-path*")));
}


s7_pointer s7_add_to_load_path(s7_scheme *sc, const char *dir)
{
  s7_pointer load_path;
  load_path = s7_make_symbol(sc, "*load-path*");

#if WITH_ENCAPSULATION
  if (is_encapsulating(sc)) encapsulate(sc, load_path);
#endif

  s7_symbol_set_value(sc, 
		      load_path,
		      s7_cons(sc, 
			      s7_make_string(sc, dir), 
			      s7_symbol_value(sc, load_path)));
  return(s7_load_path(sc));
}


static s7_pointer g_eval_string(s7_scheme *sc, s7_pointer args)
{
  #define H_eval_string "(eval-string str :optional env) returns the result of evaluating the string str as Scheme code"
  s7_pointer port;

  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "eval-string", 0, car(args), "a string"));
  
  if (cdr(args) != sc->NIL)
    {
      if (!is_pair(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "eval", 2, cadr(args), "an environment"));
      sc->envir = cadr(args);
    }

  port = s7_open_input_string(sc, s7_string(car(args)));
  push_input_port(sc, port);
  push_stack(sc, OP_EVAL_STRING, sc->args, sc->code);
  eval(sc, OP_READ_INTERNAL);
  pop_input_port(sc);
  s7_close_input_port(sc, port);

  if ((is_pair(sc->value)) &&                 /* (+ 1 (eval-string "(values 2 3)")) */
      (car(sc->value) == sc->VALUES))
    sc->value = splice_in_values(sc, cdr(sc->value));

  return(sc->value);
}


s7_pointer s7_eval_c_string(s7_scheme *sc, const char *str)
{
  bool old_longjmp;
  s7_pointer port;
  /* this can be called recursively via s7_call */

  if (sc->longjmp_ok)
    return(g_eval_string(sc, make_list_1(sc, s7_make_string(sc, str))));
  
  stack_reset(sc); 
  sc->envir = sc->global_env; 
  port = s7_open_input_string(sc, str);
  push_input_port(sc, port);
  push_stack(sc, OP_EVAL_STRING, sc->NIL, sc->NIL);
  
  old_longjmp = sc->longjmp_ok;
  if (!sc->longjmp_ok)
    {
      sc->longjmp_ok = true;
      if (setjmp(sc->goto_start) != 0)
	eval(sc, sc->op);
      else eval(sc, OP_READ_INTERNAL);
    }
  
  sc->longjmp_ok = old_longjmp;
  pop_input_port(sc);
  s7_close_input_port(sc, port);
  
  return(sc->value);
}


static s7_pointer call_with_input(s7_scheme *sc, s7_pointer port, s7_pointer args)
{
  push_stack(sc, OP_UNWIND_INPUT, sc->input_port, port);
  sc->code = cadr(args);
  sc->args = make_list_1(sc, port);
  eval(sc, OP_APPLY);
  s7_close_input_port(sc, port);

  if ((is_pair(sc->value)) &&                 /* (+ 100 (call-with-input-string "123" (lambda (p) (values (read p) 1)))) */
      (car(sc->value) == sc->VALUES))
    sc->value = splice_in_values(sc, cdr(sc->value));

  return(sc->value);
}


static s7_pointer g_call_with_input_string(s7_scheme *sc, s7_pointer args)
{
  #define H_call_with_input_string "(call-with-input-string str proc) opens a string port for str and applies proc to it"
  
  /* (call-with-input-string "44" (lambda (p) (+ 1 (read p)))) -> 45
   */
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-input-string", 1, car(args), "a string"));
  if (!is_procedure(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-input-string", 2, cadr(args), "a procedure"));
  
  return(call_with_input(sc, s7_open_input_string(sc, s7_string(car(args))), args));
}


static s7_pointer g_call_with_input_file(s7_scheme *sc, s7_pointer args)
{
  #define H_call_with_input_file "(call-with-input-file filename proc) opens filename and calls proc with the input port as its argument"
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-input-file", 1, car(args), "a string (a filename)"));
  if (!is_procedure(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-input-file", 2, cadr(args), "a procedure"));
  
  return(call_with_input(sc, s7_open_input_file(sc, s7_string(car(args)), "r"), args));
}


static s7_pointer with_input(s7_scheme *sc, s7_pointer port, s7_pointer args)
{
  s7_pointer old_input_port;
  old_input_port = sc->input_port;
  sc->input_port = port;
  
  push_stack(sc, OP_UNWIND_INPUT, old_input_port, port);
  sc->code = cadr(args);
  sc->args = sc->NIL;
  eval(sc, OP_APPLY);
  
  s7_close_input_port(sc, sc->input_port);
  sc->input_port = old_input_port;

  if ((is_pair(sc->value)) &&                 /* (+ 100 (with-input-from-string "123" (lambda () (values (read) 1)))) */
      (car(sc->value) == sc->VALUES))
    sc->value = splice_in_values(sc, cdr(sc->value));

  return(sc->value);
}


static s7_pointer g_with_input_from_string(s7_scheme *sc, s7_pointer args)
{
  #define H_with_input_from_string "(with-input-from-string str thunk) opens str as the temporary current-input-port and calls thunk"
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "with-input-from-string", 1, car(args), "a string"));
  if (!is_thunk(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, "with-input-from-string", 2, cadr(args), "a thunk"));
  
  return(with_input(sc, s7_open_input_string(sc, s7_string(car(args))), args));
}


static s7_pointer g_with_input_from_file(s7_scheme *sc, s7_pointer args)
{
  #define H_with_input_from_file "(with-input-from-file filename thunk) opens filename as the temporary current-input-port and calls thunk"
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "with-input-from-file", 1, car(args), "a string (a filename)"));
  if (!is_thunk(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, "with-input-from-file", 2, cadr(args), "a thunk"));
  
  return(with_input(sc, s7_open_input_file(sc, s7_string(car(args)), "r"), args));
}



static void char_to_string_port(char c, s7_pointer pt)
{
  if (port_string_point(pt) >= port_string_length(pt))
    {
      int loc;
      loc = port_string_length(pt);
      port_string_length(pt) *= 2;
      port_string(pt) = (char *)realloc(port_string(pt), port_string_length(pt) * sizeof(char));
      memset((void *)(port_string(pt) + loc), 0, loc);
    }
  port_string(pt)[port_string_point(pt)++] = c;
}


static void write_char(s7_scheme *sc, char c, s7_pointer pt) 
{
  if (pt == sc->NIL)
    fputc(c, stderr);
  else
    {
      if (port_is_closed(pt))
	return;
      if (is_file_port(pt))
	fputc(c, port_file(pt));
      else 
	{
	  if (is_string_port(pt))
	    char_to_string_port(c, pt);
	  else (*(port_output_function(pt)))(sc, c, pt);
	}
    }
}


static void write_string(s7_scheme *sc, const char *s, s7_pointer pt) 
{
  if (!s) return;
  
  if (pt == sc->NIL)
    fputs(s, stderr);
  else
    {
      if (port_is_closed(pt))
	return;
      
      if (is_file_port(pt))
	fputs(s, port_file(pt));
      else 
	{
	  if (is_string_port(pt))
	    {
	      for (; *s; s++)
		char_to_string_port(*s, pt);
	    }
	  else 
	    {
	      for (; *s; s++)
		(*(port_output_function(pt)))(sc, *s, pt);
	    }
	}
    }
}


static char *slashify_string(const char *p)
{
  int i, j = 0, len;
  char *s;
  len = safe_strlen(p);
  s = (char *)calloc(len + 256, sizeof(char));
  
  s[j++] = '"';
  for (i = 0; i < len; i++) 
    {
      if (slashify_table[(int)p[i]])
	{
	  s[j++] = '\\';
	  switch (p[i]) 
	    {
	    case '"':
	      s[j++] = '"';
	      break;
	      
#if 0
	    case '\n':
	      s[j++] = 'n';
	      break;
#endif
	      
	    case '\t':
	      s[j++] = 't';
	      break;
	      
	    case '\r':
	      s[j++] = 'r';
	      break;
	      
	    case '\\':
	      s[j++] = '\\';
	      break;
	      
	    default: 
	      { 
		int d = p[i] / 16;
		s[j++] = 'x';
		if (d < 10) 
		  s[j++] = d + '0';
		else s[j++] = d - 10 + 'A';
		
		d = p[i] % 16;
		if (d < 10) 
		  s[j++] = d + '0';
		else s[j++] = d - 10 + 'A';
	      }
	    }
	}
      else s[j++] = p[i];
    }
  s[j++] = '"';
  return(s);
}


static char *s7_atom_to_c_string(s7_scheme *sc, s7_pointer obj, bool use_write)
{
  switch (type(obj))
    {
    case T_BOOLEAN:
      if (obj == sc->T)
	return(s7_strdup("#t"));
      return(s7_strdup("#f"));

    case T_NIL:
      return(s7_strdup("()"));
  
    case T_UNTYPED:
      if (obj == sc->EOF_OBJECT)
	return(s7_strdup("#<eof>"));
  
      if (obj == sc->UNDEFINED) 
	return(s7_strdup("#<undefined>"));
  
      if (obj == sc->UNSPECIFIED) 
	return(s7_strdup("#<unspecified>"));
      break;

    case T_INPUT_PORT:
    case T_OUTPUT_PORT:
      return(describe_port(sc, obj));

    case T_NUMBER:
      return(s7_number_to_string_base_10(obj, 0, 14, 'g')); /* 20 digits is excessive in this context */
  
    case T_STRING:
      if (string_length(obj) > 0)
	{
	  if (!use_write) 
	    return(s7_strdup_with_len(string_value(obj), string_length(obj)));
	  return(slashify_string(string_value(obj)));
	}
      if (!use_write)
	return(NULL);
      else return(s7_strdup("\"\""));

    case T_CHARACTER:
      {
	#define P_SIZE 16
	char *p;
	unsigned char c;
	p = (char *)malloc(P_SIZE * sizeof(char));
	c = (unsigned char)s7_character(obj);             /* if not unsigned, (write (integer->char 212) -> #\xffffffd4! */
	if (!use_write) 
	  {
	    p[0]= c;
	    p[1]= 0;
	  } 
	else 
	  {
	    switch (c) 
	      {
	      case ' ':
		snprintf(p, P_SIZE, "#\\space"); 
		break;

	      case '\n':
		snprintf(p, P_SIZE, "#\\newline"); 
		break;

	      case '\r':
		snprintf(p, P_SIZE, "#\\return"); 
		break;

	      case '\t':
		snprintf(p, P_SIZE, "#\\tab"); 
		break;

	      case '\0':
		snprintf(p, P_SIZE, "#\\null");
		break;

	      default:
		if (c < 32) 
		  snprintf(p, P_SIZE, "#\\x%x", c);
		else snprintf(p, P_SIZE, "#\\%c", c); 
		break;
	      }
	  }
	return(p);
      }
  
    case T_SYMBOL:
      return(s7_strdup(symbol_name(obj)));
  
    case T_MACRO:
      return(s7_strdup("#<macro>"));
  
    case T_CLOSURE:
    case T_CLOSURE_STAR:
      {
	/* try to find obj in the current environment and return its name */
	s7_pointer binding;
	binding = s7_find_value_in_environment(sc, obj);
	if (is_pair(binding))
	  return(s7_strdup(symbol_name(car(binding))));
	return(s7_strdup("#<closure>"));
      }
  
    case T_C_FUNCTION:
      return(s7_strdup(c_function_name(obj)));

    case T_C_MACRO:
      return(s7_strdup(c_macro_name(obj)));
  
    case T_C_POINTER:
      return(s7_strdup("#<c_pointer>"));
  
    case T_CONTINUATION:
      return(s7_strdup("#<continuation>"));
  
    case T_GOTO:
      return(s7_strdup("#<goto>"));
  
    case T_CATCH:
      return(s7_strdup("#<catch>"));
  
    case T_DYNAMIC_WIND:
      return(s7_strdup("#<dynamic-wind>"));
  
    case T_C_OBJECT:
      return(s7_describe_object(sc, obj)); /* this allocates already */

    default:
      break;
    }

  {
    char *buf;
    buf = (char *)calloc(512, sizeof(char));
    snprintf(buf, 512, "<unknown object! type: %d (%s), flags: %x%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s>", 
	     type(obj), 
	     (type(obj) < BUILT_IN_TYPES) ? s7_type_name(obj) : "none",
	     typeflag(obj),
	     is_simple(obj) ? " simple" : "",
	     is_atom(obj) ? " atom" : "",
	     is_eternal(obj) ? " eternal" : "",
	     is_procedure(obj) ? " procedure" : "",
	     is_marked(obj) ? " gc-marked" : "",
	     is_immutable(obj) ? " immutable" : "",
	     is_syntax(obj) ? " syntax" : "",
	     dont_copy(obj) ? " dont-copy" : "",
	     ((typeflag(obj) & T_OBJECT) != 0) ? " obj" : "",
	     is_finalizable(obj) ? " gc-finalize" : "",
	     is_setter(obj) ? " setter" : "",
	     is_any_macro(obj) ? " (anymac)" : "",
	     is_expansion(obj) ? " (expansion)" : "",
	     (!is_not_local(obj)) ? " (local)" : "",
	     ((typeflag(obj) & UNUSED_BITS) != 0) ? " bad bits at top" : "");
    return(buf);
  }
}


bool s7_is_valid_pointer(s7_pointer arg)
{
  return((arg) &&
	 (type(arg) > T_UNTYPED) && (type(arg) < BUILT_IN_TYPES));
}


static char *s7_vector_to_c_string(s7_scheme *sc, s7_pointer vect, int depth, bool to_file);
static char *s7_list_to_c_string(s7_scheme *sc, s7_pointer lst, int depth);

static char *s7_object_to_c_string_1(s7_scheme *sc, s7_pointer obj, bool use_write, int depth, bool to_file)
{
  if ((s7_is_vector(obj)) ||
      (s7_is_hash_table(obj)))
    return(s7_vector_to_c_string(sc, obj, depth, to_file));

  if (is_pair(obj))
    return(s7_list_to_c_string(sc, obj, depth));

  return(s7_atom_to_c_string(sc, obj, use_write));
}


static char *object_to_c_string_with_circle_check(s7_scheme *sc, s7_pointer vr, int depth)
{
  int k, lim;

  lim = depth;
  if (lim >= CIRCULAR_REFS_SIZE) lim = CIRCULAR_REFS_SIZE - 1;

  for (k = 0; k <= lim; k++)
    if (s7_is_eq(vr, sc->circular_refs[k]))
      {
	if (s7_is_vector(vr))
	  return(s7_strdup("[circular vector]"));
	if (s7_is_hash_table(vr))
	  return(s7_strdup("[circular hash-table]"));
	return(s7_strdup("[circular list]"));
      }

  return(s7_object_to_c_string_1(sc, vr, true, depth + 1, false));
}


static char *s7_vector_to_c_string(s7_scheme *sc, s7_pointer vect, int depth, bool to_file)
{
  s7_Int i, len, bufsize = 0;
  bool too_long = false;
  char **elements = NULL;
  char *buf;
  
  len = vector_length(vect);
  if (len == 0)
    return(s7_strdup("#()"));
  
  if (!to_file)
    {
      int plen;
      /* if to_file we ignore *vector-print-length* so a subsequent read will be ok
       *
       * (with-output-to-file "test.test" 
       *   (lambda () 
       *     (let ((vect (make-vector (+ *vector-print-length* 2) 1.0))) 
       *       (write vect))))
       */
      plen = s7_integer(s7_symbol_value(sc, s7_make_symbol(sc, "*vector-print-length*")));
      if (len > plen)
	{
	  too_long = true;
	  len = plen;
	}
    }

  if (depth < CIRCULAR_REFS_SIZE)
    sc->circular_refs[depth] = vect;             /* (let ((v (vector 1 2))) (vector-set! v 0 v) v) */

  elements = (char **)malloc(len * sizeof(char *));
  for (i = 0; i < len; i++)
    {
      elements[i] = object_to_c_string_with_circle_check(sc, vector_element(vect, i), depth);
      bufsize += safe_strlen(elements[i]);
    }

  bufsize += (len * 2 + 256);
  buf = (char *)malloc(bufsize * sizeof(char));
  sprintf(buf, "#(");

  for (i = 0; i < len - 1; i++)
    {
      if (elements[i])
	{
	  strcat(buf, elements[i]);
	  free(elements[i]);
	  strcat(buf, " ");
	}
    }
  if (elements[len - 1])
    {
      strcat(buf, elements[len - 1]);
      free(elements[len - 1]);
    }
  free(elements);
  if (too_long)
    strcat(buf, " ...");
  strcat(buf, ")");
  return(buf);
}


static s7_pointer s7_vector_to_string(s7_scheme *sc, s7_pointer vect)
{
  char *buf;
  s7_pointer result;
  buf = s7_vector_to_c_string(sc, vect, 0, false);
  result = s7_make_string(sc, buf);
  free(buf);
  return(result);
}


static char *s7_list_to_c_string(s7_scheme *sc, s7_pointer lst, int depth)
{
  bool dotted = false;
  s7_pointer x;
  int i, len, bufsize = 0;
  char **elements = NULL;
  char *buf;

  len = s7_list_length(sc, lst);
  if (len < 0)                    /* a dotted list -- handle cars, then final cdr */
    {
      len = (-len + 1);
      dotted = true;
    }
  
  if (len == 0)                   /* either '() or a circular list */
    {
      if (lst != sc->NIL)
	return(s7_strdup("[circular list]"));
      return(s7_strdup("()"));
    }
  
  if (depth < CIRCULAR_REFS_SIZE)
    sc->circular_refs[depth] = lst;            /* (let ((l (list 1 2))) (list-set! l 0 l) l) */

  elements = (char **)malloc(len * sizeof(char *));
  for (x = lst, i = 0; is_pair(x) && (i < len); i++, x = cdr(x))
    {
      elements[i] = object_to_c_string_with_circle_check(sc, car(x), depth);
      bufsize += safe_strlen(elements[i]);
    }

  if (dotted)
    {
      if (s7_is_eq(x, lst))
	elements[i] = s7_strdup("[circular list]");
      elements[i] = object_to_c_string_with_circle_check(sc, x, depth);
      bufsize += safe_strlen(elements[i]);
    }
  
  bufsize += (256 + len * 2); /* len spaces */
  buf = (char *)malloc(bufsize * sizeof(char));
  
  sprintf(buf, "(");
  for (i = 0; i < len - 1; i++)
    {
      if (elements[i])
	{
	  strcat(buf, elements[i]);
	  strcat(buf, " ");
	}
    }
  if (dotted) strcat(buf, ". ");
  if (elements[len - 1])
    {
      strcat(buf, elements[len - 1]);
      strcat(buf, ")");
    }
  for (i = 0; i < len; i++)
    if (elements[i])
      free(elements[i]);
  free(elements);
  return(buf);
}


static s7_pointer s7_list_to_string(s7_scheme *sc, s7_pointer lst)
{
  s7_pointer result;
  char *buf;
  buf = s7_list_to_c_string(sc, lst, 0);
  result = s7_make_string(sc, buf);
  free(buf);
  return(result);
}


char *s7_object_to_c_string(s7_scheme *sc, s7_pointer obj)
{
  return(s7_object_to_c_string_1(sc, obj, true, 0, false));
}


s7_pointer s7_object_to_string(s7_scheme *sc, s7_pointer obj)
{
  char *str = NULL;
  s7_pointer x;
  if ((s7_is_vector(obj)) ||
      (s7_is_hash_table(obj)))
    return(s7_vector_to_string(sc, obj));

  if (is_pair(obj))
    return(s7_list_to_string(sc, obj));

  x = s7_make_string(sc, str = s7_atom_to_c_string(sc, obj, true));
  if (str) free(str);
  return(x);
}


void s7_newline(s7_scheme *sc, s7_pointer port)
{
  write_char(sc, '\n', port);
}


static s7_pointer g_newline(s7_scheme *sc, s7_pointer args)
{
  #define H_newline "(newline :optional port) writes a carriage return to the port"
  s7_pointer port;
  
  if (args != sc->NIL)
    {
      port = car(args);
      if (!s7_is_output_port(sc, port))
	return(s7_wrong_type_arg_error(sc, "newline", 0, port, "an output port"));
    }
  else port = sc->output_port;
  
  s7_newline(sc, port);
  return(sc->UNSPECIFIED);
}


void s7_write_char(s7_scheme *sc, char c, s7_pointer port)
{
  write_char(sc, c, port);
}


static s7_pointer g_write_char(s7_scheme *sc, s7_pointer args)
{
  #define H_write_char "(write-char char :optional port) writes char to the output port"
  s7_pointer port;
  
  if (!s7_is_character(car(args)))
    return(s7_wrong_type_arg_error(sc, "write-char", 1, car(args), "a character"));
  
  if (is_pair(cdr(args)))
    {
      port = cadr(args);
      if (!s7_is_output_port(sc, port))
	return(s7_wrong_type_arg_error(sc, "write-char", 2, port, "an output port"));
    }
  else port = sc->output_port;
  s7_write_char(sc, s7_character(car(args)), port);
  return(sc->UNSPECIFIED);
}


static void write_or_display(s7_scheme *sc, s7_pointer obj, s7_pointer port, bool use_write)
{
  char *val;
  val = s7_object_to_c_string_1(sc, obj, use_write, 0, is_file_port(port));
  write_string(sc, val, port);
  if (val) free(val);
}


void s7_write(s7_scheme *sc, s7_pointer obj, s7_pointer port)
{
  write_or_display(sc, obj, port, true);
}


static s7_pointer g_write(s7_scheme *sc, s7_pointer args)
{
  #define H_write "(write str :optional port) writes str (a string) to the output port"
  s7_pointer port;
  
  if (is_pair(cdr(args)))
    {
      port = cadr(args);
      if (!s7_is_output_port(sc, port))
	return(s7_wrong_type_arg_error(sc, "write", 2, port, "an output port"));
    }
  else port = sc->output_port;
  write_or_display(sc, car(args), port, true);
  return(sc->UNSPECIFIED);
}


void s7_display(s7_scheme *sc, s7_pointer obj, s7_pointer port)
{
  write_or_display(sc, obj, port, false);
}


static s7_pointer g_display(s7_scheme *sc, s7_pointer args)
{
  #define H_display "(display str :optional port) writes str (a string) to the output port"
  s7_pointer port;
  
  if (is_pair(cdr(args)))
    {
      port = cadr(args);
      if (!s7_is_output_port(sc, port))
	return(s7_wrong_type_arg_error(sc, "display", 2, port, "an output port"));
    }
  else port = sc->output_port;
  write_or_display(sc, car(args), port, false);
  return(sc->UNSPECIFIED);
}


static s7_pointer g_read_byte(s7_scheme *sc, s7_pointer args)
{
  #define H_read_byte "(read-byte :optional port): reads a byte from the input port"
  s7_pointer port;
  
  if (args != sc->NIL)
    port = car(args);
  else port = sc->input_port;

  if ((!is_input_port(port)) ||
      (!is_file_port(port)))
    return(s7_wrong_type_arg_error(sc, "read-byte", 0, port, "an input file port"));

  if (is_file_port(port))
    return(s7_make_integer(sc, fgetc(port_file(port))));

  return((*(port_input_function(port)))(sc, S7_READ_BYTE, port));
}


static s7_pointer g_write_byte(s7_scheme *sc, s7_pointer args)
{
  #define H_write_byte "(write-byte byte :optional port): writes byte to the output port"
  s7_pointer port;
  
  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "write-byte", 1, car(args), "an integer"));
  
  if (is_pair(cdr(args)))
    port = cadr(args);
  else port = sc->output_port;
  if ((!is_output_port(port)) ||
      (is_string_port(port)))
    return(s7_wrong_type_arg_error(sc, "write-byte", 2, port, "an output file or function port"));
  
  if (is_file_port(port))
    fputc((unsigned char)s7_integer(car(args)), port_file(port));
  else (*(port_output_function(port)))(sc, (char)s7_integer(car(args)), port);

  return(car(args));
}


static s7_pointer g_call_with_output_string(s7_scheme *sc, s7_pointer args)
{
  #define H_call_with_output_string "(call-with-output-string proc) opens a string port applies proc to it, then returns the collected output"
  s7_pointer port, result;

  if (!is_procedure(car(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-output-string", 1, car(args), "a procedure"));
  
  port = s7_open_output_string(sc);
  push_stack(sc, OP_UNWIND_OUTPUT, sc->F, port);
  sc->code = car(args);
  sc->args = make_list_1(sc, port);
  eval(sc, OP_APPLY);
  result = s7_make_string(sc, s7_get_output_string(sc, port));
  s7_close_output_port(sc, port);
  return(result);
}


static s7_pointer g_call_with_output_file(s7_scheme *sc, s7_pointer args)
{
  #define H_call_with_output_file "(call-with-output-file filename proc) opens filename and calls proc with the output port as its argument"
  s7_pointer port;
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-output-file", 1, car(args), "a string (a filename)"));
  if (!is_procedure(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "call-with-output-file", 2, cadr(args), "a procedure"));
  
  port = s7_open_output_file(sc, s7_string(car(args)), "w");
  push_stack(sc, OP_UNWIND_OUTPUT, sc->F, port);
  sc->code = cadr(args);
  sc->args = make_list_1(sc, port);
  eval(sc, OP_APPLY);
  s7_close_output_port(sc, port);

  if ((is_pair(sc->value)) &&                 /* (+ 100 (with-input-from-string "123" (lambda () (values (read) 1)))) */
      (car(sc->value) == sc->VALUES))
    sc->value = splice_in_values(sc, cdr(sc->value));

  return(sc->value);
}


static s7_pointer g_with_output_to_string(s7_scheme *sc, s7_pointer args)
{
  #define H_with_output_to_string "(with-output-to-string thunk) opens a string as a temporary current-output-port, calls thunk, then returns the collected output"
  s7_pointer old_output_port, result;

  if (!is_thunk(sc, car(args)))
    return(s7_wrong_type_arg_error(sc, "with-output-to-string", 1, car(args), "a thunk"));
  
  old_output_port = sc->output_port;
  sc->output_port = s7_open_output_string(sc);
  push_stack(sc, OP_UNWIND_OUTPUT, old_output_port, sc->output_port);
  sc->code = car(args);
  sc->args = sc->NIL;
  eval(sc, OP_APPLY);
  result = s7_make_string(sc, s7_get_output_string(sc, sc->output_port));
  s7_close_output_port(sc, sc->output_port);
  sc->output_port = old_output_port;
  return(result);
}

/* (string-ref (with-output-to-string (lambda () (write "1234") (values (get-output-string) 1)))) */


static s7_pointer g_with_output_to_file(s7_scheme *sc, s7_pointer args)
{
  #define H_with_output_to_file "(with-output-to-file filename thunk) opens filename as the temporary current-output-port and calls thunk"
  s7_pointer old_output_port;
  
  if (!s7_is_string(car(args)))
    return(s7_wrong_type_arg_error(sc, "with-output-to-file", 1, car(args), "a string (a filename)"));
  if (!is_thunk(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, "with-output-to-file", 2, cadr(args), "a thunk"));
  
  old_output_port = sc->output_port;
  sc->output_port = s7_open_output_file(sc, s7_string(car(args)), "w");
  push_stack(sc, OP_UNWIND_OUTPUT, old_output_port, sc->output_port);
  sc->code = cadr(args);
  sc->args = sc->NIL;
  eval(sc, OP_APPLY);
  s7_close_output_port(sc, sc->output_port);
  sc->output_port = old_output_port;

  if ((is_pair(sc->value)) &&            
      (car(sc->value) == sc->VALUES))
    sc->value = splice_in_values(sc, cdr(sc->value));

  return(sc->value);
}



/* -------------------------------- lists -------------------------------- */

static s7_pointer s7_immutable_cons(s7_scheme *sc, s7_pointer a, s7_pointer b)
{
  s7_pointer x;
  x = new_cell(sc); /* might trigger gc, expansion here does not help */
  car(x) = a;
  cdr(x) = b;
  set_type(x, T_PAIR | T_IMMUTABLE);
  return(x);
}


s7_pointer s7_cons(s7_scheme *sc, s7_pointer a, s7_pointer b) 
{
  s7_pointer x;

#if HAVE_PTHREADS
  x = new_cell(sc); /* might trigger gc */
#else
  /* expand new_cell for speed */
  if (sc->free_heap_top > 0)
    {
      x = sc->free_heap[--(sc->free_heap_top)];
      sc->temps[sc->temps_ctr++] = x;
      if (sc->temps_ctr >= sc->temps_size)
	sc->temps_ctr = 0;
    }
  else x = new_cell(sc);
#endif
  car(x) = a;
  cdr(x) = b;
  set_type(x, T_PAIR);
  return(x);
}


static s7_pointer s7_permanent_cons(s7_pointer a, s7_pointer b, int type)
{
  /* for the symbol table which is never GC'd (and its contents aren't marked) */
  s7_pointer x;
  x = (s7_cell *)calloc(1, sizeof(s7_cell));
  x->hloc = NOT_IN_HEAP;
  car(x) = a;
  cdr(x) = b;
  set_type(x, type);
  return(x);
}


bool s7_is_pair(s7_pointer p)     
{ 
  return(is_pair(p));
}


s7_pointer s7_car(s7_pointer p)           
{
  return(car(p));
}


s7_pointer s7_cdr(s7_pointer p)           
{
  return(cdr(p));
}


s7_pointer s7_set_car(s7_pointer p, s7_pointer q) 
{ 
  car(p) = q;
  return(p);
}


s7_pointer s7_set_cdr(s7_pointer p, s7_pointer q) 
{ 
  cdr(p) = q;
  return(p);
}


static s7_pointer make_list_1(s7_scheme *sc, s7_pointer a) 
{
  s7_pointer x;
  x = new_cell(sc);
  car(x) = a;
  cdr(x) = sc->NIL;
  set_type(x, T_PAIR);
  return(x);
}


static s7_pointer make_list_2(s7_scheme *sc, s7_pointer a, s7_pointer b) 
{
  s7_pointer x, y;
  y = new_cell(sc);
  car(y) = b;
  cdr(y) = sc->NIL;
  set_type(y, T_PAIR);
  x = new_cell(sc); /* order matters because the GC will see "y" and expect it to have legit car/cdr */
  car(x) = a;
  cdr(x) = y;
  set_type(x, T_PAIR);
  return(x);
}


static s7_pointer make_list_3(s7_scheme *sc, s7_pointer a, s7_pointer b, s7_pointer c) 
{
  s7_pointer x, y, z;
  z = new_cell(sc);
  car(z) = c;
  cdr(z) = sc->NIL;
  set_type(z, T_PAIR);
  y = new_cell(sc);
  car(y) = b;
  cdr(y) = z;
  set_type(y, T_PAIR);
  x = new_cell(sc);
  car(x) = a;
  cdr(x) = y;
  set_type(x, T_PAIR);
  return(x);
}


s7_pointer s7_list_ref(s7_scheme *sc, s7_pointer lst, int num)
{
  int i;
  s7_pointer x;
  
  if (num == 0)
    return(s7_car(lst));
  if (num < 0)
    return(sc->NIL);
  for (x = lst, i = 0; (i < num) && (is_pair(x)); i++, x = cdr(x)) {}
  if ((i == num) &&
      (is_pair(x)))
    return(car(x));
  return(sc->NIL);
}


s7_pointer s7_list_set(s7_scheme *sc, s7_pointer lst, int num, s7_pointer val)
{
  int i;
  s7_pointer x;
  for (x = lst, i = 0; (i < num) && (is_pair(x)); i++, x = cdr(x)) {}
  if ((i == num) &&
      (is_pair(x)))
    car(x) = val;
  return(val);
}


s7_pointer s7_member(s7_scheme *sc, s7_pointer sym, s7_pointer lst)
{
  s7_pointer x;
  for (x = lst; is_pair(x); x = cdr(x))
    if (s7_is_equal(sym, car(x)))
      return(x);
  return(sc->F);
}


s7_pointer s7_assoc(s7_scheme *sc, s7_pointer sym, s7_pointer lst)
{
  s7_pointer x;
  for (x = lst; is_pair(x); x = cdr(x))
    if ((is_pair(s7_car(x))) &&
	(s7_is_equal(sym, car(car(x)))))
      return(car(x));
  return(sc->F);
}


s7_pointer s7_reverse(s7_scheme *sc, s7_pointer a) 
{
  /* reverse list -- produce new list */
  s7_pointer p = sc->NIL;
  for ( ; is_pair(a); a = cdr(a)) 
    p = s7_cons(sc, car(a), p);
  if (a == sc->NIL)
    return(p);
  return(sc->NIL);
}


static s7_pointer s7_reverse_in_place(s7_scheme *sc, s7_pointer term, s7_pointer list) 
{
  s7_pointer p = list, result = term, q;
  while (p != sc->NIL)
    {
      q = cdr(p);
      if ((!is_pair(q)) &&
	  (q != sc->NIL))
	return(sc->NIL); /* improper list? */
      cdr(p) = result;
      result = p;
      p = q;
    }
  return(result);
}


static s7_pointer safe_reverse_in_place(s7_scheme *sc, s7_pointer list) /* "safe" here means we guarantee this list is unproblematic */
{
  s7_pointer p = list, result, q;
  result = sc->NIL;

  while (p != sc->NIL)
    {
      q = cdr(p);
      /*   also if (list == sc->NIL) || (cdr(list) == sc->NIL) return(list) */
      cdr(p) = result;
      result = p;
      p = q;
    }
  return(result);
}


s7_pointer s7_append(s7_scheme *sc, s7_pointer a, s7_pointer b) 
{
  s7_pointer p = b, q;
  
  if (a != sc->NIL) 
    {
      a = s7_reverse(sc, a);
      while (a != sc->NIL) 
	{
	  q = cdr(a);
	  cdr(a) = p;
	  p = a;
	  a = q;
	}
    }
  return(p);
}


static int safe_list_length(s7_scheme *sc, s7_pointer a)
{
  /* assume that "a" is a proper list */
  int i = 0;
  s7_pointer b;
  for (b = a; b != sc->NIL; i++, b = cdr(b)) {};
  return(i);
}


int s7_list_length(s7_scheme *sc, s7_pointer a) 
{
  int i;
  s7_pointer slow, fast;
  
  slow = fast = a;
  for (i = 0; ; i += 2)
    {
      if (!is_pair(fast))
	{
	  if (fast == sc->NIL)
	    return(i);
	  return(-i);
	}
      
      fast = cdr(fast);
      if (!is_pair(fast)) 
	{
	  if (fast == sc->NIL)
	    return(i + 1);
	  return(-i - 1);
	}
      
      fast = cdr(fast);
      slow = cdr(slow);
      if (fast == slow) 
	{
	  /* the fast pointer has looped back around and caught up
	     with the slow pointer, hence the structure is circular,
	     not of finite length, and therefore not a list */
	  return(0);
	}
    }
  return(0);
}


static s7_pointer g_is_null(s7_scheme *sc, s7_pointer args)
{
  #define H_is_null "(null? obj) returns #t if obj is the empty list"
  return(make_boolean(sc, car(args) == sc->NIL));
}


static s7_pointer g_is_pair(s7_scheme *sc, s7_pointer args)
{
  #define H_is_pair "(pair? obj) returns #t if obj is a pair (a non-empty list)"
  return(make_boolean(sc, is_pair(car(args))));
}


bool s7_is_list(s7_scheme *sc, s7_pointer p)
{
  return((p == sc->NIL) ||
	 (is_pair(p)));
}


static bool is_proper_list(s7_scheme *sc, s7_pointer lst)
{
  s7_pointer slow, fast;
  slow = fast = lst;
  while (true)
    {
      if (!is_pair(fast)) 
	return(fast == sc->NIL); /* else it's an improper list */
      
      fast = cdr(fast);
      if (!is_pair(fast)) 
	return(fast == sc->NIL);
      
      fast = cdr(fast);
      slow = cdr(slow);
      if (fast == slow) 
	return(false);
    }
  return(true);
}


static s7_pointer g_is_list(s7_scheme *sc, s7_pointer args)
{
  #define H_is_list "(list? obj) returns #t if obj is a list"
  return(make_boolean(sc, is_proper_list(sc, car(args))));
}


static s7_pointer g_make_list(s7_scheme *sc, s7_pointer args)
{
  #define H_make_list "(make-list length (initial-element #f)) returns a list of 'length' elements whose value is 'initial-element'."

  s7_pointer init, result;
  int i, len;

  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "make-list", 1, car(args), "an integer"));
  len = s7_integer(car(args));
  if (len < 0)
    return(s7_out_of_range_error(sc, "make-list", 1, car(args), "length should be non-negative"));
  if (len == 0) return(sc->NIL);          /* what about (make-list 0 123)? */

  if (is_pair(cdr(args)))
    init = cadr(args);
  else init = sc->F;
  
  result = sc->NIL;
  if (sc->free_heap_top <= (unsigned int)len) gc(sc);

  s7_gc_on(sc, false);
  for (i = 0; i < len; i++)
    result = s7_cons(sc, init, result);
  s7_gc_on(sc, true);

  return(result);
}


static s7_pointer g_list_ref(s7_scheme *sc, s7_pointer args)
{
  #define H_list_ref "(list-ref lst i) returns the i-th element (0-based) of the list"
  
  int i;
  s7_Int index;
  s7_pointer p;

  if (!is_pair(car(args)))
    return(s7_wrong_type_arg_error(sc, "list-ref", 1, car(args), "a pair"));
  if ((!s7_is_integer(cadr(args))) ||
      (s7_integer(cadr(args)) < 0))
    return(s7_wrong_type_arg_error(sc, "list-ref", 2, cadr(args), "a non-negative integer"));
  
  index = s7_integer(cadr(args));
  if (index < 0)
    return(s7_out_of_range_error(sc, "list-ref", 2, cadr(args), "index should be non-negative"));
  
  for (i = 0, p = car(args); (i < index) && is_pair(p); i++, p = cdr(p)) {}
  
  if (p == sc->NIL)
    return(s7_out_of_range_error(sc, "list-ref", 2, cadr(args), "index should be less than list length"));
  if (!is_pair(p))
    return(s7_wrong_type_arg_error(sc, "list-ref", i, p, "a proper list"));
  
  return(car(p));
}


static s7_pointer g_list_set(s7_scheme *sc, s7_pointer args)
{
  #define H_list_set "(list-set! lst i val) sets the i-th element (0-based) of the list to val"
  
  int i;
  s7_Int index;
  s7_pointer p;
  
  if (!is_pair(car(args)))
    return(s7_wrong_type_arg_error(sc, "list-set!", 1, car(args), "a pair"));
  if ((!s7_is_integer(cadr(args))) ||
      (s7_integer(cadr(args)) < 0))
    return(s7_wrong_type_arg_error(sc, "list-set!", 2, cadr(args), "a non-negative integer"));
  
  index = s7_integer(cadr(args));
  if (index < 0)
    return(s7_out_of_range_error(sc, "list-set!", 2, cadr(args), "index should be non-negative"));
  
  for (i = 0, p = car(args); (i < index) && is_pair(p); i++, p = cdr(p)) {}
  
  if (p == sc->NIL)
    return(s7_out_of_range_error(sc, "list-set!", 2, cadr(args), "index should be less than list length"));
  if (!is_pair(p))
    return(s7_wrong_type_arg_error(sc, "list-set!", i, p, "a proper list"));
  
  car(p) = caddr(args);
  return(caddr(args));
}


static s7_pointer g_list_tail(s7_scheme *sc, s7_pointer args)
{
  #define H_list_tail "(list-tail lst i) returns the list from the i-th element on"
  
  int i;
  s7_Int index;
  s7_pointer p;

  if ((!is_pair(car(args))) &&
      (car(args) != sc->NIL))
    return(s7_wrong_type_arg_error(sc, "list-tail", 1, car(args), "a list"));
  if (!s7_is_integer(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "list-tail", 2, cadr(args), "an integer"));
  
  index = s7_integer(cadr(args));
  if (index < 0)
    return(s7_out_of_range_error(sc, "list-tail", 2, cadr(args), "index should be non-negative"));
  
  for (i = 0, p = car(args); (i < index) && is_pair(p); i++, p = cdr(p)) {}
  
  if (i < index)
    return(s7_out_of_range_error(sc, "list-tail", 2, cadr(args), "index should be less than list length"));
  
  return(p);
}


static s7_pointer g_car(s7_scheme *sc, s7_pointer args)
{
  #define H_car "(car pair) returns the first element of the pair"
  if (!is_pair(car(args))) return(s7_wrong_type_arg_error(sc, "car", 0, car(args), "a pair"));
  
  return(caar(args));
}


static s7_pointer g_cdr(s7_scheme *sc, s7_pointer args)
{
  #define H_cdr "(cdr pair) returns the second element of the pair"
  if (!is_pair(car(args))) return(s7_wrong_type_arg_error(sc, "cdr", 0, car(args), "a pair"));
  
  return(cdar(args));
}


static s7_pointer g_cons(s7_scheme *sc, s7_pointer args)
{
  /* n-ary cons could be the equivalent of CL's list*? */
  /*   it would be neater to have a single cons cell able to contain (directly) any number of elements */
  /*   (set! (cadr (cons 1 2 3)) 4) -> (1 4 . 3) */

  #define H_cons "(cons a b) returns a pair containing a and b"
  
  /* cdr(args) = cadr(args);
   * this is not safe -- it changes a variable's value directly:
   *   (let ((lst (list 1 2))) (list (apply cons lst) lst)) -> '((1 . 2) (1 . 2))
   */
  s7_pointer x;

#if HAVE_PTHREADS
  x = new_cell(sc); /* might trigger gc */
#else
  /* expand new_cell for speed */
  if (sc->free_heap_top > 0)
    {
      x = sc->free_heap[--(sc->free_heap_top)];
      sc->temps[sc->temps_ctr++] = x;
      if (sc->temps_ctr >= sc->temps_size)
	sc->temps_ctr = 0;
    }
  else x = new_cell(sc);
#endif
  car(x) = car(args);
  cdr(x) = cadr(args);
  set_type(x, T_PAIR);
  return(x);
}


static s7_pointer g_set_car(s7_scheme *sc, s7_pointer args)
{
  #define H_set_car "(set-car! pair val) sets the pair's first element to val"
  
  if (!is_pair(car(args)))  return(s7_wrong_type_arg_error(sc, "set-car!", 1, car(args), "a pair"));
  if (s7_is_immutable(car(args))) return(s7_wrong_type_arg_error(sc, "set-car!", 1, car(args), "a mutable pair"));
  
  caar(args) = cadr(args);
  /* return(args);
   * (set-car! (cdr (cons 1 (cons 2 3))) 4) -> ((4 . 3) 4)?? 
   *    perhaps this should return #<unspecified> instead since:
   * (let ((lst (cons 1 (cons 2 3)))) (set-car! (cdr lst) 4) lst) -> (1 4 . 3)
   */
  return(sc->UNSPECIFIED);
}


static s7_pointer g_set_cdr(s7_scheme *sc, s7_pointer args)
{
  #define H_set_cdr "(set-cdr! pair val) sets the pair's second element to val"
  
  if (!is_pair(car(args))) return(s7_wrong_type_arg_error(sc, "set-cdr!", 1, car(args), "a pair"));
  if (s7_is_immutable(car(args))) return(s7_wrong_type_arg_error(sc, "set-cdr!", 1, car(args), "a mutable pair"));
  
  cdar(args) = cadr(args);
  return(sc->UNSPECIFIED); /* see above */
}


static s7_pointer g_caar(s7_scheme *sc, s7_pointer args)
{
  #define H_caar "(caar lst) returns (car (car lst))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "caar", 0, lst, "a list whose car is also a list"));

 return(car(car(lst)));
}


static s7_pointer g_cadr(s7_scheme *sc, s7_pointer args)
{
  #define H_cadr "(cadr lst) returns (car (cdr lst))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cadr", 0, lst, "a list whose cdr is also a list"));

  return(car(cdr(lst)));
}


static s7_pointer g_cdar(s7_scheme *sc, s7_pointer args)
{
  #define H_cdar "(cdar lst) returns (cdr (car lst))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cdar", 0, lst, "a list whose car is also a list"));

  return(cdr(car(lst)));
}


static s7_pointer g_cddr(s7_scheme *sc, s7_pointer args)
{
  #define H_cddr "(cddr lst) returns (cdr (cdr lst))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cddr", 0, lst, "a list whose cdr is also a list"));

  return(cdr(cdr(lst)));
}


static s7_pointer g_caaar(s7_scheme *sc, s7_pointer args)
{
  #define H_caaar "(caaar lst) returns (car (car (car lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caaar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "caaar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(car(car(lst)))) return(s7_wrong_type_arg_error(sc, "caaar", 0, lst, "a list whose caar is also a list"));

  return(car(car(car(lst))));
}


static s7_pointer g_caadr(s7_scheme *sc, s7_pointer args)
{
  #define H_caadr "(caadr lst) returns (car (car (cdr lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "caadr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(car(cdr(lst)))) return(s7_wrong_type_arg_error(sc, "caadr", 0, lst, "a list whose cadr is also a list"));

  return(car(car(cdr(lst))));
}


static s7_pointer g_cadar(s7_scheme *sc, s7_pointer args)
{
  #define H_cadar "(cadar lst) returns (car (cdr (car lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cadar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cadar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(cdr(car(lst)))) return(s7_wrong_type_arg_error(sc, "cadar", 0, lst, "a list whose cdar is also a list"));

  return(car(cdr(car(lst))));
}


static s7_pointer g_cdaar(s7_scheme *sc, s7_pointer args)
{
  #define H_cdaar "(cdaar lst) returns (cdr (car (car lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdaar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cdaar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(car(car(lst)))) return(s7_wrong_type_arg_error(sc, "cdaar", 0, lst, "a list whose caar is also a list"));

  return(cdr(car(car(lst))));
}


static s7_pointer g_caddr(s7_scheme *sc, s7_pointer args)
{
  #define H_caddr "(caddr lst) returns (car (cdr (cdr lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "caddr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cdr(cdr(lst)))) return(s7_wrong_type_arg_error(sc, "caddr", 0, lst, "a list whose cddr is also a list"));

  return(car(cdr(cdr(lst))));
}


static s7_pointer g_cdddr(s7_scheme *sc, s7_pointer args)
{
  #define H_cdddr "(cdddr lst) returns (cdr (cdr (cdr lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cdddr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cdr(cdr(lst)))) return(s7_wrong_type_arg_error(sc, "cdddr", 0, lst, "a list whose cddr is also a list"));

  return(cdr(cdr(cdr(lst))));
}


static s7_pointer g_cdadr(s7_scheme *sc, s7_pointer args)
{
  #define H_cdadr "(cdadr lst) returns (cdr (car (cdr lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cdadr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(car(cdr(lst)))) return(s7_wrong_type_arg_error(sc, "cdadr", 0, lst, "a list whose cadr is also a list"));

  return(cdr(car(cdr(lst))));
}


static s7_pointer g_cddar(s7_scheme *sc, s7_pointer args)
{
  #define H_cddar "(cddar lst) returns (cdr (cdr (car lst)))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cddar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cddar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(cdr(car(lst)))) return(s7_wrong_type_arg_error(sc, "cddar", 0, lst, "a list whose cdar is also a list"));

  return(cdr(cdr(car(lst))));
}


static s7_pointer g_caaaar(s7_scheme *sc, s7_pointer args)
{
  #define H_caaaar "(caaaar lst) returns (car (car (car (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caaaar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "caaaar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(caar(lst))) return(s7_wrong_type_arg_error(sc, "caaaar", 0, lst, "a list whose caar is also a list"));
  if (!is_pair(caaar(lst))) return(s7_wrong_type_arg_error(sc, "caaaar", 0, lst, "a list whose caaar is also a list"));

  return(car(car(car(car(lst)))));
}


static s7_pointer g_caaadr(s7_scheme *sc, s7_pointer args)
{
  #define H_caaadr "(caaadr lst) returns (car (car (car (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caaadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "caaadr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cadr(lst))) return(s7_wrong_type_arg_error(sc, "caaadr", 0, lst, "a list whose cadr is also a list"));
  if (!is_pair(caadr(lst))) return(s7_wrong_type_arg_error(sc, "caaadr", 0, lst, "a list whose caadr is also a list"));

  return(car(car(car(cdr(lst)))));
}


static s7_pointer g_caadar(s7_scheme *sc, s7_pointer args)
{
  #define H_caadar "(caadar lst) returns (car (car (cdr (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caadar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "caadar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(cdar(lst))) return(s7_wrong_type_arg_error(sc, "caadar", 0, lst, "a list whose cdar is also a list"));
  if (!is_pair(cadar(lst))) return(s7_wrong_type_arg_error(sc, "caadar", 0, lst, "a list whose cadar is also a list"));

  return(car(car(cdr(car(lst)))));
}


static s7_pointer g_cadaar(s7_scheme *sc, s7_pointer args)
{
  #define H_cadaar "(cadaar lst) returns (car (cdr (car (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cadaar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cadaar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(caar(lst))) return(s7_wrong_type_arg_error(sc, "cadaar", 0, lst, "a list whose caar is also a list"));
  if (!is_pair(cdaar(lst))) return(s7_wrong_type_arg_error(sc, "cadaar", 0, lst, "a list whose cdaar is also a list"));

  return(car(cdr(car(car(lst)))));
}


static s7_pointer g_caaddr(s7_scheme *sc, s7_pointer args)
{
  #define H_caaddr "(caaddr lst) returns (car (car (cdr (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caaddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "caaddr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cddr(lst))) return(s7_wrong_type_arg_error(sc, "caaddr", 0, lst, "a list whose cddr is also a list"));
  if (!is_pair(caddr(lst))) return(s7_wrong_type_arg_error(sc, "caaddr", 0, lst, "a list whose caddr is also a list"));

  return(car(car(cdr(cdr(lst)))));
}


static s7_pointer g_cadddr(s7_scheme *sc, s7_pointer args)
{
  #define H_cadddr "(cadddr lst) returns (car (cdr (cdr (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cadddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cadddr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cddr(lst))) return(s7_wrong_type_arg_error(sc, "cadddr", 0, lst, "a list whose cddr is also a list"));
  if (!is_pair(cdddr(lst))) return(s7_wrong_type_arg_error(sc, "cadddr", 0, lst, "a list whose cdddr is also a list"));

  return(car(cdr(cdr(cdr(lst)))));
}


static s7_pointer g_cadadr(s7_scheme *sc, s7_pointer args)
{
  #define H_cadadr "(cadadr lst) returns (car (cdr (car (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cadadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cadadr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cadr(lst))) return(s7_wrong_type_arg_error(sc, "cadadr", 0, lst, "a list whose cadr is also a list"));
  if (!is_pair(cdadr(lst))) return(s7_wrong_type_arg_error(sc, "cadadr", 0, lst, "a list whose cdadr is also a list"));

  return(car(cdr(car(cdr(lst)))));
}


static s7_pointer g_caddar(s7_scheme *sc, s7_pointer args)
{
  #define H_caddar "(caddar lst) returns (car (cdr (cdr (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "caddar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "caddar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(cdar(lst))) return(s7_wrong_type_arg_error(sc, "caddar", 0, lst, "a list whose cdar is also a list"));
  if (!is_pair(cddar(lst))) return(s7_wrong_type_arg_error(sc, "caddar", 0, lst, "a list whose cddar is also a list"));

  return(car(cdr(cdr(car(lst)))));
}


static s7_pointer g_cdaaar(s7_scheme *sc, s7_pointer args)
{
  #define H_cdaaar "(cdaaar lst) returns (cdr (car (car (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdaaar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cdaaar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(caar(lst))) return(s7_wrong_type_arg_error(sc, "cdaaar", 0, lst, "a list whose caar is also a list"));
  if (!is_pair(caaar(lst))) return(s7_wrong_type_arg_error(sc, "cdaaar", 0, lst, "a list whose caaar is also a list"));

  return(cdr(car(car(car(lst)))));
}


static s7_pointer g_cdaadr(s7_scheme *sc, s7_pointer args)
{
  #define H_cdaadr "(cdaadr lst) returns (cdr (car (car (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdaadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cdaadr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cadr(lst))) return(s7_wrong_type_arg_error(sc, "cdaadr", 0, lst, "a list whose cadr is also a list"));
  if (!is_pair(caadr(lst))) return(s7_wrong_type_arg_error(sc, "cdaadr", 0, lst, "a list whose caadr is also a list"));

  return(cdr(car(car(cdr(lst)))));
}


static s7_pointer g_cdadar(s7_scheme *sc, s7_pointer args)
{
  #define H_cdadar "(cdadar lst) returns (cdr (car (cdr (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdadar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cdadar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(cdar(lst))) return(s7_wrong_type_arg_error(sc, "cdadar", 0, lst, "a list whose cdar is also a list"));
  if (!is_pair(cadar(lst))) return(s7_wrong_type_arg_error(sc, "cdadar", 0, lst, "a list whose cadar is also a list"));

  return(cdr(car(cdr(car(lst)))));
}


static s7_pointer g_cddaar(s7_scheme *sc, s7_pointer args)
{
  #define H_cddaar "(cddaar lst) returns (cdr (cdr (car (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cddaar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cddaar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(caar(lst))) return(s7_wrong_type_arg_error(sc, "cddaar", 0, lst, "a list whose caar is also a list"));
  if (!is_pair(cdaar(lst))) return(s7_wrong_type_arg_error(sc, "cddaar", 0, lst, "a list whose cdaar is also a list"));

  return(cdr(cdr(car(car(lst)))));
}


static s7_pointer g_cdaddr(s7_scheme *sc, s7_pointer args)
{
  #define H_cdaddr "(cdaddr lst) returns (cdr (car (cdr (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdaddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cdaddr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cddr(lst))) return(s7_wrong_type_arg_error(sc, "cdaddr", 0, lst, "a list whose cddr is also a list"));
  if (!is_pair(caddr(lst))) return(s7_wrong_type_arg_error(sc, "cdaddr", 0, lst, "a list whose caddr is also a list"));

  return(cdr(car(cdr(cdr(lst)))));
}


static s7_pointer g_cddddr(s7_scheme *sc, s7_pointer args)
{
  #define H_cddddr "(cddddr lst) returns (cdr (cdr (cdr (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cddddr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cddddr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cddr(lst))) return(s7_wrong_type_arg_error(sc, "cddddr", 0, lst, "a list whose cddr is also a list"));
  if (!is_pair(cdddr(lst))) return(s7_wrong_type_arg_error(sc, "cddddr", 0, lst, "a list whose cdddr is also a list"));

  return(cdr(cdr(cdr(cdr(lst)))));
}


static s7_pointer g_cddadr(s7_scheme *sc, s7_pointer args)
{
  #define H_cddadr "(cddadr lst) returns (cdr (cdr (car (cdr lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cddadr", 0, lst, "a pair"));
  if (!is_pair(cdr(lst))) return(s7_wrong_type_arg_error(sc, "cddadr", 0, lst, "a list whose cdr is also a list"));
  if (!is_pair(cadr(lst))) return(s7_wrong_type_arg_error(sc, "cddadr", 0, lst, "a list whose cadr is also a list"));
  if (!is_pair(cdadr(lst))) return(s7_wrong_type_arg_error(sc, "cddadr", 0, lst, "a list whose cdadr is also a list"));

  return(cdr(cdr(car(cdr(lst)))));
}


static s7_pointer g_cdddar(s7_scheme *sc, s7_pointer args)
{
  #define H_cdddar "(cdddar lst) returns (cdr (cdr (cdr (car lst))))"
  s7_pointer lst = car(args);

  if (!is_pair(lst)) return(s7_wrong_type_arg_error(sc, "cdddar", 0, lst, "a pair"));
  if (!is_pair(car(lst))) return(s7_wrong_type_arg_error(sc, "cdddar", 0, lst, "a list whose car is also a list"));
  if (!is_pair(cdar(lst))) return(s7_wrong_type_arg_error(sc, "cdddar", 0, lst, "a list whose cdar is also a list"));
  if (!is_pair(cddar(lst))) return(s7_wrong_type_arg_error(sc, "cdddar", 0, lst, "a list whose cddar is also a list"));

  return(cdr(cdr(cdr(car(lst)))));
}


static s7_pointer g_reverse(s7_scheme *sc, s7_pointer args)
{
  #define H_reverse "(reverse lst) returns a list with the elements of lst in reverse order"
  s7_pointer p, np;
  
  p = car(args);
  if (p == sc->NIL)
    return(sc->NIL);

  if (!is_pair(p))
    return(s7_wrong_type_arg_error(sc, "reverse", 0, p, "a list"));
  
  np = s7_reverse(sc, p);
  if (np == sc->NIL)
    return(s7_wrong_type_arg_error(sc, "reverse", 0, p, "a proper list"));
  
  return(np);
}


static s7_pointer g_reverse_in_place(s7_scheme *sc, s7_pointer args)
{
  #define H_reverse_in_place "(reverse! lst) reverses lst in place"
  s7_pointer p, np;
  
  p = car(args);
  if (p == sc->NIL)
    return(sc->NIL);
  
  if (!is_pair(p))
    return(s7_wrong_type_arg_error(sc, "reverse!", 0, p, "a list"));
  
  np = s7_reverse_in_place(sc, sc->NIL, p);
  if (np == sc->NIL)
    return(s7_wrong_type_arg_error(sc, "reverse!", 0, p, "a proper list"));
  
  return(np);
}


s7_pointer s7_remv(s7_scheme *sc, s7_pointer a, s7_pointer obj) 
{
  /* used in xen.c */
  s7_pointer p = sc->NIL;
  for ( ; is_pair(a); a = cdr(a))
    if (car(a) != obj)
      p = s7_cons(sc, car(a), p);
  return(s7_reverse(sc, p));
}


static s7_pointer g_assq_1(s7_scheme *sc, s7_pointer args, const char *name, bool (*eq_func)(s7_pointer a, s7_pointer b))
{
  #define H_assq "(assq obj alist) returns the key-value pair associated (via eq?) with the key obj in the association list alist"
  #define H_assv "(assv obj alist) returns the key-value pair associated (via eqv?) with the key obj in the association list alist"
  #define H_assoc "(assoc obj alist) returns the key-value pair associated (via equal?) with the key obj in the association list alist"
  s7_pointer x, y;

  if (!s7_is_list(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, name, 2, cadr(args), "a list"));

  if (cadr(args) == sc->NIL)
    return(sc->F);

  if (s7_list_length(sc, cadr(args)) <= 0)
    return(s7_wrong_type_arg_error(sc, name, 2, cadr(args), "a proper list"));
  
  x = car(args);
  for (y = cadr(args); is_pair(y) && is_pair(car(y)); y = cdr(y)) 
    if (eq_func(x, caar(y)))
      return(car(y));
  
  return(sc->F);
}      


static s7_pointer g_assq(s7_scheme *sc, s7_pointer args) {return(g_assq_1(sc, args, "assq", s7_is_eq));}
static s7_pointer g_assv(s7_scheme *sc, s7_pointer args) {return(g_assq_1(sc, args, "assv", s7_is_eqv));}
static s7_pointer g_assoc(s7_scheme *sc, s7_pointer args) {return(g_assq_1(sc, args, "assoc", s7_is_equal));}


static s7_pointer g_memq_1(s7_scheme *sc, s7_pointer args, const char *name, bool (*eq_func)(s7_pointer a, s7_pointer b))
{
  #define H_memq "(memq obj list) looks for obj in list and returns the list from that point if it is found, otherwise #f. memq uses eq?"
  #define H_memv "(memv obj list) looks for obj in list and returns the list from that point if it is found, otherwise #f. memv uses eqv?"
  #define H_member "(member obj list) looks for obj in list and returns the list from that point if it is found, otherwise #f. member uses equal?"
  s7_pointer x;

  if (!s7_is_list(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, name, 2, cadr(args), "a list"));

  if (cadr(args) == sc->NIL)
    return(sc->F);

  if (s7_list_length(sc, cadr(args)) <= 0)
    return(s7_wrong_type_arg_error(sc, name, 2, cadr(args), "a proper list"));
  
  for (x = cadr(args); is_pair(x); x = cdr(x)) 
    if (eq_func(car(args), car(x)))
      return(x);
  
  return(sc->F);
}     


static s7_pointer g_memq(s7_scheme *sc, s7_pointer args) {return(g_memq_1(sc, args, "memq", s7_is_eq));}
static s7_pointer g_memv(s7_scheme *sc, s7_pointer args) {return(g_memq_1(sc, args, "memv", s7_is_eqv));}
static s7_pointer g_member(s7_scheme *sc, s7_pointer args) {return(g_memq_1(sc, args, "member", s7_is_equal));}


static bool is_member(s7_pointer sym, s7_pointer lst)
{
  s7_pointer x;
  for (x = lst; is_pair(x); x = cdr(x))
    if (sym == car(x))
      return(true);
  return(false);
}


static s7_pointer g_is_provided(s7_scheme *sc, s7_pointer args)
{
  #define H_is_provided "(provided? sym) returns #t if sym is a member of the *features* list"
  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "provided?", 1, car(args), "a symbol"));

  return(make_boolean(sc, is_member(car(args), s7_name_to_value(sc, "*features*"))));
}


static s7_pointer g_provide(s7_scheme *sc, s7_pointer args)
{
  #define H_provide "(provide sym) adds sym to the *features* list"
  s7_pointer features;

  if (!s7_is_symbol(car(args)))
    return(s7_wrong_type_arg_error(sc, "provide", 1, car(args), "a symbol"));

  features = s7_make_symbol(sc, "*features*");
  if (!is_member(car(args), s7_symbol_value(sc, features)))
    {
#if WITH_ENCAPSULATION
      if (is_encapsulating(sc)) encapsulate(sc, features);
#endif

      s7_symbol_set_value(sc, 
			  features,
			  s7_cons(sc, 
				  car(args), 
				  s7_symbol_value(sc, features)));
    }
  return(car(args));
}


static s7_pointer g_list(s7_scheme *sc, s7_pointer args)
{
  #define H_list "(list ...) returns its arguments in a list"

  return(args);
}


static s7_pointer g_append(s7_scheme *sc, s7_pointer args)
{
  #define H_append "(append ...) returns its argument lists appended into one list"
  s7_pointer x, y;
  int i;

  if (args == sc->NIL) 
    return(sc->NIL);

  if (cdr(args) == sc->NIL)
    return(car(args)); 
  
  x = sc->NIL;
  for (i = 1, y = args; y != sc->NIL; i++, y = cdr(y)) 
    {
      if (cdr(y) == sc->NIL)
	return(s7_append(sc, x, car(y)));
      
      if (!is_proper_list(sc, car(y)))
	return(s7_wrong_type_arg_error(sc, "append", i, car(y), "a list"));
      
      x = s7_append(sc, x, car(y));
    }
  return(x);
}



/* -------------------------------- vectors -------------------------------- */

bool s7_is_vector(s7_pointer p)    
{ 
  return(type(p) == T_VECTOR);
}


static s7_pointer s7_make_vector_1(s7_scheme *sc, s7_Int len, bool filled) 
{
  s7_pointer x;
  if (len > 0)
    {
      /* len is an "int" currently */
      float ilog2;

      ilog2 = log((double)len) / log(2.0);
      if (sizeof(size_t) > 4)
	{
	  if (ilog2 > 56.0)
	    return(s7_out_of_range_error(sc, "make-vector", 1, s7_make_integer(sc, len), "length should be less than about 2^56 probably"));
	}
      else
	{
	  if (ilog2 > 28.0)
	    return(s7_out_of_range_error(sc, "make-vector", 1, s7_make_integer(sc, len), "length should be less than about 2^28 probably"));
	}
    }

  /* this has to follow the error checks!  (else garbage in temps array confuses GC when "vector" is finalized) */
  x = new_cell(sc);
  set_type(x, T_VECTOR | T_FINALIZABLE | T_DONT_COPY);
  vector_length(x) = len;
  if (len > 0)
    {
      vector_elements(x) = (s7_pointer *)malloc(len * sizeof(s7_pointer));
      if (!(vector_elements(x)))
	{
	  vector_length(x) = 0;
	  return(s7_error(sc, s7_make_symbol(sc, "out-of-memory"), s7_make_string(sc, "make-vector allocation failed!")));
	}

      if (filled) s7_vector_fill(sc, x, sc->NIL);
    }
  else vector_elements(x) = NULL;
#if WITH_MULTIDIMENSIONAL_VECTORS
  x->object.vector.dim_info = NULL;
#endif

  return(x);
}


s7_pointer s7_make_vector(s7_scheme *sc, s7_Int len)
{
  return(s7_make_vector_1(sc, len, true));
}


s7_Int s7_vector_length(s7_pointer vec)
{
  return(vector_length(vec));
}


#if (!WITH_GMP)
void s7_vector_fill(s7_scheme *sc, s7_pointer vec, s7_pointer obj)
#else
static void vector_fill(s7_scheme *sc, s7_pointer vec, s7_pointer obj)
#endif
{
  s7_Int len;
  s7_pointer *tp;

  tp = (s7_pointer *)(vector_elements(vec));
  len = vector_length(vec);

  if ((obj == sc->NIL) || (obj == sc->UNSPECIFIED))
    {
      s7_Int i;
      s7_pointer *v_els, *from_els;
      v_els = vector_elements(vec);
      if (obj == sc->NIL)
	from_els = sc->nil_vector;
      else from_els = sc->unspecified_vector;

      for (i = 0; i < len; i += BLOCK_VECTOR_SIZE)
	memcpy((void *)(v_els + i),
	       (void *)from_els,
	       (((i + BLOCK_VECTOR_SIZE) > len) ? (len - i) : BLOCK_VECTOR_SIZE) * sizeof(s7_pointer));
      return;
    }

  /* splitting out the int case saves a lot of compute time */
  if (len < INT_MAX)
    {
      int j, jlen;
      jlen = len;
      for (j = 0; j < jlen; j++) 
	tp[j] = obj;
    }
  else
    {
      s7_Int i;
      for (i = 0; i < len; i++) 
	tp[i] = obj;
    }
}


static s7_pointer s7_vector_copy(s7_scheme *sc, s7_pointer old_vect)
{
  s7_Int len;
  s7_pointer new_vect;

  len = vector_length(old_vect);
  new_vect = s7_make_vector_1(sc, len, false);
  /* 
   * here and in vector-fill! we have a problem with bignums -- should new bignums be allocated? (copy_list also)
   */
  memcpy((void *)(vector_elements(new_vect)), (void *)(vector_elements(old_vect)), len * sizeof(s7_pointer));

  return(new_vect);
}


static s7_pointer g_vector_fill(s7_scheme *sc, s7_pointer args)
{
  #define H_vector_fill "(vector-fill! v val) sets all elements of the vector v to val"
  s7_pointer x;
  x = car(args);

  if (!s7_is_vector(x))
    return(s7_wrong_type_arg_error(sc, "vector-fill!", 1, x, "a vector"));
  if (s7_is_immutable(x))
    return(s7_wrong_type_arg_error(sc, "vector-fill!", 1, x, "a mutable vector"));

  s7_vector_fill(sc, x, cadr(args));
  return(sc->UNSPECIFIED);
}


s7_pointer s7_vector_ref(s7_scheme *sc, s7_pointer vec, s7_Int index) 
{
  if (index >= vector_length(vec))
    return(s7_out_of_range_error(sc, "vector-ref", 2, s7_make_integer(sc, index), "index should be less than vector length"));

  return(vector_element(vec, index));
}


s7_pointer s7_vector_set(s7_scheme *sc, s7_pointer vec, s7_Int index, s7_pointer a) 
{
  /* it's possible to have a vector that points to itself:
   *   (let ((v (make-vector 2))) (vector-set! v 0 v) v)
   */
  if (index >= vector_length(vec))
    return(s7_out_of_range_error(sc, "vector-set!", 2, s7_make_integer(sc, index), "index should be less than vector length"));

  vector_element(vec, index) = a;
  return(a);
}


s7_pointer *s7_vector_elements(s7_pointer vec)
{
  return(vector_elements(vec));
}


s7_Int *s7_vector_dimensions(s7_pointer vec)
{
  s7_Int *dims;
#if WITH_MULTIDIMENSIONAL_VECTORS
  if (vector_is_multidimensional(vec))
    return(vec->object.vector.dim_info->dims);
#endif
  dims = (s7_Int *)malloc(sizeof(s7_Int));
  dims[0] = vector_length(vec);
  return(dims);
}


s7_Int *s7_vector_offsets(s7_pointer vec)
{
  s7_Int *offs;
#if WITH_MULTIDIMENSIONAL_VECTORS
  if (vector_is_multidimensional(vec))
    return(vec->object.vector.dim_info->offsets);
#endif
  offs = (s7_Int *)malloc(sizeof(s7_Int));
  offs[0] = 1;
  return(offs);
}


s7_pointer s7_vector_to_list(s7_scheme *sc, s7_pointer vect)
{
  s7_pointer lst = sc->NIL;
  s7_Int i, len;
  len = vector_length(vect);
  for (i = len - 1; i >= 0; i--)
    lst = s7_cons(sc, vector_element(vect, i), lst);
  return(lst);
}


static s7_pointer g_vector_to_list(s7_scheme *sc, s7_pointer args)
{
  #define H_vector_to_list "(vector->list v) returns the elements of the vector v as a list"
  if (!s7_is_vector(car(args)))
    return(s7_wrong_type_arg_error(sc, "vector->list", 0, car(args), "a vector"));
  return(s7_vector_to_list(sc, car(args)));
}


static bool vectors_equal(s7_pointer x, s7_pointer y)
{
  s7_Int i, len;

  len = vector_length(x);
  if (len != vector_length(y)) return(false);

#if WITH_MULTIDIMENSIONAL_VECTORS
  if (vector_is_multidimensional(x))
    {
      if (!(vector_is_multidimensional(y)))
	return(false);
      if (vector_ndims(x) != vector_ndims(y))
	return(false);
      for (i = 0; i < vector_ndims(x); i++)
	if (vector_dimension(x, i) != vector_dimension(y, i))
	  return(false);
    }
  else
    {
      if (vector_is_multidimensional(y))
	return(false);
    }
#endif

  for (i = 0; i < len; i++)
    if (!(s7_is_equal(vector_element(x, i), vector_element(y, i))))
      return(false);
  return(true);
}


s7_pointer s7_make_and_fill_vector(s7_scheme *sc, s7_Int len, s7_pointer fill)
{
  s7_pointer vect;
  vect = s7_make_vector_1(sc, len, false);
  s7_vector_fill(sc, vect, fill);
  return(vect);
}


static s7_pointer g_vector(s7_scheme *sc, s7_pointer args)
{
  #define H_vector "(vector ...) returns a vector whose elements are the arguments"
  s7_Int i, len;
  s7_pointer vec;
  
  len = s7_list_length(sc, args);
  if (len < 0) 
    return(s7_wrong_type_arg_error(sc, "vector", 1, car(args), "a proper list"));
  
  vec = s7_make_vector_1(sc, len, false);
  if (len > 0)
    {
      s7_pointer x;
      for (x = args, i = 0; is_pair(x); x = cdr(x), i++) 
	vector_element(vec, i) =  car(x);
    }
  return(vec);
}


static s7_pointer g_list_to_vector(s7_scheme *sc, s7_pointer args)
{
  #define H_list_to_vector "(list->vector lst) returns a vector containing the elements of lst"
  
  if (car(args) == sc->NIL)
    return(s7_make_vector(sc, 0));
  if (!is_proper_list(sc, car(args)))
    return(s7_wrong_type_arg_error(sc, "list->vector", 0, car(args), "a proper list"));
  return(g_vector(sc, car(args)));
}


static s7_pointer g_vector_length(s7_scheme *sc, s7_pointer args)
{
  #define H_vector_length "(vector-length v) returns the length of vector v"
  if (!s7_is_vector(car(args)))
    return(s7_wrong_type_arg_error(sc, "vector-length", 0, car(args), "a vector"));
  return(s7_make_integer(sc, vector_length(car(args))));
}


static s7_pointer vector_ref_1(s7_scheme *sc, s7_pointer vect, s7_pointer indices)
{
  s7_Int index = 0;
#if WITH_MULTIDIMENSIONAL_VECTORS
  if (vector_is_multidimensional(vect))
    {
      int i;
      s7_pointer x;
      for (x = indices, i = 0; (x != sc->NIL) && (i < vector_ndims(vect)); x = cdr(x), i++)
	{
	  s7_Int n;
	  if (!s7_is_integer(car(x)))
	    return(s7_wrong_type_arg_error(sc, "vector ref", i + 2, car(x), "an integer"));
	  n = s7_integer(car(x));
	  if ((n < 0) || 
	      (n >= vector_dimension(vect, i)))
	    return(s7_out_of_range_error(sc, "vector ref", i + 2, car(x), "index should be between 0 and the dimension size"));
	  index += n * vector_offset(vect, i);
	}
      if ((x != sc->NIL) ||
	  (i != vector_ndims(vect)))
	return(s7_wrong_number_of_args_error(sc, "vector ref", indices));
    }
  else
#endif
    {
      /* (let ((hi (make-vector 3 0.0)) (sum 0.0)) (do ((i 0 (+ i 1))) ((= i 3)) (set! sum (+ sum (hi i)))) sum) */
      if (!s7_is_integer(car(indices)))
	return(s7_wrong_type_arg_error(sc, "vector ref", 2, car(indices), "an integer"));
      if (cdr(indices) != sc->NIL)                            /* (#(1 2) 1 2) */
	return(s7_wrong_number_of_args_error(sc, "vector ref", indices));

      index = s7_integer(car(indices));
      if ((index < 0) ||
	  (index >= vector_length(vect)))
	return(s7_out_of_range_error(sc, "vector ref", 2, s7_make_integer(sc, index), "index should be between 0 and the vector length"));
    }

  return(vector_element(vect, index));
}


static s7_pointer g_vector_ref(s7_scheme *sc, s7_pointer args)
{
#if WITH_MULTIDIMENSIONAL_VECTORS
  #define H_vector_ref "(vector-ref v i) returns the i-th element of vector v.  If v \
is a multidimensional vector, you can also use (vector-ref v ...) where the trailing args \
are the indices, or omit 'vector-ref': (v ...)."
#else
  #define H_vector_ref "(vector-ref v i) returns the i-th element of vector v"
#endif

  s7_pointer vec;
  vec = car(args);
  if (!s7_is_vector(vec))
    return(s7_wrong_type_arg_error(sc, "vector-ref", 1, vec, "a vector"));

  return(vector_ref_1(sc, vec, cdr(args)));
}


static s7_pointer g_vector_set(s7_scheme *sc, s7_pointer args)
{
#if WITH_MULTIDIMENSIONAL_VECTORS
  #define H_vector_set "(vector-set! v i value) sets the i-th element of vector v to value.  If 'v' is \
multidimensional you can also use (vector-set! v ... val) where the ellipsis refers to the indices.  You \
can also use 'set!' instead of 'vector-set!': (set! (v ...) val) -- I find this form much easier to read."
#else
  #define H_vector_set "(vector-set! v i value) sets the i-th element of vector v to value"
#endif
  s7_pointer vec, val;
  s7_Int index;
  
  vec = car(args);
  if (!s7_is_vector(vec))
    return(s7_wrong_type_arg_error(sc, "vector-set!", 1, vec, "a vector"));
  if (s7_is_immutable(vec))
    return(s7_wrong_type_arg_error(sc, "vector-set!", 1, vec, "a mutable vector"));
  
#if WITH_MULTIDIMENSIONAL_VECTORS
  if (vector_is_multidimensional(vec))
    {
      int i;
      s7_pointer x;
      index = 0;
      for (x = cdr(args), i = 0; (cdr(x) != sc->NIL) && (i < vector_ndims(vec)); x = cdr(x), i++)
	{
	  s7_Int n;
	  if (!s7_is_integer(car(x)))
	    return(s7_wrong_type_arg_error(sc, "vector-set!", i + 2, car(x), "an integer"));
	  n = s7_integer(car(x));
	  if ((n < 0) || 
	      (n >= vector_dimension(vec, i)))
	    return(s7_out_of_range_error(sc, "vector-set!", i, car(x), "index should be between 0 and the dimension size"));
	  index += n * vector_offset(vec, i);
	}

      if ((cdr(x) != sc->NIL) ||
	  ((i != vector_ndims(vec)) &&
	   (i != 1)))
	return(s7_wrong_number_of_args_error(sc, "vector-set!", args));

      val = car(x);
    }
  else
#endif
    {
      if (!s7_is_integer(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "vector-set!", 2, cadr(args), "an integer"));

      index = s7_integer(cadr(args));
      if ((index < 0) ||
	  (index >= vector_length(vec)))
	return(s7_out_of_range_error(sc, "vector-set!", 2, cadr(args), "index should be between 0 and the vector length"));

      val = caddr(args);
    }
  
  vector_element(vec, index) = val;
  return(val);
}


static s7_pointer g_make_vector(s7_scheme *sc, s7_pointer args)
{
#if WITH_MULTIDIMENSIONAL_VECTORS
  #define H_make_vector "(make-vector len :optional (value #f)) returns a vector of len elements initialized to value. \
To create a multidimensional vector, put the dimension bounds in a list (this is to avoid ambiguities such as \
(make-vector 1 2) where it's not clear whether the '2' is an initial value or a dimension size).  (make-vector '(2 3) 1.0) \
returns a 2 dimensional vector of 6 total elements, all initialized to 1.0."
#else
  #define H_make_vector "(make-vector len :optional (value #f)) returns a vector of len elements initialized to value"
#endif

  s7_Int len;
  s7_pointer x, fill, vec;
  fill = sc->UNSPECIFIED;

  x = car(args);
  if (s7_is_integer(x))
    {
      if (s7_integer(x) < 0)
	return(s7_wrong_type_arg_error(sc, "make-vector", 1, x, "a non-negative integer"));
      len = s7_integer(x);
    }
#if WITH_MULTIDIMENSIONAL_VECTORS
  else
    {
      s7_pointer y;
      if (!(is_pair(x)))
	return(s7_wrong_type_arg_error(sc, "make-vector", 1, x, "an integer or a list of integers"));

      if (!s7_is_integer(car(x)))
	return(s7_wrong_type_arg_error(sc, "make-vector", 1, car(x), "each dimension should be an integer"));

      if (cdr(x) == sc->NIL)
	len = s7_integer(car(x));
      else
	{
	  int i;
	  for (i = 1, len = 1, y = x; y != sc->NIL; y = cdr(y), i++)
	    {
	      if (!is_pair(y))
		return(s7_wrong_type_arg_error(sc, "make-vector", 1, x, "a proper list of dimensions"));
	      if (!s7_is_integer(car(y)))
		return(s7_wrong_type_arg_error(sc, "make-vector", i, car(y), "an integer"));
	      len *= s7_integer(car(y));
	      if (len < 0)
		return(s7_wrong_type_arg_error(sc, "make-vector", i, car(y), "a non-negative integer"));
	    }
	}
    }
#endif
  
  if (cdr(args) != sc->NIL) 
    fill = cadr(args);

  vec = s7_make_vector_1(sc, len, false);
  s7_vector_fill(sc, vec, fill);

#if WITH_MULTIDIMENSIONAL_VECTORS
  if ((is_pair(x)) &&
      (is_pair(cdr(x))))
    {
      int i;
      s7_Int offset = 1;
      s7_pointer y;
      s7_vdims_t *v;

      v = (s7_vdims_t *)malloc(sizeof(s7_vdims_t));
      v->ndims = safe_list_length(sc, x);
      v->dims = (s7_Int *)malloc(v->ndims * sizeof(s7_Int));
      v->offsets = (s7_Int *)malloc(v->ndims * sizeof(s7_Int));

      for (i = 0, y = x; y != sc->NIL; i++, y = cdr(y))
	v->dims[i] = s7_integer(car(y));

      for (i = v->ndims - 1; i >= 0; i--)
	{
	  v->offsets[i] = offset;
	  offset *= v->dims[i];
	}

      vec->object.vector.dim_info = v;
    }
#endif

  return(vec);
}


static s7_pointer g_is_vector(s7_scheme *sc, s7_pointer args)
{
  #define H_is_vector "(vector? obj) returns #t if obj is a vector"
  return(make_boolean(sc, s7_is_vector(car(args))));
}


int s7_vector_rank(s7_pointer vect)
{
#if WITH_MULTIDIMENSIONAL_VECTORS
  if (vector_is_multidimensional(vect))
    return(vector_ndims(vect));
#endif
  return(1);
}


#if WITH_MULTIDIMENSIONAL_VECTORS
static s7_pointer g_vector_dimensions(s7_scheme *sc, s7_pointer args)
{
  #define H_vector_dimensions "(vector-dimensions vect) returns a list of vect's dimensions.  In srfi-63 terms:\n\
    (define array-dimensions vector-dimensions)\n\
    (define (array-rank v) (length (vector-dimensions v)))"

  s7_pointer x;

  x = car(args);
  if (!s7_is_vector(x))
    return(s7_wrong_type_arg_error(sc, "vector-dimensions", 0, x, "a vector"));

  if (vector_is_multidimensional(x))
    {
      int i;
      s7_pointer lst;
      lst = sc->NIL;
      for (i = vector_ndims(x) - 1; i >= 0; i--)
	lst = s7_cons(sc, s7_make_integer(sc, vector_dimension(x, i)), lst);
      return(lst);
    }
  
  return(make_list_1(sc, s7_make_integer(sc, vector_length(x))));
}
#endif


static s7_pointer g_vector_for_each(s7_scheme *sc, s7_pointer args)
{
  /* (vector-for-each (lambda (n) (format #t "~A " n)) (vector 1 2 3)) */
  #define H_vector_for_each "(vector-for-each func vector1 ...) applies func to each element of the vectors traversed in parallel"
  int i, len;
  s7_pointer vect, x, fargs;

  sc->code = car(args);
  if (!is_procedure(sc->code))
    return(s7_wrong_type_arg_error(sc, "vector-for-each", 1, sc->code, "a procedure"));

  vect = cadr(args);
  if (!s7_is_vector(vect))
    return(s7_wrong_type_arg_error(sc, "vector-for-each", 2, vect, "a vector"));

  len = vector_length(vect);
  fargs = s7_cons(sc, sc->NIL, sc->NIL);

  if (cddr(args) != sc->NIL)
    {
      for (i = 3, x = cddr(args); x != sc->NIL; x = cdr(x), i++)
	{
	  if (!s7_is_vector(car(x)))
	    return(s7_wrong_type_arg_error(sc, "vector-for-each", i, car(x), "a vector"));
	  if (len != vector_length(car(x)))
	    return(s7_wrong_type_arg_error(sc, "vector-for-each", i, car(x), "a vector whose length matches the other vectors"));
	  fargs = s7_cons(sc, sc->NIL, fargs);
	}
    }

  /* the actual loop has to be done in the evaluator else (for example) call/cc won't work right */
  /* (call/cc (lambda (return) (vector-for-each (lambda (n) (display n) (if (even? n) (return n))) (vector 1 3 8 7 9 10)))) */
  
  /* during the loop we need the vector list (cdddr(sc->args)), the function (sc->code), 
   *   the (mutable) loop counter (car(sc->args)) and end (cadr(sc->args)), and the current function arg list (caddr(sc->args)).
   */

  sc->args = s7_cons(sc, make_mutable_integer(sc, 0), 
               s7_cons(sc, s7_make_integer(sc, len), 
                 s7_cons(sc, fargs, cdr(args))));
  push_stack(sc, OP_VECTOR_FOR_EACH, sc->args, sc->code);
  return(sc->UNSPECIFIED);
}


static s7_pointer g_hash_table_for_each(s7_scheme *sc, s7_pointer args)
{
  /* (hash-table-for-each (lambda (n) (format #t "~A " n)) hash) */
  #define H_hash_table_for_each "(hash-table-for-each func table1 ...) applies func to each element of the hash tables traversed in parallel"
  int i, len;
  s7_pointer vect, x, fargs;

  sc->code = car(args);
  if (!is_procedure(sc->code))
    return(s7_wrong_type_arg_error(sc, "hash-table-for-each", 1, sc->code, "a procedure"));

  vect = cadr(args);
  if (!s7_is_hash_table(vect))
    return(s7_wrong_type_arg_error(sc, "hash-table-for-each", 2, vect, "a hash-table"));

  len = vector_length(vect);
  fargs = s7_cons(sc, sc->NIL, sc->NIL);

  if (cddr(args) != sc->NIL)
    {
      for (i = 3, x = cddr(args); x != sc->NIL; x = cdr(x), i++)
	{
	  if (!s7_is_hash_table(car(x)))
	    return(s7_wrong_type_arg_error(sc, "hash-table-for-each", i, car(x), "a hash-table"));
	  if (len != vector_length(car(x)))
	    return(s7_wrong_type_arg_error(sc, "hash-table-for-each", i, car(x), "a hash-table whose length matches the other hash-tables"));
	  fargs = s7_cons(sc, sc->NIL, fargs);
	}
    }

  sc->args = s7_cons(sc, make_mutable_integer(sc, 0), 
               s7_cons(sc, s7_make_integer(sc, len), 
                 s7_cons(sc, fargs, cdr(args))));
  push_stack(sc, OP_HASH_TABLE_FOR_EACH, sc->args, sc->code);
  return(sc->UNSPECIFIED);
}


static s7_pointer g_vector_map(s7_scheme *sc, s7_pointer args)
{
  /* (vector-map (lambda (n) (+ n 1)) (vector 1 2 3)) */
  #define H_vector_map "(vector-map func vector1 ...) applies func to each element of the vectors traversed in parallel, returning the results in a new vector"
  int i, len;
  s7_pointer vect, x, fargs;

  sc->code = car(args);
  if (!is_procedure(sc->code))
    return(s7_wrong_type_arg_error(sc, "vector-map", 1, sc->code, "a procedure"));

  vect = cadr(args);
  if (!s7_is_vector(vect))
    return(s7_wrong_type_arg_error(sc, "vector-map", 2, vect, "a vector"));

  len = vector_length(vect);
  fargs = s7_cons(sc, sc->NIL, sc->NIL);

  if (cddr(args) != sc->NIL)
    {
      for (i = 3, x = cddr(args); x != sc->NIL; x = cdr(x), i++)
	{
	  if (!s7_is_vector(car(x)))
	    return(s7_wrong_type_arg_error(sc, "vector-map", i, car(x), "a vector"));
	  if (len != vector_length(car(x)))
	    return(s7_wrong_type_arg_error(sc, "vector-map", i, car(x), "a vector whose length matches the other vectors"));
	  fargs = s7_cons(sc, sc->NIL, fargs);
	}
    }

  /* the actual loop has to be done in the evaluator else (for example) call/cc won't work right */
  /* (call/cc (lambda (return) (vector-for-each (lambda (n) (display n) (if (even? n) (return n))) (vector 1 3 8 7 9 10)))) */
  
  /* during the loop we need the vector list (cddddr(sc->args)), the function (sc->code), the new vector (car(sc->args)),
   *   the (mutable) loop counter (cadr(sc->args)) and end (caddr(sc->args)), and the current function arg list (cadddr(sc->args)).
   */

  sc->args = s7_cons(sc, s7_make_vector(sc, len),
               s7_cons(sc, make_mutable_integer(sc, 0), 
                 s7_cons(sc, s7_make_integer(sc, len), 
		   s7_cons(sc, fargs, cdr(args)))));
  push_stack(sc, OP_VECTOR_MAP0, sc->args, sc->code);
  return(sc->UNSPECIFIED);
}



/* -------- sort! -------- */

#if (!HAVE_NESTED_FUNCTIONS)
  static s7_pointer compare_proc, compare_proc_args;
  static s7_scheme *compare_sc; /* ugh */

  static int vector_compare(const void *v1, const void *v2)
  {
    int start;
    car(compare_proc_args) = (*(s7_pointer *)v1);
    cadr(compare_proc_args) = (*(s7_pointer *)v2);

    start = compare_sc->stack_top; /* see note below */
    push_stack(compare_sc, OP_EVAL_DONE, compare_sc->args, compare_sc->code); 
    compare_sc->args = compare_proc_args;
    compare_sc->code = compare_proc;
    eval(compare_sc, OP_APPLY);

    if (compare_sc->stack_top < start)
      {
	s7_gc_on(compare_sc, true);
	longjmp(compare_sc->goto_qsort_end, 1);
      }
    if (is_true(compare_sc, compare_sc->value))
      return(-1);
    return(1);
  }
#endif


#if HAVE_PTHREADS
static pthread_mutex_t sort_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static s7_pointer g_sort_in_place(s7_scheme *sc, s7_pointer args)
{
  #define H_sort_in_place "(sort! list-or-vector func) sorts a list or vector using func to compare elements.\
If its first argument is a list, the list is copied (despite the '!')."

  int gc_loc;
  s7_pointer vect;

#if HAVE_NESTED_FUNCTIONS
  s7_pointer compare_proc, compare_proc_args;
  auto int vector_compare(const void *v1, const void *v2);

  int vector_compare(const void *v1, const void *v2)
  {
    int start;
    car(compare_proc_args) = (*(s7_pointer *)v1);
    cadr(compare_proc_args) = (*(s7_pointer *)v2);

    start = sc->stack_top;
    /* qsort is a large and complex function (250 lines in libc), so we can't easily
     *   expand it in our eval loop, but we may want to jump out of the sort via call/cc,
     *   so we look for the stack being unwound past the start point -- this is a kludge!
     * Currently, if an error occurs in the sort function, we depend on s7_error to
     *   turn it back on; is there any case where we'd want it off despite hitting
     *   an error?
     */

    push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
    sc->args = compare_proc_args;
    sc->code = compare_proc;
    eval(sc, OP_APPLY);

    if (sc->stack_top < start)
      {
	s7_gc_on(sc, true);
	longjmp(sc->goto_qsort_end, 1);
      }
    if (is_true(sc, sc->value))
      return(-1);
    return(1);
  }
#endif

  vect = car(args);

  if (s7_is_list(sc, vect))
    {
      s7_pointer val;
      if (sc->free_heap_top < 4096) gc(sc);
      s7_gc_on(sc, false);
      val = g_sort_in_place(sc, make_list_2(sc, 
					    g_list_to_vector(sc, make_list_1(sc, vect)), 
					    cadr(args)));
      if (s7_is_vector(val))
	val = s7_vector_to_list(sc, val);
      s7_gc_on(sc, true);
      return(val);
    }

  if (!s7_is_vector(vect))
    return(s7_wrong_type_arg_error(sc, "sort!", 1, vect, "a vector or a list"));
  if (!s7_is_procedure(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "sort!", 2, cadr(args), "a procedure"));

#if HAVE_PTHREADS
  pthread_mutex_lock(&sort_lock);
#endif

  compare_proc = cadr(args);
#if (!HAVE_NESTED_FUNCTIONS)
  compare_sc = sc;
#endif

  compare_proc_args = make_list_2(sc, sc->F, sc->F);
  gc_loc = s7_gc_protect(sc, compare_proc_args);

  qsort((void *)s7_vector_elements(vect), vector_length(vect), sizeof(s7_pointer), vector_compare); /* qsort sizes are type size_t */

  if (setjmp(sc->goto_qsort_end) != 0)
    vect = sc->value;

  s7_gc_unprotect_at(sc, gc_loc);

#if HAVE_PTHREADS
  pthread_mutex_unlock(&sort_lock);
#endif

  return(vect);
}



/* -------- hash tables -------- */

bool s7_is_hash_table(s7_pointer p)
{
  return(type(p) == T_HASH_TABLE);
}


static s7_pointer g_is_hash_table(s7_scheme *sc, s7_pointer args)
{
  #define H_is_hash_table "(hash-table? obj) returns #t if obj is a hash-table"
  return(make_boolean(sc, s7_is_hash_table(car(args))));
}


static s7_pointer g_hash_table_size(s7_scheme *sc, s7_pointer args)
{
  #define H_hash_table_size "(hash-table-size obj) returns the size of the hash-table obj"
  if (!s7_is_hash_table(car(args)))
    return(s7_wrong_type_arg_error(sc, "hash-table-size", 0, car(args), "a hash-table"));
  return(s7_make_integer(sc, vector_length(car(args))));
}


s7_pointer s7_make_hash_table(s7_scheme *sc, s7_Int size)
{
  s7_pointer table;
  table = s7_make_vector(sc, size);
  set_type(table, T_HASH_TABLE | T_FINALIZABLE | T_DONT_COPY);
  return(table);
}


static s7_pointer g_make_hash_table(s7_scheme *sc, s7_pointer args)
{
  #define H_make_hash_table "(make-hash-table :optional size) returns a new hash table"
  s7_Int size = 461;

  if (args != sc->NIL)
    {
      if (s7_is_integer(car(args)))
	{
	  size = s7_integer(car(args));
	  if (size <= 0)
	    return(s7_out_of_range_error(sc, "make-hash-table", 0, car(args), "size should be a positive integer"));
	}
      else return(s7_wrong_type_arg_error(sc, "make-hash-table", 0, car(args), "an integer"));
    }
  
  return(s7_make_hash_table(sc, size));
}


s7_pointer s7_hash_table_ref(s7_scheme *sc, s7_pointer table, const char *name)
{
  s7_Int location;
  s7_pointer x;
  
  location = hash_table_hash(name, vector_length(table));
  for (x = vector_element(table, location); x != sc->NIL; x = cdr(x)) 
    if (strcmp(name, string_value(caar(x))) == 0) 
      return(cdar(x)); 
  
  return(sc->F);
}


s7_pointer s7_hash_table_set(s7_scheme *sc, s7_pointer table, const char *name, s7_pointer value)
{
  s7_Int location;
  s7_pointer x;
  location = hash_table_hash(name, vector_length(table)); 
  
  /* if it exists, update value, else add to table */
  for (x = vector_element(table, location); x != sc->NIL; x = cdr(x)) 
    if (strcmp(name, string_value(caar(x))) == 0)
      {
	cdar(x) = value;
	return(value);
      }
  vector_element(table, location) = s7_cons(sc, 
					    s7_cons(sc, 
						    s7_make_string(sc, name), 
						    value),
					    vector_element(table, location)); 
  return(value);
}


#define HASHED_INTEGER_BUFFER_SIZE 64

static char *s7_hashed_integer_name(s7_Int key, char *intbuf) /* not const here because snprintf is declared char* */
{
  snprintf(intbuf, HASHED_INTEGER_BUFFER_SIZE, "\b%lld\b", (long long int)key);
  return(intbuf);
}


static char *s7_hashed_real_name(s7_Double key, char *intbuf)
{
  /* this is actually not safe due to the challenges faced by %f */
  snprintf(intbuf, HASHED_INTEGER_BUFFER_SIZE, "\b%.20f\b", key); /* default precision is not enough */
  return(intbuf);
}


static s7_pointer g_hash_table_ref(s7_scheme *sc, s7_pointer args)
{
  /* basically the same layout as the global symbol table */
  #define H_hash_table_ref "(hash-table-ref table key) returns the value associated with key (a string or symbol) in the hash table"
  const char *name;
  char intbuf[HASHED_INTEGER_BUFFER_SIZE];
  s7_pointer table, key;

  table = car(args);
  key = cadr(args);
  
  if (!s7_is_hash_table(table))
    return(s7_wrong_type_arg_error(sc, "hash-table-ref", 1, table, "a hash-table"));

  if (s7_is_string(key))
    name = string_value(key);
  else 
    {
      if (s7_is_symbol(key))
	name = s7_symbol_name(key);
      else
	{
	  if (s7_is_integer(key))
	    name = s7_hashed_integer_name(s7_integer(key), intbuf);
	  else
	    {
	      if ((s7_is_real(key)) && (!s7_is_ratio(key)))
		name = s7_hashed_real_name(s7_real(key), intbuf);
	      else return(s7_wrong_type_arg_error(sc, "hash-table-ref", 2, key, "a string, symbol, integer, or (non-ratio) real"));
	    }
	}
    }
  return(s7_hash_table_ref(sc, table, name));
}


static s7_pointer g_hash_table_set(s7_scheme *sc, s7_pointer args)
{
  #define H_hash_table_set "(hash-table-set! table key value) sets the value associated with key (a string or symbol) in the hash table to value"
  const char *name;
  char intbuf[HASHED_INTEGER_BUFFER_SIZE];
  s7_pointer table, key;

  table = car(args);
  key = cadr(args);
  
  if (!s7_is_hash_table(table))
    return(s7_wrong_type_arg_error(sc, "hash-table-set!", 1, table, "a hash-table"));

  if (s7_is_string(key))
    name = string_value(key);
  else 
    {
      if (s7_is_symbol(key))
	name = s7_symbol_name(key);
      else
	{
	  if (s7_is_integer(key))
	    name = s7_hashed_integer_name(s7_integer(key), intbuf);
	  else
	    {
	      if ((s7_is_real(key)) && (!s7_is_ratio(key)))
		name = s7_hashed_real_name(s7_real(key), intbuf);
	      else return(s7_wrong_type_arg_error(sc, "hash-table-set!", 2, key, "a string, symbol, integer, or (non-ratio) real"));
	    }
	}
    }
  
  return(s7_hash_table_set(sc, table, name, caddr(args)));
}




/* -------------------------------- objects and functions --------------------------------
 *
 * this could be made available in Scheme:
 *
 * define-type (make-type in ext)
 * (define-type "name"
 *              (lambda (obj) (display obj))       ; print
 *	        #f                                 ; free
 *	        (lambda (obj1 obj2) (= obj1 obj2)) ; equal?
 *	        #f                                 ; gc mark
 *	        (lambda (obj arg) arg)             ; apply
 *	        #f                                 ; set
 *              #f #f #f)                          ; length copy fill!
 *  (make-object <tag> value)
 *  (object? <tag> obj) or maybe (name? obj) and (make-name value)
 *
 * should probably add a list of methods (reverse, member, etc)
 */

bool s7_is_function(s7_pointer p)  
{ 
  return(is_c_function(p));
}


bool s7_is_object(s7_pointer p) 
{ 
  return(is_c_object(p));
}


s7_pointer s7_make_function(s7_scheme *sc, const char *name, s7_function f, int required_args, int optional_args, bool rest_arg, const char *doc)
{
  s7_func_t *ptr;
  s7_pointer x = new_cell(sc);

  /* these are normally not gc'd (C-level function defs), but they can be in a few cases
   *   (C-level redefinition as in the gmp case), and then we can't be certain they haven't
   *   been protected as the value of some Scheme variable.  It saves about 1% overall
   *   compute time (in s7test.scm) if we ignore those issues and use calloc here, rather
   *   than new_cell.
   */

  ptr = (s7_func_t *)calloc(1, sizeof(s7_func_t));
  set_type(x, T_C_FUNCTION | T_ATOM | T_SIMPLE | T_FINALIZABLE | T_DONT_COPY | T_PROCEDURE);
  /* these guys can be freed -- in Snd, for example, "random" is defined in C, but then later redefined in snd-test.scm */

  c_function(x) = ptr;
  c_function_call(x) = f;
  c_function_name(x) = name;       /* (procedure-name proc) => (format #f "~A" proc); perhaps add this as a built-in function? */
  if (doc)
    c_function_documentation(x) = s7_strdup(doc);
  c_function_required_args(x) = required_args;
  c_function_optional_args(x) = optional_args;
  c_function_has_rest_arg(x) = rest_arg;
  if (rest_arg)
    c_function_all_args(x) = 10000000;
  else c_function_all_args(x) = required_args + optional_args;

  return(x);
}


static void s7_free_function(s7_pointer a)
{
  if (c_function_documentation(a))
    free(c_function_documentation(a));
  free(c_function(a));
}


s7_pointer s7_apply_function(s7_scheme *sc, s7_pointer fnc, s7_pointer args)
{
  return(c_function_call(fnc)(sc, args));
}


bool s7_is_procedure(s7_pointer x)
{
  return(is_procedure(x) || is_closure(x) || is_closure_star(x));
}


static s7_pointer g_is_procedure(s7_scheme *sc, s7_pointer args)
{
  #define H_is_procedure "(procedure? obj) returns #t if obj is a procedure"
  return(make_boolean(sc, s7_is_procedure(car(args))));
}


static char *pws_documentation(s7_pointer x);
static s7_pointer pws_source(s7_scheme *sc, s7_pointer x);
static int pws_get_req_args(s7_pointer x);
static int pws_get_opt_args(s7_pointer x);


s7_pointer s7_procedure_source(s7_scheme *sc, s7_pointer p)
{
  /* make it look like an internal lambda form */
  
  /* in this context, there's no way to distinguish between:
   *    (procedure-source (let ((b 1)) (lambda (a) (+ a b))))
   * and
   *    (let ((b 1)) (procedure-source (lambda (a) (+ a b))))
   * both become:
   * ((a) (+ a b)) (((b . 1)) #(() () () () () ((make-filtered-comb . make-filtered-comb)) () () ...))
   */
  
  if (is_closure(p) || is_closure_star(p) || is_macro(p))
    {
      return(s7_cons(sc, 
		     s7_append(sc, 
			       make_list_2(sc, 
					   (is_closure_star(p)) ? sc->LAMBDA_STAR : sc->LAMBDA, 
					   closure_args(p)),
			       closure_body(p)),
		     closure_environment(p)));
    }
  
  if (s7_is_procedure_with_setter(p))
    return(pws_source(sc, p));
  return(sc->NIL);
}


static s7_pointer g_procedure_source(s7_scheme *sc, s7_pointer args)
{
  /* make it look like a scheme-level lambda */
  s7_pointer p;
  
  #define H_procedure_source "(procedure-source func) tries to return the definition of func"
  
  if (s7_is_symbol(car(args)))
    p = s7_symbol_value(sc, car(args));
  else p = car(args);
  if ((!is_procedure(p)) &&
      (!is_macro(p)))
    return(s7_wrong_type_arg_error(sc, "procedure-source", 0, p, "a procedure or a macro"));

  if (is_closure(p) || is_closure_star(p) || is_macro(p))
    return(s7_append(sc, 
		     make_list_2(sc, 
				 (is_closure_star(p)) ? sc->LAMBDA_STAR : sc->LAMBDA, 
				 closure_args(p)),
		     closure_body(p)));

  if (s7_is_procedure_with_setter(p))
    return(pws_source(sc, p));
  return(sc->NIL);
}


s7_pointer s7_procedure_environment(s7_pointer p)    
{ 
  return(closure_environment(p));
}


static s7_pointer g_procedure_environment(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  
  #define H_procedure_environment "(procedure-environment func) tries to return func's environment"
  
  if (s7_is_symbol(car(args)))
    p = s7_symbol_value(sc, car(args));
  else p = car(args);
  if (!is_procedure(p))
    return(s7_wrong_type_arg_error(sc, "procedure-environment", 0, p, "a procedure"));

  if (is_closure(p) || is_closure_star(p))
    return(closure_environment(p));
  return(sc->global_env);
}


void s7_define_function(s7_scheme *sc, const char *name, s7_function fnc, int required_args, int optional_args, bool rest_arg, const char *doc)
{
  s7_pointer func;
  func = s7_make_function(sc, name, fnc, required_args, optional_args, rest_arg, doc);
  s7_define(sc, s7_global_environment(sc), s7_make_symbol(sc, name), func);
}


void s7_define_set_function(s7_scheme *sc, const char *name, s7_function fnc, int required_args, int optional_args, bool rest_arg, const char *doc)
{
  s7_pointer func;
  func = s7_make_function(sc, name, fnc, required_args, optional_args, rest_arg, doc);
  set_type(func, T_C_FUNCTION | T_ATOM | T_SIMPLE | T_FINALIZABLE | T_DONT_COPY | T_PROCEDURE | T_SETTER);
  s7_define(sc, s7_global_environment(sc), s7_make_symbol(sc, name), func);
}


void s7_define_macro(s7_scheme *sc, const char *name, s7_function fnc, int required_args, int optional_args, bool rest_arg, const char *doc)
{
  s7_pointer func;
  func = s7_make_function(sc, name, fnc, required_args, optional_args, rest_arg, doc);
  set_type(func, T_C_MACRO | T_ATOM | T_SIMPLE | T_FINALIZABLE | T_DONT_COPY | T_PROCEDURE | T_ANY_MACRO);
  s7_define(sc, s7_global_environment(sc), s7_make_symbol(sc, name), func);
}


void s7_define_function_star(s7_scheme *sc, const char *name, s7_function fnc, const char *arglist, const char *doc)
{
  /* make an internal function of any args that calls fnc, then wrap it in define* and use eval_c_string */
  /* should (does) this ignore :key and other such noise? */
  char *internal_function, *internal_arglist;
  int arglist_len, len, args;
  const char *local_sym;
  s7_pointer local_args;
  
  arglist_len = safe_strlen(arglist);
  internal_arglist = (char *)calloc(arglist_len + 64, sizeof(char));
  snprintf(internal_arglist, arglist_len + 64, "(map (lambda (arg) (if (symbol? arg) arg (car arg))) '(%s))", arglist);
  local_args = s7_eval_c_string(sc, internal_arglist);
  free(internal_arglist);
  
  args = s7_list_length(sc, local_args);
  internal_arglist = s7_object_to_c_string(sc, local_args);
  /* this has an opening paren which we don't want */
  internal_arglist[0] = ' ';

  local_sym = symbol_name(s7_gensym(sc, "define*"));
  s7_define_function(sc, local_sym, fnc, args, 0, 0, NULL);

  len = 32 + 2 * arglist_len + safe_strlen(doc) + safe_strlen(name) + safe_strlen(local_sym);
  internal_function = (char *)calloc(len, sizeof(char));
  snprintf(internal_function, len, "(define* (%s %s) \"%s\" (%s %s)", name, arglist, doc, local_sym, internal_arglist);
  s7_eval_c_string(sc, internal_function);

  free(internal_function);
  free(internal_arglist);
}


const char *s7_procedure_documentation(s7_scheme *sc, s7_pointer x)
{
  if (s7_is_symbol(x))
    x = s7_symbol_value(sc, x); /* this is needed by Snd */

  if ((s7_is_function(x)) ||
      (is_c_macro(x)))
    return((char *)c_function_documentation(x));
  
  if (((is_closure(x)) || 
       (is_closure_star(x))) &&
      (s7_is_string(car(closure_body(x)))))
       return(s7_string(car(closure_body(x))));
  
  if (s7_is_procedure_with_setter(x))
    return(pws_documentation(x));
  
  return(""); /* not NULL here so that (string=? "" (procedure-documentation no-doc-func)) -> #t */
}


static s7_pointer g_procedure_documentation(s7_scheme *sc, s7_pointer args)
{
  #define H_procedure_documentation "(procedure-documentation func) returns func's documentation string"
  s7_pointer x;
  if (s7_is_symbol(car(args)))
    x = s7_symbol_value(sc, car(args));
  else x = car(args);

  if (!is_procedure(x))
    return(s7_wrong_type_arg_error(sc, "procedure-documentation", 0, x, "a procedure"));
  return(s7_make_string(sc, s7_procedure_documentation(sc, x)));
}


s7_pointer s7_procedure_arity(s7_scheme *sc, s7_pointer x)
{
  if (s7_is_function(x))
    return(make_list_3(sc, 
		       s7_make_integer(sc, c_function_required_args(x)), 
		       s7_make_integer(sc, c_function_optional_args(x)),
		       (c_function_has_rest_arg(x)) ? sc->T : sc->F));
  
  if ((is_closure(x)) ||
      (is_closure_star(x)) ||
      (is_pair(x)))
    {
      int len;
      
      if (is_pair(x))
	len = s7_list_length(sc, car(x));
      else 
	{
	  if (s7_is_symbol(closure_args(x)))
	    return(make_list_3(sc, small_int(sc, 0), small_int(sc, 0), sc->T));
	  len = s7_list_length(sc, closure_args(x));
	}
      
      if (is_closure_star(x))
	return(make_list_3(sc, small_int(sc, 0), s7_make_integer(sc, abs(len)), make_boolean(sc, len < 0)));
      return(make_list_3(sc, s7_make_integer(sc, abs(len)), small_int(sc, 0), make_boolean(sc, len < 0)));
    }
  
  if (s7_is_procedure_with_setter(x))
    return(make_list_3(sc, 
		       s7_make_integer(sc, pws_get_req_args(x)),
		       s7_make_integer(sc, pws_get_opt_args(x)),
		       sc->F));
  
  if ((s7_is_applicable_object(x)) ||
      (s7_is_continuation(x)))
    return(make_list_3(sc, small_int(sc, 0), small_int(sc, 0), sc->T));

  return(sc->NIL);
}


static bool is_thunk(s7_scheme *sc, s7_pointer x)
{
  switch (type(x))
    {
    case T_C_FUNCTION:
      return(c_function_all_args(x) == 0);

    case T_CLOSURE:
    case T_CLOSURE_STAR:
      return(caar(x) == sc->NIL);
    }
  return(false);
}


static s7_pointer g_procedure_arity(s7_scheme *sc, s7_pointer args)
{
  #define H_procedure_arity "(procedure-arity func) returns a list '(required optional rest)"
  s7_pointer x;
  if (s7_is_symbol(car(args)))
    x = s7_symbol_value(sc, car(args));
  else x = car(args);

  if (!is_procedure(x))
    return(s7_wrong_type_arg_error(sc, "procedure-arity", 0, x, "a procedure"));
  return(s7_procedure_arity(sc, x));
}


typedef struct {
  int type;
  const char *name;
  char *(*print)(s7_scheme *sc, void *value);
  void (*free)(void *value);
  bool (*equal)(void *val1, void *val2);
  void (*gc_mark)(void *val);
  s7_pointer (*apply)(s7_scheme *sc, s7_pointer obj, s7_pointer args);
  s7_pointer (*set)(s7_scheme *sc, s7_pointer obj, s7_pointer args);
  s7_pointer (*length)(s7_scheme *sc, s7_pointer obj);
  s7_pointer (*copy)(s7_scheme *sc, s7_pointer obj);
  s7_pointer (*fill)(s7_scheme *sc, s7_pointer obj, s7_pointer args);
} s7_c_object_t;

/* for-each and map? currently these assume (obj i) gets and (set! (obj i) val) sets
 *    aribtrary method list?
 * reverse append make section[subseq]
 */



static s7_c_object_t *object_types = NULL;
static int object_types_size = 0;
static int num_types = 0;


int s7_new_type(const char *name, 
		char *(*print)(s7_scheme *sc, void *value), 
		void (*free)(void *value), 
		bool (*equal)(void *val1, void *val2),
		void (*gc_mark)(void *val),
                s7_pointer (*apply)(s7_scheme *sc, s7_pointer obj, s7_pointer args),
                s7_pointer (*set)(s7_scheme *sc, s7_pointer obj, s7_pointer args))
{
  int tag;
  tag = num_types++;
  if (tag >= object_types_size)
    {
      if (object_types_size == 0)
	{
	  object_types_size = 8;
	  object_types = (s7_c_object_t *)calloc(object_types_size, sizeof(s7_c_object_t));
	}
      else
	{
	  object_types_size = tag + 8;
	  object_types = (s7_c_object_t *)realloc((void *)object_types, object_types_size * sizeof(s7_c_object_t));
	}
    }
  object_types[tag].type = tag;
  object_types[tag].name = s7_strdup(name);
  object_types[tag].free = free;
  object_types[tag].print = print;
  object_types[tag].equal = equal;
  object_types[tag].gc_mark = gc_mark;
  object_types[tag].apply = apply;
  object_types[tag].set = set;
  object_types[tag].length = NULL;
  object_types[tag].copy = NULL;
  object_types[tag].fill = NULL;
  return(tag);
}


int s7_new_type_x(const char *name, 
		  char *(*print)(s7_scheme *sc, void *value), 
		  void (*free)(void *value), 
		  bool (*equal)(void *val1, void *val2),
		  void (*gc_mark)(void *val),
		  s7_pointer (*apply)(s7_scheme *sc, s7_pointer obj, s7_pointer args),
		  s7_pointer (*set)(s7_scheme *sc, s7_pointer obj, s7_pointer args),
		  s7_pointer (*length)(s7_scheme *sc, s7_pointer obj),
		  s7_pointer (*copy)(s7_scheme *sc, s7_pointer obj),
		  s7_pointer (*fill)(s7_scheme *sc, s7_pointer obj, s7_pointer val))
{
  int tag;
  tag = s7_new_type(name, print, free, equal, gc_mark, apply, set);
  object_types[tag].length = length;
  object_types[tag].copy = copy;
  object_types[tag].fill = fill;
  return(tag);
}


static char *s7_describe_object(s7_scheme *sc, s7_pointer a)
{
  int tag;
  tag = c_object_type(a);
  if (object_types[tag].print)
    return((*(object_types[tag].print))(sc, c_object_value(a))); /* assume allocation here (so we'll free the string later) */
  return(s7_strdup(object_types[tag].name));
}


static void s7_free_object(s7_pointer a)
{
  int tag;
  tag = c_object_type(a);
  if (object_types[tag].free)
    (*(object_types[tag].free))(c_object_value(a));
}


static bool s7_equalp_objects(s7_pointer a, s7_pointer b)
{
  if ((is_c_object(a)) &&
      (is_c_object(b)) &&
      (c_object_type(a) == c_object_type(b)))
    {
      int tag;
      tag = c_object_type(a);
      if (object_types[tag].equal)
	return((*(object_types[tag].equal))(c_object_value(a), c_object_value(b)));
      return(a == b);
    }
  return(false);
}


static void s7_mark_embedded_objects(s7_pointer a) /* called by gc, calls fobj's mark func */
{
  int tag;
  tag = c_object_type(a);
  if (tag < num_types)
    {
      if (object_types[tag].gc_mark)
	(*(object_types[tag].gc_mark))(c_object_value(a));
    }
}


static bool s7_is_applicable_object(s7_pointer x)
{
  return((is_c_object(x)) &&
	 (object_types[c_object_type(x)].apply));
}


static s7_pointer s7_apply_object(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  int tag;
  tag = c_object_type(obj);
  if (object_types[tag].apply)
    return((*(object_types[tag].apply))(sc, obj, args));

  return(eval_error(sc, "attempt to apply ~A?", obj));
}


#define object_set_function(Obj) object_types[c_object_type(Obj)].set

static s7_pointer s7_object_set(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  int tag;
  tag = c_object_type(obj);
  if (object_types[tag].set)
    return((*(object_types[tag].set))(sc, obj, args));

  return(eval_error(sc, "attempt to set ~A?", obj));
}


/* generalized set! calls g_object_set which then calls the object's set function */

static s7_pointer g_object_set(s7_scheme *sc, s7_pointer args)
{
  return(s7_object_set(sc, car(args), cdr(args)));
}


void *s7_object_value(s7_pointer obj)
{
  return(c_object_value(obj));
}


int s7_object_type(s7_pointer obj)
{
  return(c_object_type(obj));
}


s7_pointer s7_make_object(s7_scheme *sc, int type, void *value)
{
  s7_pointer x;
  x = new_cell(sc);
  c_object_type(x) = type;
  c_object_value(x) = value;
  set_type(x, T_C_OBJECT | T_ATOM | T_FINALIZABLE | T_DONT_COPY);
  if (object_types[type].apply)
    typeflag(x) |= T_PROCEDURE;
  return(x);
}


static s7_pointer s7_object_length(s7_scheme *sc, s7_pointer obj)
{
  int tag;
  tag = c_object_type(obj);
  if (object_types[tag].length)
    return((*(object_types[tag].length))(sc, obj));

  return(eval_error(sc, "attempt to get length of ~A?", obj));
}


static s7_pointer s7_object_copy(s7_scheme *sc, s7_pointer obj)
{
  int tag;
  tag = c_object_type(obj);
  if (object_types[tag].copy)
    return((*(object_types[tag].copy))(sc, obj));

  return(eval_error(sc, "attempt to copy ~A?", obj));
}


static s7_pointer s7_object_fill(s7_scheme *sc, s7_pointer obj, s7_pointer val)
{
  int tag;
  tag = c_object_type(obj);
  if (object_types[tag].fill)
    return((*(object_types[tag].fill))(sc, obj, val));

  return(eval_error(sc, "attempt to fill ~A?", obj));
}


static s7_pointer g_object_for_each(s7_scheme *sc, s7_pointer args)
{
  /* (for-each (lambda (n) (format #t "~A " n)) (vct 1.0 2.0 3.0)) */
  int i, len;
  s7_pointer obj, x, fargs;

  sc->code = car(args);
  if (!is_procedure(sc->code))
    return(s7_wrong_type_arg_error(sc, "for-each", 1, sc->code, "a procedure"));

  obj = cadr(args); /* already checked */
  len = s7_integer(s7_object_length(sc, obj));
  fargs = s7_cons(sc, sc->NIL, sc->NIL);

  if (cddr(args) != sc->NIL)
    {
      for (i = 3, x = cddr(args); x != sc->NIL; x = cdr(x), i++)
	{
	  if (!s7_is_applicable_object(car(x)))
	    return(s7_wrong_type_arg_error(sc, "for-each", i, car(x), "an applicable object"));
	  if (len != s7_integer(s7_object_length(sc, car(x))))
	    return(s7_wrong_type_arg_error(sc, "for-each", i, car(x), "an object whose length matches the other objects"));
	  fargs = s7_cons(sc, sc->NIL, fargs);
	}
    }

  sc->args = s7_cons(sc, make_mutable_integer(sc, 0), 
               s7_cons(sc, s7_make_integer(sc, len), 
                 s7_cons(sc, fargs, cdr(args))));
  push_stack(sc, OP_OBJECT_FOR_EACH, sc->args, sc->code);
  return(sc->UNSPECIFIED);
}



/* -------- procedure-with-setter -------- */

static int pws_tag;

typedef struct {
  s7_pointer (*getter)(s7_scheme *sc, s7_pointer args);
  int get_req_args, get_opt_args;
  s7_pointer (*setter)(s7_scheme *sc, s7_pointer args);
  int set_req_args, set_opt_args;
  s7_pointer scheme_getter;
  s7_pointer scheme_setter;
  char *documentation;
  char *name;
} s7_pws_t;


s7_pointer s7_make_procedure_with_setter(s7_scheme *sc, 
					 const char *name,
					 s7_pointer (*getter)(s7_scheme *sc, s7_pointer args), 
					 int get_req_args, int get_opt_args,
					 s7_pointer (*setter)(s7_scheme *sc, s7_pointer args),
					 int set_req_args, int set_opt_args,
					 const char *documentation)
{
  s7_pws_t *f;
  s7_pointer obj;
  f = (s7_pws_t *)calloc(1, sizeof(s7_pws_t));
  f->getter = getter;
  f->get_req_args = get_req_args;
  f->get_opt_args = get_opt_args;
  f->setter = setter;
  f->set_req_args = set_req_args;
  f->set_opt_args = set_opt_args;
  if (documentation)
    f->documentation = s7_strdup(documentation);
  else f->documentation = NULL;
  if (name)
    f->name = s7_strdup(name);
  else f->name = NULL;
  f->scheme_getter = sc->NIL;
  f->scheme_setter = sc->NIL;
  obj = s7_make_object(sc, pws_tag, (void *)f);
  typeflag(obj) |= T_PROCEDURE;
  return(obj);
}


static char *pws_print(s7_scheme *sc, void *obj)
{
  s7_pws_t *f = (s7_pws_t *)obj;
  if (f->name)
    return(s7_strdup(f->name));
  return(s7_strdup((char *)"#<procedure-with-setter>"));
}


static void pws_free(void *obj)
{
  s7_pws_t *f = (s7_pws_t *)obj;
  if (f)
    {
      if (f->documentation)
	free(f->documentation);
      if (f->name)
	free(f->name);
      free(f);
    }
}


static void pws_mark(void *val)
{
  s7_pws_t *f = (s7_pws_t *)val;
  S7_MARK(f->scheme_getter);
  S7_MARK(f->scheme_setter);
}


static bool pws_equal(void *obj1, void *obj2)
{
  return(obj1 == obj2);
}


static s7_pointer pws_apply(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  /* this is called as the pws object apply method, not as the actual getter */
  s7_pws_t *f;
  f = (s7_pws_t *)s7_object_value(obj);
  if (f->getter != NULL)
    return((*(f->getter))(sc, args));

  push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
  sc->args = args;
  sc->code = f->scheme_getter;
  eval(sc, OP_APPLY);
  return(sc->value);
}


static s7_pointer pws_set(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  /* this is the pws set method, not the actual setter */
  s7_pws_t *f;
  f = (s7_pws_t *)s7_object_value(obj);
  if (f->setter != NULL)
    return((*(f->setter))(sc, args));

  push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
  sc->args = args;
  sc->code = f->scheme_setter;
  eval(sc, OP_APPLY);
  return(sc->value);
}


static s7_pointer g_make_procedure_with_setter(s7_scheme *sc, s7_pointer args)
{
  #define H_make_procedure_with_setter "(make-procedure-with-setter getter setter) combines its \
two function arguments as a procedure-with-setter.  The 'getter' is called unless the procedure \
occurs as the object of set!."

  s7_pointer p, getter, setter;
  s7_pws_t *f;
  /* the two args should be functions, the setter taking one more arg than the getter */

  getter = car(args);
  if (!is_procedure(getter))
    return(s7_wrong_type_arg_error(sc, "make-procedure-with-setter", 1, getter, "a procedure"));
  setter = cadr(args);
  if (!is_procedure(setter))
    return(s7_wrong_type_arg_error(sc, "make-procedure-with-setter", 2, setter, "a procedure"));

  p = s7_make_procedure_with_setter(sc, NULL, NULL, -1, 0, NULL, -1, 0, NULL);
  f = (s7_pws_t *)s7_object_value(p);

  f->scheme_getter = getter;
  if ((is_closure(getter)) ||
      (is_closure_star(getter)))
    f->get_req_args = s7_list_length(sc, closure_args(getter));
  else f->get_req_args = s7_list_length(sc, caar(args));
  
  f->scheme_setter = setter;
  if ((is_closure(setter)) ||
      (is_closure_star(setter)))
    f->set_req_args = s7_list_length(sc, closure_args(setter));
  else f->set_req_args = s7_list_length(sc, caadr(args));
  
  return(p);
}


bool s7_is_procedure_with_setter(s7_pointer obj)
{
  return((is_c_object(obj)) &&
	 (c_object_type(obj) == pws_tag));
}


s7_pointer s7_procedure_with_setter_getter(s7_pointer obj)
{
  s7_pws_t *f;
  f = (s7_pws_t *)s7_object_value(obj);
  return(f->scheme_getter);
}


s7_pointer s7_procedure_with_setter_setter(s7_pointer obj)
{
  s7_pws_t *f;
  f = (s7_pws_t *)s7_object_value(obj);
  return(f->scheme_setter);
}


static s7_pointer g_is_procedure_with_setter(s7_scheme *sc, s7_pointer args)
{
  #define H_is_procedure_with_setter "(procedure-with-setter? obj) returns #t if obj is a procedure-with-setter"
  return(make_boolean(sc, s7_is_procedure_with_setter(car(args))));
}


static char *pws_documentation(s7_pointer x)
{
  s7_pws_t *f = (s7_pws_t *)s7_object_value(x);
  return(f->documentation);
}


#if 0
static char *pws_name(s7_pointer x)
{
  s7_pws_t *f = (s7_pws_t *)s7_object_value(x);
  return(f->name);
}
#endif


static int pws_get_req_args(s7_pointer x)
{
  s7_pws_t *f = (s7_pws_t *)s7_object_value(x);
  return(f->get_req_args);
}


static int pws_get_opt_args(s7_pointer x)
{
  s7_pws_t *f = (s7_pws_t *)s7_object_value(x);
  return(f->get_opt_args);
}


static s7_pointer g_procedure_with_setter_setter_arity(s7_scheme *sc, s7_pointer args)
{
  s7_pws_t *f;
  if (!s7_is_procedure_with_setter(car(args)))
    return(s7_wrong_type_arg_error(sc, "procedure-with-setter-setter-arity", 0, car(args), "a procedure-with-setter"));

  f = (s7_pws_t *)s7_object_value(car(args));
  return(make_list_3(sc,
		     s7_make_integer(sc, f->set_req_args),
		     s7_make_integer(sc, f->set_opt_args),
		     sc->F));
}


static s7_pointer pws_source(s7_scheme *sc, s7_pointer x)
{
  s7_pws_t *f;
  f = (s7_pws_t *)s7_object_value(x);
  if ((is_closure(f->scheme_getter)) ||
      (is_closure_star(f->scheme_getter)))
    return(s7_append(sc, 
		     make_list_2(sc,
				 (is_closure(f->scheme_getter)) ? sc->LAMBDA : sc->LAMBDA_STAR,
				 closure_args(f->scheme_getter)),
		     closure_body(f->scheme_getter)));
  return(sc->NIL);
}


void s7_define_function_with_setter(s7_scheme *sc, const char *name, s7_function get_fnc, s7_function set_fnc, int req_args, int opt_args, const char *doc)
{
  s7_define_variable(sc, name, 
    s7_make_procedure_with_setter(sc, name, get_fnc, req_args, opt_args, set_fnc, req_args + 1, opt_args, doc));
}



/* -------------------------------- eq etc -------------------------------- */

bool s7_is_eq(s7_pointer obj1, s7_pointer obj2)
{
  return(obj1 == obj2);
}


bool s7_is_eqv(s7_pointer a, s7_pointer b) 
{
  if (a == b) 
    return(true);
  
#if WITH_GMP
  if (big_numbers_are_eqv(a, b)) return(true); /* T_NUMBER != T_C_OBJECT but both can represent numbers */
#endif

  if (type(a) != type(b)) 
    return(false);
  
  if (s7_is_string(a)) 
    return(string_value(a) == string_value(b));
  
  if (s7_is_number(a))
    return(numbers_are_eqv(a, b));
  
  if (s7_is_character(a))
    return(s7_character(a) == s7_character(b));
  
  return(false);
}


bool s7_is_equal(s7_pointer x, s7_pointer y)
{
  if (x == y) 
    return(true);
  
#if WITH_GMP
  if (big_numbers_are_eqv(x, y)) return(true); /* T_NUMBER != T_C_OBJECT but both can represent numbers */
#endif

  if (type(x) != type(y)) 
    return(false);
  
  if (is_pair(x))
    return((s7_is_equal(car(x), car(y))) &&
	   (s7_is_equal(cdr(x), cdr(y))));
  
  if (s7_is_string(x))
    return((string_length(x) == string_length(y)) &&
	   ((string_length(x) == 0) ||
	    (strcmp(string_value(x), string_value(y)) == 0)));
  
  if (is_c_object(x))
    return(s7_equalp_objects(x, y));
  
  if ((s7_is_vector(x)) ||
      (s7_is_hash_table(x)))
    return(vectors_equal(x, y));
  
  if (s7_is_character(x)) 
    return(s7_character(x) == s7_character(y));
  
  if (s7_is_number(x))
    return(numbers_are_eqv(x, y));
  
  return(false); /* we already checked that x != y (port etc) */
}


static s7_pointer g_is_eq(s7_scheme *sc, s7_pointer args)
{
  #define H_is_eq "(eq? obj1 obj2) returns #t if obj1 is eq to (the same object as) obj2"
  return(make_boolean(sc, car(args) == cadr(args)));
}


static s7_pointer g_is_eqv(s7_scheme *sc, s7_pointer args)
{
  #define H_is_eqv "(eqv? obj1 obj2) returns #t if obj1 is equivalent to obj2"
  return(make_boolean(sc, s7_is_eqv(car(args), cadr(args))));
}


static s7_pointer g_is_equal(s7_scheme *sc, s7_pointer args)
{
  #define H_is_equal "(equal? obj1 obj2) returns #t if obj1 is equal to obj2"
  return(make_boolean(sc, s7_is_equal(car(args), cadr(args))));
}



/* ---------------------------------------- length, copy, fill ---------------------------------------- */

static s7_pointer g_length(s7_scheme *sc, s7_pointer args)
{
  #define H_length "(length obj) returns the length of obj, which can be a list, vector, string, or hash-table"
  
  s7_pointer lst = car(args);
  
  if (lst == sc->NIL)
    return(small_int(sc, 0));

  switch (type(lst))
    {
    case T_PAIR:
      {
	int len;
	len = s7_list_length(sc, lst);
  
	if (len < 0) 
	  return(s7_wrong_type_arg_error(sc, "length:", 0, lst, "a proper (not a dotted) list"));
	if (len == 0)
	  return(s7_wrong_type_arg_error(sc, "length:", 0, lst, "a proper (not a circular) list"));
	
	return(s7_make_integer(sc, len));
      }

    case T_VECTOR:
      return(g_vector_length(sc, args));

    case T_STRING:
      return(g_string_length(sc, args));

    case T_HASH_TABLE:
      return(g_hash_table_size(sc, args));

    case T_C_OBJECT:
      return(s7_object_length(sc, lst));

    default:
      return(s7_wrong_type_arg_error(sc, "length", 0, lst, "a list, vector, string, or hash-table"));
    }
  
  return(small_int(sc, 0));
}


static s7_pointer s7_list_copy(s7_scheme *sc, s7_pointer obj)
{
  if (is_pair(obj))
    return(s7_cons(sc, car(obj), s7_list_copy(sc, cdr(obj))));
  return(obj);
}


static s7_pointer s7_copy(s7_scheme *sc, s7_pointer obj)
{
  switch (type(obj))
    {
    case T_STRING:
      return(s7_make_string_with_length(sc, s7_strdup_with_len(string_value(obj), string_length(obj)), string_length(obj)));

    case T_C_OBJECT:
      return(s7_object_copy(sc, obj));

    case T_HASH_TABLE:
    case T_VECTOR:
      return(s7_vector_copy(sc, obj));

    case T_PAIR:
      return(s7_list_copy(sc, obj)); /* should vector/list copy the objects as well as the container? */
    }
  return(obj);
}


static s7_pointer g_copy(s7_scheme *sc, s7_pointer args)
{
  #define H_copy "(copy obj) returns a copy of obj"
  return(s7_copy(sc, car(args)));
}


static s7_pointer s7_list_fill(s7_scheme *sc, s7_pointer obj, s7_pointer val)
{
  if (is_pair(obj))
    {
      if (is_pair(car(obj)))
	s7_list_fill(sc, car(obj), val);
      else car(obj) = val;

      if (is_pair(cdr(obj)))
	s7_list_fill(sc, cdr(obj), val);
      else
	{
	  if (cdr(obj) != sc->NIL)
	    cdr(obj) = val;
	}
    }
  return(obj);
}


static s7_pointer g_fill(s7_scheme *sc, s7_pointer args)
{
  #define H_fill "(fill obj val) fills obj with the value val"

  switch (type(car(args)))
    {
    case T_STRING:
      return(g_string_fill(sc, args));

    case T_HASH_TABLE:
    case T_VECTOR:
      return(g_vector_fill(sc, args));

    case T_C_OBJECT:
      return(s7_object_fill(sc, car(args), cadr(args)));

    case T_PAIR:
      return(s7_list_fill(sc, car(args), cadr(args)));
      
    }
  return(cadr(args));
}



/* -------------------------------- encapsulation -------------------------------- */

/* encapsulate:

   open-encapsulator, close-encapsulator, (obj) to restore, encapsulator-bindings
 
   (define-macro (encapsulate . body) 
      (let ((encap (gensym)))
        `(let ((,encap (open-encapsulator)))
           (dynamic-wind
              (lambda () 
	        #f)
              (lambda () 
	        ,@body)
              (lambda () 
	        ((,encap))  ; restore saved vars
                (close-encapsulator ,encap))))))
*/

#if WITH_ENCAPSULATION

typedef struct {
  s7_pointer bindings;
  s7_pointer envir;
} encap_t;

static int encapsulator_tag = 0;


static char *encapsulator_print(s7_scheme *sc, void *obj)
{
  /* show vars? */
  char *buf;
  buf = (char *)malloc(32 * sizeof(char));
  snprintf(buf, 32, "#<encapsulator>");
  return(buf);
}


static void encapsulator_free(void *obj)
{
  if (obj) free(obj);
}


static void encapsulator_mark(void *val)
{
  encap_t *f = (encap_t *)val;
  if ((f) && (f->bindings)) /* possibly still in open_encapsulator */
    {
      S7_MARK(f->bindings);
    }
}


static bool encapsulator_equal(void *obj1, void *obj2)
{
  return(obj1 == obj2);
}


static s7_pointer g_open_encapsulator(s7_scheme *sc, s7_pointer args)
{
  #define H_open_encapsulator "(open-encapsulator) opens an encapsulator; until it is closed, it will save variable values."

  /* create object, add to s7 list */
  encap_t *e;
  s7_pointer obj;
  e = (encap_t *)malloc(sizeof(encap_t));
  e->bindings = sc->NIL;
  e->envir = sc->envir;
  obj = s7_make_object(sc, encapsulator_tag, (void *)e);  
  sc->encapsulators = s7_cons(sc, obj, sc->encapsulators);
  return(obj);
}


static s7_pointer g_close_encapsulator(s7_scheme *sc, s7_pointer args)
{
  #define H_close_encapsulator "(close-encapsulator obj) closes an encapsulator; it will no longer try to protect anything."

  /* remove from s7 list */
  encap_t *e;
  s7_pointer x, obj;
  
  obj = car(args);
  if ((!is_c_object(obj)) ||
      (c_object_type(obj) != encapsulator_tag))
    return(s7_wrong_type_arg_error(sc, "close-encapsulator", 0, obj, "an encapsulator"));

  e = (encap_t *)s7_object_value(obj);
  e->bindings = sc->NIL;
  e->envir = sc->NIL;

  if (obj == car(sc->encapsulators))
    sc->encapsulators = cdr(sc->encapsulators);
  else
    {
      for (x = sc->encapsulators; is_pair(x); x = cdr(x))
	{
	  s7_pointer y;
	  y = cdr(x);
	  if (obj == car(y))
	    {
	      cdr(x) = cdr(y);
	      break;
	    }
	}
    }
  return(sc->UNSPECIFIED);
}


static s7_pointer g_is_encapsulator(s7_scheme *sc, s7_pointer args)
{
  #define H_is_encapsulator "(encapsulator? obj) returns #t if obj is an encapsulator"
  return(make_boolean(sc, (is_c_object(car(args))) && (c_object_type(car(args)) == encapsulator_tag)));
}


static s7_pointer g_encapsulator_bindings(s7_scheme *sc, s7_pointer args)
{
  #define H_encapsulator_bindings "(encapsulator-bindings obj) returns the values currently awaiting restoration in an encapsulator."

  encap_t *e;
  s7_pointer obj;
  
  obj = car(args);
  if ((!is_c_object(obj)) ||
      (c_object_type(obj) != encapsulator_tag))
    return(s7_wrong_type_arg_error(sc, "encapsulator-bindings", 0, obj, "an encapsulator"));

  e = (encap_t *)s7_object_value(obj);
  return(e->bindings);
}


static s7_pointer encapsulator_apply(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  encap_t *e;
  e = (encap_t *)s7_object_value(obj);

  if (args == sc->NIL)
    {
      /* no args, restore all values */
      s7_pointer x, y;
      for (x = e->bindings; is_pair(x); x = cdr(x)) 
	{
	  y = s7_find_symbol_in_environment(sc, sc->envir, caar(x), true);
	  if (y != sc->NIL)
	    set_symbol_value(y, cdar(x));
	}
      e->bindings = sc->NIL;
    }
  else
    {
      if (s7_is_symbol(car(args)))
	{
	  /* find symbol in this encapsulation, return its value */
	  s7_pointer y, sym;
	  sym = car(args);
	  for (y = e->bindings; is_pair(y); y = cdr(y)) 
	    if (sym == caar(y))
	      return(cdar(y));
	  
	  return(sc->UNDEFINED);
	}
      else return(s7_wrong_type_arg_error(sc, "encapsulator", 0, car(args), "a symbol"));
    }
  return(obj);
}


static void encapsulate(s7_scheme *sc, s7_pointer sym)
{
  s7_pointer x;

  for (x = sc->encapsulators; is_pair(x); x = cdr(x))
    {
      encap_t *e;
      s7_pointer y;
      bool ok = false;

      e = (encap_t *)s7_object_value(car(x));

      for (y = e->bindings; is_pair(y); y = cdr(y)) 
	if (sym == caar(y))
	  {
	    ok = true;
	    break;
	  }

      if (!ok)
	{
	  y = s7_find_symbol_in_environment(sc, e->envir, sym, true);
	  if (y != sc->NIL)
	    e->bindings = s7_cons(sc, 
                            s7_cons(sc, sym, s7_copy(sc, symbol_value(y))), 
                            e->bindings); 

	  /* a C-side procedure-with-setter becomes a no-op here -- accessing the underlying value is too tricky */
	}
    }
}
#endif




/* -------------------------------- format -------------------------------- */

typedef struct {
  char *str;
  int len, loc;
  s7_pointer args;
} format_data;


static char *s7_format_error(s7_scheme *sc, const char *msg, const char *str, s7_pointer args, format_data *dat)
{
  int len;
  char *errmsg;
  s7_pointer x;

  if (dat->loc == 0)
    {
      len = safe_strlen(msg) + 32;
      errmsg = (char *)malloc(len * sizeof(char));
      snprintf(errmsg, len, "format ~S ~{~A~^ ~}: %s", msg);
    }
  else 
    {
      char *filler;
      int i;
      filler = (char *)calloc(dat->loc + 12, sizeof(char));
      for (i = 0; i < dat->loc + 11; i++)
	filler[i] = ' ';
      len = safe_strlen(msg) + 32 + dat->loc + 12;
      errmsg = (char *)malloc(len * sizeof(char));
      snprintf(errmsg, len, "\nformat: ~S ~{~A~^ ~}\n%s^: %s", filler, msg);
      free(filler);
    }

  x = make_list_3(sc, s7_make_string(sc, errmsg), s7_make_string(sc, str), args);

  free(errmsg);
  if (dat->str) free(dat->str);
  free(dat);

  s7_error(sc, sc->FORMAT_ERROR, x);
  return(NULL);
}


static void format_append_char(format_data *dat, char c)
{
  if (dat->len <= dat->loc + 2)
    {
      dat->len *= 2;
      dat->str = (char *)realloc(dat->str, dat->len * sizeof(char));
    }
  dat->str[dat->loc++] = c;
}


static void format_append_string(format_data *dat, const char *str)
{
  int i, len;
  len = safe_strlen(str);
  for (i = 0; i < len; i++)
    format_append_char(dat, str[i]);
}


static int format_read_integer(s7_scheme *sc, int *cur_i, int str_len, const char *str, s7_pointer args, format_data *fdat)
{
  int i, arg1 = -1;
  char *tmp;
  i = *cur_i;
  if (isdigit(str[i]))
    {
      tmp = (char *)(str + i);
      if (sscanf(tmp, "%d", &arg1) < 1)
	s7_format_error(sc, "bad number?", str, args, fdat);

      for (i = i + 1; i < str_len - 1; i++)
	if (!isdigit(str[i]))
	  break;
      if (i >= str_len)
	s7_format_error(sc, "numeric argument, but no directive!", str, args, fdat);
    }
  *cur_i = i;
  return(arg1);
}


static void format_number(s7_scheme *sc, format_data *fdat, int radix, int width, int precision, char float_choice, char pad)
{
  char *tmp;
  if (width < 0) width = 0;
  tmp = s7_number_to_string_with_radix(sc, car(fdat->args), radix, width, precision, float_choice);
  if (pad != ' ')
    {
      char *padtmp;
      padtmp = tmp;
      while (*padtmp == ' ') (*(padtmp++)) = pad;
    }
  format_append_string(fdat, tmp);
  free(tmp);
  fdat->args = cdr(fdat->args);
}


static char *format_to_c_string(s7_scheme *sc, const char *str, s7_pointer args, s7_pointer *next_arg)
{
  #define INITIAL_FORMAT_LENGTH 128
  int i = 0, str_len = 0;
  format_data *fdat = NULL;
  char *result, *tmp = NULL;

  str_len = strlen(str);

  fdat = (format_data *)malloc(sizeof(format_data));
  fdat->loc = 0;
  fdat->len = INITIAL_FORMAT_LENGTH;
  fdat->str = (char *)calloc(fdat->len, sizeof(char)); /* ~nT col checks need true current string length, so malloc here is messy */
  fdat->str[0] = '\0';
  fdat->args = args;

  if (str_len == 0)
    {
      if ((args != sc->NIL) &&
	  (next_arg == NULL))
	return(s7_format_error(sc, "too many arguments", str, args, fdat));
    }
  else
    {
      for (i = 0; i < str_len - 1; i++)
	{
	  if (str[i] == '~')
	    {
	      switch (str[i + 1])
		{
		case '%':                           /* -------- newline -------- */
		  format_append_char(fdat, '\n');
		  i++;
		  break;

		case '&':                           /* -------- conditional newline -------- */
		  if ((fdat->loc > 0) &&
		      (fdat->str[fdat->loc - 1] != '\n'))
		    format_append_char(fdat, '\n');
		  i++;
		  break;
		  
		case '~':                           /* -------- tilde -------- */
		  format_append_char(fdat, '~');
		  i++;
		  break;

		case '\n':                          /* -------- trim white-space -------- */
		  for (i = i + 2; i <str_len - 1; i++)
		    if (!(isspace(str[i])))
		      {
			i--;
			break;
		      }
		  break;
		  
		case '*':                           /* -------- ignore arg -------- */
		  i++;
		  fdat->args = cdr(fdat->args);
		  break;

		case '^':                           /* -------- exit -------- */
		  if (fdat->args == sc->NIL)
		    {
		      i = str_len;
		      goto ALL_DONE;
		    }
		  i++;
		  break;
		  
		case 'A': case 'a':                 /* -------- object->string -------- */
		case 'C': case 'c':
		case 'S': case 's':

		  /* slib suggests num arg to ~A and ~S to truncate: ~20A sends only (up to) 20 chars of object->string result,
		   *   but that could easily(?) be handled with substring and an embedded format arg.
		   */
		  if (fdat->args == sc->NIL)
		    return(s7_format_error(sc, "missing argument", str, args, fdat));
		  i++;
		  if (((str[i] == 'C') || (str[i] == 'c')) &&
		      (!s7_is_character(car(fdat->args))))
		    return(s7_format_error(sc, "~C directive requires a character argument", str, args, fdat));

		  tmp = s7_object_to_c_string_1(sc, car(fdat->args), (str[i] == 'S') || (str[i] == 's'), 0, false);
		  format_append_string(fdat, tmp);
		  if (tmp) free(tmp);
		  fdat->args = cdr(fdat->args);
		  break;
		  
		case '{':                           /* -------- iteration -------- */
		  {
		    s7_pointer curly_arg;
		    char *curly_str = NULL;
		    int k, curly_len = -1, curly_nesting = 1;
		    if (!s7_is_list(sc, car(fdat->args)))
		      return(s7_format_error(sc, "'{' directive argument should be a list", str, args, fdat));

		    for (k = i + 2; k < str_len - 1; k++)
		      if (str[k] == '~')
			{
			  if (str[k + 1] == '}')
			    {
			      curly_nesting--;
			      if (curly_nesting == 0)
				{
				  curly_len = k - i - 1;
				  break;
				}
			    }
			  else
			    {
			      if (str[k + 1] == '{')
				curly_nesting++;
			    }
			}
		    if (curly_len == -1)
		      return(s7_format_error(sc, "'{' directive, but no matching '}'", str, args, fdat));

		    if (curly_len <= 1)
		      return(s7_format_error(sc, "~{...~} doesn't consume any arguments!", str, args, fdat));

		    curly_str = (char *)malloc(curly_len * sizeof(char));
		    for (k = 0; k < curly_len - 1; k++)
		      curly_str[k] = str[i + 2 + k];
		    curly_str[curly_len - 1] = '\0';

		    curly_arg = car(fdat->args); 
		    while (curly_arg != sc->NIL)
		      {
			s7_pointer new_arg = sc->NIL;
			tmp = format_to_c_string(sc, curly_str, curly_arg, &new_arg);
			format_append_string(fdat, tmp);
			if (tmp) free(tmp);
			if (curly_arg == new_arg)
			  {
			    if (curly_str) free(curly_str);
			    return(s7_format_error(sc, "~{...~} doesn't consume any arguments!", str, args, fdat));
			  }
			curly_arg = new_arg;
		      }

		    i += (curly_len + 2); /* jump past the ending '}' too */
		    fdat->args = cdr(fdat->args);
		    free(curly_str);
		  }
		  break;
		  
		case '}':
		  return(s7_format_error(sc, "unmatched '}'", str, args, fdat));
		  
		  /* -------- numeric args -------- */
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case ',':
		case 'B': case 'b':
		case 'D': case 'd':
		case 'E': case 'e':
		case 'F': case 'f':
		case 'G': case 'g':
		case 'O': case 'o':
		case 'T': case 't':
		case 'X': case 'x':
		  {
		    int width = -1, precision = -1;
		    char pad = ' ';
		    i++;
		    if (isdigit(str[i]))
		      width = format_read_integer(sc, &i, str_len, str, args, fdat);
		    if (str[i] == ',')
		      {
			i++;
			if (isdigit(str[i]))
			  precision = format_read_integer(sc, &i, str_len, str, args, fdat);
			else
			  {
			    if (str[i] == '\'')       /* (format #f "~12,'xD" 1) -> "xxxxxxxxxxx1" */
			      {
				pad = str[i + 1];
				i += 2;
			      }
			  }
		      }
		    if ((str[i] != 'T') && (str[i] != 't'))
		      {
			if (fdat->args == sc->NIL)
			  return(s7_format_error(sc, "missing argument", str, args, fdat));
			if (!(s7_is_number(car(fdat->args))))
			  return(s7_format_error(sc, "numeric argument required", str, args, fdat));
		      }

		    switch (str[i])
		      {
			/* -------- pad to column -------- */
			/*   are columns numbered from 1 or 0?  there seems to be disagreement about this directive */
			/*   does "space over to" mean including? */
		      case 'T': case 't':
			if (width == -1) width = 0;
			if (precision == -1) precision = 0;
			if ((width > 0) || (precision > 0))         /* (format #f "a~8Tb") */
			  {
			    int j, k, outstr_len;
			    outstr_len = safe_strlen(fdat->str);
			    for (k = outstr_len - 1; k > 0; k--)
			      if (fdat->str[k] == '\n')
				break;
			    
			    if (precision > 0)
			      {
				int mult;
				mult = (int)(ceil((s7_Double)(outstr_len - k - width) / (s7_Double)precision)); /* CLtL2 ("least positive int") */
				if (mult < 1) mult = 1;
				width += (precision * mult);
			      }

			    for (j = outstr_len - k; j < width; j++)
			      format_append_char(fdat, pad);
			  }
			break;

			/* -------- numbers -------- */
		      case 'F': case 'f':
			format_number(sc, fdat, 10, width, (precision < 0) ? 6 : precision, 'f', pad);
			break;

		      case 'G': case 'g':
			format_number(sc, fdat, 10, width, (precision < 0) ? 6 : precision, 'g', pad);
			break;

		      case 'E': case 'e':
			format_number(sc, fdat, 10, width, (precision < 0) ? 6 : precision, 'e', pad);
			break;

		      case 'D': case 'd':
			format_number(sc, fdat, 10, width, (precision < 0) ? 0 : precision, 'd', pad);
			break;

		      case 'O': case 'o':
			format_number(sc, fdat, 8, width, (precision < 0) ? 0 : precision, 'o', pad);
			break;

		      case 'X': case 'x':
			format_number(sc, fdat, 16, width, (precision < 0) ? 0 : precision, 'x', pad);
			break;

		      case 'B': case 'b':
			format_number(sc, fdat, 2, width, (precision < 0) ? 0 : precision, 'b', pad);
			break;
		      
		      default:
			return(s7_format_error(sc, "unimplemented format directive", str, args, fdat));
		      }
		  }
		  break;

		default:
		  return(s7_format_error(sc, "unimplemented format directive", str, args, fdat));
		}
	    }
	  else format_append_char(fdat, str[i]);
	}
    }

 ALL_DONE:
  if (next_arg)
    (*next_arg) = fdat->args;
  else
    {
      if (fdat->args != sc->NIL)
	return(s7_format_error(sc, "too many arguments", str, args, fdat));
    }
  if (i < str_len)
    format_append_char(fdat, str[i]);    /* possible trailing ~ is sent out */
  format_append_char(fdat, '\0');
  result = fdat->str;
  free(fdat);
  return(result);
}


static s7_pointer format_to_output(s7_scheme *sc, s7_pointer out_loc, const char *in_str, s7_pointer args)
{
  char *out_str;
  s7_pointer result;

  if (!in_str)
    {
      if (args != sc->NIL)
	return(s7_error(sc, 
			sc->FORMAT_ERROR, 
			make_list_2(sc, s7_make_string(sc, "format control string is null, but there are other arguments"), args)));
      return(s7_make_string(sc, ""));
    }

  out_str = format_to_c_string(sc, in_str, args, NULL);
  result = s7_make_string(sc, out_str);
  if (out_str) free(out_str);

  if (out_loc != sc->F)
    s7_display(sc, result, out_loc);

  return(result);
}


static s7_pointer g_format(s7_scheme *sc, s7_pointer args)
{
  #define H_format "(format out str . args) substitutes args into str sending the result to out. Most of \
s7's format directives are taken from CL: ~% = newline, ~& = newline if the preceding output character was \
no a newline, ~~ = ~, ~<newline> trims white space, ~* skips an argument, ~^ exits {} iteration if the arg list is exhausted, \
~nT inserts n tabs, ~A prints a representation of any object, ~S is the same, but puts strings in double quotes, \
~C prints a character, numbers are handled by ~F, ~E, ~G, ~B, ~O, ~D, and ~X with preceding numbers giving \
spacing (and spacing character) and precision.  ~{ starts an embedded format directive which is ended by ~}: \n\
\n\
  >(format #f \"dashed: ~{~A~^-~}\" '(1 2 3))\n\
  \"dashed: 1-2-3\""

  s7_pointer pt;
  pt = car(args);

  if (s7_is_string(pt))
    return(format_to_output(sc, sc->F, s7_string(pt), cdr(args)));

  if (!s7_is_string(cadr(args)))
    return(s7_wrong_type_arg_error(sc, "format", 2, cadr(args), "a string"));
    
  if (!((s7_is_boolean(pt)) ||               /* #f or #t */
	(pt == sc->NIL) ||                   /* default current-output-port = stdout -> nil */
	(s7_is_output_port(sc, pt))))        /* (current-output-port) or call-with-open-file arg, etc */
    return(s7_wrong_type_arg_error(sc, "format", 1, pt, "#f, #t, or an output port"));

  return(format_to_output(sc, (pt == sc->T) ? sc->output_port : pt, s7_string(cadr(args)), cddr(args)));
}


const char *s7_format(s7_scheme *sc, s7_pointer args)
{
  return(s7_string(g_format(sc, args))); /* for the run macro in run.c */
}



/* -------------------------------- errors -------------------------------- */

/* -------- trace -------- */

/* 
    (define (hiho arg) (if (> arg 0) (+ 1 (hiho (- arg 1))) 0))
    (trace hiho)
    (hiho 3)

    [hiho 3]
     [hiho 2]
      [hiho 1]
       [hiho 0]
        0
       1
      2
     3
*/

static s7_pointer g_trace(s7_scheme *sc, s7_pointer args)
{
  #define H_trace "(trace . args) adds each function in its argument list to the trace list.\
Each argument can be a function, symbol, macro, or any applicable object: (trace abs '+ v) where v is a vct \
prints out data about any call on abs or +, and any reference to the vct v. Trace output is sent \
to the current-output-port.  If trace is called without any arguments, everything is traced -- use \
untrace without arguments to turn this off."

  int i;
  s7_pointer x;

#if HAVE_PTHREADS
  sc = sc->orig_sc;
#endif

  if (args == sc->NIL)
    {
      (*(sc->trace_all)) = true;
      (*(sc->tracing)) = true;
      return(sc->F);
    }
  
  for (i = 0, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if ((!s7_is_symbol(car(x))) &&
	(!s7_is_procedure(car(x))) &&
	(!is_any_macro(car(x))))
      return(s7_wrong_type_arg_error(sc, "trace", i + 1, car(x), "a symbol, a function, or some other applicable object"));

  for (i = 0, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      if (s7_is_symbol(car(x)))
	sc->trace_list[sc->trace_top++] = eval_symbol(sc, car(x));
      else sc->trace_list[sc->trace_top++] = car(x);
      if (sc->trace_top >= sc->trace_list_size)
	{
	  sc->trace_list_size *= 2;
	  sc->trace_list = (s7_pointer *)realloc(sc->trace_list, sc->trace_list_size * sizeof(s7_pointer));
	}
    }

  (*(sc->tracing)) = (sc->trace_top > 0);
  return(sc->T);
}


static s7_pointer g_untrace(s7_scheme *sc, s7_pointer args)
{
  #define H_untrace "(untrace . args) removes each function in its arg list from the trace list. \
If untrace is called with no arguments, all functions are removed, turning off all tracing."
  int i, j;
  s7_pointer x;

#if HAVE_PTHREADS
  sc = sc->orig_sc;
#endif

  if (args == sc->NIL)
    {
      (*(sc->trace_all)) = false;
      for (i = 0; i < sc->trace_top; i++)
	sc->trace_list[i] = sc->NIL;
      sc->trace_top = 0;
      (*(sc->tracing)) = false;
      return(sc->F);
    }

  for (x = args; x != sc->NIL; x = cdr(x)) 
    {
      s7_pointer value;
      if (s7_is_symbol(car(x)))
	value = eval_symbol(sc, car(x));
      else value = car(x);
      
      for (i = 0; i < sc->trace_top; i++)
	if (value == sc->trace_list[i])
	  sc->trace_list[i] = sc->NIL;
    }
  
  /* now collapse list and reset trace_top (and possibly tracing) */
  for (i = 0, j = 0; i < sc->trace_top; i++)
    if (sc->trace_list[i] != sc->NIL)
      sc->trace_list[j++] = sc->trace_list[i];
  
  sc->trace_top = j;
  (*(sc->tracing)) = (sc->trace_top > 0);
  return(sc->T);
}


static void trace_apply(s7_scheme *sc)
{
  int i;
  bool trace_it = false;

#if HAVE_PTHREADS
  int id;
  id = sc->thread_id;
  sc = sc->orig_sc;
#endif

  if (*(sc->trace_all))
    trace_it = true;
  else
    {
      for (i = 0; i < sc->trace_top; i++)
	if (sc->code == sc->trace_list[i])
	  {
	    trace_it = true;
	    break;
	  }
    }

  if (trace_it)
    {
      int k, len;
      char *tmp1, *tmp2, *str;
      push_stack(sc, OP_TRACE_RETURN, sc->code, sc->NIL);
      tmp1 = s7_object_to_c_string(sc, sc->code);
      tmp2 = s7_object_to_c_string(sc, sc->args);
      len = safe_strlen(tmp2);
      tmp2[0] = ' ';
      tmp2[len - 1] = ']';
      
      len += (safe_strlen(tmp1) + sc->trace_depth + 64);
      str = (char *)calloc(len, sizeof(char));
      
      for (k = 0; k < sc->trace_depth; k++) str[k] = ' ';
      str[k] = '[';
      strcat(str, tmp1);
      strcat(str, tmp2);
      free(tmp1);
      free(tmp2);
      
#if HAVE_PTHREADS
      if (id != 0) /* main thread */
	{
	  char *tmp3;
	  tmp3 = (char *)calloc(64, sizeof(char));
	  snprintf(tmp3, 64, " (thread %d)", id);
	  strcat(str, tmp3);
	  free(tmp3);
	}
#endif
      
      strcat(str, "\n");
      write_string(sc, str, sc->output_port);
      free(str);
      
      sc->trace_depth++;

      {
	/* handle *trace-hook* */
	s7_pointer trace_hook, trace_hook_binding;
	trace_hook_binding = s7_find_symbol_in_environment(sc, sc->envir, sc->TRACE_HOOK, true);
	if ((trace_hook_binding != sc->NIL) &&
	    (is_procedure(symbol_value(trace_hook_binding))))
	  {
	    trace_hook = symbol_value(trace_hook_binding);
	    push_stack(sc, OP_TRACE_HOOK_QUIT, sc->args, sc->code); /* restore current state after dealing with the trace hook func */
	    sc->args = make_list_2(sc, sc->code, sc->args);
	    sc->code = trace_hook;
	  }
      }
    }
}


static void trace_return(s7_scheme *sc)
{
  int k, len;
  char *str, *tmp;

#if HAVE_PTHREADS
  sc = sc->orig_sc;
#endif

  tmp = s7_object_to_c_string(sc, sc->value);  

  len = sc->trace_depth + safe_strlen(tmp) + 2;
  str = (char *)calloc(len, sizeof(char));

  for (k = 0; k < sc->trace_depth; k++) str[k] = ' ';
  strcat(str, tmp);
  strcat(str, "\n");
  free(tmp);

  write_string(sc, str, sc->output_port);
  free(str);

  sc->trace_depth--;
}



/* -------- error handlers -------- */

static const char *s7_type_name(s7_pointer arg)
{
  switch (type(arg))
    {
    case T_NIL:          return("nil");
    case T_UNTYPED:      return("untyped");
    case T_BOOLEAN:      return("boolean");
    case T_STRING:       return("string");
    case T_SYMBOL:       return("symbol");
    case T_PAIR:         return("pair");
    case T_CLOSURE:
    case T_CLOSURE_STAR: return("closure");
    case T_GOTO:
    case T_CONTINUATION: return("continuation");
    case T_C_FUNCTION:   return("function");
    case T_C_MACRO:      return("macro");
    case T_C_POINTER:    return("c-pointer");
    case T_CHARACTER:    return("character");
    case T_VECTOR:       return("vector");
    case T_MACRO:        return("macro");
    case T_CATCH:        return("catch");
    case T_DYNAMIC_WIND: return("dynamic-wind");
    case T_HASH_TABLE:   return("hash-table");
    case T_C_OBJECT:     return(object_types[c_object_type(arg)].name);

    case T_INPUT_PORT:
      {
	if (is_file_port(arg))
	  return("input file port");
	if (is_string_port(arg))
	  return("input string port");
	return("input port");
      }

    case T_OUTPUT_PORT:
      {
	if (is_file_port(arg))
	  return("output file port");
	if (is_string_port(arg))
	  return("output string port");
	return("output port");
      }
      
    case T_NUMBER: 
      {
	switch (number_type(arg))
	  {
	  case NUM_INT:   return("integer");
	  case NUM_RATIO: return("ratio");
	  case NUM_REAL:  
	  case NUM_REAL2: return("real");
	  default:        return("complex number"); /* "a complex" doesn't sound right */
	  }
      }
    }
  return("messed up object");
}


s7_pointer s7_wrong_type_arg_error(s7_scheme *sc, const char *caller, int arg_n, s7_pointer arg, const char *descr)
{
  int len, slen;
  char *errmsg;
  s7_pointer x;
  const char *typ, *a_or_an = "a ";

  typ = s7_type_name(arg);
  if ((typ[0] == 'a') || (typ[0] == 'e') || (typ[0] == 'i') || (typ[0] == 'o') || (typ[0] == 'u'))
    a_or_an = "an ";
  if ((type(arg) == T_NIL) || (type(arg) == T_UNTYPED))
    a_or_an = "";

  len = safe_strlen(descr) + safe_strlen(caller) + 64;
  errmsg = (char *)malloc(len * sizeof(char));
  if (arg_n == 0)
    slen = snprintf(errmsg, len, "%s argument ~S is %s%s, but should be %s", caller, a_or_an, typ, descr);
  else
    {
      if (arg_n < 0) arg_n = 1;
      slen = snprintf(errmsg, len, "%s argument %d, ~S, is %s%s, but should be %s", caller, arg_n, a_or_an, typ, descr);
    }
  x = make_list_2(sc, s7_make_string_with_length(sc, errmsg, slen), arg);
  free(errmsg);

  return(s7_error(sc, sc->WRONG_TYPE_ARG, x));
}


s7_pointer s7_out_of_range_error(s7_scheme *sc, const char *caller, int arg_n, s7_pointer arg, const char *descr)
{
  int len, slen;
  char *errmsg;
  s7_pointer x;
  
  len = safe_strlen(descr) + safe_strlen(caller) + 64;
  errmsg = (char *)malloc(len * sizeof(char));
  if (arg_n <= 0) arg_n = 1;
  slen = snprintf(errmsg, len, "%s argument %d is out of range (%s)", caller, arg_n, descr);
  x = make_list_2(sc, s7_make_string_with_length(sc, errmsg, slen), arg);
  free(errmsg);

  return(s7_error(sc, sc->OUT_OF_RANGE, x));
}


s7_pointer s7_wrong_number_of_args_error(s7_scheme *sc, const char *caller, s7_pointer args)
{
  return(s7_error(sc, sc->WRONG_NUMBER_OF_ARGS, make_list_2(sc, s7_make_string(sc, caller), args)));
}


static s7_pointer s7_division_by_zero_error(s7_scheme *sc, const char *caller, s7_pointer arg)
{
  int len, slen;
  char *errmsg;
  s7_pointer x;

  len = safe_strlen(caller) + 128;
  errmsg = (char *)malloc(len * sizeof(char));
  slen = snprintf(errmsg, len, "%s: division by zero in ~A", caller);
  x = make_list_2(sc, s7_make_string_with_length(sc, errmsg, slen), arg);
  free(errmsg);

  return(s7_error(sc, s7_make_symbol(sc, "division-by-zero"), x));
}


static s7_pointer s7_file_error(s7_scheme *sc, const char *caller, const char *descr, const char *name)
{
  int len, slen;
  char *errmsg;
  s7_pointer x;

  len = safe_strlen(descr) + safe_strlen(name) + safe_strlen(caller) + 8;
  errmsg = (char *)malloc(len * sizeof(char));
  slen = snprintf(errmsg, len, "%s: %s %s", caller, descr, name);
  x = s7_make_string_with_length(sc, errmsg, slen);
  free(errmsg);

  return(s7_error(sc, s7_make_symbol(sc, "io-error"), x));
}


void s7_set_error_exiter(s7_scheme *sc, void (*error_exiter)(void))
{
  sc->error_exiter = error_exiter;
}


static s7_pointer g_dynamic_wind(s7_scheme *sc, s7_pointer args)
{
  #define H_dynamic_wind "(dynamic-wind init body finish) calls init, then body, then finish, \
each a function of no arguments, guaranteeing that finish is called even if body is exited"
  s7_pointer p;

  if (!is_thunk(sc, car(args)))
    return(s7_wrong_type_arg_error(sc, "dynamic-wind", 1, car(args), "a thunk"));
  if (!is_thunk(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, "dynamic-wind", 2, cadr(args), "a thunk"));
  if (!is_thunk(sc, caddr(args)))
    return(s7_wrong_type_arg_error(sc, "dynamic-wind", 3, caddr(args), "a thunk"));
  
  p = new_cell(sc);
  dynamic_wind_in(p) = car(args);
  dynamic_wind_body(p) = cadr(args);
  dynamic_wind_out(p) = caddr(args);
  set_type(p, T_DYNAMIC_WIND | T_ATOM | T_DWIND_INIT | T_DONT_COPY); /* atom -> don't mark car/cdr, don't copy */

  push_stack(sc, OP_DYNAMIC_WIND, sc->NIL, p);          /* args will be the saved result, code = s7_dynwind_t obj */
  
  sc->args = sc->NIL;
  sc->code = car(args);
  push_stack(sc, OP_APPLY, sc->args, sc->code);
  return(sc->F);
}


static s7_pointer g_catch(s7_scheme *sc, s7_pointer args)
{
  #define H_catch "(catch tag thunk handler) evaluates thunk; if an error occurs that matches tag (#t matches all), the handler is called"
  s7_pointer p;

  if (!is_thunk(sc, cadr(args)))
    return(s7_wrong_type_arg_error(sc, "catch", 2, cadr(args), "a thunk"));
  if (!is_procedure(caddr(args)))
    return(s7_wrong_type_arg_error(sc, "catch", 3, caddr(args), "a procedure"));
  
  p = new_cell(sc);
  catch_tag(p) = car(args);
  catch_goto_loc(p) = sc->stack_top;
  catch_handler(p) = caddr(args);
  set_type(p, T_CATCH | T_ATOM | T_DONT_COPY); /* atom -> don't mark car/cdr, don't copy */

  push_stack(sc, OP_CATCH, sc->NIL, p);
  sc->args = sc->NIL;
  sc->code = cadr(args);
  push_stack(sc, OP_APPLY, sc->args, sc->code);
  return(sc->F);
}


/* error reporting info -- save filename and line number */

#define INITIAL_FILE_NAMES_SIZE 8
static char **file_names = NULL;
static int file_names_size = 0;
static int file_names_top = -1;

#if HAVE_PTHREADS
static pthread_mutex_t remember_files_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

#define remembered_line_number(Line) (Line & 0xfffff)
#define remembered_file_name(Line)   (((Line >> 20) <= file_names_top) ? file_names[Line >> 20] : "?")
/* this gives room for 4000 files each of 1000000 lines */

static s7_pointer remember_line(s7_scheme *sc, s7_pointer obj)
{
  if (sc->input_port != sc->NIL)
    set_pair_line_number(obj, port_line_number(sc->input_port) | (port_file_number(sc->input_port) << 20));
  return(obj);
}


static int remember_file_name(const char *file)
{
#if HAVE_PTHREADS
  pthread_mutex_lock(&remember_files_lock);
#endif

  file_names_top++;
  if (file_names_top >= file_names_size)
    {
      if (file_names_size == 0)
	{
	  file_names_size = INITIAL_FILE_NAMES_SIZE;
	  file_names = (char **)calloc(file_names_size, sizeof(char *));
	}
      else
	{
	  int i, old_size;
	  old_size = file_names_size;
	  file_names_size *= 2;
	  file_names = (char **)realloc(file_names, file_names_size * sizeof(char *));
	  for (i = old_size; i < file_names_size; i++)
	    file_names[i] = NULL;
	}
    }
  file_names[file_names_top] = s7_strdup(file);

#if HAVE_PTHREADS
  pthread_mutex_unlock(&remember_files_lock);
#endif

  return(file_names_top);
}


#define ERROR_INFO_DEFAULT sc->F
#define ERROR_TYPE 0
#define ERROR_DATA 1
#define ERROR_CODE 2
#define ERROR_CODE_LINE 3
#define ERROR_CODE_FILE 4
#define ERROR_ENVIRONMENT 5
#define ERROR_STACK_SIZE 8
#define ERROR_INFO_SIZE (6 + ERROR_STACK_SIZE)

/* *error-info* is a vector of 6 or more elements:
 *    0: the error type or tag ('division-by-zero)
 *    1: the message or information passed by the error function
 *    2: if not #f, the code that s7 thinks triggered the error
 *    3: if not #f, the line number of that code
 *    4: if not #f, the file name of that code
 *    5: the environment at the point of the error
 *    6..top: stack enviroment pointers (giving enough info to reconstruct the current call stack), ending in #f
 */

/* slightly ugly:

(define-macro (cerror . args)
  `(call/cc
    (lambda (continue)
      (apply error continue ',args))))

;;; now ((vector-ref *error-info* 0)) will continue from the error
*/


static s7_pointer s7_error_1(s7_scheme *sc, s7_pointer type, s7_pointer info, bool exit_eval)
{
  int i;
  s7_pointer catcher;
  catcher = sc->F;

  vector_element(sc->error_info, ERROR_TYPE) = type;
  vector_element(sc->error_info, ERROR_DATA) = info;
  vector_element(sc->error_info, ERROR_CODE) = sc->cur_code;
  vector_element(sc->error_info, ERROR_CODE_LINE) = ERROR_INFO_DEFAULT;
  vector_element(sc->error_info, ERROR_CODE_FILE) = ERROR_INFO_DEFAULT;
  vector_element(sc->error_info, ERROR_ENVIRONMENT) = sc->envir;
  s7_gc_on(sc, true);  /* this is in case we were triggered from the sort function -- clumsy! */

  /* (let ((x 32)) (define (h1 a) (* a "hi")) (define (h2 b) (+ b (h1 b))) (h2 1)) */

  if (is_pair(sc->cur_code))
    {
      int line, j, top;
      line = pair_line_number(sc->cur_code);
      if ((line > 0) &&
	  (remembered_line_number(line) != 0) &&
	  (remembered_file_name(line)))
	{
	  vector_element(sc->error_info, ERROR_CODE_LINE) = s7_make_integer(sc, remembered_line_number(line));
	  vector_element(sc->error_info, ERROR_CODE_FILE) = s7_make_string(sc, remembered_file_name(line));	  
	}

      for (top = sc->stack_top, j = ERROR_ENVIRONMENT + 1; (top > 0) && (j < ERROR_INFO_SIZE); top -= 4, j++)
	vector_element(sc->error_info, j) = vector_element(sc->stack, top - 3);
      if (j < ERROR_INFO_SIZE)
	vector_element(sc->error_info, j) = ERROR_INFO_DEFAULT;
    }

  sc->cur_code = ERROR_INFO_DEFAULT;

  /* if (!s7_is_continuation(type))... */

  /* top is 1 past actual top, top - 1 is op, if op = OP_CATCH, top - 4 is the cell containing the catch struct */
  for (i = sc->stack_top - 1; i >= 3; i -= 4)
    {
      opcode_t op;
      s7_pointer x;
      op = (opcode_t)s7_integer(vector_element(sc->stack, i));
      
      switch (op)
	{
	case OP_DYNAMIC_WIND:
	  x = vector_element(sc->stack, i - 3);
	  if (dynamic_wind_state(x) == T_DWIND_BODY)
	    {
	      push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
	      sc->args = sc->NIL;
	      sc->code = dynamic_wind_out(x);
	      eval(sc, OP_APPLY);
	    }
	  break;

	case OP_CATCH:
	  x = vector_element(sc->stack, i - 3);
	  if ((type == sc->T) ||
	      (catch_tag(x) == sc->T) ||
	      (s7_is_eq(catch_tag(x), type)))
	    {
	      catcher = x;
	      goto GOT_CATCH;
	    }
	  break;

	case OP_UNWIND_OUTPUT:
	  x = vector_element(sc->stack, i - 3); /* "code" = port that we opened */
	  s7_close_output_port(sc, x);
	  x = vector_element(sc->stack, i - 1); /* "args" = port that we shadowed, if not #f */
	  if (x != sc->F)
	    sc->output_port = x;
	  break;

	case OP_UNWIND_INPUT:
	  s7_close_input_port(sc, vector_element(sc->stack, i - 3)); /* "code" = port that we opened */
	  sc->input_port = vector_element(sc->stack, i - 1); /* "args" = port that we shadowed */
	  break;

	case OP_TRACE_RETURN:
	  sc->trace_depth--;
	  break;

	default:
	  break;
	}
    }
  
GOT_CATCH:
  if (catcher != sc->F)
    {
      sc->args = make_list_2(sc, type, info);
      sc->code = catch_handler(catcher);
      sc->stack_top = catch_goto_loc(catcher);
      sc->op = OP_APPLY;

      /* explicit eval needed if s7_call called into scheme where a caught error occurred (ex6 in exs7.c)
       *  but putting it here (via eval(sc, OP_APPLY)) means the C stack is not cleared correctly in non-s7-call cases, 
       *  so defer it until s7_call 
       */
    }
  else
    {
      /* (set! *error-hook* (lambda (tag args) (apply format (cons #t args)))) */
      s7_pointer error_hook, error_hook_binding;

      error_hook_binding = s7_find_symbol_in_environment(sc, sc->envir, sc->ERROR_HOOK, true);
      if ((error_hook_binding != sc->NIL) &&
	  (is_procedure(symbol_value(error_hook_binding))))
	{
	  error_hook = symbol_value(error_hook_binding);
	  set_symbol_value(error_hook_binding, sc->NIL);
	  /* if the *error-hook* function triggers an error, we had better not have *error-hook* still set! */

	  push_stack(sc, OP_ERROR_HOOK_QUIT, sc->NIL, error_hook); /* restore *error-hook* upon successful evaluation */
	  sc->args = make_list_2(sc, type, info);
	  sc->code = error_hook;
	  sc->op = OP_APPLY;
	}
      else
	{
	  /* if info is not a list, send object->string to current error port,
	   *   else assume car(info) is a format control string, and cdr(info) are its args
	   */
	  if ((!s7_is_list(sc, info)) ||
	      (!s7_is_string(car(info))))
	    format_to_output(sc, 
			     s7_current_error_port(sc), 
			     "\n;~A ~A",
			     make_list_2(sc, type, info));
	  else
	    {
	      const char *carstr;
	      int i, len;
	      bool use_format = false;
	      
	      /* it's possible that the error string is just a string -- not intended for format */
	      carstr = s7_string(car(info));
	      len = string_length(car(info));
	      for (i = 0; i < len; i++)
		if (carstr[i] == '~')
		  {
		    use_format = true;
		    break;
		  }
	      
	      if (use_format)
		{
		  char *errstr;
		  len += 8;
		  errstr = (char *)malloc(len * sizeof(char));
		  snprintf(errstr, len, "\n;%s", s7_string(car(info)));
		  format_to_output(sc,
				   s7_current_error_port(sc), 
				   errstr,
				   cdr(info));
		  free(errstr);
		}
	      else format_to_output(sc, 
				    s7_current_error_port(sc), 
				    "\n;~A ~A",
				    make_list_2(sc, type, info));
	    }
	  
	  /* now display location and \n at end */
	  
	  if (is_input_port(sc->input_port))
	    {
	      const char *filename = NULL;
	      int line;
	      
	      filename = port_filename(sc->input_port);
	      line = port_line_number(sc->input_port);
	      
	      if (filename)
		format_to_output(sc,
				 s7_current_error_port(sc), 
				 ", ~A[~D]",
				 make_list_2(sc, s7_make_string(sc, filename), s7_make_integer(sc, line)));
	      else 
		{
		  if (line > 0)
		    format_to_output(sc,
				     s7_current_error_port(sc), 
				     ", line ~D", 
				     make_list_1(sc, s7_make_integer(sc, line)));
		}
	    }
	  s7_newline(sc, s7_current_error_port(sc));

	  if (is_pair(vector_element(sc->error_info, ERROR_CODE)))
	    {
	      format_to_output(sc, 
			       s7_current_error_port(sc), 
			       ";    ~A", 
			       make_list_1(sc, vector_element(sc->error_info, ERROR_CODE)));
	      s7_newline(sc, s7_current_error_port(sc));

	      if (s7_is_string(vector_element(sc->error_info, ERROR_CODE_FILE)))
		{
		  format_to_output(sc,
				   s7_current_error_port(sc), 
				   ";    [~S, line ~D]",
				   make_list_2(sc, 
					       vector_element(sc->error_info, ERROR_CODE_FILE), 
					       vector_element(sc->error_info, ERROR_CODE_LINE)));
		  s7_newline(sc, s7_current_error_port(sc));
		}
	    }
	  
	  if ((exit_eval) &&
	      (sc->error_exiter))
	    (*(sc->error_exiter))();

	  /* if (s7_is_continuation(type))
	   *   go into repl here with access to continuation?  Or expect *error-handler* to deal with it?
	   */
	  sc->value = type;
	  stack_reset(sc);
	  sc->op = OP_ERROR_QUIT;
	}
    }
  
  if (sc->longjmp_ok)
    {
      longjmp(sc->goto_start, 1); /* this is trying to clear the C stack back to some clean state */
    }

  return(type);
}


s7_pointer s7_error(s7_scheme *sc, s7_pointer type, s7_pointer info)
{
  return(s7_error_1(sc, type, info, false));
}


s7_pointer s7_error_and_exit(s7_scheme *sc, s7_pointer type, s7_pointer info)
{
  return(s7_error_1(sc, type, info, true));
}


static s7_pointer eval_error(s7_scheme *sc, const char *errmsg, s7_pointer obj)
{
  return(s7_error(sc, sc->ERROR, make_list_2(sc, s7_make_string(sc, errmsg), obj)));
}


static s7_pointer read_error(s7_scheme *sc, const char *errmsg)
{
  /* reader errors happen before the evaluator gets involved, so forms such as:
   *   (catch #t (lambda () (car '( . ))) (lambda arg 'error))
   * do not catch the error if we simply signal an error when we encounter it.
   * We try to encapsulate the bad input in a call on "error" --
   * some cases are working...
   */
  char *msg;
  int len;
  s7_pointer pt, result;
  pt = sc->input_port;

  if (is_string_port(sc->input_port))
    {
      #define QUOTE_SIZE 12
      int start, end, slen;
      char *recent_input = NULL;
      
      start = port_string_point(pt) - QUOTE_SIZE;
      if (start < 0) start = 0;
      end = port_string_point(pt) + QUOTE_SIZE;
      if (end > port_string_length(pt))
	end = port_string_length(pt);
      slen = end - start;

      if (slen > 0)
	{
	  int i;
	  recent_input = (char *)malloc((slen + 8) * sizeof(char));
	  for (i = 0; i < 3; i++) recent_input[i] = '.';
	  recent_input[3] = ' ';
	  for (i = 0; i < slen; i++) recent_input[i + 4] = port_string(pt)[start + i];
	  recent_input[i + 4] = ' ';
	  for (i = 0; i < 3; i++) recent_input[i + slen + 5] = '.';
	  recent_input[7 + slen] = '\0';
	}

      if (port_line_number(pt) > 0)
	{
	  len = safe_strlen(recent_input) + safe_strlen(errmsg) + safe_strlen(port_filename(pt)) + 32;
	  msg = (char *)malloc(len * sizeof(char));
	  len = snprintf(msg, len, "%s: %s %s[%d]", errmsg, (recent_input) ? recent_input : "", port_filename(pt), port_line_number(pt));
	}
      else
	{
	  len = safe_strlen(recent_input) + safe_strlen(errmsg) + 32;
	  msg = (char *)malloc(len * sizeof(char));
	  len = snprintf(msg, len, "%s: %s", errmsg, (recent_input) ? recent_input : "");
	}
      
      if (recent_input) free(recent_input);
    }
  else
    {
      len = safe_strlen(errmsg) + safe_strlen(port_filename(pt)) + 32;
      msg = (char *)malloc(len * sizeof(char));
      len = snprintf(msg, len, "%s %s[%d]", errmsg, port_filename(pt), port_line_number(pt));
    }
  
  s7_newline(sc, s7_current_error_port(sc));
  write_string(sc, msg, s7_current_error_port(sc)); /* make sure we complain ... */
  s7_newline(sc, s7_current_error_port(sc));

  result = make_list_3(sc, 
		       s7_symbol_value(sc, sc->ERROR), 
		       sc->ERROR, 
		       s7_make_string_with_length(sc, msg, len));

  free(msg);
  return(result);
}


static s7_pointer g_error(s7_scheme *sc, s7_pointer args)
{
  #define H_error "(error type ...) signals an error"
  if (args != sc->NIL)
    {
      if (s7_is_string(car(args)))                    /* CL-style error? -- use tag = 'no-catch */
	{
	  s7_error(sc, s7_make_symbol(sc, "no-catch"), args);
	  return(sc->UNSPECIFIED);
	}
      return(s7_error(sc, car(args), cdr(args)));
    }
  return(s7_error(sc, sc->NIL, sc->NIL));
}


static s7_pointer missing_close_paren_error(s7_scheme *sc)
{
  s7_pointer x;
  int line;
  x = sc->args;
  while (is_pair(cdr(x))) x = cdr(x);
  line = pair_line_number(x);
  if (line > 0)
    {
      s7_pointer result;
      char *str1;
      int len;
      len = 64 + safe_strlen(port_filename(sc->input_port)); 
      str1 = (char *)malloc(len * sizeof(char));
      len = snprintf(str1, len, "missing close paren, list started around line %d of %s: ~A", 
		     remembered_line_number(line), port_filename(sc->input_port));
      result = s7_make_string_with_length(sc, str1, len);
      free(str1);
      return(s7_error(sc, sc->ERROR, make_list_2(sc, result, safe_reverse_in_place(sc, sc->args))));
    }
  return(read_error(sc, "missing close paren"));
}


static void display_frame(s7_scheme *sc, s7_pointer envir)
{
  if ((is_pair(envir)) &&
      (is_pair(cdr(envir))))
    {
      s7_pointer args, op;
      args = car(envir);
      op = cadr(envir);
      if ((is_pair(op)) &&
	  (is_pair(car(op))) &&
	  (caar(op) == sc->__FUNC__))
	{
	  s7_pointer lst, sym, proc;
	  lst = car(op);
	  if (s7_is_symbol(cdr(lst)))
	    sym = cdr(lst);
	  else sym = cadr(lst);
	  proc = s7_symbol_local_value(sc, sym, envir);
	  if (is_procedure(proc))
	    {
	      s7_pointer local_env, file_info = sc->F;
	      local_env = s7_reverse(sc, args);
	      if (!s7_is_symbol(cdr(lst)))
		file_info = cddr(lst);
	      
	      format_to_output(sc, s7_current_output_port(sc), "(~A~{ ~A~})", make_list_2(sc, sym, local_env));
	      if (is_pair(file_info))
		format_to_output(sc, s7_current_output_port(sc), "    [~S ~D]", file_info);
	    }
          else
	    format_to_output(sc, s7_current_output_port(sc), "(~A~{~^ ~A~})", make_list_2(sc, cdr(lst), s7_reverse(sc, args)));
          s7_newline(sc, s7_current_output_port(sc));
	}
    }
}


static s7_pointer g_stacktrace(s7_scheme *sc, s7_pointer args)
{
  /* 4 cases currently: 
   *    if args=nil, show current stack
   *           =thread obj, show its stack
   *           =vector, assume it is a vector of envs from *error-info*
   *           =continuation, show its stack
   */
  #define H_stacktrace "(stacktrace :optional obj) displays a stacktrace.  If obj is not \
given, the current stack is displayed, if obj is a thread object, its stack is displayed, \
if obj is *error-info*, the stack at the point of the error is displayed, and if obj \
is a continuation, its stack is displayed."

  int i, top = 0;
  s7_pointer stk = sc->F;

  /* *error-info* is the special case here */
  if (s7_is_vector(car(args)))
    {
      for (i = ERROR_ENVIRONMENT; i < ERROR_INFO_SIZE; i++)
	{
	  if (vector_element(car(args), i) == ERROR_INFO_DEFAULT)
	    break;
	  display_frame(sc, vector_element(car(args), i));
	}
      return(sc->UNSPECIFIED);
    }

  if (args == sc->NIL)
    {
      top = sc->stack_top;
      stk = sc->stack;
    }
  else
    {
      if (s7_is_continuation(car(args)))
	{
	  top = continuation_stack_top(car(args));
	  stk = continuation_stack(car(args));
	}
#if HAVE_PTHREADS
      else
	{
	  if (g_is_thread(sc, args) != sc->F)
	    {
	      thred *f;
	      f = (thred *)s7_object_value(car(args));
	      top = f->sc->stack_top;
	      stk = f->sc->stack;
	    }
	}
#endif	    
    }
  if (stk == sc->F)
    return(s7_wrong_type_arg_error(sc, "stacktrace", 0, car(args), "a vector, thread object, or continuation"));
  
  for (i = top; i > 0; i -= 4)
    display_frame(sc, vector_element(stk, i - 3));

  return(sc->UNSPECIFIED);
}


s7_pointer s7_stacktrace(s7_scheme *sc, s7_pointer arg)
{
  if (arg == sc->NIL)
    return(g_stacktrace(sc, arg));
  return(g_stacktrace(sc, make_list_1(sc, arg)));
}



/* -------------------------------- leftovers -------------------------------- */

static s7_pointer g_quit(s7_scheme *sc, s7_pointer args)
{
  #define H_quit "(quit) returns from the evaluator"

  push_stack(sc, OP_QUIT, sc->NIL, sc->NIL);
  return(sc->NIL);
}


void s7_quit(s7_scheme *sc)
{
  stack_reset(sc);
  push_stack(sc, OP_QUIT, sc->NIL, sc->NIL);
}


static s7_pointer apply_list_star(s7_scheme *sc, s7_pointer d) 
{
  s7_pointer p, q;
  if (cdr(d) == sc->NIL) 
    return(car(d));
  
  p = s7_cons(sc, car(d), cdr(d));
  q = p;
  while (cdr(cdr(p)) != sc->NIL) 
    {
      d = s7_cons(sc, car(p), cdr(p));
      if (cdr(cdr(p)) != sc->NIL) 
	p = cdr(d);
    }
  cdr(p) = car(cdr(p));
  return(q);
}


static s7_pointer g_apply(s7_scheme *sc, s7_pointer args)
{
  #define H_apply "(apply func ...) applies func to the rest of the arguments"
  sc->code = car(args);
  if (cdr(args) == sc->NIL)
    sc->args = sc->NIL;
  else 
    {
      sc->args = apply_list_star(sc, cdr(args));

      if (!is_proper_list(sc, sc->args))        /* (apply + #f) etc */
	return(s7_error(sc, sc->WRONG_TYPE_ARG, s7_make_string(sc, "apply's last argument should be a list")));
    }
  push_stack(sc, OP_APPLY, sc->args, sc->code);
  return(sc->NIL);
}


static s7_pointer g_eval(s7_scheme *sc, s7_pointer args)
{
  #define H_eval "(eval code :optional env) evaluates code in the environment env. 'env' \
defaults to the current environment; to evaluate something in the top-level environment instead, \
pass (global-environment):\n\
\n\
  (define x 32) \n\
  (let ((x 3))\n\
    (eval 'x (global-environment)))\n\
\n\
  returns 32"
  
  if (cdr(args) != sc->NIL)
    {
      if (!is_pair(cadr(args)))
	return(s7_wrong_type_arg_error(sc, "eval", 2, cadr(args), "an environment"));
      sc->envir = cadr(args);
    }
  sc->code = car(args);
  push_stack(sc, OP_EVAL, sc->args, sc->code);
  return(sc->NIL);
}


s7_pointer s7_call(s7_scheme *sc, s7_pointer func, s7_pointer args)
{ 
  bool old_longjmp;
  jmp_buf old_goto_start;

  /* this can be called while we are in the eval loop (within eval_c_string for instance),
   *   and if we reset the stack, the previously running evaluation steps off the end
   *   of the stack == segfault. 
   */
  old_longjmp = sc->longjmp_ok;
  memcpy((void *)old_goto_start, (void *)(sc->goto_start), sizeof(jmp_buf));

  /* if an error occurs during s7_call, and it is caught, catch (above) wants to longjmp
   *   to its caller to complete the error handler evaluation so that the C stack is
   *   cleaned up -- this means we have to have the longjmp location set here, but
   *   we could get here from anywhere, so we need to save and restore the incoming
   *   longjmp location.
   */

  sc->longjmp_ok = true;
  if (setjmp(sc->goto_start) != 0) /* returning from s7_error catch handler */
    {
      sc->longjmp_ok = old_longjmp;
      memcpy((void *)(sc->goto_start), (void *)old_goto_start, sizeof(jmp_buf));
      
      if ((sc->op == OP_ERROR_QUIT) &&
	  (sc->longjmp_ok))
	{
	  longjmp(sc->goto_start, 1); /* this is trying to clear the C stack back to some clean state */
	}

      eval(sc, sc->op);
      return(sc->value);
    }

  push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); /* this saves the current evaluation and will eventually finish this (possibly) nested call */
  sc->args = args; 
  sc->code = func; 
  eval(sc, OP_APPLY);

  sc->longjmp_ok = old_longjmp;
  memcpy((void *)(sc->goto_start), (void *)old_goto_start, sizeof(jmp_buf));
  return(sc->value);
} 


static s7_pointer g_s7_version(s7_scheme *sc, s7_pointer args)
{
  #define H_s7_version "(s7-version) returns some string describing the current S7"
  return(s7_make_string(sc, "s7 " S7_VERSION ", " S7_DATE));
}


static s7_pointer g_list_for_each(s7_scheme *sc, s7_pointer args)
{
  #define H_list_for_each "(for-each proc lst . lists) applies proc to each element of the lists traversed in parallel"
  s7_pointer lists, y;
  int i;

  if ((!is_procedure(car(args))) ||
      (is_thunk(sc, car(args))))
    return(s7_wrong_type_arg_error(sc, "for-each", 1, car(args), "a thunk"));

  sc->code = car(args);
  lists = cdr(args);
  if (car(lists) == sc->NIL)
    {
      /* if any non-nil lists (or not-lists) follow, there's an error even in this no-op */
      for (i = 2, y = lists; y != sc->NIL; i++, y = cdr(y))
	if (car(y) != sc->NIL)
	  return(s7_wrong_type_arg_error(sc, "for-each", i, car(y), "a nil list (first list was nil)"));
      
    return(sc->NIL); /* not a bug -- (for-each (lambda (x) x) '()) -> no-op */
    }

  sc->x = sc->NIL;
  
  /* get car of each arg list making the current proc arglist */
  sc->args = sc->NIL;
  
  for (i = 2, y = lists; y != sc->NIL; i++, y = cdr(y))
    {
      if (!is_proper_list(sc, car(y)))
	return(s7_wrong_type_arg_error(sc, "for-each", i, car(y), "a list"));
      
      sc->args = s7_cons(sc, caar(y), sc->args);
      sc->x = s7_cons(sc, cdar(y), sc->x);
    }
  sc->args = safe_reverse_in_place(sc, sc->args);
  sc->x = safe_reverse_in_place(sc, sc->x);
  
  /* if lists have no cdr (just 1 set of args), apply the proc to them */
  if (car(sc->x) == sc->NIL)
    {
      for (y = cdr(sc->x); y != sc->NIL; y = cdr(y))
	if (car(y) != sc->NIL)
	  return(eval_error(sc, "for-each args are not the same length", sc->x));
      push_stack(sc, OP_APPLY, sc->args, sc->code);
      return(sc->NIL);
    }
  
  /* set up for repeated call walking down the lists of args */
  push_stack(sc, OP_LIST_FOR_EACH, sc->x, sc->code);
  push_stack(sc, OP_APPLY, sc->args, sc->code);
  return(sc->NIL);
}


static s7_pointer g_list_map(s7_scheme *sc, s7_pointer args)
{
  #define H_list_map "(map proc lst . lists) applies proc to a list made up of the car of each arg list, returning a list of the values returned by proc"
  s7_pointer lists, y;
  int i;
  
  if ((!is_procedure(car(args))) ||
      (is_thunk(sc, car(args))))
    return(s7_wrong_type_arg_error(sc, "map", 1, car(args), "a thunk"));

  sc->code = car(args);
  lists = cdr(args);
  if (car(lists) == sc->NIL)
    {
      /* if any non-nil lists (or not-lists) follow, there's an error even in this no-op */
      for (i = 2, y = lists; y != sc->NIL; i++, y = cdr(y))
	if (car(y) != sc->NIL)
	  return(s7_wrong_type_arg_error(sc, "map", i, car(y), "a nil list (first list was nil)"));
      
    return(sc->NIL);
    }

  sc->x = sc->NIL;
  
  /* get car of each arg list making the current proc arglist */
  sc->args = sc->NIL;
  for (i = 2, y = lists; y != sc->NIL; i++, y = cdr(y))
    {
      if (!is_proper_list(sc, car(y)))
	return(s7_wrong_type_arg_error(sc, "map", i, car(y), "a list"));
      
      sc->args = s7_cons(sc, caar(y), sc->args);
      /* car(y) = cdar(y); */ /* this clobbers the original lists -- we need to copy */
      sc->x = s7_cons(sc, cdar(y), sc->x);
    }
  sc->args = safe_reverse_in_place(sc, sc->args);
  sc->x = safe_reverse_in_place(sc, sc->x);
  
  /* set up for repeated call walking down the lists of args, values list is cdr, current args is car */
  push_stack(sc, OP_LIST_MAP, make_list_1(sc, sc->x), sc->code);
  push_stack(sc, OP_APPLY, sc->args, sc->code);
  return(sc->NIL);
}


static s7_pointer g_for_each(s7_scheme *sc, s7_pointer args)
{
  s7_pointer fargs;
  fargs = cdr(args);

  if ((fargs == sc->NIL) || (s7_is_list(sc, car(fargs))))
    return(g_list_for_each(sc, args));

  if (s7_is_vector(car(fargs)))
    return(g_vector_for_each(sc, args));

  if (s7_is_hash_table(car(fargs)))
    return(g_hash_table_for_each(sc, args));

  if (s7_is_string(car(fargs)))
    return(g_string_for_each(sc, args));

  if (s7_is_applicable_object(car(fargs)))
    return(g_object_for_each(sc, args));
  
  return(s7_wrong_type_arg_error(sc, "for-each", 2, car(fargs), "an applicable object"));
}


static s7_pointer g_map(s7_scheme *sc, s7_pointer args)
{
  s7_pointer fargs;
  fargs = cdr(args);
  if ((fargs == sc->NIL) || (s7_is_list(sc, car(fargs))))
    return(g_list_map(sc, args));
  if (s7_is_vector(car(fargs)))
    return(g_vector_map(sc, args));
  
  return(s7_wrong_type_arg_error(sc, "map", 2, car(fargs), "a list or a vector"));
}



/* -------------------------------- multiple-values -------------------------------- */

static s7_pointer splice_in_values(s7_scheme *sc, s7_pointer args)
{
  if (sc->stack_top > 0)
    {
      /* code = args yet to eval in order, args = evalled args reversed */
      s7_pointer *vel;
      int top;
      top = sc->stack_top - 4;
      vel = (s7_pointer *)(vector_elements(sc->stack) + top);
      /*
       * code = vel[0]
       * args = vel[2]
       * op =   (opcode_t)integer(number(vel[3]))
      */
      if ((opcode_t)integer(number(vel[3])) == OP_EVAL_ARGS1)
	{
	  s7_pointer x;
	  /* splice into the caller's arg list, leaving the last for the eval args loop to handle */
	  for (x = args; cdr(x) != sc->NIL; x = cdr(x))
	    vel[2] = s7_cons(sc, car(x), vel[2]);
	  return(car(x));
	}
    }

  /* let it meander back up the call chain until someone knows where to splice it */
  return(s7_cons(sc, sc->VALUES, args));
}


static s7_pointer g_values(s7_scheme *sc, s7_pointer args)
{
  #define H_values "(values obj ...) splices its arguments into whatever list holds it (its 'continuation')"
  
  if (args == sc->NIL)
    return(sc->NIL);
  
  if (cdr(args) == sc->NIL)
    return(car(args));

  return(splice_in_values(sc, args));
}


s7_pointer s7_values(s7_scheme *sc, int num_values, ...)
{
  s7_pointer args = sc->NIL;
  int i;
  va_list ap;
  
  if (num_values == 0)
    return(sc->NIL);

  va_start(ap, num_values);
  for (i = 0; i < num_values; i++)
    args = s7_cons(sc, va_arg(ap, s7_pointer), args);
  va_end(ap);

  return(g_values(sc, s7_reverse(sc, args)));
}


/* -------------------------------- quasiquote -------------------------------- */

static s7_pointer g_quasiquote_1(s7_scheme *sc, int level, s7_pointer form)
{
  if (!is_pair(form))
    {
      if ((s7_is_number(form)) ||
	  (s7_is_string(form)) ||
	  (s7_is_procedure(form)))
	return(form);
      return(make_list_2(sc, sc->QUOTE, form));
    }
  /* from here, form is a pair */

  if (car(form) == sc->QUASIQUOTE)
    {
      s7_pointer r;
      r = g_quasiquote_1(sc, level + 1, cdr(form));
      if ((is_pair(r)) &&
	  (car(r) == sc->QUOTE) &&
	  (car(cdr(r)) == cdr(form)))
	return(make_list_2(sc, sc->QUOTE, form));
      return(make_list_3(sc, sc->CONS, make_list_2(sc, sc->QUOTE, sc->QUASIQUOTE), r));
    }

  if (level == 0)
    {
      if (car(form) == sc->UNQUOTE)
	return(car(cdr(form)));
	      
      if (car(form) == sc->UNQUOTE_SPLICING)
	return(form);
	      
      if ((is_pair(car(form))) &&
	  (caar(form) == sc->UNQUOTE_SPLICING))
	{
	  s7_pointer l, r;
	  l = car(cdr(car(form)));
	  if (cdr(form) == sc->NIL)
	    return(l);

	  r = g_quasiquote_1(sc, level, cdr(form));

	  if ((is_pair(r)) &&
	      (car(r) == sc->QUOTE) &&
	      (car(cdr(r)) == sc->NIL))
	    return(l);
	  return(make_list_3(sc, sc->APPEND, l, r));
	}
      
      goto MCONS;
    }

  /* level != 0 */
  if (car(form) == sc->UNQUOTE)
    {
      s7_pointer r;
      r = g_quasiquote_1(sc, level - 1, cdr(form));

      if ((is_pair(r)) &&
	  (car(r) == sc->QUOTE) &&
	  (car(cdr(r)) == cdr(form)))
	return(make_list_2(sc, sc->QUOTE, form));

      return(make_list_3(sc, sc->CONS, make_list_2(sc, sc->QUOTE, sc->UNQUOTE), r));
    }
  
  if (car(form) == sc->UNQUOTE_SPLICING)
    {
      s7_pointer r;
      r = g_quasiquote_1(sc, level - 1, cdr(form));

      if ((is_pair(r)) &&
	  (car(r) == sc->QUOTE) &&
	  (car(cdr(r)) == cdr(form)))
	return(make_list_2(sc, sc->QUOTE, form));

      return(make_list_3(sc, sc->CONS, make_list_2(sc, sc->QUOTE, sc->UNQUOTE_SPLICING), r));
    }
	      
 MCONS:
  {
    s7_pointer l, r;

    l = g_quasiquote_1(sc, level, car(form));
    r = g_quasiquote_1(sc, level, cdr(form));

    if ((is_pair(r)) &&
	(is_pair(l)) &&
	(car(r) == sc->QUOTE) &&
	(car(l) == car(r)) &&
	(car(cdr(r)) == cdr(form)) &&
	(car(cdr(l)) == car(form)))
      return(make_list_2(sc, sc->QUOTE, form));

    if (l == sc->VECTOR_FUNCTION)
      return(g_vector(sc, make_list_1(sc, r))); /* eval? */

    return(make_list_3(sc, sc->CONS, l, r));
  }
}

static s7_pointer g_quasiquote_2(s7_scheme *sc, int level, s7_pointer form)
{
  /* the lists built up by quasiquote can be arbitrarily large, and it would be a nightmare to locally GC-protect,
   *   then later unprotect every cons, so we turn off the GC until we're done.
   */
  s7_pointer x;
  if (sc->free_heap_top < 4096) gc(sc);
  s7_gc_on(sc, false);
  x = g_quasiquote_1(sc, level, form);
  s7_gc_on(sc, true);
  return(x);
}

static s7_pointer g_quasiquote(s7_scheme *sc, s7_pointer args)
{
  return(g_quasiquote_2(sc, s7_integer(car(args)), cadr(args)));
}  



/* ---------------- reader funcs for eval ---------------- */

static void back_up_stack(s7_scheme *sc)
{
  opcode_t top_op;
  top_op =(opcode_t)s7_integer(vector_element(sc->stack, sc->stack_top - 1));
  if (top_op == OP_READ_DOT)
    {
      pop_stack(sc);
      top_op =(opcode_t)s7_integer(vector_element(sc->stack, sc->stack_top - 1));
    }
  if (top_op == OP_READ_VECTOR)
    {
      pop_stack(sc);
      top_op =(opcode_t)s7_integer(vector_element(sc->stack, sc->stack_top - 1));
    }
  if (top_op == OP_READ_QUOTE)
    pop_stack(sc);

  if (top_op == OP_EVAL_STRING) /* ?? */
    pop_stack(sc);
}


static token_t token(s7_scheme *sc)
{
  int c = 0;
  s7_pointer pt;
  pt = sc->input_port;

  if (is_file_port(pt))
    {
      while (isspace(c = fgetc(port_file(pt))))
	if (c == '\n')
	  port_line_number(pt)++;
    }
  else 
    {
      char *orig_str, *str;
      str = (char *)(port_string(pt) + port_string_point(pt));
      orig_str = str;
      while (isspace(c = (*str++)))
	if (c == '\n')
	  port_line_number(pt)++;
      port_string_point(pt) += (str - orig_str);
    }

  switch (c) 
    {
    case EOF:
    case 0:
      return(TOKEN_EOF);
      
    case '(':
      return(TOKEN_LEFT_PAREN);
      
    case ')':
      return(TOKEN_RIGHT_PAREN);
      
    case '.':
      c = inchar(sc, pt);
      if (dot_table[c])
	return(TOKEN_DOT);

      backchar(sc, c, pt);
      sc->strbuf[0] = '.'; /* see below */
      return(TOKEN_ATOM); /* assuming a number eventually? or can symbol names start with "."?
			   * in either case, we'd want alphanumeric here -- " .(" for example
			   * is probably an error in this scheme since we don't support the
			   * quasiquote dot business.  I think I'll flag (){}[]#'\.  I can't
			   * leave this for make_atom to detect because by then the delimiter
			   * has been lost (i.e. ".)" gets to make_atom as "." which is still
			   * a bad symbol name, but make_atom is also called by number->string
			   * which wants to return #f silently in such a case.
			   */
    case '\'':
      return(TOKEN_QUOTE);
      
    case ';':
      {
	int c;
	if (is_file_port(pt))
	  do (c = fgetc(port_file(pt))); while ((c != '\n') && (c != EOF));
	else do (c = port_string(pt)[port_string_point(pt)++]); while ((c != '\n') && (c != 0));
	port_line_number(pt)++;
	if (c == 0)
	  return(TOKEN_EOF);
	return(token(sc));
      }

    case '"':
      return(TOKEN_DOUBLE_QUOTE);
      
    case '`':
      return(TOKEN_BACK_QUOTE);
      
    case ',':

      /* here we probably should check for symbol names that start with "@":

	 :(defmacro hi (@foo) `(+ ,@foo 1))
	 hi
	 :(hi 2)
	 ;foo: unbound variable
	 
	 but

	 :(defmacro hi (.foo) `(+ ,.foo 1))
	 hi
	 :(hi 2)
	 3

	 and ambiguous:
	 :(defmacro hi (@foo foo) `(+ ,@foo 1))
      */

      if ((c = inchar(sc, pt)) == '@') 
	return(TOKEN_AT_MARK);
      
      backchar(sc, c, pt);
      return(TOKEN_COMMA);
      
    case '#':
      c = inchar(sc, pt);
      if (c == '(') 
	return(TOKEN_VECTOR);

      if (c == ':')  /* turn #: into : */
	{
	  sc->strbuf[0] = ':';
	  return(TOKEN_ATOM);
	}

      /* block comments in either #! ... !# */
      if (c == '!') 
	{
	  char last_char;
	  last_char = ' ';
	  while ((c = inchar(sc, pt)) != EOF)
	    {
	      if ((c == '#') &&
		  (last_char == '!'))
		break;
	      last_char = c;
	    }
	  return(token(sc));
	}
      
      /*   or #| ... |# */
      if (c == '|') 
	{
	  char last_char;
	  last_char = ' ';
	  while ((c = inchar(sc, pt)) != EOF)
	    {
	      if ((c == '#') &&
		  (last_char == '|'))
		break;
	      last_char = c;
	    }
	  return(token(sc));
	}
      
      backchar(sc, c, pt);
      return(TOKEN_SHARP_CONST); /* next stage notices any errors */
      
    default: 
      sc->strbuf[0] = c; /* every TOKEN_ATOM return goes to read_string_upto, so we save a backchar/inchar shuffle by starting the read here */
      return(TOKEN_ATOM);
    }
}


static void resize_strbuf(s7_scheme *sc)
{
  int i, old_size;
  old_size = sc->strbuf_size;
  sc->strbuf_size *= 2;
  sc->strbuf = (char *)realloc(sc->strbuf, sc->strbuf_size * sizeof(char));
  for (i = old_size; i < sc->strbuf_size; i++) sc->strbuf[i] = '\0';
}


static char *read_string_upto(s7_scheme *sc, int start) 
{
  int i, c;
  s7_pointer pt;
  pt = sc->input_port;
  i = start;

  if (is_file_port(pt))
    {
      do
	{
	  c = fgetc(port_file(pt)); /* might return EOF */
	  if (c == '\n')
	    port_line_number(pt)++;

	  sc->strbuf[i++] = c;
	  if (i >= sc->strbuf_size)
	    resize_strbuf(sc);
	}
      while ((c != EOF) && (string_delimiter_table[c]));

      if ((i == 2) && 
	  (sc->strbuf[0] == '\\'))
	sc->strbuf[i] = '\0';
      else 
	{
	  if (c != EOF)
	    {
	      if (c == '\n')
		port_line_number(pt)--;
	      ungetc(c, port_file(pt));
	    }
	  sc->strbuf[i - 1] = '\0';
	}
    }
  else
    {
      if (port_string(pt))
	{
	  int k = 0;
	  char *orig_str, *str;

	  str = (char *)(port_string(pt) + port_string_point(pt));
	  orig_str = str;
	  do {c = (int)(*str++);} while (string_delimiter_table[c]);

	  k = str - orig_str;
	  port_string_point(pt) += k;
	  i += k;
	  if (i >= sc->strbuf_size)
	    resize_strbuf(sc);

	  memcpy((void *)(sc->strbuf + start), (void *)orig_str, k);

	  if ((i == 2) && 
	      (sc->strbuf[0] == '\\'))
	    sc->strbuf[i] = '\0';
	  else 
	    {
	      if (c != 0)
		{
		  port_string_point(pt)--;
		  sc->strbuf[i - 1] = '\0';
		}
	    }
	}
      else sc->strbuf[0] = '\0';
    }
  return(sc->strbuf);
}


static s7_pointer read_string_constant(s7_scheme *sc, s7_pointer pt)
{
  /* sc->F => error */
  int i, c;
  for (i = 0; ; ) 
    {
      c = inchar(sc, pt);
      if (c == EOF)
	return(sc->F);

      if (c == '"')
	{
	  sc->strbuf[i] = '\0';
	  return(s7_set_immutable(s7_make_string_with_length(sc, sc->strbuf, i))); /* string constant can't be target of string-set! */
	}
      else
	{
	  if (c == '\\')
	    {
	      c = inchar(sc, pt);
	      if (c == '\\')
		sc->strbuf[i++] = '\\';
	      else
		{
		  if (c == '"')
		    sc->strbuf[i++] = '"';
		  else
		    {
		      if (c == 'n')
			sc->strbuf[i++] = '\n';
		      else 
			{
			  if (!isspace(c))
			    return(sc->T); /* #f here would give confusing error message "end of input", so return #t=bad backslash */
			}
		    }
		}
	    }
	  else sc->strbuf[i++] = c;
	}
      if (i >= sc->strbuf_size)
	resize_strbuf(sc);
    }
}


static s7_pointer read_expression(s7_scheme *sc)
{
  while (true) 
    {
      int c;
      switch (sc->tok) 
	{
	case TOKEN_EOF:
	  return(sc->EOF_OBJECT);
	  
	case TOKEN_VECTOR:  /* already read #( */
	  push_stack(sc, OP_READ_VECTOR, sc->NIL, sc->NIL);
	  /* fall through */
	  
	case TOKEN_LEFT_PAREN:
	  sc->tok = token(sc);

	  if (sc->tok == TOKEN_RIGHT_PAREN)
	    return(sc->NIL);

	  if (sc->tok == TOKEN_DOT) 
	    {
	      back_up_stack(sc);
	      do {c = inchar(sc, sc->input_port);} while ((c != ')') && (c != EOF));
	      return(read_error(sc, "stray dot after '('?"));         /* (car '( . )) */
	    }

	  if (sc->tok == TOKEN_EOF)
	    return(missing_close_paren_error(sc));

	  push_stack(sc, OP_READ_LIST, sc->NIL, sc->NIL);
	  break;
	  
	case TOKEN_QUOTE:
	  push_stack(sc, OP_READ_QUOTE, sc->NIL, sc->NIL);
	  sc->tok = token(sc);
	  break;
	  
	case TOKEN_BACK_QUOTE:
	  sc->tok = token(sc);
	  if (sc->tok == TOKEN_VECTOR) 
	    {
	      push_stack(sc, OP_READ_QUASIQUOTE_VECTOR, sc->NIL, sc->NIL);
	      sc->tok= TOKEN_LEFT_PAREN;
	    } 
	  else push_stack(sc, OP_READ_QUASIQUOTE, sc->NIL, sc->NIL);
	  break;
	  
	case TOKEN_COMMA:
	  push_stack(sc, OP_READ_UNQUOTE, sc->NIL, sc->NIL);
	  sc->tok = token(sc);
	  break;
	  
	case TOKEN_AT_MARK:
	  push_stack(sc, OP_READ_UNQUOTE_SPLICING, sc->NIL, sc->NIL);
	  sc->tok = token(sc);
	  break;
	  
	case TOKEN_ATOM:
	  return(make_atom(sc, read_string_upto(sc, 1), 10, true)); /* true = not just string->number */
	  /* If reading list (from lparen), this will finally get us to op_read_list */
	  
	case TOKEN_DOUBLE_QUOTE:
	  sc->value = read_string_constant(sc, sc->input_port);
	  if (sc->value == sc->F)                                /* can happen if input code ends in the middle of a string */
	    return(read_error(sc, "end of input encountered while in a string"));
	  if (sc->value == sc->T)
	    return(read_error(sc, "unknown backslash usage"));
	  return(sc->value);
	  
	case TOKEN_SHARP_CONST:
	  {
	    char *expr;
	    expr = read_string_upto(sc, 0);
	    sc->value = make_sharp_constant(sc, expr, true);
	    if (sc->value == sc->NIL)
	      return(read_error(sc, "undefined sharp expression")); /* (list #b) */
	    return(sc->value);
	  }

	case TOKEN_DOT: /* (catch #t (lambda () (+ 1 . . )) (lambda args 'hiho)) */
	  back_up_stack(sc);
	  do {c = inchar(sc, sc->input_port);} while ((c != ')') && (c != EOF));
	  return(read_error(sc, "stray dot in list?"));             /* (+ 1 . . ) */

	case TOKEN_RIGHT_PAREN: /* (catch #t (lambda () '(1 2 . )) (lambda args 'hiho)) */
	  back_up_stack(sc);
	  return(read_error(sc, "unexpected close paren"));         /* (+ 1 2)) or (+ 1 . ) */

	}
    }

  /* we never get here */
  return(sc->NIL);
}




static s7_pointer eval_symbol(s7_scheme *sc, s7_pointer sym)
{
  s7_pointer x;

  x = s7_find_symbol_in_environment(sc, sc->envir, sym, true);

  if (x != sc->NIL) 
    return(symbol_value(x));

  if (s7_is_keyword(sym))
    return(sym);
	  
  x = symbol_table_find_by_name(sc, symbol_name(sym), symbol_location(sym));
  if (is_syntax(x))
    return(x);

  if (sym == sc->UNQUOTE)
    return(eval_error(sc, "unquote (',') occurred outside quasiquote", sc->NIL));
  if (sym == sc->UNQUOTE_SPLICING)
    return(eval_error(sc, "unquote-splicing (',@') occurred outside quasiquote", sc->NIL));

  return(eval_error(sc, "~A: unbound variable", sym));
}


static void assign_syntax(s7_scheme *sc, const char *name, opcode_t op) 
{
  s7_pointer x;
  x = symbol_table_add_by_name(sc, name); 
  typeflag(x) |= (T_SYNTAX | T_DONT_COPY); 
  clear_finalizable(x);
  syntax_opcode(x) = (int)op;
}




/* -------------------------------- eval -------------------------------- */

/* all explicit write-* in eval assume current-output-port -- error fallback handling, etc */
/*   internal reads assume sc->input_port is the input port */

static s7_pointer eval(s7_scheme *sc, opcode_t first_op) 
{
  sc->cur_code = ERROR_INFO_DEFAULT;
  sc->op = first_op;
  
  /* this procedure can be entered recursively (via s7_call for example), so it's no place for a setjmp
   *   I don't think the recursion can hurt our continuations because s7_call is coming from hooks and
   *   callbacks that are implicit in our stack.
   */
  
 START:

  switch (sc->op) 
    {
      /* in gcc, this becomes a jump table, so we're not doing a linear search (gcc s7.c -S -I.) */
    case OP_READ_INTERNAL:
      sc->tok = token(sc);

      switch (sc->tok)
	{
	case TOKEN_EOF:
	  pop_stack(sc);
	  goto START;

	case TOKEN_RIGHT_PAREN:
	  return(read_error(sc, "unexpected close paren"));

	case TOKEN_COMMA:
	  return(read_error(sc, "unexpected comma"));

	default:
	  sc->value = read_expression(sc);
	  pop_stack(sc);
	  goto START;
	}

      
      /* g_read(p) from C 
       *   read one expr, return it, let caller deal with input port setup 
       */
    case OP_READ_RETURN_EXPRESSION:
      return(sc->F);
      
      
      /* (read p) from scheme
       *    "p" becomes current input port for eval's duration, then pops back before returning value into calling expr
       */
    case OP_READ_POP_AND_RETURN_EXPRESSION:
      pop_input_port(sc);
      
      if (sc->tok == TOKEN_EOF)
	{
	  sc->value = sc->EOF_OBJECT;
	  pop_stack(sc);
	  goto START;
	}
      pop_stack(sc);
      goto START;
      
      
      /* load("file"); from C (g_load) -- assume caller will clean up
       *   read and evaluate exprs until EOF that matches (stack reflects nesting)
       */
    case OP_LOAD_RETURN_IF_EOF:  /* loop here until eof (via push stack below) */
      if (sc->tok != TOKEN_EOF)
	{
	  push_stack(sc, OP_LOAD_RETURN_IF_EOF, sc->NIL, sc->NIL);
	  push_stack(sc, OP_READ_INTERNAL, sc->NIL, sc->NIL);
	  sc->code = sc->value;
	  goto EVAL;             /* we read an expression, now evaluate it, and return to read the next */
	}
      return(sc->F);
      
      
      /* (load "file") in scheme 
       *    read and evaluate all exprs, then upon EOF, close current and pop input port stack
       */
    case OP_LOAD_CLOSE_AND_POP_IF_EOF:
      if (sc->tok != TOKEN_EOF)
	{
	  push_stack(sc, OP_LOAD_CLOSE_AND_POP_IF_EOF, sc->NIL, sc->NIL); /* was push args, code */
	  push_stack(sc, OP_READ_INTERNAL, sc->NIL, sc->NIL);
	  sc->code = sc->value;
	  goto EVAL;             /* we read an expression, now evaluate it, and return to read the next */
	}
      s7_close_input_port(sc, sc->input_port);
      pop_input_port(sc);
      pop_stack(sc);
      goto START;
      
      
      /* read and evaluate string expression(s?)
       *    assume caller (C via g_eval_c_string) is dealing with the string port
       */
    case OP_EVAL_STRING:
      /* (eval-string (string-append "(list 1 2 3)" (string #\newline) (string #\newline))) 
       *    needs to be sure to get rid of the trailing white space before checking for EOF
       *    else it tries to eval twice and gets "attempt to apply 1?, line 2"
       */
      if ((sc->tok != TOKEN_EOF) && 
	  (port_string_point(sc->input_port) < port_string_length(sc->input_port))) /* ran past end somehow? */
	{
	  int c = 0;
	  while (isspace(c = port_string(sc->input_port)[port_string_point(sc->input_port)++]))
	    if (c == '\n')
	      port_line_number(sc->input_port)++;

	  if ((c != EOF) && (c != 0))
	    {
	      backchar(sc, c, sc->input_port);
	      push_stack(sc, OP_EVAL_STRING, sc->NIL, sc->value);
	      push_stack(sc, OP_READ_INTERNAL, sc->NIL, sc->NIL);
	    }
	  else push_stack(sc, OP_EVAL_STRING_DONE, sc->NIL, sc->value);
	}
      else push_stack(sc, OP_EVAL_STRING_DONE, sc->NIL, sc->value);
      sc->code = sc->value;
      goto EVAL;
      
      
    case OP_EVAL_DONE:
    case OP_EVAL_STRING_DONE:
     return(sc->F);



    case OP_LIST_FOR_EACH:
      sc->x = sc->args; /* save lists */
      sc->args = sc->NIL;
      for (sc->y = sc->x; sc->y != sc->NIL; sc->y = cdr(sc->y))
	{
	  if (car(sc->y) == sc->NIL)
	    return(eval_error(sc, "for-each args are not the same length", sc->x));

	  sc->args = s7_cons(sc, caar(sc->y), sc->args);
	  car(sc->y) = cdar(sc->y);                           /* cdr this arg list */
	}
      sc->args = safe_reverse_in_place(sc, sc->args);
      if (car(sc->x) == sc->NIL)                              /* reached the end of the 1st arg list, so don't loop back via push_stack */
	{
	  for (sc->y = cdr(sc->x); sc->y != sc->NIL; sc->y = cdr(sc->y))
	    if (car(sc->y) != sc->NIL)
	      return(eval_error(sc, "for-each args are not the same length", sc->x));
	  goto APPLY;
	}
      push_stack(sc, OP_LIST_FOR_EACH, sc->x, sc->code);
      goto APPLY;
      
      
    case OP_LIST_MAP:
      /* car of args incoming is arglist, cdr is values list (nil to start) */
      sc->x = sc->args;
      cdr(sc->x) = s7_cons(sc, sc->value, cdr(sc->x));       /* add current value to list */
      
      if (caar(sc->x) == sc->NIL)
	{
	  for (sc->y = cdar(sc->x); sc->y != sc->NIL; sc->y = cdr(sc->y))
	    if (car(sc->y) != sc->NIL)
	      return(eval_error(sc, "map args are not the same length", sc->x));

	  sc->value = safe_reverse_in_place(sc, cdr(sc->x));
	  /* should this expand values objects?
	   *    (apply + (map (lambda (n) (values n (+ n 1))) (list 1 2)))
	   */
	  pop_stack(sc);
	  goto START;
	}
      sc->args = sc->NIL;
      for (sc->y = car(sc->x); sc->y != sc->NIL; sc->y = cdr(sc->y))
	{
	  if (car(sc->y) == sc->NIL)
	    return(eval_error(sc, "map args are not the same length", sc->x));

	  sc->args = s7_cons(sc, caar(sc->y), sc->args);
	  car(sc->y) = cdar(sc->y);
	}
      sc->args = safe_reverse_in_place(sc, sc->args);
      push_stack(sc, OP_LIST_MAP, sc->x, sc->code);
      goto APPLY;


    case OP_VECTOR_FOR_EACH:
    case OP_HASH_TABLE_FOR_EACH:
      /* func = sc->code, func-args = caddr(sc->args), counter = car(sc->args), len = cadr(sc->args), vector(s) = cdddr(sc->args) */
      if (s7_integer(car(sc->args)) < s7_integer(cadr(sc->args)))
	{
	  s7_pointer x, y, vargs, fargs;
	  int loc;
	  /* next loop iteration */

	  vargs = cdddr(sc->args);
	  fargs = caddr(sc->args);
	  loc = s7_integer(car(sc->args));
	  for (x = fargs, y = vargs; x != sc->NIL; x = cdr(x), y = cdr(y))
	    car(x) = vector_element(car(y), loc);   /* make func's arglist */

	  integer(number(car(sc->args))) = loc + 1;
	  push_stack(sc, sc->op, sc->args, sc->code);
	  sc->args = fargs;
	  goto APPLY;
	}
      
      /* loop done */
      sc->value = sc->UNSPECIFIED;
      pop_stack(sc);
      goto START;
      

    case OP_VECTOR_MAP1:
      {
	int loc;
	loc = s7_integer(cadr(sc->args));
	integer(number(cadr(sc->args))) = loc + 1;
	vector_element(car(sc->args), loc) = sc->value;
      }
      /* drop into VECTOR_MAP0 */

      
    case OP_VECTOR_MAP0:
      /* func = sc->code, 
       * new-vector = car(sc->args), 
       * func-args = cadddr(sc->args), 
       * counter = cadr(sc->args), 
       * len = caddr(sc->args), 
       * vector(s) = cddddr(sc->args) 
       */
      if (s7_integer(cadr(sc->args)) < s7_integer(caddr(sc->args)))
	{
	  s7_pointer x, y, vargs, fargs;
	  int loc;
	  /* next loop iteration */

	  vargs = cddddr(sc->args);
	  fargs = cadddr(sc->args);
	  loc = s7_integer(cadr(sc->args));
	  for (x = fargs, y = vargs; x != sc->NIL; x = cdr(x), y = cdr(y))
	    car(x) = vector_element(car(y), loc);

	  push_stack(sc, OP_VECTOR_MAP1, sc->args, sc->code);
	  sc->args = fargs;
	  goto APPLY;
	}
      
      /* loop done */
      sc->value = car(sc->args);
      pop_stack(sc);
      goto START;
      
      
    case OP_STRING_FOR_EACH:
      /* func = sc->code, func-args = caddr(sc->args), counter = car(sc->args), len = cadr(sc->args), string(s) = cdddr(sc->args) */
      if (s7_integer(car(sc->args)) < s7_integer(cadr(sc->args)))
	{
	  s7_pointer x, y, vargs, fargs;
	  int loc;

	  vargs = cdddr(sc->args);
	  fargs = caddr(sc->args);
	  loc = s7_integer(car(sc->args));
	  for (x = fargs, y = vargs; x != sc->NIL; x = cdr(x), y = cdr(y))
	    car(x) = s7_make_character(sc, string_value(car(y))[loc]);

	  integer(number(car(sc->args))) = loc + 1;
	  push_stack(sc, OP_STRING_FOR_EACH, sc->args, sc->code);
	  sc->args = fargs;
	  goto APPLY;
	}
      
      sc->value = sc->UNSPECIFIED;
      pop_stack(sc);
      goto START;
      

    case OP_OBJECT_FOR_EACH:
      /* func = sc->code, func-args = caddr(sc->args), counter = car(sc->args), len = cadr(sc->args), object(s) = cdddr(sc->args) */
      if (s7_integer(car(sc->args)) < s7_integer(cadr(sc->args)))
	{
	  s7_pointer x, y, vargs, fargs;
	  int loc;

	  vargs = cdddr(sc->args);
	  fargs = caddr(sc->args);
	  loc = s7_integer(car(sc->args));
	  sc->x = s7_cons(sc, car(sc->args), sc->NIL);

	  for (x = fargs, y = vargs; x != sc->NIL; x = cdr(x), y = cdr(y))
	    car(x) = (*(object_types[c_object_type(car(y))].apply))(sc, car(y), sc->x);

	  integer(number(car(sc->args))) = loc + 1;
	  push_stack(sc, OP_OBJECT_FOR_EACH, sc->args, sc->code);
	  sc->args = fargs;
	  goto APPLY;
	}
      
      sc->value = sc->UNSPECIFIED;
      pop_stack(sc);
      goto START;
      

    case OP_DO: 
      /* setup is very similar to let */
      /* sc->code is the stuff after "do" */

      if ((!is_pair(sc->code)) ||            /* (do . 1) */
	  ((!is_pair(car(sc->code))) &&      /* (do 123) */
	   (car(sc->code) != sc->NIL)))      /* (do () ...) is ok */
	return(eval_error(sc, "do: var list is not a list: ~S", sc->code));

      if ((!is_pair(cadr(sc->code))) &&      /* (do ((i 0)) 123) */
	  (cadr(sc->code) != sc->NIL))       /* no end-test? */
	return(eval_error(sc, "do: end-test and end-value list is not a list: ~A", sc->code));

      if (car(sc->code) == sc->NIL)            /* (do () ...) */
	{
	  sc->envir = new_frame_in_env(sc, sc->envir); 
	  sc->args = s7_cons(sc, sc->NIL, cadr(sc->code));
	  sc->code = cddr(sc->code);
	  goto DO_END0;
	}
      
      /* eval each init value, then set up the new frame (like let, not let*) */
      
      sc->args = sc->NIL;       /* the evaluated var-data */
      sc->value = sc->code;     /* protect it */
      sc->code = car(sc->code); /* the vars */
      
      
    case OP_DO_INIT:
      sc->args = s7_cons(sc, sc->value, sc->args); /* code will be last element (first after reverse) */
      if (is_pair(sc->code))
	{
	  /* here sc->code is a list like: ((i 0 (+ i 1)) ...)
	   *   so cadar gets the init value
	   */
	  if (!(is_pair(car(sc->code))))          /* (do (4) (= 3)) */
	    return(eval_error(sc, "do: variable name missing? ~A", sc->code));

	  if (is_pair(cdar(sc->code)))
	    {
	      if ((!is_pair(cddar(sc->code))) &&
		  (cddar(sc->code) != sc->NIL))       /* (do ((i 0 . 1)) ...) */
		return(eval_error(sc, "do: variable info is an improper list?: ~A", sc->code));

	      if ((is_pair(cddar(sc->code))) && 
		  (cdr(cddar(sc->code)) != sc->NIL))  /* (do ((i 0 1 (+ i 1))) ...) */
		return(eval_error(sc, "do: variable info has extra stuff after the increment: ~A", sc->code));
	    }

	  push_stack(sc, OP_DO_INIT, sc->args, cdr(sc->code));
	  sc->code = cadar(sc->code);
	  sc->args = sc->NIL;
	  goto EVAL;
	}

      if (sc->code != sc->NIL)                    /* (do ((i 0 i) . 1) ((= i 1))) */
	return(eval_error(sc, "do: list of variables is improper: ~A", sc->code));
      
      /* all done */
      sc->args = safe_reverse_in_place(sc, sc->args);
      sc->code = car(sc->args); /* saved at the start */
      sc->args = cdr(sc->args); /* init values */
      sc->envir = new_frame_in_env(sc, sc->envir); 

      sc->value = sc->NIL;
      for (sc->x = car(sc->code), sc->y = sc->args; sc->y != sc->NIL; sc->x = cdr(sc->x), sc->y = cdr(sc->y)) 
	sc->value = s7_cons(sc, add_to_local_environment(sc, caar(sc->x), car(sc->y)), sc->value);

      /* now we've set up the environment, next set up for loop */

      sc->y = safe_reverse_in_place(sc, sc->value);
      sc->args = sc->NIL;
      for (sc->x = car(sc->code); sc->y != sc->NIL; sc->x = cdr(sc->x), sc->y = cdr(sc->y))       
	if (cddar(sc->x) != sc->NIL) /* no incr expr, so ignore it henceforth */
	  {
	    sc->value = s7_cons(sc, caddar(sc->x), sc->NIL);
	    sc->value = s7_cons(sc, car(sc->y), sc->value);
	    sc->args = s7_cons(sc, sc->value, sc->args);
	  }
      sc->value = safe_reverse_in_place(sc, sc->args);
      sc->args = s7_cons(sc, sc->value, cadr(sc->code));
      sc->code = cddr(sc->code);
      
      
    DO_END0:
    case OP_DO_END0:
      /* here vars have been init'd or incr'd
       *    args = (cons var-data end-data)
       *    code = body
       */
      
      push_stack(sc, OP_DO_END1, sc->args, sc->code);
      /* evaluate the endtest */
      sc->code = cadr(sc->args);
      sc->args = sc->NIL;
      goto EVAL;
      
      
    case OP_DO_END1:
      /* sc->value should be result of endtest evaluation */
      if (is_true(sc, sc->value))
	{
	  /* we're done -- deal with result exprs */
	  sc->code = cddr(sc->args);
	  sc->args = sc->NIL;
	  goto BEGIN;
	}
      
      /* evaluate the body and step vars, etc */
      push_stack(sc, OP_DO_STEP0, sc->args, sc->code);
      /* sc->code is ready to go */
      sc->args = sc->NIL;
      goto BEGIN;
      
      
    case OP_DO_STEP0:
      /* increment all vars, return to endtest 
       *   these are also updated in parallel at the end, so we gather all the incremented values first
       */
      if (car(sc->args) == sc->NIL)
	goto DO_END0;
      
      push_stack(sc, OP_DO_END0, sc->args, sc->code);
      sc->code = s7_cons(sc, sc->NIL, car(sc->args));   /* car = list of newly incremented values, cdr = list of slots */
      sc->args = car(sc->args);
      
      
    DO_STEP1:
    case OP_DO_STEP1:
      if (sc->args == sc->NIL)
	{
	  sc->y = cdr(sc->code);
	  sc->code = safe_reverse_in_place(sc, car(sc->code));
	  for (sc->x = sc->code; sc->y != sc->NIL && sc->x != sc->NIL; sc->x = cdr(sc->x), sc->y = cdr(sc->y))
	    set_symbol_value(caar(sc->y), car(sc->x));

	  /* "real" schemes rebind here, rather than reset, but that is expensive,
	   *    and only matters once in a blue moon (closure over enclosed lambda referring to a do var)
	   *    and the caller can easily mimic the correct behavior in that case by adding a let,
	   *    making the rebinding explicit.
	   *
	   * Hmmm... I'll leave this alone, but there are other less cut-and-dried cases:
	   *
	   *   (let ((j (lambda () 0))
	   *         (k 0))
	   *     (do ((i (j) (j))
	   *          (j (lambda () 1) (lambda () (+ i 1)))) ; bind here hits different "i" than reset
	   *         ((= i 3) k)
	   *       (set! k (+ k i))))
	   *
	   *   is it 6 or 3?
	   *
	   * if we had a way to tell that there were no lambdas in the do expression, would that
	   *   guarantee that set was ok?  Here's a bad case:
	   *
	   *   (let ((f #f))
	   *     (do ((i 0 (+ i 1)))
	   *         ((= i 3))
	   *       (let () ; so that the define is ok
	   *         (define (x) i)
	   *         (if (= i 1) (set! f x))))
	   *    (f))
	   *
	   * s7 says 3, guile says 1.
	   */
	  
	  sc->value = sc->NIL;
	  pop_stack(sc); 
	  goto DO_END0;
	}
      push_stack(sc, OP_DO_STEP2, sc->args, sc->code);
      /* here sc->args is a list like (((i . 0) (+ i 1)) ...)
       *   so sc->code becomes (+ i 1) in this case 
       */
      sc->code = cadar(sc->args);
      sc->args = sc->NIL;
      goto EVAL;
      

    case OP_DO_STEP2:
      car(sc->code) = s7_cons(sc, sc->value, car(sc->code));  /* add this value to our growing list */
      sc->args = cdr(sc->args);                               /* go to next */
      goto DO_STEP1;
      

    case OP_WITH_ENV1:
      sc->envir = sc->args;                              /* restore previous environment */
      pop_stack(sc);
      goto START;


    case OP_WITH_ENV0:
      /* (with-environment env . body) */
      push_stack(sc, OP_WITH_ENV1, sc->envir, sc->NIL);  /* save current env */
      push_stack(sc, OP_WITH_ENV2, sc->NIL, sc->code);
      sc->args = sc->NIL;
      sc->code = car(sc->code);                          /* eval env arg */
      goto EVAL;

      
    case OP_WITH_ENV2:
      sc->envir = sc->value;                             /* in new env... */
      sc->code = cdr(sc->code);                          /*   handle body */
      /* goto BEGIN; */


    BEGIN:
    case OP_BEGIN:
      if (!is_pair(sc->code)) 
	{
	  if (sc->code != sc->NIL)            /* (begin . 1) */
	    return(eval_error(sc, "unexpected dot or '() at end of body?", sc->code));

	  sc->value = sc->code;
	  pop_stack(sc);
	  goto START;
	}
      
      if (cdr(sc->code) != sc->NIL) 
	push_stack(sc, OP_BEGIN, sc->NIL, cdr(sc->code));
      
      sc->code = car(sc->code);
      sc->cur_code = sc->code;
      /* goto EVAL; */
      

    EVAL:
    case OP_EVAL:       /* main part of evaluation */
      if (is_pair(sc->code))                /* switching type check order here slows us down */
	{
	  /* using a local s7_pointer for sc->x here drastically slows things down?!? */
	  sc->x = car(sc->code);
	  if (is_syntax(sc->x))
	    {     
	      sc->code = cdr(sc->code);
	      sc->op = (opcode_t)syntax_opcode(sc->x);
	      goto START;
	    } 
	  push_stack(sc, OP_EVAL_ARGS0, sc->NIL, sc->code);
	  sc->code = sc->x;
	  goto EVAL;
	} 

      if (s7_is_symbol(sc->code))
	{
	  /* expand eval_symbol here to speed it up by a lot */
	  s7_pointer x;
	  if (is_not_local(sc->code))
	    x = symbol_global_slot(sc->code);
	  else x = s7_find_symbol_in_environment(sc, sc->envir, sc->code, true);
	  if (x != sc->NIL) 
	    sc->value = symbol_value(x);
	  else sc->value = eval_symbol(sc, sc->code);
	  pop_stack(sc);
	  goto START;
	}

      sc->value = sc->code;
      pop_stack(sc);
      goto START;
      
      
    case OP_EVAL_ARGS0:
      if (is_any_macro(sc->value))
	{    
	  /* macro expansion */
	  if (!(is_c_macro(sc->value)))
	    push_stack(sc, OP_EVAL_MACRO, sc->NIL, sc->NIL);
	  /* code is the macro invocation: (mac-name ...), args is nil, value is the macro code bound to mac-name */
	  sc->args = make_list_1(sc, sc->code); 
	  sc->code = sc->value;
	  goto APPLY;
	} 
      else 
	{
	  /* here sc->value is the func, sc->code is the entire expression */
#if WITH_ENCAPSULATION
	  if ((is_encapsulating(sc)) &&
	      (cdr(sc->code) != sc->NIL) &&
	      (is_setter(sc->value)) &&
	      (s7_is_symbol(cadr(sc->code))))
	    encapsulate(sc, cadr(sc->code));
	  
	  /* we currently ignore any case that involves an accessor such as (string-set! (vector-ref ...)).
	   *   In cases that don't involve set-applicable objects, we can say (for example) that the vector itself is not
	   *   being changed, and it is impossible to handle multi-level cases because we have to save both the values
	   *   and the accessor-paths to those values: 
	   *
	   *     (let* ((s (make-string 8 #\x)) (v (vector s))) (encapsulate (string-set! (vector-ref v 0) 1 #\a)))
	   *
	   *   here both the vector string value, and its reference back to the string variable need to be saved,
	   *   else a later string-set! is ambiguous (if a new string holds the original string value, "s" won't change
	   *   in the post-encapsulated new string-set! case, but would have changed without the encapsulation).  The set-applicable 
	   *   object case ought to be do-able, but then the check above becomes very messy -- we have to make sure 
	   *   it's a single level reference, eval_symbol caadr(?), save, but then restore becomes problematic.
	   * This has become trickier than I expected.
	   */
#endif
	  sc->code = cdr(sc->code);

	  /* here [after the cdr] sc->args is nil, sc->value is the operator (car of list), sc->code is the rest -- the args.
	   *   EVAL_ARGS0 can be called within the EVAL_ARGS1 loop if it's a nested expression:
	   * (+ 1 2 (* 2 3)):
	   *   e0args: (), value: +, code: (1 2 (* 2 3))
	   *   e1args: (+), value: +, code: (1 2 (* 2 3))
	   *   e1args: (1 +), value: +, code: (2 (* 2 3))
	   *   e1args: (2 1 +), value: +, code: ((* 2 3))
	   *   e0args: (), value: *, code: (2 3)
	   *   e1args: (*), value: *, code: (2 3)
	   *   e1args: (2 *), value: *, code: (3)
	   *   e1args: (3 2 *), value: *, code: ()
	   *   <end -> apply the * op>
	   *   e1args: (6 2 1 +), value: +, code: ()
	   */
	}
      
      
    EVAL_ARGS:
    case OP_EVAL_ARGS1:
      /* this is where most of s7's compute time goes */
      /* expanding the function calls (s7_cons, new_cell, and eval_symbol) in place seems to speed up s7 by a noticeable amount! */
      /*    before expansion: sc->args = s7_cons(sc, sc->value, sc->args); */
      {
        s7_pointer x;
#if HAVE_PTHREADS
	x = new_cell(sc); 
#else
	if (sc->free_heap_top > 0)
	  x = sc->free_heap[--(sc->free_heap_top)];
	else x = new_cell(sc);
#endif
	car(x) = sc->value;
	cdr(x) = sc->args;
	set_type(x, T_PAIR);
	sc->args = x;
      }

      /* 1st time, value = op, args = nil (only e0 entry is from op_eval above), code is full list (at e0) */
      if (is_pair(sc->code))  /* evaluate current arg */
	{ 
	  int typ;
	  s7_pointer car_code;
	  car_code = car(sc->code);
	  typ = type(car_code);

	  if (typ == T_PAIR)
	    {
	      push_stack(sc, OP_EVAL_ARGS1, sc->args, cdr(sc->code));
	      sc->code = car_code;
	      sc->args = sc->NIL;
	      goto EVAL;
	    }

	  if (typ == T_SYMBOL)
	    {
	      /* expand eval_symbol here to speed it up, was sc->value = eval_symbol(sc, car(sc->code)); */
	      s7_pointer x;
	      if (is_not_local(car_code))
		x = symbol_global_slot(car_code);
	      else x = s7_find_symbol_in_environment(sc, sc->envir, car_code, true);
	      if (x != sc->NIL) 
		sc->value = symbol_value(x);
	      else sc->value = eval_symbol(sc, car_code);
	    }
	  
	  else sc->value = car_code;
	  sc->code = cdr(sc->code);
	  goto EVAL_ARGS;
	}
      else                       /* got all args -- go to apply */
	{
	  if (sc->code != sc->NIL)
	    {
	      int len;
	      char *msg, *argstr;
	      s7_pointer result;
	      argstr = s7_object_to_c_string(sc, s7_append(sc, 
							   s7_reverse(sc, cdr(sc->args)), 
							   s7_cons(sc, car(sc->args), sc->code)));
	      len = strlen(argstr) + 32;
	      msg = (char *)malloc(len * sizeof(char));
	      len = snprintf(msg, len, "improper list of arguments: %s?", argstr);
	      free(argstr);
	      result = s7_make_string_with_length(sc, msg, len);
	      free(msg);
	      s7_error(sc, sc->ERROR, result);
	    }

	  sc->args = safe_reverse_in_place(sc, sc->args); 
	  /* we could omit this reversal in many cases: all built in ops could
	   *   assume reversed args, things like eq? and + don't care about order, etc.
	   *   But, I think the reversal is not taking any noticeable percentage of
	   *   the overall compute time (ca 1% according to callgrind).
	   */
	  sc->code = car(sc->args);
	  sc->args = cdr(sc->args);
	  /* goto APPLY;  */
	}
      
      
      /* ---------------- OP_APPLY ---------------- */
    APPLY:
    case OP_APPLY:      /* apply 'code' to 'args' */

#if WITH_PROFILING
      symbol_calls(sc->code)++;
#endif

      if (*(sc->tracing)) 
	trace_apply(sc);

    APPLY_WITHOUT_TRACE:
      if (sc->stack_top >= sc->stack_size2)
	increase_stack_size(sc);

      switch (type(sc->code))
	{
	case T_C_FUNCTION: 	                  /* -------- C-based function -------- */
	  {
	    int len;
	    len = safe_list_length(sc, sc->args);
	    if (len < c_function_required_args(sc->code))
	      return(s7_error(sc, 
			      sc->WRONG_NUMBER_OF_ARGS, 
			      make_list_3(sc, 
					  s7_make_string_with_length(sc, "~A: not enough arguments: ~A", 28), 
					  sc->code, sc->args)));
	    
	    if (c_function_all_args(sc->code) < len)
	      return(s7_error(sc, 
			      sc->WRONG_NUMBER_OF_ARGS, 
			      make_list_3(sc, 
					  s7_make_string_with_length(sc, "~A: too many arguments: ~A", 26),
					  sc->code, sc->args)));

	    sc->value = c_function_call(sc->code)(sc, sc->args);

	    if (sc->stack_top != 0)
	      pop_stack(sc);
	    else return(sc->value); /* or perhaps sc->F? */

	    /* this is trying to get around an existing, but very well-hidden bug:
	     *  (defgenerator tanhsin (frequency 100.0))
	     *  (define (tanhsin gen) #f)
	     *  (define (crash) (let ((hi (make-tanhsin))) (catch #t (lambda () (mus-run hi 0.0)) (lambda args 123))))
	     *  (crash)
	     * and we pop_stack once too often somewhere.  I'd put a guard OP_QUIT at the start, but 
	     *   reset_stack would need to be changed and so on.
	     */

	    goto START;
	  }


	case T_C_MACRO: 	                    /* -------- C-based macro -------- */
	  {
	    int len;
	    s7_pointer macsym;
	    macsym = caar(sc->args);
	    sc->args = cdar(sc->args);

	    len = safe_list_length(sc, sc->args);
	    if (len < c_macro_required_args(sc->code))
	      return(s7_error(sc, 
			      sc->WRONG_NUMBER_OF_ARGS, 
			      make_list_3(sc, 
					  s7_make_string_with_length(sc, "~A: not enough arguments: ~A", 28), 
					  macsym, sc->args)));
	    
	    if (c_macro_all_args(sc->code) < len)
	      return(s7_error(sc, 
			      sc->WRONG_NUMBER_OF_ARGS, 
			      make_list_3(sc, 
					  s7_make_string_with_length(sc, "~A: too many arguments: ~A", 26),
					  macsym, sc->args)));

	    sc->code = c_macro_call(sc->code)(sc, sc->args);
	    sc->args = sc->NIL;
	    goto EVAL;
	  }

	  
	case T_CLOSURE:                              /* -------- normal function (lambda), or macro -------- */
	case T_MACRO:
	  sc->envir = new_frame_in_env(sc, closure_environment(sc->code)); 
	  
	  /* load up the current args into the ((args) (lambda)) layout [via the current environment] */

	  /* (defmacro hi (a b) `(+ ,a ,b)) */
	  /*   -> code: #<macro>, args: ((hi 2 3)), closure args: (defmac-9) */
	  /*   then back again: code: #<closure>, args: (2 3), closure args: (a b) */

	  for (sc->x = closure_args(sc->code), sc->y = sc->args; is_pair(sc->x); sc->x = cdr(sc->x), sc->y = cdr(sc->y)) 
	    {
	      if (sc->y == sc->NIL)
		return(s7_error(sc, 
				sc->WRONG_NUMBER_OF_ARGS, 
				make_list_3(sc, 
					    s7_make_string_with_length(sc, "~A: not enough arguments: ~A", 28),
					    g_procedure_source(sc, make_list_1(sc, sc->code)), 
					    sc->args)));
	      add_to_local_environment(sc, car(sc->x), car(sc->y));
	    }
	  
	  if (sc->x == sc->NIL) 
	    {
	      if (sc->y != sc->NIL)
		return(s7_error(sc, 
				sc->WRONG_NUMBER_OF_ARGS, 
				make_list_3(sc, 
					    s7_make_string_with_length(sc, "~A: too many arguments: ~A", 26), 
					    g_procedure_source(sc, make_list_1(sc, sc->code)), 
					    sc->args)));
	    } 
	  else 
	    {
	      if (s7_is_symbol(sc->x))
		add_to_local_environment(sc, sc->x, sc->y); 
	      else 
		{
		  if (is_macro(sc->code))
		    return(eval_error(sc, "~A: undefined argument to macro?", sc->x));
		  else return(eval_error(sc, "~A: undefined argument to function?", sc->x));
		}
	    }

	  sc->code = closure_body(sc->code);
	  sc->args = sc->NIL;
	  goto BEGIN;
	  
	case T_CLOSURE_STAR:	                  /* -------- define* (lambda*) -------- */
	  { 
	    sc->envir = new_frame_in_env(sc, closure_environment(sc->code)); 
	    
	    /* sc->code is a closure: ((args body) envir)
	     * (define* (hi a (b 1)) (+ a b))
	     * (procedure-source hi) -> (lambda* (a (b 1)) (+ a b))
	     *
	     * so rather than spinning through the args binding names to values in the
	     *   procedure's new environment (as in the usual closure case above),
	     *   we scan the current args, and match against the
	     *   template in the car of the closure, binding as we go.
	     *
	     * for each actual arg, if it's not a keyword that matches a member of the 
	     *   template, bind it to its current (place-wise) arg, else bind it to
	     *   that arg.  If it's the symbol :key or :optional, just go on.
	     *   If it's :rest bind the next arg to the trailing args at this point.
	     *   All args can be accessed by their name as a keyword.
	     *   In other words (define* (hi (a 1)) ...) is the same as (define* (hi :key (a 1)) ...) etc.
	     *
	     * all args are optional, any arg with no default value defaults to #f.
	     */
	    
	    /* set all default values */
	    for (sc->z = closure_args(sc->code); is_pair(sc->z); sc->z = cdr(sc->z))
	      {
		/* bind all the args to something (default value or #f or maybe #undefined) */
		if (!((car(sc->z) == sc->KEY_KEY) ||
		      (car(sc->z) == sc->KEY_OPTIONAL) ||
		      (car(sc->z) == sc->KEY_REST)))                  /* :optional and :key always ignored, :rest dealt with later */
		  {
		    if (is_pair(car(sc->z)))                          /* (define* (hi (a mus-next)) a) */
		      add_to_local_environment(sc,                    /* or (define* (hi (a 'hi)) (list a (eq? a 'hi))) */
					       caar(sc->z), 
					       lambda_star_argument_default_value(sc, cadar(sc->z)));
		                                                      /* mus-next, for example, needs to be evaluated before binding */
		    else add_to_local_environment(sc, car(sc->z), sc->F);
		  }
	      }
	    if (s7_is_symbol(sc->z))                                  /* dotted (last) arg? -- make sure its name exists in the current environment */
	      add_to_local_environment(sc, sc->z, sc->F);
	    
	    /* now get the current args, re-setting args that have explicit values */
	    sc->x = closure_args(sc->code);
	    sc->y = sc->args; 
	    sc->z = sc->NIL;
	    while ((is_pair(sc->x)) &&
		   (is_pair(sc->y)))
	      {
		if ((car(sc->x) == sc->KEY_KEY) ||
		    (car(sc->x) == sc->KEY_OPTIONAL))
		  sc->x = cdr(sc->x);
		else
		  {
		    if (car(sc->x) == sc->KEY_REST)
		      {
			/* next arg is bound to trailing args from this point as a list */
			sc->z = sc->KEY_REST;
			sc->x = cdr(sc->x);
			if (is_pair(car(sc->x)))
			  lambda_star_argument_set_value(sc, caar(sc->x), sc->y);
			else lambda_star_argument_set_value(sc, car(sc->x), sc->y);
			sc->y = cdr(sc->y);
			sc->x = cdr(sc->x);
		      }
		    else
		      {
			if (s7_is_keyword(car(sc->y)))
			  {
			    char *name;   /* need to remove the ':' before checking the lambda args */
			    s7_pointer sym;
			    name = symbol_name(car(sc->y));
			    if (name[0] == ':')
			      sym = s7_make_symbol(sc, (const char *)(name + 1));
			    else
			      {
				/* must be a trailing ':' here, else not s7_is_keyword */
				name[symbol_name_length(car(sc->y)) - 1] = '\0';
				sym = s7_make_symbol(sc, name);
				name[symbol_name_length(car(sc->y)) - 1] = ':';
			      }
			    if (lambda_star_argument_set_value(sc, sym, car(cdr(sc->y))))
			      sc->y = cddr(sc->y);
			    else         /* might be passing a keyword as a normal argument value! */
			      {
				if (is_pair(car(sc->x)))
				  lambda_star_argument_set_value(sc, caar(sc->x), car(sc->y));
				else lambda_star_argument_set_value(sc, car(sc->x), car(sc->y));
				sc->y = cdr(sc->y);
			      }
			  }
			else 
			  {
			    if (is_pair(car(sc->x)))
			      lambda_star_argument_set_value(sc, caar(sc->x), car(sc->y));
			    else lambda_star_argument_set_value(sc, car(sc->x), car(sc->y));
			    sc->y = cdr(sc->y);
			  }
			sc->x = cdr(sc->x);
		      }
		  }
	      }

	    /* check for trailing args with no :rest arg */
	    if (sc->y != sc->NIL)
	      {
		if ((sc->x == sc->NIL) &&
		    (is_pair(sc->y)))
		  {
		    if (sc->z != sc->KEY_REST)
		      return(s7_error(sc, 
				      sc->WRONG_NUMBER_OF_ARGS, 
				      make_list_3(sc, 
						  s7_make_string_with_length(sc, "~A: too many arguments: ~A", 22), 
						  g_procedure_source(sc, make_list_1(sc, sc->code)), 
						  sc->args)));
		  } 
		else 
		  {
		    /* final arg was dotted? */
		    if (s7_is_symbol(sc->x))
		      add_to_local_environment(sc, sc->x, sc->y); 
		  }
	      }

	    /* evaluate the function body */
	    sc->code = closure_body(sc->code);
	    sc->args = sc->NIL;
	    goto BEGIN;
	  }
		
	case T_CONTINUATION:	                  /* -------- continuation ("call-with-continuation") -------- */
	  {
	    check_for_dynamic_winds(sc, sc->code);
	    sc->stack = copy_stack(sc, continuation_stack(sc->code), continuation_stack_top(sc->code));
	    sc->stack_size = continuation_stack_size(sc->code);
	    sc->stack_size2 = sc->stack_size / 2;
	    sc->stack_top = continuation_stack_top(sc->code);
	    if (sc->args == sc->NIL)
	      sc->value = sc->NIL;
	    else
	      {
		if (cdr(sc->args) == sc->NIL)
		  sc->value = car(sc->args);
		else sc->value = splice_in_values(sc, sc->args);
	      }
	    pop_stack(sc);
	    goto START;
	  }

	case T_GOTO:	                          /* -------- goto ("call-with-exit") -------- */
	  {
	    int i, new_stack_top;
	    new_stack_top = (sc->code)->object.goto_loc;
	    /* look for dynamic-wind in the stack section that we are jumping out of */
	    for (i = sc->stack_top - 1; i > new_stack_top; i -= 4)
	      {
		opcode_t op;
		op = (opcode_t)s7_integer(vector_element(sc->stack, i));
		if (op == OP_DYNAMIC_WIND)
		  {
		    sc->z = vector_element(sc->stack, i - 3);
		    if (dynamic_wind_state(sc->z) == T_DWIND_BODY)
		      {
			push_stack(sc, OP_EVAL_DONE, sc->args, sc->code); 
			sc->args = sc->NIL;
			sc->code = dynamic_wind_out(sc->z);
			eval(sc, OP_APPLY);
		      }
		  }
		else
		  {
		    if (op == OP_TRACE_RETURN)
		      sc->trace_depth--;
		  }
	      }
	    
	    sc->stack_top = new_stack_top;
	    sc->value = (sc->args != sc->NIL) ? car(sc->args) : sc->NIL;
	    pop_stack(sc);
	    goto START;
	  }

	case T_C_OBJECT:	                  /* -------- applicable object -------- */
	  sc ->value = s7_apply_object(sc, sc->code, sc->args);
	  if (sc->stack_top > 0)
	    pop_stack(sc);
	  goto START;

	case T_VECTOR:                            /* -------- vector as applicable object -------- */
	  /* sc->code is the vector, sc->args is the list of dimensions */
	  sc->value = vector_ref_1(sc, sc->code, sc->args);
	  pop_stack(sc);
	  goto START;

	case T_STRING:                            /* -------- string as applicable object -------- */
	  if (cdr(sc->args) != sc->NIL)
	    return(s7_wrong_number_of_args_error(sc, "string ref (via string as applicable object)", sc->args));
	  sc->value = g_string_ref(sc, make_list_2(sc, sc->code, car(sc->args)));
	  pop_stack(sc);
	  goto START;

	case T_PAIR:                              /* -------- list as applicable object -------- */
	  if (cdr(sc->args) != sc->NIL)
	    return(s7_wrong_number_of_args_error(sc, "list ref (via list as applicable object)", sc->args));
	  /* 
	   * I suppose we could take n args here = repeated list-refs
	   * ((list (list 1 2) 3) 0 0) -> 1 (caar)
	   */
	  sc->value = g_list_ref(sc, make_list_2(sc, sc->code, car(sc->args)));
	  pop_stack(sc);
	  goto START;

	case T_HASH_TABLE:                        /* -------- hash-table as applicable object -------- */
	  if (cdr(sc->args) != sc->NIL)
	    return(s7_wrong_number_of_args_error(sc, "hash-table ref (via hash-table as applicable object)", sc->args));
	  sc->value = g_hash_table_ref(sc, make_list_2(sc, sc->code, car(sc->args)));
	  pop_stack(sc);
	  goto START;

	default:
	  return(eval_error(sc, "attempt to apply ~S?", sc->code));
	}
      /* ---------------- end OP_APPLY ---------------- */

      
    case OP_EVAL_MACRO:    /* after (scheme-side) macroexpansion, evaluate the resulting expression */
      /* if the macro is more than a quasiquoted expression, then we come through here twice */
      sc->code = sc->value;
      goto EVAL;
      
      
    case OP_LAMBDA: 
      /* this includes unevaluated symbols (direct symbol table refs) in macro arg list */
      if ((!is_pair(sc->code)) ||
	  (!is_pair(cdr(sc->code))))                               /* (lambda) or (lambda #f) */
	return(eval_error(sc, "lambda: no args or no body? ~A", sc->code));

      if (!s7_is_list(sc, car(sc->code)))
	{
	  if (!s7_is_symbol(car(sc->code)))                        /* (lambda "hi" ...) */
	    return(eval_error(sc, "lambda parameter ~S is not a symbol", car(sc->code)));
	}
      else
	{
	  for (sc->x = car(sc->code); sc->x != sc->NIL; sc->x = cdr(sc->x))
	    if ((!s7_is_symbol(sc->x)) &&                          /* (lambda (a . b) 0) */
		((!is_pair(sc->x)) ||                              /* (lambda (a . 0.0) a) */
		 (!s7_is_symbol(car(sc->x)))))                     /* (lambda ("a") a) or (lambda (a "a") a) */
	      return(eval_error(sc, "lambda parameter ~S is not a symbol", sc->x));
	}
      sc->value = make_closure(sc, sc->code, sc->envir, T_CLOSURE);
      pop_stack(sc);
      goto START;


    case OP_LAMBDA_STAR:
      if ((!is_pair(sc->code)) ||
	  (!is_pair(cdr(sc->code))))                                /* (lambda*) or (lambda* #f) */
	return(eval_error(sc, "lambda*: no args or no body? ~A", sc->code));

      if (!s7_is_list(sc, car(sc->code)))
	{
	  if (!s7_is_symbol(car(sc->code)))                        /* (lambda* "hi" ...) */
	    return(eval_error(sc, "lambda* parameter ~S is not a symbol", car(sc->code)));
	}
      else
	{ 
	  for (sc->x = car(sc->code); sc->x != sc->NIL; sc->x = cdr(sc->x))
	    if ((!s7_is_symbol(sc->x)) &&                          
		((!is_pair(sc->x)) ||
		 ((!s7_is_symbol(car(sc->x))) &&
		  ((!is_pair(car(sc->x))) ||                       /* check for stuff like (lambda* (()) 1) (lambda* ((a . 0)) 1) etc */
		   (!s7_is_symbol(caar(sc->x))) ||
		   (cdar(sc->x) == sc->NIL) ||
		   (cddar(sc->x) != sc->NIL)))))
	      return(eval_error(sc, "lambda* parameter ~S is confused", sc->x));
	}
      sc->value = make_closure(sc, sc->code, sc->envir, T_CLOSURE_STAR);
      pop_stack(sc);
      goto START;
      
      
    case OP_QUOTE:
      if ((!is_pair(sc->code)) ||                /* (quote . -1) */
	  (cdr(sc->code) != sc->NIL))            /* (quote . (1 2)) or (quote 1 1) */
	return(eval_error(sc, "quote syntax error ~A", sc->code));

      sc->value = car(sc->code);
      /* should this be immutable? (set-car! '(1 . 2) 3) */
      pop_stack(sc);
      goto START;

      
    case OP_DEFINE_CONSTANT1:
      /* define-constant -> OP_DEFINE_CONSTANT0 -> OP_DEFINE0..1, then back to here */
      /*   at this point, sc->value is the symbol that we want to be immutable, sc->code is the original pair */

      sc->x = s7_find_symbol_in_environment(sc, sc->envir, sc->value, false);
      s7_set_immutable(car(sc->x));
      pop_stack(sc);
      goto START;


    case OP_DEFINE_CONSTANT0:
      push_stack(sc, OP_DEFINE_CONSTANT1, sc->NIL, sc->code);

      
    case OP_DEFINE_STAR:
    case OP_DEFINE0:
      if (!is_pair(sc->code))
	return(eval_error(sc, "define: nothing to define? ~A", sc->code));   /* (define) */

      if (!is_pair(cdr(sc->code)))
	return(eval_error(sc, "define: no value? ~A", sc->code));            /* (define var) */

      if ((!is_pair(car(sc->code))) &&
	  (cddr(sc->code) != sc->NIL))                                       /* (define var 1 . 2) */
	return(eval_error(sc, "define: more than 1 value? ~A", sc->code));   /* (define var 1 2) */

      if (is_pair(car(sc->code))) 
	{
	  sc->x = caar(sc->code);
	  if (sc->op == OP_DEFINE_STAR)
	    sc->code = s7_cons(sc, sc->LAMBDA_STAR, s7_cons(sc, cdar(sc->code), cdr(sc->code)));
	  else sc->code = s7_cons(sc, sc->LAMBDA, s7_cons(sc, cdar(sc->code), cdr(sc->code)));
	} 
      else 
	{
	  sc->x = car(sc->code);
	  sc->code = cadr(sc->code);
	}
      if (!s7_is_symbol(sc->x))                                             /* (define (3 a) a) */
	return(eval_error(sc, "define a non-symbol? ~S", sc->x));

      if (s7_is_immutable(sc->x))                                           /* (define pi 3) or (define (pi a) a) */
	return(eval_error(sc, "define: ~S is immutable", sc->x));
      
      push_stack(sc, OP_DEFINE1, sc->NIL, sc->x);
      goto EVAL;
      
      
    case OP_DEFINE1:
      /* sc->code is the symbol being defined, sc->value is its value
       *   if sc->value is a closure, car is of the form ((args...) body...)
       *   so the doc string if any is (cadr (car value))
       *   and the arg list gives the number of optional args up to the dot
       */

      /* it's not possible to expand and replace macros at this point without evaluating
       *   the body.  Just as examples, say we have a macro "mac", 
       *   (define (hi) (call/cc (lambda (mac) (mac 1))))
       *   (define (hi) (quote (mac 1))) or macroexpand etc
       *   (define (hi mac) (mac 1)) assuming mac here is a function passed as an arg,
       * etc...  
       */

      /* if we're defining a function, add its symbol to the new function's environment under the name __func__ */
      if ((is_closure(sc->value)) || 
	  (is_closure_star(sc->value)))
	closure_environment(sc->value) = 
	  s7_cons(sc, 
		  s7_cons(sc, 
			  port_filename(sc->input_port) ? s7_immutable_cons(sc, 
									   sc->__FUNC__, 									       
									   make_list_3(sc, 
										       sc->code,
										       s7_make_string(sc, port_filename(sc->input_port)),
										       s7_make_integer(sc, port_line_number(sc->input_port)))) :
							 s7_immutable_cons(sc, sc->__FUNC__, sc->code), 
			  sc->NIL), 
		  closure_environment(sc->value));
      else
	{
	  if (s7_is_procedure_with_setter(sc->value))
	    {
	      s7_pws_t *f = (s7_pws_t *)s7_object_value(sc->value);
	      f->name = s7_strdup(symbol_name(sc->code));
	    }
	}

      /* add the newly defined thing to the current environment */
      sc->x = s7_find_symbol_in_environment(sc, sc->envir, sc->code, false);
      if (sc->x != sc->NIL) 
	set_symbol_value(sc->x, sc->value); 
      else add_to_current_environment(sc, sc->code, sc->value); 
      sc->value = sc->code;
      pop_stack(sc);
      goto START;
      
      
    case OP_SET2:
      sc->code = s7_cons(sc, s7_cons(sc, sc->value, sc->args), sc->code);

      
    case OP_SET0:                                                 /* entry for set! */
      if (!is_pair(sc->code))
	return(eval_error(sc, "set! syntax error ~A", sc->code)); /* set! . 1) */

      if (s7_is_immutable(car(sc->code)))                         /* (set! pi 3) */
	return(eval_error(sc, "set!: can't alter immutable object: ~S", car(sc->code)));
      
      if ((cdr(sc->code) == sc->NIL) ||
	  (cddr(sc->code) != sc->NIL))                            /* (set! var) */
	return(eval_error(sc, "~A: wrong number of args to set!", sc->code));

      if (is_pair(car(sc->code))) /* has accessor */
	{
	  if (is_pair(caar(sc->code)))
	    {
	      push_stack(sc, OP_SET2, cdar(sc->code), cdr(sc->code));
	      sc->code = caar(sc->code);
	      goto EVAL;
	    }
	  
	  if (!s7_is_symbol(caar(sc->code)))                      /* (set! (1 2) #t) */
	    return(eval_error(sc, "~A: non-symbol as generalized set! accessor?", sc->code)); 

	  sc->x = s7_symbol_value(sc, caar(sc->code));
	  if ((is_c_object(sc->x)) &&
	      (object_set_function(sc->x)))
	    sc->code = s7_cons(sc, sc->OBJECT_SET, s7_append(sc, car(sc->code), cdr(sc->code)));   /* use set method (append flattens the lists) */
	  else 
	    {
	      switch (type(sc->x))
		{
		case T_VECTOR:
		  /* sc->x is the vector, sc->code is expr without the set! */
		  /*  args have not been evaluated! */
		  sc->code = s7_cons(sc, sc->VECTOR_SET, s7_append(sc, car(sc->code), cdr(sc->code)));
		  break;
		  
		case T_STRING:
		  sc->code = s7_cons(sc, sc->STRING_SET, s7_append(sc, car(sc->code), cdr(sc->code))); 
		  break;

		case T_PAIR:
		  sc->code = s7_cons(sc, sc->LIST_SET, s7_append(sc, car(sc->code), cdr(sc->code))); 
		  break;

		case T_HASH_TABLE:
		  sc->code = s7_cons(sc, sc->HASH_TABLE_SET, s7_append(sc, car(sc->code), cdr(sc->code))); 
		  break;

		default:
		  return(eval_error(sc, "no generalized set for ~A", caar(sc->code)));
		}
	    }
	}
      else 
	{
	  if (!s7_is_symbol(car(sc->code)))                  /* (set! 12345 1) */
	    return(eval_error(sc, "set! can't change ~S", car(sc->code)));
	  
	  push_stack(sc, OP_SET1, sc->NIL, car(sc->code));
	  sc->code = cadr(sc->code);
	}
      goto EVAL;
      
      
    case OP_SET1:     
      sc->y = s7_find_symbol_in_environment(sc, sc->envir, sc->code, true);
      if (sc->y != sc->NIL) 
	{
#if WITH_ENCAPSULATION
	  if (is_encapsulating(sc))
	    encapsulate(sc, sc->code);
#endif
	  set_symbol_value(sc->y, sc->value); 
	  pop_stack(sc);
	  goto START;
	}
      return(eval_error(sc, "set! ~A: unbound variable", sc->code));
      
      
    case OP_IF0:
      if (!is_pair(sc->code))                               /* (if) or (if . 1) */
	return(eval_error(sc, "(if): if needs at least 2 expressions", sc->code));

      if (!is_pair(cdr(sc->code)))                          /* (if 1) */
	return(eval_error(sc, "(if ~A): if needs another clause", car(sc->code)));
      
      if ((!is_pair(cddr(sc->code))) &&
	  (cddr(sc->code) != sc->NIL))                      /* (if 1 2 . 3) */
	return(eval_error(sc, "if: ~A has improper list?", sc->code));

      if ((is_pair(cddr(sc->code))) && 
	  (cdddr(sc->code) != sc->NIL))                     /* (if 1 2 3 4) */
	return(eval_error(sc, "too many clauses for if: ~A", sc->code));
      
      
      push_stack(sc, OP_IF1, sc->NIL, cdr(sc->code));
      sc->code = car(sc->code);
      goto EVAL;
      
      
    case OP_IF1:
      if (is_true(sc, sc->value))
	sc->code = car(sc->code);
      else
	sc->code = cadr(sc->code);  /* (if #f 1) ==> #<unspecified> because car(sc->NIL) = sc->UNSPECIFIED */
      goto EVAL;
      
      
    case OP_LET0:
      /* sc->code is everything after the let: (let ((a 1)) a) so sc->code is (((a 1)) a) */
      /*   car it can be either a list or a symbol ("named let") */

      if ((!is_pair(sc->code)) ||            /* (let . 1) */
	  (!is_pair(cdr(sc->code))) ||       /* (let) */
	  ((!is_pair(car(sc->code))) &&      /* (let 1 ...) */
	   (car(sc->code) != sc->NIL) &&
	   (!s7_is_symbol(car(sc->code)))))
	return(eval_error(sc, "let syntax error: ~A", sc->code));

      if ((s7_is_symbol(car(sc->code))) &&
	  (((!is_pair(cadr(sc->code))) &&    /* (let hi #t) */
	    (cadr(sc->code) != sc->NIL)) ||
	   (cddr(sc->code) == sc->NIL)))     /* (let hi ()) */
      	return(eval_error(sc, "named let syntax error: ~A", sc->code));

      sc->args = sc->NIL;
      sc->value = sc->code;
      sc->code = s7_is_symbol(car(sc->code)) ? cadr(sc->code) : car(sc->code);
      
      
    case OP_LET1:       /* let -- calculate parameters */
      sc->args = s7_cons(sc, sc->value, sc->args);
      if (is_pair(sc->code)) 
	{ 
	  if (!is_pair(car(sc->code)))          /* (let ((x)) ...) or (let ((x 1) . (y 2)) ...) */
	    return(eval_error(sc, "let syntax error (no value?): ~A", car(sc->code)));

	  if (!(is_pair(cdar(sc->code))))       /* (let ((x . 1))...) */
	    return(eval_error(sc, "let syntax error (not a proper list?): ~A", car(sc->code)));

	  if (cddar(sc->code) != sc->NIL)       /* (let ((x 1 2 3)) ...) */
	    return(eval_error(sc, "let syntax error (more than one value?): ~A", car(sc->code)));

	  push_stack(sc, OP_LET1, sc->args, cdr(sc->code));
	  sc->code = cadar(sc->code);
	  sc->args = sc->NIL;
	  goto EVAL;
	} 

      if (sc->code != sc->NIL)                  /* (let* ((a 1) . b) a) */
	return(eval_error(sc, "let var list improper?: ~A", sc->code));

      sc->args = safe_reverse_in_place(sc, sc->args);
      sc->code = car(sc->args);
      sc->args = cdr(sc->args);
      
      
    case OP_LET2:
      sc->envir = new_frame_in_env(sc, sc->envir); 
      for (sc->x = s7_is_symbol(car(sc->code)) ? cadr(sc->code) : car(sc->code), sc->y = sc->args; sc->y != sc->NIL; sc->x = cdr(sc->x), sc->y = cdr(sc->y)) 
	{
	  if (!(s7_is_symbol(caar(sc->x))))
	    return(eval_error(sc, "bad variable ~S in let bindings", car(sc->code)));

	  /* check for name collisions -- not sure this is required by Scheme */
	  if (s7_find_symbol_in_environment(sc, sc->envir, caar(sc->x), false) != sc->NIL)
	    return(eval_error(sc, "duplicate identifier in let", caar(sc->x)));

	  add_to_local_environment(sc, caar(sc->x), car(sc->y)); /* expansion here does not help */
	}
      if (s7_is_symbol(car(sc->code))) 
	{    /* named let */
	  for (sc->x = cadr(sc->code), sc->args = sc->NIL; sc->x != sc->NIL; sc->x = cdr(sc->x)) 
	    sc->args = s7_cons(sc, caar(sc->x), sc->args);
	  
	  sc->x = s7_make_closure(sc, s7_cons(sc, safe_reverse_in_place(sc, sc->args), cddr(sc->code)), sc->envir); 
	  add_to_local_environment(sc, car(sc->code), sc->x); 
	  sc->code = cddr(sc->code);
	  sc->args = sc->NIL;
	} 
      else 
	{
	  sc->code = cdr(sc->code);
	  sc->args = sc->NIL;
	}
      goto BEGIN;
      
      
    case OP_LET_STAR0:
      if ((!is_pair(sc->code)) ||                /* (let* . 1) */
	  (!is_pair(cdr(sc->code))) ||           /* (let*) */
	  ((!is_pair(car(sc->code))) &&          /* (let* 1 ...), also there's no named let* */
	   (car(sc->code) != sc->NIL)))
	return(eval_error(sc, "let* syntax error: ~A", sc->code));
      
      if (car(sc->code) == sc->NIL) 
	{
	  sc->envir = new_frame_in_env(sc, sc->envir); 
	  sc->code = cdr(sc->code);
	  goto BEGIN;
	}
      
      if ((!is_pair(car(sc->code))) ||            /* (let* x ... ) */
	  (!is_pair(caar(sc->code))) ||           /* (let* (x) ...) */
	  (!is_pair(cdaar(sc->code))))            /* (let* ((x . 1)) ...) */
	return(eval_error(sc, "let* variable list syntax error: ~A", sc->code));
      
      push_stack(sc, OP_LET_STAR1, cdr(sc->code), car(sc->code));
      sc->code = cadaar(sc->code);
      goto EVAL;
      
      
    case OP_LET_STAR1:    /* let* -- calculate parameters */
      if (!(s7_is_symbol(caar(sc->code))))
	return(eval_error(sc, "bad variable ~S in let* bindings", car(sc->code)));

      if (!is_pair(car(sc->code)))          /* (let* ((x)) ...) */
	return(eval_error(sc, "let* syntax error (no value?): ~A", car(sc->code)));

      if (!(is_pair(cdar(sc->code))))       /* (let* ((x . 1))...) */
	return(eval_error(sc, "let* syntax error (not a proper list?): ~A", car(sc->code)));

      if (cddar(sc->code) != sc->NIL)       /* (let* ((x 1 2 3)) ...) */
	return(eval_error(sc, "let* syntax error (more than one value?): ~A", car(sc->code)));

      sc->envir = new_frame_in_env(sc, sc->envir); 
      /* we can't skip this new frame -- we have to imitate a nested let, otherwise
       *
       *   (let ((f1 (lambda (arg) (+ arg 1))))
       *     (let* ((x 32)
       *            (f1 (lambda (arg) (f1 (+ x arg)))))
       *       (f1 1)))
       *
       * will hang.
       */

      add_to_local_environment(sc, caar(sc->code), sc->value); 
      sc->code = cdr(sc->code);
      if (is_pair(sc->code)) 
	{ 
	  push_stack(sc, OP_LET_STAR1, sc->args, sc->code);
	  sc->code = cadar(sc->code);
	  sc->args = sc->NIL;
	  goto EVAL;
	} 

      if (sc->code != sc->NIL)                    /* (let* ((a 1) . b) a) */
	return(eval_error(sc, "let* var list improper?: ~A", sc->code));

      sc->code = sc->args;
      sc->args = sc->NIL;
      goto BEGIN;
      
      
    case OP_LETREC0:
      if ((!is_pair(sc->code)) ||                 /* (letrec . 1) */
	  (!is_pair(cdr(sc->code))) ||            /* (letrec) */
	  ((!is_pair(car(sc->code))) &&           /* (letrec 1 ...) */
	   (car(sc->code) != sc->NIL)))
	return(eval_error(sc, "letrec syntax error: ~A", sc->code));
      
      /* get all local vars and set to #undefined
       * get parallel list of values
       * eval each member of values list with env still full of #undefined's
       * assign each value to its variable
       * eval body
       */
      sc->envir = new_frame_in_env(sc, sc->envir); 
      sc->args = sc->NIL;
      sc->value = sc->code;
      sc->code = car(sc->code);

      for (sc->x = sc->code; sc->x != sc->NIL; sc->x = cdr(sc->x))
	{
	  if ((!is_pair(car(sc->x))) ||             /* (letrec (1 2) #t) */
	      (!(s7_is_symbol(caar(sc->x)))))
	    return(eval_error(sc, "bad variable ~S in letrec bindings", car(sc->x)));

	  add_to_local_environment(sc, caar(sc->x), sc->UNDEFINED);
	}

      
    case OP_LETREC1:    /* letrec -- calculate parameters */
      sc->args = s7_cons(sc, sc->value, sc->args);
      if (is_pair(sc->code)) 
	{ 
	  if (!is_pair(car(sc->code)))          /* (letrec ((x)) x) -- perhaps this is legal? */
	    return(eval_error(sc, "letrec syntax error (no value?): ~A", car(sc->code)));

	  if (!(is_pair(cdar(sc->code))))       /* (letrec ((x . 1))...) */
	    return(eval_error(sc, "letrec syntax error (not a proper list?): ~A", car(sc->code)));

	  if (cddar(sc->code) != sc->NIL)       /* (letrec ((x 1 2 3)) ...) */
	    return(eval_error(sc, "letrec syntax error (more than one value?): ~A", car(sc->code)));

	  push_stack(sc, OP_LETREC1, sc->args, cdr(sc->code));
	  sc->code = cadar(sc->code);
	  sc->args = sc->NIL;
	  goto EVAL;
	} 

      sc->args = safe_reverse_in_place(sc, sc->args); 
      sc->code = car(sc->args);
      sc->args = cdr(sc->args);
      

    case OP_LETREC2:
      for (sc->x = car(sc->code), sc->y = sc->args; sc->y != sc->NIL; sc->x = cdr(sc->x), sc->y = cdr(sc->y))
	s7_symbol_set_value(sc, caar(sc->x), car(sc->y));
      sc->code = cdr(sc->code);
      sc->args = sc->NIL;
      goto BEGIN;
      
      
    case OP_COND0:
      if (!is_pair(sc->code))
	return(eval_error(sc, "syntax error in cond: ~A", sc->code));
      for (sc->x = sc->code; sc->x != sc->NIL; sc->x = cdr(sc->x))
	if (!is_pair(car(sc->x)))    /* (cond 1) or (cond (#t 1) 3) */
	  return(eval_error(sc, "every clause in cond must be a list: ~A", car(sc->x)));

      push_stack(sc, OP_COND1, sc->NIL, sc->code);
      sc->code = caar(sc->code);
      goto EVAL;
      
      
    case OP_COND1:
      if (is_true(sc, sc->value))     /* got a hit */
	{
	  sc->code = cdar(sc->code);
	  if (sc->code == sc->NIL)
	    {
	      pop_stack(sc);      /* no result clause, so return test, (cond (#t)) -> #t, (cond ((+ 1 2))) -> 3 */
	      goto START;
	    }
	  if (!is_pair(sc->code)) /* (cond (1 . 2)...) */
	    return(eval_error(sc, "syntax error in cond: ~A", sc->code));
	  
	  if (car(sc->code) == sc->FEED_TO) 
	    {
	      if (!is_pair(cdr(sc->code))) 
		return(eval_error(sc, "syntax error in cond: ~A", cdr(sc->code)));

	      sc->x = make_list_2(sc, sc->QUOTE, sc->value); 
	      sc->code = make_list_2(sc, cadr(sc->code), sc->x);
	      goto EVAL;
	    }
	  
	  goto BEGIN;
	}

      sc->code = cdr(sc->code);
      if (sc->code == sc->NIL)
	{
	  sc->value = sc->NIL;
	  pop_stack(sc);
	  goto START;
	} 
	  
      push_stack(sc, OP_COND1, sc->NIL, sc->code);
      sc->code = caar(sc->code);
      goto EVAL;
      
      
    case OP_AND0:
      if (sc->code == sc->NIL) 
	{
	  sc->value = sc->T;
	  pop_stack(sc);
	  goto START;
	}
      if (!is_pair(sc->code))
	return(eval_error(sc, "and: syntax error: ~A", sc->code));
      push_stack(sc, OP_AND1, sc->NIL, cdr(sc->code));
      sc->code = car(sc->code);
      goto EVAL;
      
      
    case OP_AND1:
      if ((is_false(sc, sc->value)) ||
	  (sc->code == sc->NIL))
	{
	  pop_stack(sc);
	  goto START;
	}
      if (!is_pair(sc->code))
	return(eval_error(sc, "and: syntax error: ~A", sc->code));

      if (cdr(sc->code) != sc->NIL)
	push_stack(sc, OP_AND1, sc->NIL, cdr(sc->code));
      sc->code = car(sc->code);
      goto EVAL;
      
      
    case OP_OR0:
      if (sc->code == sc->NIL) 
	{
	  sc->value = sc->F;
	  pop_stack(sc);
	  goto START;
	}
      if (!is_pair(sc->code))
	return(eval_error(sc, "or: syntax error: ~A", sc->code));
      push_stack(sc, OP_OR1, sc->NIL, cdr(sc->code));
      sc->code = car(sc->code);
      goto EVAL;
      
      
    case OP_OR1:
      if ((is_true(sc, sc->value)) ||
	  (sc->code == sc->NIL))
	{
	  pop_stack(sc);
	  goto START;
	}
      if (!is_pair(sc->code))
	return(eval_error(sc, "and: syntax error: ~A", sc->code));

      if (cdr(sc->code) != sc->NIL)
	push_stack(sc, OP_OR1, sc->NIL, cdr(sc->code));
      sc->code = car(sc->code);
      goto EVAL;
      
      
    case OP_MACRO0:     /* this is tinyscheme's weird macro syntax */
      /*
	(macro (when form)
	`(if ,(cadr form) (begin ,@(cddr form))))
      */
      /* (macro (when form) ...) or (macro do (lambda (form) ...))
       *   sc->code is the business after the "macro"
       *   so in 1st case, car(sc->code) is '(when form), and in 2nd it is 'do
       *   in 1st case, put caar(sc->code) "when" into sc->x for later symbol definition, in 2nd use car(sc->code)
       *   in 1st case, wrap up a lambda:
       *      '(lambda (form) ...)
       *   in 2nd case, it's ready to go
       * goto eval popping to OP_MACRO1
       *   eval sees the lambda and creates a closure (s7_make_closure): car => code, cdr => environment
       */
      if (is_pair(car(sc->code))) 
	{
	  sc->x = caar(sc->code);
	  sc->code = s7_cons(sc, sc->LAMBDA, s7_cons(sc, cdar(sc->code), cdr(sc->code)));
	} 
      else 
	{
	  sc->x = car(sc->code);
	  sc->code = cadr(sc->code);
	}
      if (!s7_is_symbol(sc->x)) 
	return(eval_error(sc, "~S: variable is not a symbol", sc->x));

      push_stack(sc, OP_MACRO1, sc->NIL, sc->x);   /* sc->x (the name symbol) will be sc->code when we pop to OP_MACRO1 */
      goto EVAL;
      
      
    case OP_MACRO1:
      /* here sc->code is the name (a symbol), sc->value is a closure object, its car is the form as called
       *     (macro (when form)
       *       `(if ,(cadr form) (begin ,@(cddr form))))
       * has become:
       *     ((form) 
       *      (quasiquote 
       *        (if (unquote (cadr form)) 
       *            (begin (unquote-splicing (cddr form))))))
       * with 
       *   sc->code: when 
       *   sc->value: #<closure> 
       * where "form" is the thing presented to us in the code, i.e. (when mumble do-this)
       *   and the following code takes that as its argument and transforms it in some way
       */

      if (!s7_is_symbol(sc->code))
	return(eval_error(sc, "macro name is not a symbol?", sc->code));

      set_type(sc->value, T_MACRO | T_ANY_MACRO);
      
      /* find name in environment, and define it */
      sc->x = s7_find_symbol_in_environment(sc, sc->envir, sc->code, false); 
      if (sc->x != sc->NIL) 
	set_symbol_value(sc->x, sc->value); 
      else add_to_current_environment(sc, sc->code, sc->value); 
      
      /* pop back to wherever the macro call was */
      sc->x = sc->value;
      sc->value = sc->code;
      pop_stack(sc);
      goto START;
      
      
    case OP_DEFMACRO:
    case OP_DEFMACRO_STAR:
      
      /* (defmacro name (args) body) ->
       *
       *    (macro (defmacro dform)
       *      (let ((form (gensym "defmac")))            
       *        `(macro (,(cadr dform) ,form)   
       *          (apply
       *            (lambda ,(caddr dform)      
       *             ,@(cdddr dform))          
       *            (cdr ,form)))))             
       *    
       *    end up with name as sc->x, args and body as sc->z going to OP_MACRO1, ((gensym) (lambda (args) body)) going to eval
       */

      sc->x = car(sc->code);
      if (!s7_is_symbol(sc->x))
	return(eval_error(sc, "defmacro: ~S is not a symbol?", sc->x)); /* (defmacro 3 (a) #f) */

      if (is_immutable(sc->x))
	return(eval_error(sc, "defmacro: ~S is immutable", sc->x));     /* (defmacro pi (a) `(+ ,a 1)) */

      /* (defmacro hi (a) `(+ ,a 1))
       *   cdr(sc->code): ((a) (quasiquote (+ (unquote a) 1)))
       *   caddr(sc->code):    (quasiquote (+ (unquote a) 1))
       *   cadr(caddr(sc->code):           (+ (unquote a) 1)
       *   g_quasiquote_2(sc, 0, ^):       (cons (quote +) (cons a (cons 1 (quote ()))))
       *
       * so the quasiquote can be evaluated immediately.  It's possible that we could
       *   always precompute quasiquotes, but this change takes care of 99% of the cases.
       */

      if ((is_pair(cdr(sc->code))) &&
	  (is_pair(cddr(sc->code))) &&
	  (is_pair(caddr(sc->code))) &&
	  (s7_is_symbol(caaddr(sc->code))) &&
	  (caaddr(sc->code) == sc->QUASIQUOTE) &&
	  (cdddr(sc->code) == sc->NIL))            /* protect against (defmacro hi (a) `(+ ,a 1) #f) */
	{
	  sc->z = s7_cons(sc,
			  cadr(sc->code),
			  s7_cons(sc,
				  g_quasiquote_2(sc, 0, cadr(caddr(sc->code))),
				  sc->NIL));
	}
      else sc->z = cdr(sc->code);

      /* could we make macros safe automatically by doing the symbol lookups right now?
       *   we'd replace each name with a reference to the current binding cons.  I think
       *   this is how Guile implements hygenic macros -- is it worth the bother?
       *
       * Isn't it just as good to say:
       *
       * (define-macro (mac a b) 
       *   `(with-environment (global-environment)
       *      (+ ,a ,b)))
       *
       * now if we rebind +
       *
       * (let ((+ -)) 
       *   (mac 1 2))
       * 3
       */

      sc->x = car(sc->code);            /* just in case g_quasiquote stepped on sc->x */
      sc->y = s7_gensym(sc, "defmac");
      sc->code = s7_cons(sc, 
			 sc->LAMBDA,
			 s7_cons(sc, 
				 make_list_1(sc, sc->y),
				 make_list_1(sc, 
					     s7_cons(sc, 
						     sc->APPLY,
						     s7_cons(sc, 
							     s7_cons(sc, 
								     (sc->op == OP_DEFMACRO_STAR) ? sc->LAMBDA_STAR : sc->LAMBDA,
								     sc->z),
							     make_list_1(sc, make_list_2(sc, sc->CDR, sc->y)))))));

      /* so, (defmacro hi (a b) `(+ ,a ,b)) becomes:
       *   sc->x: hi
       *   sc->code: (lambda (defmac-21) 
       *               (apply (lambda (a b) 
       *                        (cons (quote +) (cons a (cons b (quote ()))))) 
       *                      (cdr defmac-21)))
       */
      push_stack(sc, OP_MACRO1, sc->NIL, sc->x);   /* sc->x (the name symbol) will be sc->code when we pop to OP_MACRO1 */
      goto EVAL;


    case OP_EXPANSION:
      /* sc->x is the value (sc->value right now is sc->code, the macro name symbol) */
      set_type(sc->x, T_MACRO | T_ANY_MACRO | T_EXPANSION);
      set_type(sc->value, type(sc->value) | T_EXPANSION);
      pop_stack(sc);
      goto START;


    case OP_DEFINE_EXPANSION:
      /* read-time macros, suggested by Rick */
      push_stack(sc, OP_EXPANSION, sc->NIL, sc->NIL);
      /* drop into define-macro */


    case OP_DEFINE_MACRO:
    case OP_DEFINE_MACRO_STAR:
      if (!is_pair(car(sc->code)))
	return(s7_wrong_type_arg_error(sc, "define-macro", 1, car(sc->code), "a list (name ...)"));

      sc->x = caar(sc->code);
      if (!s7_is_symbol(sc->x))
	return(eval_error(sc, "define-macro: ~S is not a symbol?", sc->x));

      if (is_immutable(sc->x))
	return(eval_error(sc, "define-macro: ~S is immutable", sc->x));

      /* (define-macro (hi a) `(+ ,a 1))
       *   cdr(sc->code): ((quasiquote (+ (unquote a) 1)))
       *   so in this case we want cadr, not caddr of defmacro
       */

      if ((is_pair(cdr(sc->code))) &&
	  (is_pair(cadr(sc->code))) &&
	  (s7_is_symbol(caadr(sc->code))) &&
	  (caadr(sc->code) == sc->QUASIQUOTE) &&
	  (cddr(sc->code) == sc->NIL))
	{
	  sc->z = s7_cons(sc,
			  g_quasiquote_2(sc, 0, cadr(cadr(sc->code))),
			  sc->NIL);
	}
      else sc->z = cdr(sc->code);

      sc->x = caar(sc->code); /* just in case g_quasiquote stepped on sc->x */
      sc->y = s7_gensym(sc, "defmac");
      sc->code = s7_cons(sc,
			 sc->LAMBDA,
			 s7_cons(sc, 
				 make_list_1(sc, sc->y),
				 make_list_1(sc, 
					     s7_cons(sc, 
						     sc->APPLY,
						     s7_cons(sc, 
							     s7_cons(sc, 
								     (sc->op == OP_DEFINE_MACRO_STAR) ? sc->LAMBDA_STAR : sc->LAMBDA,
								     s7_cons(sc, 
									     cdar(sc->code), /* arg list */
									     sc->z)),
							     make_list_1(sc, make_list_2(sc, sc->CDR, sc->y)))))));

      /* (define-macro (hi a b) `(+ ,a ,b)) becomes:
       *   sc->x: hi
       *   sc->code: (lambda (defmac-22) (apply (lambda (a b) (cons (quote +) (cons a (cons b (quote ()))))) (cdr defmac-22)))
       */
      push_stack(sc, OP_MACRO1, sc->NIL, sc->x);   /* sc->x (the name symbol) will be sc->code when we pop to OP_MACRO1 */
      goto EVAL;
      
      
    case OP_CASE0:      /* case, car(sc->code) is the selector */
      if ((!is_pair(sc->code)) ||
	  (!is_pair(cdr(sc->code))) ||
	  (!is_pair(cadr (sc->code)))) 
	return(eval_error(sc, "syntax error in case: ~A", sc->code));
      
      push_stack(sc, OP_CASE1, sc->NIL, cdr(sc->code));
      sc->code = car(sc->code);
      goto EVAL;
      
      
    case OP_CASE1: 
      for (sc->x = sc->code; sc->x != sc->NIL; sc->x = cdr(sc->x)) 
	{
	  if ((!is_pair(sc->x)) ||
	      (!is_pair(car(sc->x))))
	    return(eval_error(sc, "case clause ~A messed up", sc->x));	 
   
	  sc->y = caar(sc->x);
	  if (!is_pair(sc->y))
	    {
	      if (sc->y != sc->ELSE)
		return(eval_error(sc, "case clause key list ~A is not a list or 'else'", sc->y));
	      if (cdr(sc->x) != sc->NIL)
		return(eval_error(sc, "case 'else' clause is not the last", sc->x));
	      break;
	    }

	  for ( ; sc->y != sc->NIL; sc->y = cdr(sc->y)) 
	    if (s7_is_eqv(car(sc->y), sc->value)) 
	      break;
	  
	  if (sc->y != sc->NIL) 
	    break;
	}

      if (sc->x != sc->NIL) 
	{
	  if (is_pair(caar(sc->x))) 
	    {
	      sc->code = cdar(sc->x);
	      goto BEGIN;
	    } 
	  push_stack(sc, OP_CASE2, sc->NIL, cdar(sc->x));
	  sc->code = caar(sc->x);
	  goto EVAL;
	} 

      sc->value = sc->NIL;
      pop_stack(sc);
      goto START;
      
      
    case OP_CASE2: 
      if (is_true(sc, sc->value)) 
	goto BEGIN;
      sc->value = sc->NIL;
      pop_stack(sc);
      goto START;
      

    case OP_TRACE_HOOK_QUIT:
      goto APPLY_WITHOUT_TRACE;

      
    case OP_ERROR_HOOK_QUIT:
      s7_symbol_set_value(sc, sc->ERROR_HOOK, sc->code);

      /* now mimic the end of the normal error handler.  Since this error hook evaluation can happen
       *   in an arbitrary s7_call nesting, we can't just return from the current evaluation --
       *   we have to jump to the original (top-level) call.  Otherwise '#<unspecified> or whatever
       *   is simply treated as the (non-error) return value, and the higher level evaluations
       *   get confused.
       */
      stack_reset(sc);
      sc->op = OP_ERROR_QUIT;
      /* sc->value = sc->UNSPECIFIED; */ /* return the *error-hook* function's value if possible */
      if (sc->longjmp_ok)
	{
	  longjmp(sc->goto_start, 1);
	}
      return(sc->value); /* not executed I hope */


    case OP_ERROR_QUIT:
    case OP_QUIT:
    case OP_UNWIND_OUTPUT:
    case OP_UNWIND_INPUT:
      return(sc->F);
      break;
      

    case OP_DYNAMIC_WIND:
      if (dynamic_wind_state(sc->code) == T_DWIND_INIT)
	{
	  dynamic_wind_set_state(sc->code, T_DWIND_BODY);
	  push_stack(sc, OP_DYNAMIC_WIND, sc->NIL, sc->code);
	  sc->args = sc->NIL;
	  sc->code = dynamic_wind_body(sc->code);
	  goto APPLY;
	}
      else
	{
	  if (dynamic_wind_state(sc->code) == T_DWIND_BODY)
	    {
	      dynamic_wind_set_state(sc->code, T_DWIND_FINISH);
	      push_stack(sc, OP_DYNAMIC_WIND, sc->value, sc->code);
	      sc->args = sc->NIL;
	      sc->code = dynamic_wind_out(sc->code);
	      goto APPLY;
	    }
	  else
	    {
	      /* (+ 1 (dynamic-wind (lambda () #f) (lambda () (values 2 3 4)) (lambda () #f)) 5) */
	      if ((is_pair(sc->args)) &&
		  (car(sc->args) == sc->VALUES))
		sc->value = splice_in_values(sc, cdr(sc->args));
	      else sc->value = sc->args;                         /* value saved above */ 
	      pop_stack(sc); 
	      goto START;
	    }
	}
      break;
      

    case OP_CATCH:
      pop_stack(sc);
      goto START;


    case OP_TRACE_RETURN:
      trace_return(sc);
      pop_stack(sc);
      goto START;
      
      
    case OP_READ_LIST: 
      /* sc->args is sc->NIL at first */
      /*    was: sc->args = s7_cons(sc, sc->value, sc->args); */ 
      {
	s7_pointer x;
#if HAVE_PTHREADS
	x = new_cell(sc);
#else
	if (sc->free_heap_top > 0)
	  x = sc->free_heap[--(sc->free_heap_top)];
	else x = new_cell(sc);
#endif
	car(x) = sc->value;
	cdr(x) = sc->args;
	set_type(x, T_PAIR);
	sc->args = x;
      }
      sc->tok = token(sc);

      switch (sc->tok)
	{
	case TOKEN_RIGHT_PAREN:
	  sc->value = remember_line(sc, safe_reverse_in_place(sc, sc->args));

	  /* read-time macro expansion
	   *
	   *   (defmacro hi (a) (format #t "hi...") `(+ ,a 1))
	   *   (define (ho b) (+ 1 (hi b)))
	   *
	   * here sc->value is: (ho b), (hi b), (+ 1 (hi b)), (define (ho b) (+ 1 (hi b)))
	   * 
	   * but... first we can't tell for sure at this point that "hi" really is a macro
	   *
	   *   (letrec ((hi ... (hi...))) will be confused about the 2nd hi,
	   *   or (call/cc (lambda (hi) (hi 1))) etc.
	   *
	   * second, figuring out that we're quoted is not easy -- we have to march all the
	   * way to the bottom of the stack looking for op_read_quote or op_read_vector
	   *
	   *    #(((hi)) 2) or '(((hi)))
	   *
	   * or op_read_list with args not equal (quote) or (macroexapand)
	   *
	   *    '(hi 3) or (macroexpand (hi 3) or (quote (hi 3))
	   *
	   * and those are only the problems I noticed!
	   *
	   * The hardest of these problems involve shadowing, so Rick asked for "define-expansion"
	   *   which is just like define-macro, but the programmer guarantees that the macro
	   *   name will not be shadowed.  So I'll also assume that the other funny cases are
	   *   being avoided -- see what happens!
	   *
	   *   (define-expansion (hi a) `(+ ,a 1))
	   *   (define (ho b) (+ 1 (hi b)))
	   *   (procedure-source ho) -> (lambda (b) (+ 1 (+ b 1)))
	   */
	  
	  if ((sc->value != sc->NIL) &&
	      (is_expansion(car(sc->value))) &&
	      (sc->stack_top >= 4) &&
	      ((int)integer(number(vector_element(sc->stack, sc->stack_top - 1))) != OP_READ_QUOTE) && /* '(hi 1) for example */
	      (car(vector_element(sc->stack, sc->stack_top - 2)) != sc->QUOTE) &&                      /* (quote (hi 1)) */
	      (car(vector_element(sc->stack, sc->stack_top - 2)) != sc->MACROEXPAND))                  /* (macroexpand (hi 1)) */
	    {
	      s7_pointer x;
	      x = symbol_value(s7_find_symbol_in_environment(sc, sc->envir, car(sc->value), true));
	      sc->args = make_list_1(sc, sc->value); 
	      sc->code = x;
	      goto APPLY;
	    }
	  break;

	case TOKEN_DOT:
	  push_stack(sc, OP_READ_DOT, sc->args, sc->NIL);
	  sc->tok = token(sc);
	  sc->value = read_expression(sc);
	  break;

	case TOKEN_EOF:
	  /* we should be able to scan the stack for the earlist OP_READ_LIST,
	   *  and find where the current list started
	   */
	  return(missing_close_paren_error(sc));

	default:
	  push_stack(sc, OP_READ_LIST, sc->args, sc->NIL);
	  sc->value = read_expression(sc);
	  break;
	}
      pop_stack(sc);
      goto START;

      
    case OP_READ_DOT:
      if (token(sc) != TOKEN_RIGHT_PAREN)
	{
	  back_up_stack(sc);
	  sc->value = read_error(sc, "stray dot?");            /* (+ 1 . 2 3) or (list . ) */
	  goto START;
	}
      /* args = previously read stuff, value = thing just after the dot and before the ')':
       *   (list 1 2 . 3)
       *   value: 3, args: (2 1 list) 
       *   '(1 . 2)
       *   value: 2, args: (1)
       *
       * but we also get here in a lambda arg list:
       *   (lambda (a b . c) #f)
       *   value: c, args: (b a)
       *
       * so we have to leave any error checks until later, I guess
       *   -- in eval_args1, if we end with non-pair-not-nil then
       *      something is fishy
       */
      sc->value = s7_reverse_in_place(sc, sc->value, sc->args);
      pop_stack(sc);
      goto START;
      
      
    case OP_READ_QUOTE:
      sc->value = make_list_2(sc, sc->QUOTE, sc->value);
      pop_stack(sc);
      goto START;      
      
      
    case OP_READ_QUASIQUOTE:
      sc->value = make_list_2(sc, sc->QUASIQUOTE, sc->value);
      pop_stack(sc);
      goto START;
      
      
    case OP_READ_QUASIQUOTE_VECTOR:
      sc->value = make_list_3(sc, sc->APPLY, sc->VECTOR, make_list_2(sc, sc->QUASIQUOTE, sc->value));
      pop_stack(sc);
      goto START;
      
      
    case OP_READ_UNQUOTE:
      sc->value = make_list_2(sc, sc->UNQUOTE, sc->value);
      pop_stack(sc);
      goto START;
      
      
    case OP_READ_UNQUOTE_SPLICING:
      sc->value = make_list_2(sc, sc->UNQUOTE_SPLICING, sc->value);
      pop_stack(sc);
      goto START;
      
      
    case OP_READ_VECTOR:
      sc->value = g_vector(sc, sc->value);
      set_immutable(sc->value); /* a vector constant should be immutable? */
      pop_stack(sc);
      goto START;

      
    default:
      return(eval_error(sc, "~A: unknown operator!", s7_make_integer(sc, sc->op)));
    }
  return(sc->F);
}


/* -------------------------------- threads -------------------------------- */

#if HAVE_PTHREADS

static s7_scheme *clone_s7(s7_scheme *sc, s7_pointer vect)
{
  int i;
  s7_scheme *new_sc;

  /* make_thread which calls us has grabbed alloc_lock for the duration */

  new_sc = (s7_scheme *)malloc(sizeof(s7_scheme));
  memcpy((void *)new_sc, (void *)sc, sizeof(s7_scheme));
  
  /* share the heap, symbol table and global environment, protected objects list, and all the startup stuff (all via the memcpy),
   *   but have separate stacks and eval locals
   */
  
  new_sc->longjmp_ok = false;
  new_sc->strbuf_size = INITIAL_STRBUF_SIZE;
  new_sc->strbuf = (char *)calloc(new_sc->strbuf_size, sizeof(char));

  new_sc->read_line_buf = NULL;
  new_sc->read_line_buf_size = 0;
  
  new_sc->stack_top = 0;
  new_sc->stack = vect;
  new_sc->stack_size = INITIAL_STACK_SIZE;
  new_sc->stack_size2 = new_sc->stack_size / 2;
  
  new_sc->x = new_sc->NIL;
  new_sc->y = new_sc->NIL;
  new_sc->z = new_sc->NIL;
  new_sc->code = new_sc->NIL;
  new_sc->args = new_sc->NIL;
  new_sc->value = new_sc->NIL;
  new_sc->cur_code = ERROR_INFO_DEFAULT;

#if WITH_ENCAPSULATION
  new_sc->encapsulators = sc->NIL;
#endif
  
  new_sc->temps_size = GC_TEMPS_SIZE;
  new_sc->temps_ctr = 0;
  new_sc->temps = (s7_pointer *)malloc(new_sc->temps_size * sizeof(s7_pointer));
  for (i = 0; i < new_sc->temps_size; i++)
    new_sc->temps[i] = new_sc->NIL;

  new_sc->circular_refs = (s7_pointer *)calloc(CIRCULAR_REFS_SIZE, sizeof(s7_pointer));
#if HAVE_PTHREADS
  new_sc->key_values = sc->NIL;

  (*(sc->thread_ids))++;                   /* in case a spawned thread spawns another, we need this variable to be global to all */
  new_sc->thread_id = (*(sc->thread_ids)); /* for more readable debugging printout -- main thread is thread 0 */
#endif

  new_sc->default_rng = NULL;
#if WITH_GMP
  new_sc->default_big_rng = NULL;
#endif

  return(new_sc);
}


static s7_scheme *close_s7(s7_scheme *sc)
{
  free(sc->strbuf);
  free(sc->temps);
  if (sc->read_line_buf) free(sc->read_line_buf);
  if (sc->default_rng) free(sc->default_rng);
#if WITH_GMP
  if (sc->default_big_rng) free(sc->default_big_rng);
#endif
  free(sc);
  return(NULL);
}


static void mark_s7(s7_scheme *sc)
{
  int i;
  S7_MARK(sc->args);
  S7_MARK(sc->envir);
  S7_MARK(sc->code);
  mark_vector(sc->stack, sc->stack_top);
  S7_MARK(sc->value);
  S7_MARK(sc->x);
  S7_MARK(sc->y);
  S7_MARK(sc->z);
  for (i = 0; i < sc->temps_size; i++)
    S7_MARK(sc->temps[i]);
#if HAVE_PTHREADS
  S7_MARK(sc->key_values);
#endif
}


static int thread_tag = 0;


static char *thread_print(s7_scheme *sc, void *obj)
{
  char *buf;
  thred *p = (thred *)obj;
  buf = (char *)malloc(32 * sizeof(char));
  snprintf(buf, 32, "#<thread %d>", p->sc->thread_id);
  return(buf);
}


static void thread_free(void *obj)
{
  thred *f = (thred *)obj;
  if (f)
    {
      /* pthread_detach(*(f->thread)); */
      free(f->thread);
      f->thread = NULL;
      f->sc = close_s7(f->sc);
      free(f);
    }
}


static void thread_mark(void *val)
{
  thred *f = (thred *)val;
  if ((f) && (f->sc)) /* possibly still in make_thread */
    {
      mark_s7(f->sc);
      S7_MARK(f->func);
    }
}


static bool thread_equal(void *obj1, void *obj2)
{
  return(obj1 == obj2);
}


static void *run_thread_func(void *obj)
{
  thred *f = (thred *)obj;
  return((void *)s7_call(f->sc, f->func, f->sc->NIL));
}


/* (define hi (make-thread (lambda () (display "hi")))) */

static s7_pointer g_make_thread(s7_scheme *sc, s7_pointer args)
{
  #define H_make_thread "(make-thread thunk) creates a new thread running thunk"
  thred *f;
  s7_pointer obj, vect, frame;

  if (!is_procedure(car(args)))
    return(s7_wrong_type_arg_error(sc, "make-thread", 0, car(args), "a thunk"));
  
  pthread_mutex_lock(&alloc_lock); /* if currently in GC in some thread, wait for it */
  pthread_mutex_unlock(&alloc_lock);
  
  frame = s7_immutable_cons(sc, sc->NIL, sc->envir);
  vect = s7_make_vector(sc, INITIAL_STACK_SIZE);
  
  f = (thred *)calloc(1, sizeof(thred));
  f->func = car(args);
  
  obj = s7_make_object(sc, thread_tag, (void *)f);
  
  pthread_mutex_lock(&alloc_lock);

  f->sc = clone_s7(sc, vect);
  f->sc->envir = frame;
  f->sc->y = obj;
  f->thread = (pthread_t *)malloc(sizeof(pthread_t));

  pthread_create(f->thread, NULL, run_thread_func, (void *)f);
  pthread_mutex_unlock(&alloc_lock);
  
  return(obj);
}


static s7_pointer g_is_thread(s7_scheme *sc, s7_pointer args)
{
  #define H_is_thread "(thread? obj) returns #t if obj is a thread object"
  return(make_boolean(sc, (is_c_object(car(args))) && (c_object_type(car(args)) == thread_tag)));
}


static s7_pointer g_join_thread(s7_scheme *sc, s7_pointer args)
{
  #define H_join_thread "(join-thread thread) causes the current thread to wait for the thread to finish"
  thred *f;
  if (g_is_thread(sc, args) == sc->F)
    return(s7_wrong_type_arg_error(sc, "join-thread", 0, car(args), "a thread"));
  
  f = (thred *)s7_object_value(car(args));
  pthread_join(*(f->thread), NULL);
  return(car(args));
}


static s7_pointer thread_environment(s7_scheme *sc, s7_pointer obj)
{
  thred *f;
  f = (thred *)s7_object_value(obj);
  if ((f) && (f->sc) && (f->sc->envir))
    return(f->sc->envir);
  return(sc->NIL);
}


/* -------- locks -------- */

static int lock_tag = 0;


static char *lock_print(s7_scheme *sc, void *obj)
{
  char *buf;
  buf = (char *)malloc(64 * sizeof(char));
  snprintf(buf, 64, "#<lock %p>", obj);
  return(buf);
}


static void lock_free(void *obj)
{
  pthread_mutex_t *lock = (pthread_mutex_t *)obj;
  if (lock)
    {
      pthread_mutex_destroy(lock);
      free(lock);
    }
}


static bool lock_equal(void *obj1, void *obj2)
{
  return(obj1 == obj2);
}


static s7_pointer g_is_lock(s7_scheme *sc, s7_pointer args)
{
  #define H_is_lock "(lock? obj) returns #t if obj is a lock (mutex) object"
  return(make_boolean(sc, (is_c_object(car(args))) && (c_object_type(car(args)) == lock_tag)));
}


static s7_pointer g_make_lock(s7_scheme *sc, s7_pointer args)
{
  #define H_make_lock "(make-lock) creates a new lock (mutex variable)"
  pthread_mutex_t *lock;
  lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(lock, NULL);
  return(s7_make_object(sc, lock_tag, (void *)lock));  
}


static s7_pointer g_grab_lock(s7_scheme *sc, s7_pointer args)
{
  #define H_grab_lock "(grab-lock lock) stops the current thread until it can grab the lock."
  if (g_is_lock(sc, args) == sc->F)
    return(s7_wrong_type_arg_error(sc, "grab-lock", 0, car(args), "a lock (mutex)"));

  return(s7_make_integer(sc, pthread_mutex_lock((pthread_mutex_t *)s7_object_value(car(args)))));
}


static s7_pointer g_release_lock(s7_scheme *sc, s7_pointer args)
{
  #define H_release_lock "(release-lock lock) releases the lock"
  if (g_is_lock(sc, args) == sc->F)
    return(s7_wrong_type_arg_error(sc, "release-lock", 0, car(args), "a lock (mutex)"));

  return(s7_make_integer(sc, pthread_mutex_unlock((pthread_mutex_t *)s7_object_value(car(args)))));
}



/* -------- thread variables (pthread keys) -------- */

static int key_tag = 0;

static void *key_value(s7_pointer obj)
{
  pthread_key_t *key; 
  key = (pthread_key_t *)s7_object_value(obj);
  return(pthread_getspecific(*key));                  /* returns NULL if never set */
}


static char *key_print(s7_scheme *sc, void *obj)
{
  char *buf, *val_str;
  s7_pointer val;
  void *kval;
  int len;
  kval = key_value((s7_pointer)obj);
  if (kval)
    val = (s7_pointer)kval;
  else val = sc->F;
  val_str = s7_object_to_c_string(sc, val);
  len = 64 + safe_strlen(val_str);
  buf = (char *)malloc(len * sizeof(char));
  snprintf(buf, len, "#<[thread %d] key: %s>", sc->thread_id, val_str);
  if (val_str) free(val_str);
  return(buf);
}


static void key_free(void *obj)
{
  pthread_key_t *key = (pthread_key_t *)obj;
  if (key)
    {
      pthread_key_delete(*key);
      free(key);
    }
}


static bool key_equal(void *obj1, void *obj2)
{
  return(obj1 == obj2);
}


bool s7_is_thread_variable(s7_pointer obj)
{
  return((is_c_object(obj)) &&
	 (c_object_type(obj) == key_tag));
}


s7_pointer s7_thread_variable_value(s7_scheme *sc, s7_pointer obj)
{
  void *val;
  val = key_value(obj);
  if (val)
    return((s7_pointer)val);
  return(sc->F);
}


static s7_pointer g_is_thread_variable(s7_scheme *sc, s7_pointer args)
{
  #define H_is_thread_variable "(thread-variable? obj) returns #t if obj is a thread variable (a pthread key)"
  return(make_boolean(sc, s7_is_thread_variable(car(args))));
}


static s7_pointer g_make_thread_variable(s7_scheme *sc, s7_pointer args)
{
  #define H_make_thread_variable "(make-thread-variable) returns a new thread specific variable (a pthread key)"
  pthread_key_t *key;
  key = (pthread_key_t *)malloc(sizeof(pthread_key_t));
  pthread_key_create(key, NULL);
  return(s7_make_object(sc, key_tag, (void *)key));  
}


static s7_pointer get_key(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  if (args != sc->NIL)
    return(s7_error(sc, sc->WRONG_NUMBER_OF_ARGS,
		    make_list_3(sc, s7_make_string(sc, "thread variable is a function of no arguments: ~A ~A"),	obj, args)));
  return(s7_thread_variable_value(sc, obj));
}


static s7_pointer set_key(s7_scheme *sc, s7_pointer obj, s7_pointer args)
{
  pthread_key_t *key;  
  key = (pthread_key_t *)s7_object_value(obj);
  pthread_setspecific(*key, (void *)car(args)); 

  /* to protect from the GC until either the local key value is set again, or the thread is done,
   *   we store the key's local value in an alist '(obj value)
   */
  {
    s7_pointer curval;
    curval = g_assq(sc, make_list_2(sc, obj, sc->key_values));
    if (curval == sc->F)
      sc->key_values = s7_cons(sc, s7_cons(sc, obj, car(args)), sc->key_values);
    else cdr(curval) = car(args);
  }
  return(car(args));
}


#endif




/* -------------------------------- multiprecision arithmetic -------------------------------- */

#if WITH_GMP

static mp_prec_t mpc_precision = 128;
static mp_prec_t mpc_set_default_precision(mp_prec_t prec) {mpc_precision = prec; return(prec);}

#define mpc_init(Z) mpc_init2(Z, mpc_precision)

static void mpc_init_set(mpc_ptr z, mpc_ptr y, mpc_rnd_t rnd)
{
  mpc_init(z);
  mpc_set(z, y, rnd);
}


static s7_pointer promote_number(s7_scheme *sc, int type, s7_pointer x);

static int big_integer_tag = 0;
static int big_ratio_tag = 0;
static int big_real_tag = 0;
static int big_complex_tag = 0;

#define BIG_INTEGER(a) (*((mpz_t *)a))
#define BIG_RATIO(a) (*((mpq_t *)a))
#define BIG_REAL(a) (*((mpfr_t *)a))
#define BIG_COMPLEX(a) (*((mpc_t *)a))

#define S7_BIG_INTEGER(a) (*((mpz_t *)s7_object_value(a)))
#define S7_BIG_RATIO(a) (*((mpq_t *)s7_object_value(a)))
#define S7_BIG_REAL(a) (*((mpfr_t *)s7_object_value(a)))
#define S7_BIG_COMPLEX(a) (*((mpc_t *)s7_object_value(a)))

#define IS_BIG(p) ((is_c_object(p)) && \
		   ((c_object_type(p) == big_integer_tag) || \
                    (c_object_type(p) == big_ratio_tag) || \
                    (c_object_type(p) == big_real_tag) || \
                    (c_object_type(p) == big_complex_tag)))


mpfr_t *s7_big_real(s7_pointer x)
{
  return((mpfr_t *)s7_object_value(x));
}


mpz_t *s7_big_integer(s7_pointer x)
{
  return((mpz_t *)s7_object_value(x));
}


mpq_t *s7_big_ratio(s7_pointer x)
{
  return((mpq_t *)s7_object_value(x));
}


mpc_t *s7_big_complex(s7_pointer x)
{
  return((mpc_t *)s7_object_value(x));
}


bool s7_is_number(s7_pointer p)
{
  return((type(p) == T_NUMBER) || (IS_BIG(p)));
}


bool s7_is_integer(s7_pointer p) 
{ 
  return(((type(p) == T_NUMBER) && 
	  (number_type(p) == NUM_INT)) ||
	 ((is_c_object(p)) && 
	  (c_object_type(p) == big_integer_tag)));
}


bool s7_is_real(s7_pointer p) 
{ 
  return(((type(p) == T_NUMBER) && 
	  (number_type(p) < NUM_COMPLEX)) ||
	 ((is_c_object(p)) && 
	  ((c_object_type(p) == big_integer_tag) ||
	   (c_object_type(p) == big_ratio_tag) ||
	   (c_object_type(p) == big_real_tag))));
}


bool s7_is_rational(s7_pointer p)
{
  return(((type(p) == T_NUMBER) && 
	  (number_type(p) <= NUM_RATIO)) ||
	 ((is_c_object(p)) && 
	  ((c_object_type(p) == big_integer_tag) ||
	   (c_object_type(p) == big_ratio_tag))));
}


bool s7_is_ratio(s7_pointer p)
{
  return(((type(p) == T_NUMBER) && 
	  (number_type(p) == NUM_RATIO)) ||
	 ((is_c_object(p)) && 
	   (c_object_type(p) == big_ratio_tag)));
}


bool s7_is_complex(s7_pointer p)
{
  return(s7_is_number(p));
}


static char *print_big_integer(s7_scheme *sc, void *val)
{
  return(mpz_get_str(NULL, 10, BIG_INTEGER(val)));
}


static char *print_big_ratio(s7_scheme *sc, void *val)
{
  return(mpq_get_str(NULL, 10, BIG_RATIO(val)));
}


static char *mpfr_to_string(mpfr_t val, int radix)
{
  char *str, *tmp;
  mp_exp_t expptr;
  int i, len;

  if (mpfr_zero_p(val))
    return(s7_strdup("0.0"));

  str = mpfr_get_str(NULL, &expptr, radix, 0, val, GMP_RNDN);
  len = safe_strlen(str);

  /* remove trailing 0's */
  for (i = len - 1; i > 3; i--)
    if (str[i] != '0')
      break;
  if (i < len - 1)
    str[i + 1] = '\0';

  len += 64;
  tmp = (char *)malloc(len * sizeof(char));
  
  if (str[0] == '-')
    snprintf(tmp, len, "-%c.%sE%d", str[1], (char *)(str + 2), (int)expptr - 1);
  else snprintf(tmp, len, "%c.%sE%d", str[0], (char *)(str + 1), (int)expptr - 1);
  mpfr_free_str(str);
  return(tmp);
}


static char *print_big_real(s7_scheme *sc, void *val)
{
  return(mpfr_to_string(BIG_REAL(val), 10));
}


static char *mpc_to_string(mpc_t val, int radix)
{
  char *rl, *im, *tmp;
  int len;
  mpfr_t r;
  mpfr_init(r);
  mpc_real(r, val, GMP_RNDN);
  rl = mpfr_to_string(r, radix);
  mpc_imag(r, val, GMP_RNDN);
  im = mpfr_to_string(r, radix);
  len = safe_strlen(rl) + safe_strlen(im) + 128;
  tmp = (char *)malloc(len * sizeof(char));
  snprintf(tmp, len, "%s%s%si", rl, (im[0] == '-') ? "" : "+", im);
  free(rl);
  free(im);
  return(tmp);
}


static char *print_big_complex(s7_scheme *sc, void *val)
{
  return(mpc_to_string(BIG_COMPLEX(val), 10));
}


static char *big_number_to_string_with_radix(s7_pointer p, int radix)
{
  if (c_object_type(p) == big_integer_tag)
    return(mpz_get_str(NULL, radix, S7_BIG_INTEGER(p)));
  if (c_object_type(p) == big_ratio_tag)
    return(mpq_get_str(NULL, radix, S7_BIG_RATIO(p)));
  if (c_object_type(p) == big_real_tag)
    return(mpfr_to_string(S7_BIG_REAL(p), radix));
  return(mpc_to_string(S7_BIG_COMPLEX(p), radix));
}


static void free_big_integer(void *val)
{
  mpz_clear(BIG_INTEGER(val));
  free(val);
}


static void free_big_ratio(void *val)
{
  mpq_clear(BIG_RATIO(val));
  free(val);
}


static void free_big_real(void *val)
{
  mpfr_clear(BIG_REAL(val));
  free(val);
}


static void free_big_complex(void *val)
{
  mpc_clear(BIG_COMPLEX(val));
  free(val);
}


static bool equal_big_integer(void *val1, void *val2)
{
  return(mpz_cmp(BIG_INTEGER(val1), BIG_INTEGER(val2)) == 0);
}


static bool equal_big_ratio(void *val1, void *val2)
{
  return(mpq_cmp(BIG_RATIO(val1), BIG_RATIO(val2)) == 0);
}


static bool equal_big_real(void *val1, void *val2)
{
  return(mpfr_cmp(BIG_REAL(val1), BIG_REAL(val2)) == 0);
}


static bool equal_big_complex(void *val1, void *val2)
{
  return(mpc_cmp(BIG_COMPLEX(val1), BIG_COMPLEX(val2)) == 0);
}


static s7_pointer string_to_big_integer(s7_scheme *sc, const char *str, int radix)
{
  mpz_t *n;
  n = (mpz_t *)malloc(sizeof(mpz_t));
  mpz_init_set_str(*n, str, radix);
  return(s7_make_object(sc, big_integer_tag, (void *)n));
}


static s7_pointer mpz_to_big_integer(s7_scheme *sc, mpz_t val)
{
  mpz_t *n;
  n = (mpz_t *)malloc(sizeof(mpz_t));
  mpz_init_set(*n, val);
  return(s7_make_object(sc, big_integer_tag, (void *)n));
}


s7_pointer s7_make_big_integer(s7_scheme *sc, mpz_t *val)
{
  return(mpz_to_big_integer(sc, *val));
}


s7_pointer copy_big_integer(s7_scheme *sc, s7_pointer obj)
{
  return(s7_make_big_integer(sc, s7_big_integer(obj)));
}


static s7_pointer string_to_big_ratio(s7_scheme *sc, const char *str, int radix)
{
  mpq_t *n;
  n = (mpq_t *)malloc(sizeof(mpq_t));
  mpq_init(*n);
  mpq_set_str(*n, str, radix);
  mpq_canonicalize(*n);
  if (mpz_cmp_ui(mpq_denref(*n), 1) == 0)
    {
      s7_pointer result;
      result = mpz_to_big_integer(sc, mpq_numref(*n));
      mpq_clear(*n);
      free(n);
      return(result);
    }
  return(s7_make_object(sc, big_ratio_tag, (void *)n));
}


static s7_pointer mpq_to_big_ratio(s7_scheme *sc, mpq_t val)
{
  mpq_t *n;
  n = (mpq_t *)malloc(sizeof(mpq_t));
  mpq_init(*n);
  mpq_set_num(*n, mpq_numref(val));
  mpq_set_den(*n, mpq_denref(val));
  return(s7_make_object(sc, big_ratio_tag, (void *)n));
}


s7_pointer s7_make_big_ratio(s7_scheme *sc, mpq_t *val)
{
  return(mpq_to_big_ratio(sc, *val));
}


s7_pointer copy_big_ratio(s7_scheme *sc, s7_pointer obj)
{
  return(s7_make_big_ratio(sc, s7_big_ratio(obj)));
}


static s7_pointer mpz_to_big_ratio(s7_scheme *sc, mpz_t val)
{
  mpq_t *n;
  n = (mpq_t *)malloc(sizeof(mpq_t));
  mpq_init(*n);
  mpq_set_num(*n, val);
  return(s7_make_object(sc, big_ratio_tag, (void *)n));
}


static s7_pointer make_big_integer_or_ratio(s7_scheme *sc, s7_pointer z)
{
  if (mpz_cmp_ui(mpq_denref(S7_BIG_RATIO(z)), 1) == 0)
    return(mpz_to_big_integer(sc, mpq_numref(S7_BIG_RATIO(z))));
  return(z);
}


static s7_pointer string_to_big_real(s7_scheme *sc, const char *str, int radix)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  mpfr_init_set_str(*n, str, radix, GMP_RNDN);
  return(s7_make_object(sc, big_real_tag, (void *)n));
}


static s7_pointer s7_number_to_big_real(s7_scheme *sc, s7_pointer p)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  switch (number_type(p))
    {
    case NUM_INT: 
      {
	s7_Int val;
	val = s7_integer(p);
	if ((val <= LONG_MAX) && (val >= LONG_MIN))
	  mpfr_init_set_si(*n, (int)val, GMP_RNDN);
	else mpfr_init_set_ld(*n, (long double)val, GMP_RNDN);
      }
      break;

    case NUM_RATIO:
      mpfr_init_set_d(*n, fraction(number(p)), GMP_RNDN);
      break;

    default:
      mpfr_init_set_d(*n, s7_real(p), GMP_RNDN);
      break;
    }
  return(s7_make_object(sc, big_real_tag, (void *)n));
}


static s7_pointer mpz_to_big_real(s7_scheme *sc, mpz_t val)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  mpfr_init_set_z(*n, val, GMP_RNDN);
  return(s7_make_object(sc, big_real_tag, (void *)n));
}


static s7_pointer mpq_to_big_real(s7_scheme *sc, mpq_t val)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  mpfr_init_set_q(*n, val, GMP_RNDN);
  return(s7_make_object(sc, big_real_tag, (void *)n));
}


static s7_pointer mpfr_to_big_real(s7_scheme *sc, mpfr_t val)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  mpfr_init_set(*n, val, GMP_RNDN);
  return(s7_make_object(sc, big_real_tag, (void *)n));
}


s7_pointer s7_make_big_real(s7_scheme *sc, mpfr_t *val)
{
  return(mpfr_to_big_real(sc, *val));
}


s7_pointer copy_big_real(s7_scheme *sc, s7_pointer obj)
{
  return(s7_make_big_real(sc, s7_big_real(obj)));
}


static s7_pointer big_pi(s7_scheme *sc)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  mpfr_init(*n);
  mpfr_const_pi(*n, GMP_RNDN);
  return(s7_make_object(sc, big_real_tag, (void *)n));
}


static s7_pointer s7_number_to_big_complex(s7_scheme *sc, s7_pointer p)
{
  mpc_t *n;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  switch (number_type(p))
    {
    case NUM_INT: 
      {
	s7_Int val;
	val = s7_integer(p);
	if ((val <= LONG_MAX) && (val >= LONG_MIN))
	  mpc_set_si(*n, (int)val, MPC_RNDNN);
	else mpc_set_d(*n, (double)val, MPC_RNDNN);
      }
      break;

    case NUM_RATIO:
      mpc_set_d(*n, fraction(number(p)), MPC_RNDNN);
      break;

    case NUM_REAL:
    case NUM_REAL2:
      mpc_set_d(*n, s7_real(p), MPC_RNDNN);
      break;

    default:
      mpc_set_d_d(*n, s7_real_part(p), s7_imag_part(p), MPC_RNDNN);
      break;
    }
  return(s7_make_object(sc, big_complex_tag, (void *)n));
}


static s7_pointer make_big_real_or_complex(s7_scheme *sc, s7_pointer z)
{
  if (mpfr_cmp_ui(mpc_imagref(S7_BIG_COMPLEX(z)), 0) == 0)
    {
      mpfr_t *n;
      n = (mpfr_t *)malloc(sizeof(mpfr_t));
      mpfr_init_set(*n, mpc_realref(S7_BIG_COMPLEX(z)), GMP_RNDN);
      return(s7_make_object(sc, big_real_tag, (void *)n));
    }
  return(z);
}


static s7_pointer mpz_to_big_complex(s7_scheme *sc, mpz_t val)
{
  mpc_t *n;
  mpfr_t temp;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  mpfr_init_set_z(temp, val, GMP_RNDN);
  mpc_set_fr(*n, temp, MPC_RNDNN);
  mpfr_clear(temp);
  return(s7_make_object(sc, big_complex_tag, (void *)n));
}


static s7_pointer mpq_to_big_complex(s7_scheme *sc, mpq_t val)
{
  mpc_t *n;
  mpfr_t temp;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  mpfr_init_set_q(temp, val, GMP_RNDN);
  mpc_set_fr(*n, temp, MPC_RNDNN);
  mpfr_clear(temp);
  return(s7_make_object(sc, big_complex_tag, (void *)n));
}


static s7_pointer mpfr_to_big_complex(s7_scheme *sc, mpfr_t val)
{
  mpc_t *n;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  mpc_set_fr(*n, val, MPC_RNDNN);
  return(s7_make_object(sc, big_complex_tag, (void *)n));
}


static s7_pointer mpc_to_big_complex(s7_scheme *sc, mpc_t val)
{
  mpc_t *n;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  mpc_set(*n, val, MPC_RNDNN);
  return(s7_make_object(sc, big_complex_tag, (void *)n));
}


s7_pointer s7_make_big_complex(s7_scheme *sc, mpc_t *val)
{
  return(mpc_to_big_complex(sc, *val));
}


s7_pointer copy_big_complex(s7_scheme *sc, s7_pointer obj)
{
  return(s7_make_big_complex(sc, s7_big_complex(obj)));
}


static s7_pointer make_big_complex(s7_scheme *sc, mpfr_t rl, mpfr_t im)
{
  /* there is no mpc_get_str equivalent, so we need to split up str,
   *   use make_big_real to get the 2 halves, then mpc_init, then
   *   mpc_set_fr_fr.
   */
  mpc_t *n;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  mpc_set_fr_fr(*n, rl ,im, MPC_RNDNN);
  return(s7_make_object(sc, big_complex_tag, (void *)n));
}


static void mpz_init_set_s7_Int(mpz_t n, s7_Int uval)
{
  if ((uval <= LONG_MAX) && (uval >= LONG_MIN))
    mpz_init_set_si(n, (int)uval);
  else
    { /* long long int to gmp mpz_t */
      bool need_sign;
      long long int val;
      val = (long long int)uval;
      /* handle one special case (sigh) */
      if (val == LLONG_MIN)
	mpz_init_set_str(n, "-9223372036854775808", 10);
      else
	{
	  need_sign = (val < 0);
	  if (need_sign) val = -val;
	  mpz_init_set_si(n, val >> 32);
	  mpz_mul_2exp(n, n, 32);
	  mpz_add_ui(n, n, (int)(val & 0xffffffff));
	  if (need_sign) mpz_neg(n, n);
	}
    }
}


static s7_pointer s7_Int_to_big_integer(s7_scheme *sc, s7_Int val)
{
  mpz_t *n;
  n = (mpz_t *)malloc(sizeof(mpz_t));
  mpz_init_set_s7_Int(*n, val);
  return(s7_make_object(sc, big_integer_tag, (void *)n));
}


static s7_Int big_integer_to_s7_Int(mpz_t n)
{
  long long int high, low;
  mpz_t x;
  bool need_sign = false;

  if (mpz_fits_sint_p(n))
    return((s7_Int)mpz_get_si(n));

  mpz_init_set(x, n);
  if (mpz_cmp_ui(x, 0) < 0)
    {
      need_sign = true;
      mpz_neg(x, x);
    }
  low = mpz_get_ui(x);
  mpz_fdiv_q_2exp(x, x, 32);
  high = mpz_get_ui(x);
  mpz_clear(x);
  if (need_sign)
    return(-(low + (high << 32)));
  return(low + (high << 32));
}


s7_Double s7_number_to_real(s7_pointer x)
{
  if (is_c_object(x))
    {
      if (c_object_type(x) == big_real_tag)
	return((s7_Double)mpfr_get_d(S7_BIG_REAL(x), GMP_RNDN));
      if (c_object_type(x) == big_integer_tag)
	return((s7_Double)big_integer_to_s7_Int(S7_BIG_INTEGER(x)));
      if (c_object_type(x) == big_ratio_tag)
	return((s7_Double)((double)big_integer_to_s7_Int(mpq_numref(S7_BIG_RATIO(x))) / (double)big_integer_to_s7_Int(mpq_denref(S7_BIG_RATIO(x)))));
    }

  switch (number_type(x))
    {
    case NUM_INT:   return((s7_Double)s7_integer(x));
    case NUM_RATIO: return((s7_Double)s7_numerator(x) / (s7_Double)s7_denominator(x));
    case NUM_REAL:
    case NUM_REAL2: return(s7_real(x));
    default:        return(s7_real_part(x));
    }
}


s7_Int s7_numerator(s7_pointer x)
{
  if (is_c_object(x))
    {
      if (c_object_type(x) == big_ratio_tag)
	return(big_integer_to_s7_Int(mpq_numref(S7_BIG_RATIO(x))));
      if (c_object_type(x) == big_integer_tag)
	return(big_integer_to_s7_Int(S7_BIG_INTEGER(x)));
    }
  if (number_type(x) == NUM_RATIO)
    return(numerator(number(x)));
  return(integer(number(x)));
}


s7_Int s7_denominator(s7_pointer x)
{
  if (is_c_object(x))
    {
      if (c_object_type(x) != big_ratio_tag)
	return(1);
      return(big_integer_to_s7_Int(mpq_denref(S7_BIG_RATIO(x))));
    }
  if (number_type(x) == NUM_RATIO)
    return(denominator(number(x)));
  return(1);
}


s7_Double s7_real_part(s7_pointer x)
{
  if (is_c_object(x))
    {
      if (c_object_type(x) == big_complex_tag)
	return((s7_Double)mpfr_get_d(mpc_realref(S7_BIG_COMPLEX(x)), GMP_RNDN));

      if (c_object_type(x) == big_real_tag)
	return((s7_Double)mpfr_get_d(S7_BIG_REAL(x), GMP_RNDN));

      if (c_object_type(x) == big_integer_tag)
	return((s7_Double)big_integer_to_s7_Int(S7_BIG_INTEGER(x)));

      if (c_object_type(x) == big_ratio_tag)
	return((s7_Double)((double)big_integer_to_s7_Int(mpq_numref(S7_BIG_RATIO(x))) / (double)big_integer_to_s7_Int(mpq_denref(S7_BIG_RATIO(x)))));
    }
  return(num_to_real_part(number(x)));
}


s7_Double s7_imag_part(s7_pointer x)
{
  if (is_c_object(x))
    {
      if (c_object_type(x) == big_complex_tag)
	return((s7_Double)mpfr_get_d(mpc_imagref(S7_BIG_COMPLEX(x)), GMP_RNDN));
      return(0.0);
    }
  return(num_to_imag_part(number(x)));
}


s7_Int s7_integer(s7_pointer p)
{
  if (is_c_object(p))
    return(big_integer_to_s7_Int(S7_BIG_INTEGER(p)));
  return(integer(number(p)));
}


s7_Double s7_real(s7_pointer p)
{
  if (is_c_object(p))
    return((s7_Double)mpfr_get_d(S7_BIG_REAL(p), GMP_RNDN));
  return(real(number(p)));
}


static mpq_t *s7_Ints_to_mpq(s7_Int num, s7_Int den)
{
  mpq_t *n;
  n = (mpq_t *)malloc(sizeof(mpq_t));
  mpq_init(*n);
  if ((num <= LONG_MAX) && (num >= LONG_MIN) &&
      (den <= LONG_MAX) && (den >= LONG_MIN))
    mpq_set_si(*n, (long int)num, (long int)den);
  else
    {
      mpz_t n1, d1;
      mpz_init_set_s7_Int(n1, num);
      mpz_init_set_s7_Int(d1, den);
      mpq_set_num(*n, n1);
      mpq_set_den(*n, d1);
      mpq_canonicalize(*n);
      mpz_clear(n1);
      mpz_clear(d1);
    }
  return(n);
}


static mpfr_t *s7_Double_to_mpfr(s7_Double val)
{
  mpfr_t *n;
  n = (mpfr_t *)malloc(sizeof(mpfr_t));
  mpfr_init_set_d(*n, val, GMP_RNDN);
  return(n);
}


static mpc_t *s7_Doubles_to_mpc(s7_Double rl, s7_Double im)
{
  mpc_t *n;
  n = (mpc_t *)malloc(sizeof(mpc_t));
  mpc_init(*n);
  mpc_set_d_d(*n, rl, im, MPC_RNDNN);
  return(n);
}


static s7_pointer s7_ratio_to_big_ratio(s7_scheme *sc, s7_Int num, s7_Int den)
{
  return(s7_make_object(sc, big_ratio_tag, (void *)s7_Ints_to_mpq(num, den)));
}


static bool big_numbers_are_eqv(s7_pointer a, s7_pointer b)
{
  bool result;
  if ((!is_c_object(a)) && (!is_c_object(b))) /* either or both can be big here, but not neither */
    return(false);
  
  if (s7_is_integer(a))
    {
      mpz_t a1, b1;
      if (!(s7_is_integer(b))) return(false);

      if ((is_c_object(a)) && (is_c_object(b)))
	return(mpz_cmp(S7_BIG_INTEGER(a), S7_BIG_INTEGER(b)) == 0);

      if (is_c_object(a))
	mpz_init_set(a1, S7_BIG_INTEGER(a));
      else mpz_init_set_s7_Int(a1, s7_integer(a));

      if (is_c_object(b))
	mpz_init_set(b1, S7_BIG_INTEGER(b));
      else mpz_init_set_s7_Int(b1, s7_integer(b));
      result = (mpz_cmp(a1, b1) == 0);

      mpz_clear(a1);
      mpz_clear(b1);
      return(result);
    }

  if (s7_is_ratio(a))
    {
      mpq_t *a1, *b1;
      if (!s7_is_ratio(b)) return(false);

      if ((is_c_object(a)) && (is_c_object(b)))
	return(mpq_cmp(S7_BIG_RATIO(a), S7_BIG_RATIO(b)) == 0);

      if (is_c_object(a))
	a1 = (mpq_t *)s7_object_value(a);
      else a1 = s7_Ints_to_mpq(s7_numerator(a), s7_denominator(a));
      if (is_c_object(b))
	b1 = (mpq_t *)s7_object_value(b);
      else b1 = s7_Ints_to_mpq(s7_numerator(b), s7_denominator(b));

      result = (mpq_cmp(*a1, *b1) == 0);

      if (!is_c_object(a))
	{
	  mpq_clear(*a1);
	  free(a1);
	}
      if (!is_c_object(b))
	{
	  mpq_clear(*b1);
	  free(b1);
	}
      return(result);
    }

  if (s7_is_real(a))
    {
      mpfr_t *a1, *b1;

      /* s7_is_real is not finicky enough here -- (eqv? 1.0 1) should return #f */
      if (is_c_object(b))
	{
	  if (c_object_type(b) != big_real_tag)
	    return(false);
	}
      else
	{
	  if ((number_type(b) != NUM_REAL) &&
	      (number_type(b) != NUM_REAL2))
	    return(false);
	}

      if ((is_c_object(a)) && (is_c_object(b)))
	return(mpfr_equal_p(S7_BIG_REAL(a), S7_BIG_REAL(b)));

      if (is_c_object(a))
	a1 = (mpfr_t *)s7_object_value(a);
      else a1 = s7_Double_to_mpfr(s7_real(a));

      if (is_c_object(b))
	b1 = (mpfr_t *)s7_object_value(b);
      else b1 = s7_Double_to_mpfr(s7_real(b));

      result = (mpfr_cmp(*a1, *b1) == 0);

      if (!is_c_object(a))
	{
	  mpfr_clear(*a1);
	  free(a1);
	}
      if (!is_c_object(b))
	{
	  mpfr_clear(*b1);
	  free(b1);
	}
      return(result);
    }

  if (s7_is_complex(a))
    {
      mpc_t *a1, *b1;
      /* s7_is_complex is not finicky enough here */
      if (is_c_object(b))
	{
	  if (c_object_type(b) != big_complex_tag)
	    return(false);
	}
      else
	{
	  if (number_type(b) < NUM_COMPLEX)
	    return(false);
	}

      if ((is_c_object(a)) && (is_c_object(b)))
	return(mpc_cmp(S7_BIG_COMPLEX(a), S7_BIG_COMPLEX(b)) == 0);

      if (is_c_object(a))
	a1 = (mpc_t *)s7_object_value(a);
      else a1 = s7_Doubles_to_mpc(s7_real_part(a), s7_imag_part(a));

      if (is_c_object(b))
	b1 = (mpc_t *)s7_object_value(b);
      else b1 = s7_Doubles_to_mpc(s7_real_part(b), s7_imag_part(b));

      result = (mpc_cmp(*a1, *b1) == 0);

      if (!is_c_object(a))
	{
	  mpc_clear(*a1);
	  free(a1);
	}
      if (!is_c_object(b))
	{
	  mpc_clear(*b1);
	  free(b1);
	}
      return(result);
    }

  return(false);
}


static s7_pointer string_to_either_integer(s7_scheme *sc, const char *str, int radix)
{
  /* try strtol, check for overflow, fallback on make_big_integer */
  int val;
  errno = 0;
  val = strtol(str, (char **)NULL, radix);
  if (errno == ERANGE)
    return(string_to_big_integer(sc, str, radix));
  return(s7_make_integer(sc, (s7_Int)val));
}


static s7_pointer string_to_either_ratio(s7_scheme *sc, const char *nstr, const char *dstr, int radix)
{
  int n, d;
  errno = 0;
  n = strtol(nstr, (char **)NULL, radix);
  if (errno != ERANGE)
    {
      d = strtol(dstr, (char **)NULL, radix);
      if (errno != ERANGE)
	return(s7_make_ratio(sc, (s7_Int)n, (s7_Int)d));
    }
  return(string_to_big_ratio(sc, nstr, radix));
}


static s7_pointer string_to_either_real(s7_scheme *sc, const char *str, int radix)
{
  if (safe_strlen(str) < 20)
    {
      double val;
      if (radix == 10)
	{
	  errno = 0;
	  val = strtod(str, (char **)NULL);
	  if (errno != ERANGE)
	    return(s7_make_real(sc, val));
	}
      else
	{
	  errno = 0;
	  val = string_to_double_with_radix((char *)str, radix);
	  if (errno != ERANGE)
	    return(s7_make_real(sc, val));
	}
    }
  return(string_to_big_real(sc, str, radix));
}


static s7_pointer string_to_either_complex_1(s7_scheme *sc, char *q, char *slash1, char *ex1, bool has_dec_point1, int radix, double *d_rl)
{
  if ((has_dec_point1) ||
      (ex1))
    {
      if (safe_strlen(q) < 20)
	{
	  if (radix == 10)
	    {
	      errno = 0;
	      (*d_rl) = strtod(q, (char **)NULL);
	      if (errno == ERANGE)
		return(string_to_big_real(sc, q, radix));
	    }
	  else
	    {
	      errno = 0;
	      (*d_rl) = string_to_double_with_radix(q, radix);
	      if (errno == ERANGE)
		return(string_to_big_real(sc, q, radix));
	    }
	}
      else return(string_to_big_real(sc, q, radix));
    }
  else
    {
      if (slash1)
	{
	  int n, d;
	  n = strtol(q, (char **)NULL, radix);
	  if (errno == ERANGE)
	    return(string_to_big_ratio(sc, q, radix));
	  else
	    {
	      d = strtol(slash1, (char **)NULL, radix);
	      if (errno != ERANGE)
		(*d_rl) = (s7_Double)n / (s7_Double)d;
	      else return(string_to_big_ratio(sc, q, radix));
	    }
	}
      else
	{
	  int val;
	  errno = 0;
	  val = strtol(q, (char **)NULL, radix);
	  if (errno == ERANGE)
	    return(string_to_big_integer(sc, q, radix));
	  else (*d_rl) = (s7_Double)val;
	}
    }
  if ((*d_rl) == -0.0) (*d_rl) = 0.0;
  return(NULL);
}


#define T_BIG_INTEGER 0
#define T_BIG_RATIO 1
#define T_BIG_REAL 2
#define T_BIG_COMPLEX 3

static s7_pointer string_to_either_complex(s7_scheme *sc,
					   char *q, char *slash1, char *ex1, bool has_dec_point1, 
					   char *plus, char *slash2, char *ex2, bool has_dec_point2, 
					   int radix, int has_plus_or_minus)
{
  /* this can be just about anything involving 2 real/ratio/int portions, +/- in between and 'i' at the end */
  double d_rl = 0.0, d_im = 0.0;
  s7_pointer p_rl = NULL, p_im = NULL, result;
  mpfr_t m_rl, m_im;

  p_rl = string_to_either_complex_1(sc, q, slash1, ex1, has_dec_point1, radix, &d_rl);
  p_im = string_to_either_complex_1(sc, plus, slash2, ex2, has_dec_point2, radix, &d_im);
  
  if ((!p_rl) && (!p_im))
    return(s7_make_complex(sc, d_rl, (has_plus_or_minus == -1) ? (-d_im) : d_im));

  if ((p_im == NULL) && (d_im == 0.0))
    return(p_rl);

  if (p_rl)
    mpfr_init_set(m_rl, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p_rl)), GMP_RNDN);
  else mpfr_init_set_d(m_rl, d_rl, GMP_RNDN);

  if (p_im)
    mpfr_init_set(m_im, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p_im)), GMP_RNDN);
  else mpfr_init_set_d(m_im, d_im, GMP_RNDN);

  if (has_plus_or_minus == -1) 
    mpfr_neg(m_im, m_im, GMP_RNDN);

  result = make_big_complex(sc, m_rl, m_im);
  
  mpfr_clear(m_rl);
  mpfr_clear(m_im);
  return(result);
}


/* object_type(bignum) can be anything (in pthreads case, we first set the various thread variable tags)
 *   so we can't assume it's a very small integer.  However, NUM_COMPLEX + higher bits can't be >= 8.
 */

#define SHIFT_BIGNUM_TYPE(type) (1 << (type + NO_NUM_SHIFT))

static int canonicalize_result_type(int type)
{
  if (type >= SHIFT_BIGNUM_TYPE(big_complex_tag))
    return(T_BIG_COMPLEX);

  if (type >= SHIFT_BIGNUM_TYPE(big_real_tag))
    {
      if ((type & 0x7) >= NUM_COMPLEX)
	return(T_BIG_COMPLEX);
      return(T_BIG_REAL);
    }

  if (type >= SHIFT_BIGNUM_TYPE(big_ratio_tag))
    switch (type & 0x7)
      {
      case NUM_INT:
      case NUM_RATIO: 
	return(T_BIG_RATIO);

      case NUM_REAL:
      case NUM_REAL2:
	return(T_BIG_REAL);

      default:
	return(T_BIG_COMPLEX);
      }

  switch (type & 0x7)
    {
    case NUM_INT: 
      return(T_BIG_INTEGER); /* NUM_INT is 0, so this includes the pure big ints case */

    case NUM_RATIO: 
      return(T_BIG_RATIO);

    case NUM_REAL:
    case NUM_REAL2: 
      return(T_BIG_REAL);

    default:
      return(T_BIG_COMPLEX);
    }
}


static s7_pointer promote_number_1(s7_scheme *sc, int type, s7_pointer x, bool copy)
{
  /* x can be any number -- need to convert it to the current result type */

  switch (type)
    {
    case T_BIG_INTEGER:
      if (is_c_object(x)) 
	{
	  if (copy)
	    return(mpz_to_big_integer(sc, S7_BIG_INTEGER(x)));
	  return(x);                       /* can only be T_BIG_INTEGER here */
	}
      return(s7_Int_to_big_integer(sc, s7_integer(x))); /* can only be NUM_INT here */
      break;

    case T_BIG_RATIO:
      if (is_c_object(x))
	{
	  if (c_object_type(x) == big_ratio_tag) 
	    {
	      if (copy)
		return(mpq_to_big_ratio(sc, S7_BIG_RATIO(x)));
	      return(x);
	    }
	  return(mpz_to_big_ratio(sc, S7_BIG_INTEGER(x)));
	}
      if (number_type(x) == NUM_INT)
	return(s7_ratio_to_big_ratio(sc, s7_integer(x), 1));
      return(s7_ratio_to_big_ratio(sc, s7_numerator(x), s7_denominator(x)));
      break;

    case T_BIG_REAL:
      if (is_c_object(x))
	{
	  if (c_object_type(x) == big_real_tag) 
	    {
	      if (copy)
		return(mpfr_to_big_real(sc, S7_BIG_REAL(x)));
	      return(x);
	    }
	  if (c_object_type(x) == big_ratio_tag) return(mpq_to_big_real(sc, S7_BIG_RATIO(x)));
	  return(mpz_to_big_real(sc, S7_BIG_INTEGER(x)));
	}
      return(s7_number_to_big_real(sc, x));
      break;

    default:
      if (is_c_object(x))
	{
	  if (c_object_type(x) == big_complex_tag) 
	    {
	      if (copy)
		return(mpc_to_big_complex(sc, S7_BIG_COMPLEX(x)));
	      return(x);
	    }
	  if (c_object_type(x) == big_real_tag) return(mpfr_to_big_complex(sc, S7_BIG_REAL(x)));
	  if (c_object_type(x) == big_ratio_tag) return(mpq_to_big_complex(sc, S7_BIG_RATIO(x)));
	  return(mpz_to_big_complex(sc, S7_BIG_INTEGER(x)));
	}
      return(s7_number_to_big_complex(sc, x));
      break;
    }
  return(sc->NIL);
}


static s7_pointer promote_number(s7_scheme *sc, int type, s7_pointer x)
{
  return(promote_number_1(sc, type, x, false));
}


static s7_pointer copy_and_promote_number(s7_scheme *sc, int type, s7_pointer x)
{
  return(promote_number_1(sc, type, x, true));
}


void s7_vector_fill(s7_scheme *sc, s7_pointer vec, s7_pointer obj) 
{
  s7_Int i, len;
  s7_pointer *tp;
  len = vector_length(vec);
  tp = (s7_pointer *)(vector_elements(vec));

  /* if the same bignum object is assigned to each element, different vector elements
   *    are actually the same -- we need to make a copy of obj for each one
   */

  if (IS_BIG(obj))
    {
      int type;
      type = c_object_type(obj);
      for (i = 0; i < len; i++) 
	{
	  if (type == big_real_tag)
	    tp[i] = mpfr_to_big_real(sc, S7_BIG_REAL(obj));
	  else
	    {
	      if (type == big_integer_tag)
		tp[i] = mpz_to_big_integer(sc, S7_BIG_INTEGER(obj));
	      else
		{
		  if (type == big_complex_tag)
		    tp[i] = mpc_to_big_complex(sc, S7_BIG_COMPLEX(obj));
		  else tp[i] = mpq_to_big_ratio(sc, S7_BIG_RATIO(obj));
		}
	    }
	}
    }
  else vector_fill(sc, vec, obj);
}


static s7_pointer g_bignum(s7_scheme *sc, s7_pointer args)
{
  #define H_bignum "(bignum val :optional radix) returns a multiprecision version of the string 'val'"
  s7_pointer p;

  p = g_string_to_number(sc, args);
  if (is_c_object(p)) return(p);

  switch (number_type(p))
    {
    case NUM_INT:   return(promote_number(sc, T_BIG_INTEGER, p));
    case NUM_RATIO: return(promote_number(sc, T_BIG_RATIO, p));
    case NUM_REAL:
    case NUM_REAL2: return(promote_number(sc, T_BIG_REAL, p));
    default:        return(promote_number(sc, T_BIG_COMPLEX, p));
    }
}


bool s7_is_bignum(s7_pointer obj)
{
  return(IS_BIG(obj));
}


static s7_pointer g_is_bignum(s7_scheme *sc, s7_pointer args)
{
  #define H_is_bignum "(bignum? obj) returns #t is obj is a multiprecision number."
  return(s7_make_boolean(sc, IS_BIG(car(args))));
}


static s7_Int add_max = 0;

static s7_pointer big_add(s7_scheme *sc, s7_pointer args)
{
  int i, result_type = NUM_INT;
  s7_pointer x, result;

  if (args == sc->NIL)
    return(small_int(sc, 0));

  if ((cdr(args) == sc->NIL) && (s7_is_number(car(args))))
    return(car(args));

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      s7_pointer p;
      p = car(x);
      if (type(p) != T_NUMBER)
	{
	  if (!IS_BIG(p))
	    return(s7_wrong_type_arg_error(sc, "+", i, p, "a number"));
	  else result_type |= SHIFT_BIGNUM_TYPE(c_object_type(p));
	}
      else result_type |= number_type(p);
    }

  if (IS_NUM(result_type))
    return(g_add_unchecked(sc, args));

  result_type = canonicalize_result_type(result_type);
  result = copy_and_promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      s7_pointer arg;
      arg = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  mpz_add(S7_BIG_INTEGER(result), S7_BIG_INTEGER(result), S7_BIG_INTEGER(arg));
	  break;
	  
	case T_BIG_RATIO:
	  mpq_add(S7_BIG_RATIO(result), S7_BIG_RATIO(result), S7_BIG_RATIO(arg));
	  break;
	  
	case T_BIG_REAL:
	  mpfr_add(S7_BIG_REAL(result), S7_BIG_REAL(result), S7_BIG_REAL(arg), GMP_RNDN);
	  break;
	  
	case T_BIG_COMPLEX:
	  mpc_add(S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(arg), MPC_RNDNN);
	  break;
	}
    }
  
  switch (result_type)
      {
      case T_BIG_RATIO:
	return(make_big_integer_or_ratio(sc, result));

      case T_BIG_COMPLEX:
	return(make_big_real_or_complex(sc, result));
      }
  return(result);
}


static s7_pointer big_negate(s7_scheme *sc, s7_pointer args)
{
  /* assume cdr(args) is nil and we're called from subtract, so check for big num else call g_subtract */
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, S7_BIG_INTEGER(p));
	  mpz_neg(*n, *n);
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}

      if (c_object_type(p) == big_ratio_tag)
	{
	  mpq_t *n;
	  n = (mpq_t *)malloc(sizeof(mpq_t));
	  mpq_init(*n);
	  mpq_set(*n, S7_BIG_RATIO(p));
	  mpq_neg(*n, *n);
	  return(s7_make_object(sc, big_ratio_tag, (void *)n));
	}

      if (c_object_type(p) == big_real_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	  mpfr_neg(*n, *n, GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}

      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
	  mpc_neg(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_subtract(sc, args));
}


static s7_pointer big_subtract(s7_scheme *sc, s7_pointer args)
{
  int i, result_type = NUM_INT;
  s7_pointer x, result;

  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "-", 1, car(args), "a number"));
  
  if (cdr(args) == sc->NIL) 
    return(big_negate(sc, args));

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      s7_pointer p;
      p = car(x);
      if (type(p) != T_NUMBER)
	{
	  if (!IS_BIG(p))
	    return(s7_wrong_type_arg_error(sc, "-", i, p, "a number"));
	  else result_type |= SHIFT_BIGNUM_TYPE(c_object_type(p));
	}
      else result_type |= number_type(p);
    }

  if (IS_NUM(result_type))
    return(g_subtract_unchecked(sc, args));

  result_type = canonicalize_result_type(result_type);
  result = copy_and_promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      s7_pointer arg;
      arg = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  mpz_sub(S7_BIG_INTEGER(result), S7_BIG_INTEGER(result), S7_BIG_INTEGER(arg));
	  break;
	  
	case T_BIG_RATIO:
	  mpq_sub(S7_BIG_RATIO(result), S7_BIG_RATIO(result), S7_BIG_RATIO(arg));
	  break;
	  
	case T_BIG_REAL:
	  mpfr_sub(S7_BIG_REAL(result), S7_BIG_REAL(result), S7_BIG_REAL(arg), GMP_RNDN);
	  break;
	  
	case T_BIG_COMPLEX:
	  mpc_sub(S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(arg), MPC_RNDNN);
	  break;
	}
    }

  switch (result_type)
    {
      case T_BIG_RATIO:
	return(make_big_integer_or_ratio(sc, result));

      case T_BIG_COMPLEX:
	return(make_big_real_or_complex(sc, result));
      }
  return(result);
}


static s7_pointer big_multiply(s7_scheme *sc, s7_pointer args)
{
  int i, result_type = NUM_INT;
  s7_pointer x, result;

  if (args == sc->NIL)
    return(small_int(sc, 1));

  if ((cdr(args) == sc->NIL) && (s7_is_number(car(args))))
    return(car(args));

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      s7_pointer p;
      p = car(x);
      if (type(p) != T_NUMBER)
	{
	  if (!IS_BIG(p))
	    return(s7_wrong_type_arg_error(sc, "*", i, p, "a number"));
	  else result_type |= SHIFT_BIGNUM_TYPE(c_object_type(p));
	}
      else result_type |= number_type(p);
    }

  if (IS_NUM(result_type))
    return(g_multiply_unchecked(sc, args));

  result_type = canonicalize_result_type(result_type);
  result = copy_and_promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      s7_pointer arg;
      arg = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  mpz_mul(S7_BIG_INTEGER(result), S7_BIG_INTEGER(result), S7_BIG_INTEGER(arg));
	  break;
	  
	case T_BIG_RATIO:
	  mpq_mul(S7_BIG_RATIO(result), S7_BIG_RATIO(result), S7_BIG_RATIO(arg));
	  break;

	case T_BIG_REAL:
	  mpfr_mul(S7_BIG_REAL(result), S7_BIG_REAL(result), S7_BIG_REAL(arg), GMP_RNDN);
	  break;

	case T_BIG_COMPLEX:
	  mpc_mul(S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(arg), MPC_RNDNN);
	  break;
	}
    }

  switch (result_type)
    {
    case T_BIG_RATIO:
      return(make_big_integer_or_ratio(sc, result));

    case T_BIG_COMPLEX:
      return(make_big_real_or_complex(sc, result));
    }
  return(result);
}


static s7_pointer big_is_zero_1(s7_scheme *sc, s7_pointer x);

static s7_pointer big_invert(s7_scheme *sc, s7_pointer args)
{
  /* assume cdr(args) is nil and we're called from divide, so check for big num else call g_divide */
  s7_pointer p;
  p = car(args);
  if (big_is_zero_1(sc, p) == sc->T)
    return(s7_division_by_zero_error(sc, "/", p));

  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	{
	  mpq_t *n;
	  n = (mpq_t *)malloc(sizeof(mpq_t));
	  mpq_init(*n);
	  mpq_set_z(*n, S7_BIG_INTEGER(p));
	  mpq_inv(*n, *n);
	  /* p might have been 1 or -1 */
	  if (mpz_cmp_ui(mpq_denref(*n), 1) == 0)
	    {
	      mpz_t *z;
	      z = (mpz_t *)malloc(sizeof(mpz_t));
	      mpz_init_set(*z, mpq_numref(*n));
	      mpq_clear(*n);
	      free(n);
	      return(s7_make_object(sc, big_integer_tag, (void *)z));
	    }
	  return(s7_make_object(sc, big_ratio_tag, (void *)n));
	}

      if (c_object_type(p) == big_ratio_tag)
	{
	  mpq_t *n;
	  n = (mpq_t *)malloc(sizeof(mpq_t));
	  mpq_init(*n);
	  mpq_set(*n, S7_BIG_RATIO(p));
	  mpq_inv(*n, *n);

	  if (mpz_cmp_ui(mpq_denref(*n), 1) == 0)
	    {
	      mpz_t *z;
	      z = (mpz_t *)malloc(sizeof(mpz_t));
	      mpz_init_set(*z, mpq_numref(*n));
	      mpq_clear(*n);
	      free(n);
	      return(s7_make_object(sc, big_integer_tag, (void *)z));
	    }

	  return(s7_make_object(sc, big_ratio_tag, (void *)n));
	}

      if (c_object_type(p) == big_real_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	  mpfr_ui_div(*n, 1, *n, GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}

      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
	  mpc_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  mpc_ui_div(*n, 1, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_divide(sc, args));
}


static s7_pointer big_divide(s7_scheme *sc, s7_pointer args)
{
  int i, result_type = NUM_INT;
  s7_pointer x, divisor, result;

  if (!s7_is_number(car(args)))
    return(s7_wrong_type_arg_error(sc, "/", 1, car(args), "a number"));
  
  if (cdr(args) == sc->NIL) 
    return(big_invert(sc, args));

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      s7_pointer p;
      p = car(x);
      if (type(p) != T_NUMBER)
	{
	  if (!IS_BIG(p))
	    return(s7_wrong_type_arg_error(sc, "/", i, p, "a number"));
	  else result_type |= SHIFT_BIGNUM_TYPE(c_object_type(p));
	}
      else result_type |= number_type(p);
    }

  if (IS_NUM(result_type))
    return(g_divide_unchecked(sc, args));

  result_type = canonicalize_result_type(result_type);
  divisor = copy_and_promote_number(sc, result_type, cadr(args));

  for (x = cddr(args); x != sc->NIL; x = cdr(x)) 
    {
      s7_pointer arg;
      arg = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  mpz_mul(S7_BIG_INTEGER(divisor), S7_BIG_INTEGER(divisor), S7_BIG_INTEGER(arg));
	  break;
	  
	case T_BIG_RATIO:
	  mpq_mul(S7_BIG_RATIO(divisor), S7_BIG_RATIO(divisor), S7_BIG_RATIO(arg));
	  break;
	  
	case T_BIG_REAL:
	  mpfr_mul(S7_BIG_REAL(divisor), S7_BIG_REAL(divisor), S7_BIG_REAL(arg), GMP_RNDN);
	  break;
	  
	case T_BIG_COMPLEX:
	  mpc_mul(S7_BIG_COMPLEX(divisor), S7_BIG_COMPLEX(divisor), S7_BIG_COMPLEX(arg), MPC_RNDNN);
	  break;
	}
    }
    
  if (big_is_zero_1(sc, divisor) == sc->T)
    return(s7_division_by_zero_error(sc, "/", args));

  result = copy_and_promote_number(sc, result_type, car(args));


#if 0
/* SOMEDAY: we need to catch gmp exceptions somehow: SIGFPE (exception=deliberate /0 -- see gmp/errno.c) */
#include <signal.h>

static void s7_sigfpe(int ignored)
{
  /* can we recover? */
}

then in init: signal(SIGFPE, s7_sigfpe);
#endif


  switch (result_type)
    {
    case T_BIG_INTEGER:
      {
	mpq_t *n;
	n = (mpq_t *)malloc(sizeof(mpq_t));
	mpq_init(*n);
	mpq_set_num(*n, S7_BIG_INTEGER(result));
	mpq_set_den(*n, S7_BIG_INTEGER(divisor));
	mpq_canonicalize(*n);
	if (mpz_cmp_ui(mpq_denref(*n), 1) == 0)
	  {
	    result = mpz_to_big_integer(sc, mpq_numref(*n));
	    mpq_clear(*n);
	    free(n);
	    return(result);
	  }
	return(s7_make_object(sc, big_ratio_tag, (void *)n));
      }
      
    case T_BIG_RATIO:
      mpq_div(S7_BIG_RATIO(result), S7_BIG_RATIO(result), S7_BIG_RATIO(divisor));
      return(make_big_integer_or_ratio(sc, result));
      break;
      
    case T_BIG_REAL:
      mpfr_div(S7_BIG_REAL(result), S7_BIG_REAL(result), S7_BIG_REAL(divisor), GMP_RNDN);
      break;
      
    case T_BIG_COMPLEX:
      mpc_div(S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(divisor), MPC_RNDNN);
      return(make_big_real_or_complex(sc, result));
      break;
    }

  return(result);
}


static s7_pointer big_numerator(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(p);
      if (c_object_type(p) == big_ratio_tag)
	return(mpz_to_big_integer(sc, mpq_numref(S7_BIG_RATIO(p))));
    }
  return(g_numerator(sc, args));
}


static s7_pointer big_denominator(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(small_int(sc, 1));
      if (c_object_type(p) == big_ratio_tag)
	return(mpz_to_big_integer(sc, mpq_denref(S7_BIG_RATIO(p))));
    }
  return(g_denominator(sc, args));
}


static s7_pointer big_is_even(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if ((is_c_object(p)) &&
      (c_object_type(p) == big_integer_tag))
    return(make_boolean(sc, mpz_even_p(S7_BIG_INTEGER(p))));
  return(g_is_even(sc, args));
}


static s7_pointer big_is_odd(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if ((is_c_object(p)) &&
      (c_object_type(p) == big_integer_tag))
    return(make_boolean(sc, mpz_odd_p(S7_BIG_INTEGER(p))));
  return(g_is_odd(sc, args));
}


static s7_pointer big_real_part(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_complex_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init(*n);
	  mpc_real(*n, S7_BIG_COMPLEX(p), GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
      else
	{
	  if ((c_object_type(p) == big_integer_tag) ||
	      (c_object_type(p) == big_ratio_tag) ||
	      (c_object_type(p) == big_real_tag))
	    return(p);
	}
    }
  return(g_real_part(sc, args));
}


static s7_pointer big_imag_part(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_complex_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init(*n);
	  mpc_imag(*n, S7_BIG_COMPLEX(p), GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
      else
	{
	  if ((c_object_type(p) == big_integer_tag) ||
	      (c_object_type(p) == big_ratio_tag) ||
	      (c_object_type(p) == big_real_tag))
	    return(small_int(sc, 0));
	}
    }
  return(g_imag_part(sc, args));
}


static s7_pointer big_abs(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, S7_BIG_INTEGER(p));
	  mpz_abs(*n, *n);
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}

      if (c_object_type(p) == big_ratio_tag)
	{
	  mpq_t *n;
	  n = (mpq_t *)malloc(sizeof(mpq_t));
	  mpq_init(*n);
	  mpq_set(*n, S7_BIG_RATIO(p));
	  mpq_abs(*n, *n);
	  return(s7_make_object(sc, big_ratio_tag, (void *)n));
	}

      if (c_object_type(p) == big_real_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	  mpfr_abs(*n, *n, GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
    }
  return(g_abs(sc, args));
}


static s7_pointer big_magnitude(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	return(big_abs(sc, args));

      if (c_object_type(p) == big_complex_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init(*n);
	  mpc_abs(*n, S7_BIG_COMPLEX(p), GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
    }
  return(g_magnitude(sc, args));
}


static s7_pointer big_angle(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	{
	  if (mpz_cmp_ui(S7_BIG_INTEGER(p), 0) >= 0)
	    return(small_int(sc, 0));
	  return(big_pi(sc));
	}

      if (c_object_type(p) == big_ratio_tag)
	{
	  if (mpq_cmp_ui(S7_BIG_RATIO(p), 0, 1) >= 0)
	    return(small_int(sc, 0));
	  return(big_pi(sc));
	}

      if (c_object_type(p) == big_real_tag)
	{
	  if (mpfr_cmp_ui(S7_BIG_REAL(p), 0) >= 0)
	    return(sc->real_zero);
	  return(big_pi(sc));
	}

      if (c_object_type(p) == big_complex_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init(*n);
	  mpc_arg(*n, S7_BIG_COMPLEX(p), GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
    }
  return(g_angle(sc, args));
}


static s7_pointer big_make_rectangular(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p0, p1;
  p0 = car(args);
  p1 = cadr(args);
  if (((is_c_object(p0)) || (is_c_object(p1))) && /* if either is a bignum, and both are reals (not complex)... */
      (s7_is_real(p0)) && 
      (s7_is_real(p1)))
    {
      mpc_t *n;
      mpfr_t rl, im;

      if ((!is_c_object(p1)) && (s7_real(p1) == 0.0))
	return(p0);

      mpfr_init_set(im, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p1)), GMP_RNDN);
      if (mpfr_cmp_ui(im, 0) == 0)
	{
	  mpfr_clear(im);
	  return(p0);
	}
      mpfr_init_set(rl, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p0)), GMP_RNDN);
      n = (mpc_t *)malloc(sizeof(mpc_t));
      mpc_init(*n);
      mpc_set_fr_fr(*n, rl, im, MPC_RNDNN);
      mpfr_clear(rl);
      mpfr_clear(im);
      return(s7_make_object(sc, big_complex_tag, (void *)n));
    }
  return(g_make_rectangular(sc, args));
}


static s7_pointer big_make_polar(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p0, p1;
  p0 = car(args);
  p1 = cadr(args);
  if (((is_c_object(p0)) || (is_c_object(p1))) && 
      (s7_is_real(p0)) && 
      (s7_is_real(p1)))
    {
      mpc_t *n;
      mpfr_t ang, mag, rl, im;

      if ((!is_c_object(p1)) && (s7_real(p1) == 0.0))
	return(p0);
      if ((!is_c_object(p0)) && (s7_real(p0) == 0.0))
	return(p0);

      mpfr_init_set(mag, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p0)), GMP_RNDN);
      if (mpfr_cmp_ui(mag, 0) == 0)
	return(p0);

      mpfr_init_set(ang, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p1)), GMP_RNDN);

      mpfr_init_set(im, ang, GMP_RNDN);
      mpfr_sin(im, im, GMP_RNDN);
      mpfr_mul(im, im, mag, GMP_RNDN);

      if (mpfr_cmp_ui(im, 0) == 0)
	{
	  mpfr_clear(im);
	  mpfr_clear(ang);
	  mpfr_clear(mag);
	  return(p0);
	}

      mpfr_init_set(rl, ang, GMP_RNDN);
      mpfr_cos(rl, rl, GMP_RNDN);
      mpfr_mul(rl, rl, mag, GMP_RNDN);

      n = (mpc_t *)malloc(sizeof(mpc_t));
      mpc_init(*n);
      mpc_set_fr_fr(*n, rl, im, MPC_RNDNN);
      mpfr_clear(rl);
      mpfr_clear(im);
      mpfr_clear(ang);
      mpfr_clear(mag);
      return(s7_make_object(sc, big_complex_tag, (void *)n));
    }
  return(g_make_polar(sc, args));
}


static s7_pointer big_log(s7_scheme *sc, s7_pointer args)
{
  /* either arg can be big, 2nd is optional */
  s7_pointer p0, p1 = NULL;

  p0 = car(args);
  if (!s7_is_number(p0))
    return(s7_wrong_type_arg_error(sc, "log", 1, p0, "a number"));

  if (cdr(args) != sc->NIL)
    {
      p1 = cadr(args);
      if (!s7_is_number(p1))
	return(s7_wrong_type_arg_error(sc, "log", 2, p1, "a number"));
    }

  if ((IS_BIG(p0)) ||
      ((p1) && (IS_BIG(p1))))
    {
      if ((s7_is_real(p0)) &&
	  ((!p1) || (s7_is_real(p1))))
	{
	  int p1_cmp = 0;
	  p0 = promote_number(sc, T_BIG_REAL, p0);
	  if (p1) 
	    {
	      if ((!is_c_object(p1)) &&
		  ((s7_is_zero(p1)) || (s7_is_one(p1))))
		return(s7_out_of_range_error(sc, "log", 2, p1, "base can't be 0.0 or 1.0"));

	      p1 = promote_number(sc, T_BIG_REAL, p1);
	      p1_cmp = (mpfr_cmp_ui(S7_BIG_REAL(p1), 0));
	      if (p1_cmp == 0)
		return(s7_out_of_range_error(sc, "log", 2, p1, "base can't be 0.0"));
	      p1_cmp = (mpfr_cmp_ui(S7_BIG_REAL(p1), 1));
	      if (p1_cmp == 0)
		return(s7_out_of_range_error(sc, "log", 2, p1, "base can't be 1.0"));
	    }
	  if ((mpfr_cmp_ui(S7_BIG_REAL(p0), 0) > 0) &&
	      ((!p1) || (p1_cmp > 0)))
	    {
	      mpfr_t *n;
	      mpfr_t base;
	      n = (mpfr_t *)malloc(sizeof(mpfr_t));
	      mpfr_init_set(*n, S7_BIG_REAL(p0), GMP_RNDN);
	      mpfr_log(*n, *n, GMP_RNDN);
	      if (!p1)
		{
		  /* presumably log is safe with regard to real-part overflow giving a bogus int? */
		  if ((s7_is_rational(car(args))) &&
		      (mpfr_integer_p(*n) != 0))
		    {
		      mpz_t *k;
		      k = (mpz_t *)malloc(sizeof(mpz_t));
		      mpz_init(*k);
		      mpfr_get_z(*k, *n, GMP_RNDN);
		      mpfr_clear(*n);
		      free(n);
		      return(s7_make_object(sc, big_integer_tag, (void *)k));
		    }
		  return(s7_make_object(sc, big_real_tag, (void *)n));
		}
	      mpfr_init_set(base, S7_BIG_REAL(p1), GMP_RNDN);
	      mpfr_log(base, base, GMP_RNDN);
	      mpfr_div(*n, *n, base, GMP_RNDN);
	      mpfr_clear(base);
	      if ((s7_is_rational(car(args))) &&
		  (s7_is_rational(cadr(args))) &&
		  (mpfr_integer_p(*n) != 0))
		{
		  mpz_t *k;
		  k = (mpz_t *)malloc(sizeof(mpz_t));
		  mpz_init(*k);
		  mpfr_get_z(*k, *n, GMP_RNDN);
		  mpfr_clear(*n);
		  free(n);
		  return(s7_make_object(sc, big_integer_tag, (void *)k));
		}
	      return(s7_make_object(sc, big_real_tag, (void *)n));
	    }
	}
      p0 = promote_number(sc, T_BIG_COMPLEX, p0);
      if (p1) p1 = promote_number(sc, T_BIG_COMPLEX, p1);
      {
	mpc_t *n;
	mpc_t base;
	n = (mpc_t *)malloc(sizeof(mpc_t));
	mpc_init(*n);
	mpc_set(*n, S7_BIG_COMPLEX(p0), MPC_RNDNN);
	mpc_log(*n, *n, MPC_RNDNN);
	if (!p1)
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	mpc_init(base);
	mpc_set(base, S7_BIG_COMPLEX(p1), MPC_RNDNN);
	mpc_log(base, base, MPC_RNDNN);
	mpc_div(*n, *n, base, MPC_RNDNN);
	mpc_clear(base);

	if (mpfr_cmp_ui(mpc_imagref(*n), 0) == 0)    /* (log -1.0 -1.0) */
	  {
	    mpfr_t *x;
	    x = (mpfr_t *)malloc(sizeof(mpfr_t));
	    mpfr_init_set(*x, mpc_realref(*n), GMP_RNDN);
	    mpc_clear(*n);
	    free(n);
	    return(s7_make_object(sc, big_real_tag, (void *)x));
	  }

	return(s7_make_object(sc, big_complex_tag, (void *)n));
      }
    }
  return(g_log(sc, args));
}


static s7_pointer big_sqrt(s7_scheme *sc, s7_pointer args)
{
  /* real >= 0 -> real, else complex */
  s7_pointer p;
  p = car(args);
  if (IS_BIG(p))
    {
      /* if big integer, try to return int if perfect square */
      if (c_object_type(p) == big_integer_tag)
	{
	  if (mpz_cmp_ui(S7_BIG_INTEGER(p), 0) < 0)
	    p = promote_number(sc, T_BIG_COMPLEX, p);
	  else
	    {
	      mpz_t *n;
	      mpz_t rem;
	      n = (mpz_t *)malloc(sizeof(mpz_t));
	      mpz_init(rem);
	      mpz_init_set(*n, S7_BIG_INTEGER(p));
	      mpz_sqrtrem(*n, rem, *n);
	      if (mpz_cmp_ui(rem, 0) == 0)
		{
		  mpz_clear(rem);
		  return(s7_make_object(sc, big_integer_tag, (void *)n));
		}
	      free(n);
	      mpz_clear(rem);
	      p = promote_number(sc, T_BIG_REAL, p);
	    }
	}

      /* if big ratio, check both num and den for squares */
      if (c_object_type(p) == big_ratio_tag)
	{
	  if (mpq_cmp_ui(S7_BIG_RATIO(p), 0, 1) < 0)
	    p = promote_number(sc, T_BIG_COMPLEX, p);
	  else
	    {
	      mpz_t n1, rem;
	      mpz_init(rem);
	      mpz_init_set(n1, mpq_numref(S7_BIG_RATIO(p)));
	      mpz_sqrtrem(n1, rem, n1);
	      if (mpz_cmp_ui(rem, 0) == 0)
		{
		  mpz_t d1;
		  mpz_init_set(d1, mpq_denref(S7_BIG_RATIO(p)));
		  mpz_sqrtrem(d1, rem, d1);
		  if (mpz_cmp_ui(rem, 0) == 0)
		    {
		      mpq_t *n;
		      n = (mpq_t *)malloc(sizeof(mpq_t));
		      mpq_init(*n);
		      mpq_set_num(*n, n1);
		      mpq_set_den(*n, d1);
		      mpq_canonicalize(*n);
		      mpz_clear(n1);
		      mpz_clear(d1);
		      mpz_clear(rem);
		      return(s7_make_object(sc, big_ratio_tag, (void *)n));
		    }
		  mpz_clear(d1);
		}
	      mpz_clear(n1);
	      mpz_clear(rem);

	      p = promote_number(sc, T_BIG_REAL, p);
	    }
	}

      /* if real and not negative, use mpfr_sqrt */
      if (c_object_type(p) == big_real_tag)
	{
	  if (mpfr_cmp_ui(S7_BIG_REAL(p), 0) < 0)
	    p = promote_number(sc, T_BIG_COMPLEX, p);
	  else
	    {
	      mpfr_t *n;
	      n = (mpfr_t *)malloc(sizeof(mpfr_t));
	      mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	      mpfr_sqrt(*n, *n, GMP_RNDN);
	      return(s7_make_object(sc, big_real_tag, (void *)n));
	    }
	}
      
      /* p is a big number, so it must be complex at this point */
      {
	mpc_t *n;
	n = (mpc_t *)malloc(sizeof(mpc_t));
	mpc_init(*n);
	mpc_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	mpc_sqrt(*n, *n, MPC_RNDNN);
	return(s7_make_object(sc, big_complex_tag, (void *)n));
      }
    }
  return(g_sqrt(sc, args));
}


enum {TRIG_NO_CHECK, TRIG_TAN_CHECK, TRIG_TANH_CHECK};

static s7_pointer big_trig(s7_scheme *sc, s7_pointer args, s7_function g_trig, 
			   int (*mpfr_trig)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t),
			   int (*mpc_trig)(mpc_ptr, mpc_srcptr, mpc_rnd_t),
			   int tan_case)
/* these declarations mimic the mpfr.h and mpc.h declarations.  It seems to me that
 *   they ought to be:
 *			   int (*mpfr_trig)(mpfr_t rop, mpfr_t op, mp_rnd_t rnd),
 *			   void (*mpc_trig)(mpc_t rop, mpc_t op, mpc_rnd_t rnd))
 */
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p)), GMP_RNDN);
	  mpfr_trig(*n, *n, GMP_RNDN);
	  /* it's confusing to check for ints here via mpfr_integer_p because it
	   *   is dependent on the precision!  (exp 617/5) returns an integer if
	   *   precision is 128, but a float if 512.
	   */
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}

      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;

	  if (tan_case == TRIG_TAN_CHECK)
	    {
	      if ((MPC_INEX_IM(mpc_cmp_si_si(S7_BIG_COMPLEX(p), 1, 350))) > 0)
		return(s7_make_complex(sc, 0.0, 1.0));	
	      if ((MPC_INEX_IM(mpc_cmp_si_si(S7_BIG_COMPLEX(p), 1, -350))) < 0)
		return(s7_make_complex(sc, 0.0, -1.0));	
	    }

	  if (tan_case == TRIG_TANH_CHECK)
	    {
	      if ((MPC_INEX_RE(mpc_cmp_si_si(S7_BIG_COMPLEX(p), 350, 1))) > 0)
		return(sc->real_one);
	      if ((MPC_INEX_RE(mpc_cmp_si_si(S7_BIG_COMPLEX(p), -350, 1))) < 0)
		return(s7_make_real(sc, -1.0));
	    }

	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
	  mpc_trig(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  if (mpfr_cmp_ui(mpc_imagref(*n), 0) == 0)
	    {
	      mpfr_t *z;
	      z = (mpfr_t *)malloc(sizeof(mpfr_t));
	      mpfr_init_set(*z, mpc_realref(*n), GMP_RNDN);
	      mpc_clear(*n);
	      free(n);
	      return(s7_make_object(sc, big_real_tag, (void *)z));
	    }
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_trig(sc, args));
}


static s7_pointer big_sin(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_sin, mpfr_sin, mpc_sin, TRIG_NO_CHECK));
}


static s7_pointer big_cos(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_cos, mpfr_cos, mpc_cos, TRIG_NO_CHECK));
}


static s7_pointer big_tan(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_tan, mpfr_tan, mpc_tan, TRIG_TAN_CHECK));
}


static s7_pointer big_sinh(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_sinh, mpfr_sinh, mpc_sinh, TRIG_NO_CHECK));
}


static s7_pointer big_cosh(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_cosh, mpfr_cosh, mpc_cosh, TRIG_NO_CHECK));
}


static s7_pointer big_tanh(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_tanh, mpfr_tanh, mpc_tanh, TRIG_TANH_CHECK));
}


static s7_pointer big_exp(s7_scheme *sc, s7_pointer args)
{
  return(big_trig(sc, args, g_exp, mpfr_exp, mpc_exp, TRIG_NO_CHECK));
}


static s7_pointer big_expt(s7_scheme *sc, s7_pointer args)
{
  /* g_expt can overflow easily so I think I'll handle as many cases as possible here */

  s7_pointer x, y;

  /* see comment under g_expt
   *  if (cddr(args) != sc->NIL)
   *    return(big_expt(sc, make_list_2(sc, car(args), big_expt(sc, cdr(args)))));
   */

  x = car(args);
  if (!s7_is_number(x))
    return(s7_wrong_type_arg_error(sc, "expt", 1, x, "a number"));

  y = cadr(args);
  if (!s7_is_number(y))
    return(s7_wrong_type_arg_error(sc, "expt", 2, y, "a number"));

  if (big_is_zero_1(sc, x) == sc->T)
    {
      if ((s7_is_integer(x)) && 
	  (s7_is_integer(y)) &&
	  (big_is_zero_1(sc, y) == sc->T))
	return(small_int(sc, 1));

      if ((s7_is_real(y)) && (s7_is_negative(y)))
	return(s7_division_by_zero_error(sc, "expt", args));

      if ((s7_is_rational(x)) && 
	  (s7_is_rational(y)))
	return(small_int(sc, 0));

      return(sc->real_zero);
    }

  if (s7_is_integer(y))
    {
      s7_Int yval;

      if (!is_c_object(y))
	yval = s7_integer(y);
      else yval = big_integer_to_s7_Int(S7_BIG_INTEGER(y));

      if (yval == 0)
	{
	  if (s7_is_rational(x))
	    return(small_int(sc, 1));
	  return(sc->real_one);
	}

      if (yval == 1)
	return(x);

      if (!is_c_object(x))
	{
	  if ((s7_is_one(x)) || (s7_is_zero(x)))
	    return(x);
	}

      if ((yval < LONG_MAX) &&
	  (yval > LONG_MIN))
	{
	  if (s7_is_integer(x))
	    {
	      mpz_t *n;
	      mpq_t *r;
	      x = promote_number(sc, T_BIG_INTEGER, x);
	      n = (mpz_t *)malloc(sizeof(mpz_t));
	      mpz_init_set(*n, S7_BIG_INTEGER(x));
	      if (yval >= 0)
		{
		  mpz_pow_ui(*n, *n, (unsigned int)yval);
		  return(s7_make_object(sc, big_integer_tag, (void *)n));
		}
	      mpz_pow_ui(*n, *n, (unsigned int)(-yval));
	      r = (mpq_t *)malloc(sizeof(mpq_t));
	      mpq_init(*r);
	      mpq_set_z(*r, *n);
	      mpq_inv(*r, *r);
	      if (mpz_cmp_ui(mpq_denref(*r), 1) == 0)
		{
		  mpz_t *z;
		  z = (mpz_t *)malloc(sizeof(mpz_t));
		  mpz_init_set(*z, mpq_numref(*r));
		  mpq_clear(*r);
		  free(r);
		  mpz_clear(*n);
		  free(n);
		  return(s7_make_object(sc, big_integer_tag, (void *)z));
		}
	      mpz_clear(*n);
	      free(n);
	      return(s7_make_object(sc, big_ratio_tag, (void *)r));
	    }
	}

      if (s7_is_ratio(x)) /* (here y is an integer) */
	{
	  mpz_t n, d;
	  mpq_t *r;
	  x = promote_number(sc, T_BIG_RATIO, x);
	  mpz_init_set(n, mpq_numref(S7_BIG_RATIO(x)));
	  mpz_init_set(d, mpq_denref(S7_BIG_RATIO(x)));
	  r = (mpq_t *)malloc(sizeof(mpq_t));
	  mpq_init(*r);
	  if (yval >= 0)
	    {
	      mpz_pow_ui(n, n, (unsigned int)yval);
	      mpz_pow_ui(d, d, (unsigned int)yval);
	      mpq_set_num(*r, n);
	      mpq_set_den(*r, d);
	    }
	  else
	    {
	      yval = -yval;
	      mpz_pow_ui(n, n, (unsigned int)yval);
	      mpz_pow_ui(d, d, (unsigned int)yval);
	      mpq_set_num(*r, d);
	      mpq_set_den(*r, n);	      
	      mpq_canonicalize(*r);
	    }
	  mpz_clear(n);
	  mpz_clear(d);
	  if (mpz_cmp_ui(mpq_denref(*r), 1) == 0)
	    {
	      mpz_t *z;
	      z = (mpz_t *)malloc(sizeof(mpz_t));
	      mpz_init_set(*z, mpq_numref(*r));
	      mpq_clear(*r);
	      free(r);
	      return(s7_make_object(sc, big_integer_tag, (void *)z));
	    }
	  return(s7_make_object(sc, big_ratio_tag, (void *)r));
	}

      if (s7_is_real(x))
	{
	  mpfr_t *z;
	  x = promote_number(sc, T_BIG_REAL, x);
	  z = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*z, S7_BIG_REAL(x), GMP_RNDN);
	  mpfr_pow_si(*z, *z, yval, GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)z));
	}
    }

  if ((s7_is_ratio(y)) &&
      (numerator(number(y)) == 1))
    {
      if (denominator(number(y)) == 2)
	return(big_sqrt(sc, args));

      if ((s7_is_real(x)) &&
	  (denominator(number(y)) == 3))
	{
	  if (IS_BIG(x))
	    {
	      mpfr_t *z;
	      z = (mpfr_t *)malloc(sizeof(mpfr_t));
	      mpfr_init_set(*z, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, x)), GMP_RNDN);
	      mpfr_cbrt(*z, *z, GMP_RNDN);
	      return(s7_make_object(sc, big_real_tag, (void *)z));
	    }
	  else return(s7_make_real(sc, cbrt(num_to_real(number(x)))));
	}
    }

  if ((s7_is_real(x)) &&
      (s7_is_real(y)) &&
      (s7_is_positive(x)))
    {
      mpfr_t *z;
      z = (mpfr_t *)malloc(sizeof(mpfr_t));
      mpfr_init_set(*z, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, x)), GMP_RNDN);
      mpfr_pow(*z, *z, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, y)), GMP_RNDN);
      return(s7_make_object(sc, big_real_tag, (void *)z));
    }

  if ((s7_is_number(x)) &&
      (s7_is_number(y)))
    {
      mpc_t cy;
      mpc_t *z;
      
      x = promote_number(sc, T_BIG_COMPLEX, x);
      y = promote_number(sc, T_BIG_COMPLEX, y);
      
      z = (mpc_t *)malloc(sizeof(mpc_t));
      mpc_init(*z);
      mpc_set(*z, S7_BIG_COMPLEX(x), MPC_RNDNN);

      if (mpc_cmp_si_si(*z, 0, 0) == 0)
	{
	  mpc_clear(*z);
	  free(z);
	  return(small_int(sc, 0));
	}

      if (mpc_cmp_si_si(*z, 1, 0) == 0)
	{
	  mpc_clear(*z);
	  free(z);
	  return(small_int(sc, 1));
	}

      mpc_init(cy);
      mpc_set(cy, S7_BIG_COMPLEX(y), MPC_RNDNN);
      mpc_pow(*z, *z, cy, MPC_RNDNN);
      mpc_clear(cy);

      if (mpfr_cmp_ui(mpc_imagref(*z), 0) == 0)
	{
	  mpfr_t *n;
	  if ((s7_is_rational(car(args))) &&
	      (s7_is_rational(cadr(args))) &&
	      (mpfr_integer_p(mpc_realref(*z)) != 0))
	    {
	      /* mpfr_integer_p can be confused: (expt 2718/1000 (bignum "617/5")) returns an int if precision=128, float if 512 */
	      /*   so first make sure we're within (say) 31 bits */
	      mpfr_t zi;
	      mpfr_init_set_ui(zi, LONG_MAX, GMP_RNDN);
	      if (mpfr_cmpabs(mpc_realref(*z), zi) < 0)
		{
		  mpz_t *k;
		  k = (mpz_t *)malloc(sizeof(mpz_t));
		  mpz_init(*k);
		  mpfr_get_z(*k, mpc_realref(*z), GMP_RNDN);
		  mpc_clear(*z);
		  mpfr_clear(zi);
		  free(z);
		  return(s7_make_object(sc, big_integer_tag, (void *)k));
		}
	      mpfr_clear(zi);
	    }
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, mpc_realref(*z), GMP_RNDN);
	  mpc_clear(*z);
	  free(z);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
      return(s7_make_object(sc, big_complex_tag, (void *)z));
    }
  return(g_expt(sc, args)); /* fallback error handling */
}


static s7_pointer big_asinh(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p)), GMP_RNDN);
	  mpfr_asinh(*n, *n, GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
	  mpc_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  mpc_asinh(*n, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_asinh(sc, args));
}


static s7_pointer big_acosh(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	{
	  p = promote_number(sc, T_BIG_REAL, p);
	  if (mpfr_cmp_ui(S7_BIG_REAL(p), 1) >= 0)
	    {
	      mpfr_t *n;
	      n = (mpfr_t *)malloc(sizeof(mpfr_t));
	      mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	      mpfr_acosh(*n, *n, GMP_RNDN);
	      return(s7_make_object(sc, big_real_tag, (void *)n));
	    }
	  p = promote_number(sc, T_BIG_COMPLEX, p);
	}
      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
	  mpc_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  mpc_acosh(*n, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_acosh(sc, args));
}


static s7_pointer big_atanh(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	{
	  bool ok;
	  mpfr_t temp;
	  mpfr_init_set_ui(temp, 1, GMP_RNDN);
	  p = promote_number(sc, T_BIG_REAL, p);
	  ok = (mpfr_cmpabs(S7_BIG_REAL(p), temp) < 0);
	  mpfr_clear(temp);
	  if (ok)
	    {
	      mpfr_t *n;
	      n = (mpfr_t *)malloc(sizeof(mpfr_t));
	      mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	      mpfr_atanh(*n, *n, GMP_RNDN);
	      return(s7_make_object(sc, big_real_tag, (void *)n));
	    }
	  p = promote_number(sc, T_BIG_COMPLEX, p);
	}
      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
	  mpc_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  mpc_atanh(*n, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_atanh(sc, args));
}


static s7_pointer big_atan(s7_scheme *sc, s7_pointer args)
{
  /* either arg can be big, 2nd is optional */
  s7_pointer p0, p1 = NULL;

  p0 = car(args);
  if (cdr(args) != sc->NIL)
    p1 = cadr(args);

  if ((IS_BIG(p0)) ||
      ((p1) && (IS_BIG(p1))))
    {
      if ((p1) &&
	  ((!s7_is_real(p0)) ||
	   (!s7_is_real(p1))))
	return(s7_wrong_type_arg_error(sc, "atan", 1, p0, "if 2 args, both should be real"));

      if (s7_is_real(p0))
	{
	  mpfr_t *n;
	  p0 = promote_number(sc, T_BIG_REAL, p0);
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(p0), GMP_RNDN);
	  if (!p1)
	    mpfr_atan(*n, *n, GMP_RNDN);
	  else mpfr_atan2(*n, *n, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p1)), GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}

      if (c_object_type(p0) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init_set(*n, S7_BIG_COMPLEX(p0), MPC_RNDNN);
	  mpc_atan(*n, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_atan(sc, args));
}


static s7_pointer big_acos(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	{
	  bool ok;
	  mpfr_t temp;
	  mpfr_t *n;
	  p = promote_number(sc, T_BIG_REAL, p);
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	  mpfr_init_set_ui(temp, 1, GMP_RNDN);
	  ok = (mpfr_cmpabs(*n, temp) <= 0);
	  mpfr_clear(temp);
	  if (ok)
	    {
	      mpfr_acos(*n, *n, GMP_RNDN);
	      return(s7_make_object(sc, big_real_tag, (void *)n));
	    }
	  mpfr_clear(*n);
	  free(n);
	  p = promote_number(sc, T_BIG_COMPLEX, p);
	}

      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  mpc_acos(*n, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_acos(sc, args));
}


static s7_pointer big_asin(s7_scheme *sc, s7_pointer args)
{
  /* the complex case is not yet implmented in mpc, so use the earlier formula for casin */
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_real_tag))
	{
	  bool ok;
	  mpfr_t temp;
	  mpfr_t *n;
	  p = promote_number(sc, T_BIG_REAL, p);
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set(*n, S7_BIG_REAL(p), GMP_RNDN);
	  mpfr_init_set_ui(temp, 1, GMP_RNDN);
	  ok = (mpfr_cmpabs(*n, temp) <= 0);
	  mpfr_clear(temp);
	  if (ok)
	    {
	      mpfr_asin(*n, *n, GMP_RNDN);
	      return(s7_make_object(sc, big_real_tag, (void *)n));
	    }
	  mpfr_clear(*n);
	  free(n);
	  p = promote_number(sc, T_BIG_COMPLEX, p);
	}

      if (c_object_type(p) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init_set(*n, S7_BIG_COMPLEX(p), MPC_RNDNN);
	  mpc_asin(*n, *n, MPC_RNDNN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }
  return(g_asin(sc, args));
}


static s7_pointer big_is_zero_1(s7_scheme *sc, s7_pointer p)
{
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(make_boolean(sc, mpz_cmp_ui(S7_BIG_INTEGER(p), 0) == 0));
      if (c_object_type(p) == big_ratio_tag)
	return(make_boolean(sc, mpq_cmp_ui(S7_BIG_RATIO(p), 0, 1) == 0));
      if (c_object_type(p) == big_real_tag)
	return(make_boolean(sc, mpfr_zero_p(S7_BIG_REAL(p))));
      if (c_object_type(p) == big_complex_tag)
	return(make_boolean(sc, mpc_cmp_si_si(S7_BIG_COMPLEX(p), 0, 0) == 0));
    }

  if (!s7_is_number(p))
    return(s7_wrong_type_arg_error(sc, "zero?", 0, p, "a number"));
  return(make_boolean(sc, s7_is_zero(p)));
}


static s7_pointer big_is_zero(s7_scheme *sc, s7_pointer args)
{
  return(big_is_zero_1(sc, car(args)));
}


static s7_pointer big_is_positive(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(make_boolean(sc, mpz_cmp_ui(S7_BIG_INTEGER(p), 0) > 0));

      if (c_object_type(p) == big_ratio_tag)
	return(make_boolean(sc, mpq_cmp_ui(S7_BIG_RATIO(p), 0, 1) > 0));

      if (c_object_type(p) == big_real_tag)
	return(make_boolean(sc, mpfr_cmp_ui(S7_BIG_REAL(p), 0) > 0));
    }
  return(g_is_positive(sc, args));
}


static s7_pointer big_is_negative(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(make_boolean(sc, mpz_cmp_ui(S7_BIG_INTEGER(p), 0) < 0));

      if (c_object_type(p) == big_ratio_tag)
	return(make_boolean(sc, mpq_cmp_ui(S7_BIG_RATIO(p), 0, 1) < 0));

      if (c_object_type(p) == big_real_tag)
	return(make_boolean(sc, mpfr_cmp_ui(S7_BIG_REAL(p), 0) < 0));
    }
  return(g_is_negative(sc, args));
}


static s7_pointer big_lognot(s7_scheme *sc, s7_pointer args)
{
  if ((is_c_object(car(args))) &&
      (c_object_type(car(args)) == big_integer_tag))
    {
      mpz_t *n;
      n = (mpz_t *)malloc(sizeof(mpz_t));
      mpz_init(*n);
      mpz_com(*n, S7_BIG_INTEGER(car(args)));
      return(s7_make_object(sc, big_integer_tag, (void *)n));
    }
  return(g_lognot(sc, args));
}


static s7_pointer big_integer_length(s7_scheme *sc, s7_pointer args)
{
  if ((is_c_object(car(args))) &&
      (c_object_type(car(args)) == big_integer_tag))
    {
      s7_pointer result;
      mpfr_t n;
      mpfr_init_set_z(n, S7_BIG_INTEGER(car(args)), GMP_RNDN);
      if (mpfr_cmp_ui(n, 0) < 0)
	mpfr_neg(n, n, GMP_RNDN);
      else mpfr_add_ui(n, n, 1, GMP_RNDN);
      mpfr_log2(n, n, GMP_RNDU);
      result = s7_make_integer(sc, mpfr_get_si(n, GMP_RNDU));
      mpfr_clear(n);
      return(result);
    }
  return(g_integer_length(sc, args));
}


static s7_pointer big_ash(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p0, p1;

  p0 = car(args);
  p1 = cadr(args);
  /* here, as in expt, there are cases like (ash 1 63) which need to be handled as bignums
   *   so there's no way to tell when it's safe to drop into g_ash instead.
   */
  if ((s7_is_integer(p0)) && /* this includes bignum ints... */
      (s7_is_integer(p1)))
    {
      mpz_t *n;
      s7_Int shift;

      if (((is_c_object(p0)) &&
	   (mpz_cmp_ui(S7_BIG_INTEGER(p0), 0) == 0)) || 
	  ((!is_c_object(p0)) &&
	   (s7_integer(p0) == 0)))
	return(small_int(sc, 0));
	
      if (is_c_object(p1))
	{
	  if (!mpz_fits_sint_p(S7_BIG_INTEGER(p1)))
	    {
	      if (mpz_cmp_ui(S7_BIG_INTEGER(p1), 0) > 0)
		return(s7_out_of_range_error(sc, "ash", 2, p1, "shift is too large"));
	      return(small_int(sc, 0));
	    }
	  shift = mpz_get_si(S7_BIG_INTEGER(p1));
	}
      else shift = s7_integer(p1);
      n = (mpz_t *)malloc(sizeof(mpz_t));
      mpz_init_set(*n, S7_BIG_INTEGER(promote_number(sc, T_BIG_INTEGER, p0)));
      if (shift > 0)     /* left */
	mpz_mul_2exp(*n, *n, shift);
      else
	{
	  if (shift < 0) /* right */
	    mpz_fdiv_q_2exp(*n, *n, (unsigned int)(-shift));
	}
      return(s7_make_object(sc, big_integer_tag, (void *)n));
    }
  return(g_ash(sc, args));
}


static s7_pointer big_bits(s7_scheme *sc, s7_pointer args, const char *name, int start, s7_function g_bits, 
			   void (*mpz_bits)(mpz_ptr, mpz_srcptr, mpz_srcptr))
{
  int i;
  s7_pointer x;
  bool use_bigs = false;
  for (i = 1, x = args; x != sc->NIL; x = cdr(x), i++)
    {
      if (!s7_is_integer(car(x)))
	return(s7_wrong_type_arg_error(sc, name, i, car(x), "an integer"));  
      if ((is_c_object(car(x))) &&
	  (c_object_type(car(x)) == big_integer_tag))
	use_bigs = true;
    }
  if (use_bigs)
    {
      mpz_t *n;
      n = (mpz_t *)malloc(sizeof(mpz_t));
      mpz_init_set_si(*n, 0);
      if (start == -1)
	mpz_sub_ui(*n, *n, 1);
      for (x = args; x != sc->NIL; x = cdr(x))
	mpz_bits(*n, *n, S7_BIG_INTEGER(promote_number(sc, T_BIG_INTEGER, car(x))));
      return(s7_make_object(sc, big_integer_tag, (void *)n));      
    }
  return(g_bits(sc, args));
}


static s7_pointer big_logand(s7_scheme *sc, s7_pointer args)
{
  return(big_bits(sc, args, "logand", -1, g_logand, mpz_and));
}


static s7_pointer big_logior(s7_scheme *sc, s7_pointer args)
{
  return(big_bits(sc, args, "logior", 0, g_logior, mpz_ior));
}


static s7_pointer big_logxor(s7_scheme *sc, s7_pointer args)
{
  return(big_bits(sc, args, "logxor", 0, g_logxor, mpz_xor));
}


static s7_pointer big_rationalize(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p0, p1 = NULL;

  p0 = car(args);
  /* p0 can be exact, but we still have to check it for simplification */
  if (cdr(args) != sc->NIL)
    p1 = cadr(args);

  if (((is_c_object(p0)) || 
       ((p1) && (is_c_object(p1)))) && /* one or other is big, perhaps */
      (s7_is_real(p0)) &&
      ((!p1) || (s7_is_real(p1))))   /* both are real (or error arg is omitted) */
    {
      mpfr_t error, ux, x0, x1;
      mpz_t i, i0, i1;

      if (is_c_object(p0))
	mpfr_init_set(ux, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p0)), GMP_RNDN);
      else mpfr_init_set_d(ux, s7_number_to_real(p0), GMP_RNDN);

      if (p1)
	{
	  if (is_c_object(p1))
	    mpfr_init_set(error, S7_BIG_REAL(promote_number(sc, T_BIG_REAL, p1)), GMP_RNDN);
	  else mpfr_init_set_d(error, s7_number_to_real(p1), GMP_RNDN);
	  mpfr_abs(error, error, GMP_RNDN);
	}
      else mpfr_init_set_d(error, default_rationalize_error, GMP_RNDN);

      mpfr_init_set(x0, ux, GMP_RNDN);        /* x0 = ux - error */
      mpfr_sub(x0, x0, error, GMP_RNDN);
      mpfr_init_set(x1, ux, GMP_RNDN);        /* x1 = ux + error */
      mpfr_add(x1, x1, error, GMP_RNDN);
      mpz_init(i);
      mpfr_get_z(i, x0, GMP_RNDU);            /* i = ceil(x0) */

      if (mpfr_cmp_ui(error, 1) >= 0)         /* if (error >= 1.0) */
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));

	  if (mpfr_cmp_ui(x0, 0) < 0)                /* if (x0 < 0) */
	    {
	      if (mpfr_cmp_ui(x1, 0) < 0)            /*   if (x1 < 0) */
		{
		  mpz_init(*n);
		  mpfr_get_z(*n, x1, GMP_RNDD);      /*     num = floor(x1) */
		}
	      else mpz_init_set_ui(*n, 0);           /*   else num = 0 */
	    }
	  else mpz_init_set(*n, i);                  /* else num = i */

	  mpz_clear(i);
	  mpfr_clear(ux);
	  mpfr_clear(x0);
	  mpfr_clear(x1);
	  mpfr_clear(error);
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}

      if (mpfr_cmp_z(x1, i) >= 0)                /* if (x1 >= i) */
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  
	  if (mpz_cmp_ui(i, 0) >= 0)             /* if (i >= 0) */
	    mpz_init_set(*n, i);                 /*   num = i */
	  else
	    {
	      mpz_init(*n);
	      mpfr_get_z(*n, x1, GMP_RNDD);      /* else num = floor(x1) */
	    }

	  mpz_clear(i);
	  mpfr_clear(ux);
	  mpfr_clear(x0);
	  mpfr_clear(x1);
	  mpfr_clear(error);
	  return(s7_make_object(sc, big_integer_tag, (void *)n)); 
	}

      {
	mpz_t p0, q0, r, r1, p1, q1, old_p1, old_q1;
	mpfr_t val, e0, e1, e0p, e1p, old_e0, old_e1, old_e0p;

	mpz_init(i0);
	mpz_init(i1);
	mpfr_get_z(i0, x0, GMP_RNDD);            /* i0 = floor(x0) */
	mpfr_get_z(i1, x1, GMP_RNDU);            /* i1 = ceil(x1) */

	mpz_init_set(p0, i0);                    /* p0 = i0 */
	mpz_init_set_ui(q0, 1);                  /* q0 = 1 */
	mpz_init_set(p1, i1);                    /* p1 = i1 */
	mpz_init_set_ui(q1, 1);                  /* q1 = 1 */
	mpfr_init(e0);
	mpfr_init(e1);
	mpfr_init(e0p);
	mpfr_init(e1p);
	mpfr_sub_z(e0, x0, i1, GMP_RNDN);        /* e0 = i1 - x0 */
	mpfr_neg(e0, e0, GMP_RNDN);
	mpfr_sub_z(e1, x0, i0, GMP_RNDN);        /* e1 = x0 - i0 */
	mpfr_sub_z(e0p, x1, i1, GMP_RNDN);       /* e0p = i1 - x1 */
	mpfr_neg(e0p, e0p, GMP_RNDN);
	mpfr_sub_z(e1p, x1, i0, GMP_RNDN);       /* e1p = x1 - i0 */

	mpfr_init(val);

	mpfr_init(old_e0);
	mpfr_init(old_e1);
	mpfr_init(old_e0p);

	mpz_init(r);
	mpz_init(r1);
	mpz_init(old_p1);
	mpz_init(old_q1);

	while (true)
	  {
	    mpfr_set_z(val, p0, GMP_RNDN);
	    mpfr_div_z(val, val, q0, GMP_RNDN);  /* val = p0/q0 */
	    if ((mpfr_cmp(x0, val) <= 0) &&      /* if ((x0 <= val) && (val <= x1)) */
		(mpfr_cmp(val, x1) <= 0))
	      {
		mpq_t *q;
		q = (mpq_t *)malloc(sizeof(mpq_t));
		mpq_init(*q);
		mpq_set_num(*q, p0);            /* return(p0/q0) */
		mpq_set_den(*q, q0);

		mpz_clear(i);
		mpfr_clear(ux);
		mpfr_clear(x0);
		mpfr_clear(x1);
		mpfr_clear(error);

		mpz_clear(p0);
		mpz_clear(q0);
		mpz_clear(r);
		mpz_clear(r1);
		mpz_clear(p1);
		mpz_clear(q1);
		mpz_clear(old_p1);
		mpz_clear(old_q1);
	
		mpfr_clear(val);
		mpfr_clear(e0);
		mpfr_clear(e1);
		mpfr_clear(e0p);
		mpfr_clear(e1p);
		mpfr_clear(old_e0);
		mpfr_clear(old_e1);
		mpfr_clear(old_e0p);

		return(s7_make_object(sc, big_ratio_tag, (void *)q)); 
	      }
	    
	    mpfr_div(val, e0, e1, GMP_RNDN);
	    mpfr_get_z(r, val, GMP_RNDD);           /* r = floor(e0/e1) */
	    mpfr_div(val, e0p, e1p, GMP_RNDN);
	    mpfr_get_z(r1, val, GMP_RNDU);          /* r1 = ceil(e0p/e1p) */
	    if (mpz_cmp(r1, r) < 0)                 /* if (r1 < r) */
	      mpz_set(r, r1);                       /*   r = r1 */
	    
	    mpz_set(old_p1, p1);                    /* old_p1 = p1 */
	    mpz_set(p1, p0);                        /* p1 = p0 */
	    mpz_set(old_q1, q1);                    /* old_q1 = q1 */
	    mpz_set(q1, q0);                        /* q1 = q0 */

	    mpfr_set(old_e0, e0, GMP_RNDN);         /* old_e0 = e0 */
	    mpfr_set(e0, e1p, GMP_RNDN);            /* e0 = e1p */
	    mpfr_set(old_e0p, e0p, GMP_RNDN);       /* old_e0p = e0p */
	    mpfr_set(e0p, e1, GMP_RNDN);            /* e0p = e1 */
	    mpfr_set(old_e1, e1, GMP_RNDN);         /* old_e1 = e1 */

	    mpz_mul(p0, p0, r);                     /* p0 = old_p1 + r * p0 */
	    mpz_add(p0, p0, old_p1); 
	    
	    mpz_mul(q0, q0, r);                     /* q0 = old_q1 + r * q0 */
	    mpz_add(q0, q0, old_q1); 
	    
	    mpfr_mul_z(e1, e1p, r, GMP_RNDN);       /* e1 = old_e0p - r * e1p */
	    mpfr_sub(e1, old_e0p, e1, GMP_RNDN); 
	    
	    mpfr_mul_z(e1p, old_e1, r, GMP_RNDN);   /* e1p = old_e0 - r * old_e1 */
	    mpfr_sub(e1p, old_e0, e1p, GMP_RNDN); 
	  }
      }
    }
  return(g_rationalize(sc, args));
}


static s7_pointer big_exact_to_inexact(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_real_tag) ||
	  (c_object_type(p) == big_complex_tag))
	return(p);

      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag))
	return(promote_number(sc, T_BIG_REAL, p));
    }
  return(g_exact_to_inexact(sc, args));
}


static s7_pointer big_inexact_to_exact(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if ((c_object_type(p) == big_integer_tag) ||
	  (c_object_type(p) == big_ratio_tag) ||
	  (c_object_type(p) == big_complex_tag))
	return(p);

      if (c_object_type(p) == big_real_tag)
	return(big_rationalize(sc, args));
    }
  return(g_inexact_to_exact(sc, args));
}


static s7_pointer big_convert_to_int(s7_scheme *sc, s7_pointer args,
				     s7_function g_div_func,
				     void (*div_func)(mpz_ptr, mpz_srcptr, mpz_srcptr),
				     mp_rnd_t mode)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(p);
      if (c_object_type(p) == big_ratio_tag)
	{
	  /* apparently we have to do the divide by hand */
	  mpz_t d;
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, mpq_numref(S7_BIG_RATIO(p)));
	  mpz_init_set(d, mpq_denref(S7_BIG_RATIO(p)));
	  div_func(*n, *n, d);
	  mpz_clear(d);
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}

      if (c_object_type(p) == big_real_tag)
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init(*n);
	  mpfr_get_z(*n, S7_BIG_REAL(p), mode);
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
    }
  return(g_div_func(sc, args));
}


static s7_pointer big_floor(s7_scheme *sc, s7_pointer args)
{
  return(big_convert_to_int(sc, args, g_floor, mpz_fdiv_q, GMP_RNDD));
}


static s7_pointer big_ceiling(s7_scheme *sc, s7_pointer args)
{
  return(big_convert_to_int(sc, args, g_ceiling, mpz_cdiv_q, GMP_RNDU));
}


static s7_pointer big_truncate(s7_scheme *sc, s7_pointer args)
{
  return(big_convert_to_int(sc, args, g_truncate, mpz_tdiv_q, GMP_RNDZ));
}


static s7_pointer big_round(s7_scheme *sc, s7_pointer args)
{
  s7_pointer p;
  p = car(args);
  if (is_c_object(p))
    {
      if (c_object_type(p) == big_integer_tag)
	return(p);

      if (c_object_type(p) == big_ratio_tag)
	{
	  int rnd;
	  mpz_t *n;
	  mpz_t rm;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, mpq_numref(S7_BIG_RATIO(p)));
	  mpz_init(rm);
	  mpz_fdiv_qr(*n, rm, *n, mpq_denref(S7_BIG_RATIO(p)));
	  mpz_mul_ui(rm, rm, 2);
	  rnd = mpz_cmpabs(rm, mpq_denref(S7_BIG_RATIO(p)));
	  mpz_fdiv_q(rm, rm, mpq_denref(S7_BIG_RATIO(p)));
	  if (rnd > 0)
	    mpz_add(*n, *n, rm);
	  else
	    {
	      if (rnd == 0)
		{
		  if (mpz_odd_p(*n))
		    mpz_add_ui(*n, *n, 1);
		}
	    }
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}

      if (c_object_type(p) == big_real_tag)
	{
	  int cmp_res;
	  mpz_t *n;
	  mpz_t fl, ce;
	  mpfr_t x, dfl, dce;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpfr_init_set(x, S7_BIG_REAL(p), GMP_RNDN);
	  mpz_init(fl);
	  mpfr_get_z(fl, x, GMP_RNDD);           /* fl = floor(x) */
	  mpz_init(ce);
	  mpfr_get_z(ce, x, GMP_RNDU);           /* ce = ceil(x) */
	  mpfr_init(dfl);
	  mpfr_sub_z(dfl, x, fl, GMP_RNDN);      /* dfl = x - fl */
	  mpfr_init(dce);
	  mpfr_sub_z(dce, x, ce, GMP_RNDN);      /* dce = -(ce - x) */
	  mpfr_neg(dce, dce, GMP_RNDN);          /*    and reversed */
	  cmp_res = mpfr_cmp(dfl, dce);
	  if (cmp_res > 0)                       /* if (dfl > dce) return(ce) */
	    mpz_init_set(*n, ce);
	  else
	    {
	      if (cmp_res < 0)                   /* if (dfl < dce) return(fl) */
		mpz_init_set(*n, fl);
	      else
		{
		  if (mpz_even_p(fl))
		    mpz_init_set(*n, fl);        /* if (mod(fl, 2) == 0) return(fl) */
		  else mpz_init_set(*n, ce);     /* else return(ce) */
		}
	    }
	  mpz_clear(fl);
	  mpz_clear(ce);
	  mpfr_clear(dfl);
	  mpfr_clear(dce);
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
    }
  return(g_round(sc, args));
}


static s7_pointer big_quotient(s7_scheme *sc, s7_pointer args)
{
  s7_pointer x, y;
  x = car(args);
  y = cadr(args);

  if ((IS_BIG(x)) || IS_BIG(y))
    {
      if ((s7_is_integer(x)) &&
	  (s7_is_integer(y)))
	{
	  mpz_t *n;
	  s7_pointer divisor;
	  divisor = promote_number(sc, T_BIG_INTEGER, y);

	  if (big_is_zero_1(sc, divisor) == sc->T)
	    return(s7_division_by_zero_error(sc, "quotient", args));
	  
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, S7_BIG_INTEGER(promote_number(sc, T_BIG_INTEGER, x)));
	  mpz_tdiv_q(*n, *n, S7_BIG_INTEGER(divisor));
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
      return(big_truncate(sc, s7_cons(sc, big_divide(sc, args), sc->NIL)));
    }
  return(g_quotient(sc, args));
}


static s7_pointer big_remainder(s7_scheme *sc, s7_pointer args)
{
  s7_pointer x, y;
  x = car(args);
  y = cadr(args);

  if ((IS_BIG(x)) || IS_BIG(y))
    {
      if ((s7_is_integer(x)) &&
	  (s7_is_integer(y)))
	{
	  mpz_t *n;
	  s7_pointer divisor;
	  divisor = promote_number(sc, T_BIG_INTEGER, y);

	  if (big_is_zero_1(sc, divisor) == sc->T)
	    return(s7_division_by_zero_error(sc, "remainder", args));
	  
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, S7_BIG_INTEGER(promote_number(sc, T_BIG_INTEGER, x)));
	  mpz_tdiv_r(*n, *n, S7_BIG_INTEGER(divisor));
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
      return(big_subtract(sc, make_list_2(sc, x, big_multiply(sc, make_list_2(sc, y, big_quotient(sc, args))))));
    }
  return(g_remainder(sc, args));
}


static s7_pointer big_modulo(s7_scheme *sc, s7_pointer args)
{
  s7_pointer a, b;
  a = car(args);
  b = cadr(args);

  if ((IS_BIG(a)) || IS_BIG(b))
    {
      if ((s7_is_integer(a)) &&
	  (s7_is_integer(b)))
	{
	  s7_pointer x, y;
	  int cy, cz;
	  mpz_t *n;

	  y = promote_number(sc, T_BIG_INTEGER, b);
	  if (mpz_cmp_ui(S7_BIG_INTEGER(y), 0) == 0)
	    return(a);

	  x = promote_number(sc, T_BIG_INTEGER, a);
	  /* mpz_mod is too tricky here */
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, S7_BIG_INTEGER(x));
	  mpz_fdiv_r(*n, *n, S7_BIG_INTEGER(y));
	  cy = mpz_cmp_ui(S7_BIG_INTEGER(y), 0);
	  cz = mpz_cmp_ui(*n, 0);
	  if (((cy < 0) && (cz > 0)) ||
	      ((cy > 0) && (cz < 0)))
	    mpz_add(*n, *n, S7_BIG_INTEGER(y));
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}

      return(big_subtract(sc, 
              make_list_2(sc, a, 
	       big_multiply(sc, 
                make_list_2(sc, b, 
		 big_floor(sc, 
                  s7_cons(sc, 
                   big_divide(sc, 
                    make_list_2(sc, a, b)), sc->NIL)))))));
    }
  return(g_modulo(sc, args));
}


static int big_real_scan_args(s7_scheme *sc, s7_pointer args)
{
  int i, result_type = NUM_INT;
  s7_pointer arg;

  for (i = 1, arg = args; arg != sc->NIL; i++, arg = cdr(arg)) 
    {
      s7_pointer p;
      p = car(arg);
      if (!s7_is_real(p))
	return(-i);
      if (IS_BIG(p))
	result_type |= SHIFT_BIGNUM_TYPE(c_object_type(p));
      else result_type |= number_type(p);
    }
  return(result_type);
}


static s7_pointer big_max(s7_scheme *sc, s7_pointer args)
{
  int result_type;
  s7_pointer x, result, arg;
  
  result_type = big_real_scan_args(sc, args);
  if (result_type < 0)
    return(s7_wrong_type_arg_error(sc, "max", -result_type, s7_list_ref(sc, args, -1 - result_type), "a real number"));

  if (IS_NUM(result_type))
    return(g_max(sc, args));

  result_type = canonicalize_result_type(result_type);
  result = promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      arg = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(result), S7_BIG_INTEGER(arg)) < 0)
	    result = arg;
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(result), S7_BIG_RATIO(arg)) < 0)
	    result = arg;
	  break;

	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(result), S7_BIG_REAL(arg)) < 0)
	    result = arg;
	  break;
	}
    }
  if (result_type == T_BIG_RATIO) /* maybe actual result was an int */
    {
      if (mpz_cmp_ui(mpq_denref(S7_BIG_RATIO(result)), 1) == 0)
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, mpq_numref(S7_BIG_RATIO(result)));
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
    }
  return(result);
}


static s7_pointer big_min(s7_scheme *sc, s7_pointer args)
{
  int result_type;
  s7_pointer x, result, arg;
  
  result_type = big_real_scan_args(sc, args);
  if (result_type < 0)
    return(s7_wrong_type_arg_error(sc, "min", -result_type, s7_list_ref(sc, args, -1 - result_type), "a real number"));

  if (IS_NUM(result_type))
    return(g_min(sc, args));

  result_type = canonicalize_result_type(result_type);
  result = promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      arg = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(result), S7_BIG_INTEGER(arg)) > 0)
	    result = arg;
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(result), S7_BIG_RATIO(arg)) > 0)
	    result = arg;
	  break;

	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(result), S7_BIG_REAL(arg)) > 0)
	    result = arg;
	  break;
	}
    }
  if (result_type == T_BIG_RATIO) /* maybe actual result was an int */
    {
      if (mpz_cmp_ui(mpq_denref(S7_BIG_RATIO(result)), 1) == 0)
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init_set(*n, mpq_numref(S7_BIG_RATIO(result)));
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
    }
  return(result);
}


static s7_pointer big_less(s7_scheme *sc, s7_pointer args)
{
  int result_type;
  s7_pointer x, previous, current;
  
  result_type = big_real_scan_args(sc, args);
  if (result_type < 0)
    return(s7_wrong_type_arg_error(sc, "<", -result_type, s7_list_ref(sc, args, -1 - result_type), "a real number"));

  if (IS_NUM(result_type))
    return(g_less(sc, args));

  result_type = canonicalize_result_type(result_type);
  previous = promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      current = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(previous), S7_BIG_INTEGER(current)) >= 0) return(sc->F);
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(previous), S7_BIG_RATIO(current)) >= 0) return(sc->F);
	  break;

	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(previous), S7_BIG_REAL(current)) >= 0) return(sc->F);
	  break;
	}
      previous = current;
    }
  return(sc->T);
}


static s7_pointer big_less_or_equal(s7_scheme *sc, s7_pointer args)
{
  int result_type;
  s7_pointer x, previous, current;
  
  result_type = big_real_scan_args(sc, args);
  if (result_type < 0)
    return(s7_wrong_type_arg_error(sc, "<=", -result_type, s7_list_ref(sc, args, -1 - result_type), "a real number"));

  if (IS_NUM(result_type))
    return(g_less_or_equal(sc, args));

  result_type = canonicalize_result_type(result_type);
  previous = promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      current = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(previous), S7_BIG_INTEGER(current)) > 0) return(sc->F);
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(previous), S7_BIG_RATIO(current)) > 0) return(sc->F);
	  break;

	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(previous), S7_BIG_REAL(current)) > 0) return(sc->F);
	  break;
	}
      previous = current;
    }
  return(sc->T);
}


static s7_pointer big_greater(s7_scheme *sc, s7_pointer args)
{
  int result_type;
  s7_pointer x, previous, current;

  result_type = big_real_scan_args(sc, args);
  if (result_type < 0)
    return(s7_wrong_type_arg_error(sc, ">", -result_type, s7_list_ref(sc, args, -1 - result_type), "a real number"));

  if (IS_NUM(result_type))
    return(g_greater(sc, args));

  result_type = canonicalize_result_type(result_type);
  previous = promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      current = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(previous), S7_BIG_INTEGER(current)) <= 0) return(sc->F);
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(previous), S7_BIG_RATIO(current)) <= 0) return(sc->F);
	  break;

	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(previous), S7_BIG_REAL(current)) <= 0) return(sc->F);
	  break;
	}
      previous = current;
    }
  return(sc->T);
}


static s7_pointer big_greater_or_equal(s7_scheme *sc, s7_pointer args)
{
  int result_type;
  s7_pointer x, previous, current;
  
  result_type = big_real_scan_args(sc, args);
  if (result_type < 0)
    return(s7_wrong_type_arg_error(sc, ">=", -result_type, s7_list_ref(sc, args, -1 - result_type), "a real number"));

  if (IS_NUM(result_type))
    return(g_greater_or_equal(sc, args));

  result_type = canonicalize_result_type(result_type);
  previous = promote_number(sc, result_type, car(args));

  for (x = cdr(args); x != sc->NIL; x = cdr(x)) 
    {
      current = promote_number(sc, result_type, car(x));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(previous), S7_BIG_INTEGER(current)) < 0) return(sc->F);
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(previous), S7_BIG_RATIO(current)) < 0) return(sc->F);
	  break;

	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(previous), S7_BIG_REAL(current)) < 0) return(sc->F);
	  break;
	}
      previous = current;
    }
  return(sc->T);
}


static s7_pointer big_equal(s7_scheme *sc, s7_pointer args)
{
  int i, result_type = NUM_INT;
  s7_pointer x, y, result;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    {
      s7_pointer p;
      p = car(x);
      if (type(p) != T_NUMBER)
	{
	  if (!IS_BIG(p))
	    return(s7_wrong_type_arg_error(sc, "=", i, p, "a number"));
	  else result_type |= SHIFT_BIGNUM_TYPE(c_object_type(p));
	}
      else result_type |= number_type(p);
    }

  if (IS_NUM(result_type))
    return(g_equal(sc, args));

  result_type = canonicalize_result_type(result_type);
  result = promote_number(sc, result_type, car(args));

  for (y = cdr(args); y != sc->NIL; y = cdr(y)) 
    {
      s7_pointer arg;
      arg = promote_number(sc, result_type, car(y));
      switch (result_type)
	{
	case T_BIG_INTEGER:
	  if (mpz_cmp(S7_BIG_INTEGER(result), S7_BIG_INTEGER(arg)) != 0) return(sc->F);
	  break;
	  
	case T_BIG_RATIO:
	  if (mpq_cmp(S7_BIG_RATIO(result), S7_BIG_RATIO(arg)) != 0) return(sc->F);
	  break;
	  
	case T_BIG_REAL:
	  if (mpfr_cmp(S7_BIG_REAL(result), S7_BIG_REAL(arg)) != 0) return(sc->F);
	  break;
	  
	case T_BIG_COMPLEX:
	  if (mpc_cmp(S7_BIG_COMPLEX(result), S7_BIG_COMPLEX(arg)) != 0) return(sc->F);
	  break;
	}
    }
  return(sc->T);
}


static s7_pointer big_gcd(s7_scheme *sc, s7_pointer args)
{
  int i;
  bool rats = false, bigs = false;
  s7_pointer x;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if (!s7_is_rational(car(x)))
      return(s7_wrong_type_arg_error(sc, "gcd", i, car(x), "an integer or ratio"));
    else 
      {
	rats = ((rats) || (!s7_is_integer(car(x))));
	bigs = ((bigs) || (is_c_object(car(x))));
      }
  
  if (!bigs)
    return(g_gcd(sc, args));

  if (!rats)
    {
      mpz_t *n;
      n = (mpz_t *)malloc(sizeof(mpz_t));
      mpz_init(*n);
      for (x = args; x != sc->NIL; x = cdr(x)) 
	{
	  mpz_gcd(*n, *n, S7_BIG_INTEGER(promote_number(sc, T_BIG_INTEGER, car(x))));
	  if (mpz_cmp_ui(*n, 1) == 0)
	    {
	      mpz_clear(*n);
	      free(n);
	      return(small_int(sc, 1));
	    }
	}
      return(s7_make_object(sc, big_integer_tag, (void *)n));
    }
  else
    {
      s7_pointer rat;
      mpq_t *q;
      mpz_t n, d;
      rat = promote_number(sc, T_BIG_RATIO, car(args));
      mpz_init_set(n, mpq_numref(S7_BIG_RATIO(rat)));
      mpz_init_set(d, mpq_denref(S7_BIG_RATIO(rat)));
      for (x = cdr(args); x != sc->NIL; x = cdr(x))
	{
	  rat = promote_number(sc, T_BIG_RATIO, car(x));
	  mpz_gcd(n, n, mpq_numref(S7_BIG_RATIO(rat)));
	  mpz_lcm(d, d, mpq_denref(S7_BIG_RATIO(rat)));
	}
      if (mpz_cmp_ui(d, 1) == 0)
	{
	  rat = mpz_to_big_integer(sc, n);
	  mpz_clear(n);
	  mpz_clear(d);
	  return(rat);
	}
      q = (mpq_t *)malloc(sizeof(mpq_t));
      mpq_init(*q);
      mpq_set_num(*q, n);
      mpq_set_den(*q, d);
      mpz_clear(n);
      mpz_clear(d);
      return(s7_make_object(sc, big_ratio_tag, (void *)q));
    }
}


static s7_pointer big_lcm(s7_scheme *sc, s7_pointer args)
{
  s7_pointer x;
  int i;
  bool rats = false, bigs = false;

  for (i = 1, x = args; x != sc->NIL; i++, x = cdr(x)) 
    if (!s7_is_rational(car(x)))
      return(s7_wrong_type_arg_error(sc, "lcm", i, car(x), "an integer or ratio"));
    else 
      {
	rats = ((rats) || (!s7_is_integer(car(x))));
	bigs = ((bigs) || (is_c_object(car(x))));
      }
  
  if (!bigs)
    return(g_lcm(sc, args));

  if (!rats)
    {
      mpz_t *n;
      n = (mpz_t *)malloc(sizeof(mpz_t));
      mpz_init(*n);
      mpz_set_ui(*n, 1);
      for (x = args; x != sc->NIL; x = cdr(x)) 
	{
	  mpz_lcm(*n, *n, S7_BIG_INTEGER(promote_number(sc, T_BIG_INTEGER, car(x))));
	  if (mpz_cmp_ui(*n, 0) == 0)
	    {
	      mpz_clear(*n);
	      free(n);
	      return(small_int(sc, 0));
	    }
	}
      return(s7_make_object(sc, big_integer_tag, (void *)n));
    }
  else
    {
      s7_pointer rat;
      mpq_t *q;
      mpz_t n, d;
      rat = promote_number(sc, T_BIG_RATIO, car(args));
      mpz_init_set(n, mpq_numref(S7_BIG_RATIO(rat)));
      if (mpz_cmp_ui(n, 0) == 0)
	{
	  mpz_clear(n);
	  return(small_int(sc, 0));
	}
      mpz_init_set(d, mpq_denref(S7_BIG_RATIO(rat)));
      for (x = cdr(args); x != sc->NIL; x = cdr(x))
	{
	  rat = promote_number(sc, T_BIG_RATIO, car(x));
	  mpz_lcm(n, n, mpq_numref(S7_BIG_RATIO(rat)));
	  if (mpz_cmp_ui(n, 0) == 0)
	    {
	      mpz_clear(n);
	      mpz_clear(d);
	      return(small_int(sc, 0));
	    }
	  mpz_gcd(d, d, mpq_denref(S7_BIG_RATIO(rat)));
	}
      if (mpz_cmp_ui(d, 1) == 0)
	{
	  rat = mpz_to_big_integer(sc, n);
	  mpz_clear(n);
	  mpz_clear(d);
	  return(rat);
	}
      q = (mpq_t *)malloc(sizeof(mpq_t));
      mpq_init(*q);
      mpq_set_num(*q, n);
      mpq_set_den(*q, d);
      mpz_clear(n);
      mpz_clear(d);
      return(s7_make_object(sc, big_ratio_tag, (void *)q));
    }
}


static s7_pointer g_get_precision(s7_scheme *sc, s7_pointer args)
{
  return(s7_make_integer(sc, (int)mpfr_get_default_prec()));
}


static s7_pointer g_set_precision(s7_scheme *sc, s7_pointer args)
{
  #define H_bignum_precision "(bignum-precision) returns the number of bits used for floats and complex numbers. It can be set."
  mp_prec_t bits;
  if (!s7_is_integer(car(args)))
    return(s7_wrong_type_arg_error(sc, "bignum-precision", 0, car(args), "an integer"));

  bits = (mp_prec_t)s7_integer(car(args));
  mpfr_set_default_prec(bits);
  mpc_set_default_precision(bits);
  s7_symbol_set_value(sc, s7_make_symbol(sc, "pi"), big_pi(sc));
  return(car(args));
}


typedef struct {
  gmp_randstate_t state;
} s7_big_rng_t;


static char *print_big_rng(s7_scheme *sc, void *val)
{
  char *buf;
  s7_big_rng_t *r = (s7_big_rng_t *)val;
  buf = (char *)malloc(64 * sizeof(char));
  snprintf(buf, 64, "#<big-rng %p>", r);
  return(buf);
}


static void free_big_rng(void *val)
{
  s7_big_rng_t *r = (s7_big_rng_t *)val;
  gmp_randclear(r->state);
  free(r);
}


static bool equal_big_rng(void *val1, void *val2)
{
  return(val1 == val2);
}

  
static s7_pointer make_big_random_state(s7_scheme *sc, s7_pointer args)
{
  #define H_make_random_state "(make-random-state seed) returns a new random number state initialized with 'seed'"
  s7_big_rng_t *r;
  s7_pointer seed;

  seed = car(args);
  if (!s7_is_integer(seed))
    return(s7_wrong_type_arg_error(sc, "random", 1, seed, "an integer"));

  if (is_c_object(seed))
    {
      r = (s7_big_rng_t *)calloc(1, sizeof(s7_big_rng_t));
      gmp_randinit_default(r->state);
      gmp_randseed(r->state, S7_BIG_INTEGER(seed));
      return(s7_make_object(sc, big_rng_tag, (void *)r));
    }

  return(g_make_random_state(sc, args));
}


static s7_pointer big_random(s7_scheme *sc, s7_pointer args)
{
  s7_pointer num, state;
  state = sc->NIL;

  num = car(args); 
  if (!s7_is_number(num))
    return(s7_wrong_type_arg_error(sc, "random", 1, num, "a number"));

  if (cdr(args) != sc->NIL)
    {
      state = cadr(args);
      if ((!is_c_object(state)) ||
	  ((c_object_type(state) != big_rng_tag) &&
	   (c_object_type(state) != rng_tag)))
	return(s7_wrong_type_arg_error(sc, "random", 2, state, "a random-state object, if anything"));
    }

  if (big_is_zero(sc, args) == sc->T)
    return(num);

  if ((is_c_object(num)) ||
      ((is_c_object(state)) &&
       (c_object_type(state) == big_rng_tag)))
    {
      /* bignum case -- provide a state if none was passed,
       *   promote num if bignum state was passed but num is not a bignum
       *   if num==0, just return 0 (above) since gmp otherwise throws an arithmetic exception
       */
      s7_big_rng_t *r = NULL;

      if (state == sc->NIL)
	{
	  /* no state passed, so make one */

	  if (!sc->default_big_rng)
	    {
	      mpz_t seed;
	      r = (s7_big_rng_t *)calloc(1, sizeof(s7_big_rng_t));
	      mpz_init_set_ui(seed, (unsigned int)time(NULL));
	      gmp_randinit_default(r->state);
	      gmp_randseed(r->state, seed);
	      mpz_clear(seed);
	      sc->default_big_rng = (void *)r;
	    }
	  else r = (s7_big_rng_t *)(sc->default_big_rng);
	}
      else
	{
	  /* state was passed, check its type */

	  if (c_object_type(state) == rng_tag)
	    {
	      /* here "num" is a bignum, the state was passed, but it is intended for non-bignums */
	      if (c_object_type(num) == big_real_tag)
		num = s7_make_real(sc, (s7_Double)mpfr_get_d(S7_BIG_REAL(num), GMP_RNDN));
	      else
		{
		  if (c_object_type(num) == big_integer_tag)
		    num = s7_make_integer(sc, big_integer_to_s7_Int(S7_BIG_INTEGER(num)));
		  else
		    {
		      if (c_object_type(num) == big_ratio_tag)
			num = s7_make_ratio(sc, 
					    big_integer_to_s7_Int(mpq_numref(S7_BIG_RATIO(num))), 
					    big_integer_to_s7_Int(mpq_denref(S7_BIG_RATIO(num))));
		    }
		}
	      return(g_random(sc, make_list_2(sc, num, state)));
	    }
	  r = (s7_big_rng_t *)s7_object_value(state);
	}

      if (!is_c_object(num))
	{
	  switch (number_type(num))
	    {
	    case NUM_INT:
	      num = promote_number(sc, T_BIG_INTEGER, num);
	      break;

	    case NUM_RATIO:
	      num = promote_number(sc, T_BIG_RATIO, num);
	      break;

	    case NUM_REAL:
	    case NUM_REAL2:
	      num = promote_number(sc, T_BIG_REAL, num);
	      break;

	    default:
	      num = promote_number(sc, T_BIG_COMPLEX, num);
	      break;
	    }
	}

      /* finally both the state and the number are big */
	    
      if (c_object_type(num) == big_integer_tag)
	{
	  mpz_t *n;
	  n = (mpz_t *)malloc(sizeof(mpz_t));
	  mpz_init(*n);
#if HAVE_PTHREADS
	  pthread_mutex_lock(&rng_lock);
#endif
	  mpz_urandomm(*n, r->state, S7_BIG_INTEGER(num));
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&rng_lock);
#endif
	  return(s7_make_object(sc, big_integer_tag, (void *)n));
	}
      
      if (c_object_type(num) == big_ratio_tag)
	{
	  mpfr_t *n;
	  mpfr_t rat;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set_ui(*n, 1, GMP_RNDN);
#if HAVE_PTHREADS
	  pthread_mutex_lock(&rng_lock);
#endif
	  mpfr_urandomb(*n, r->state);
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&rng_lock);
#endif
	  mpfr_init_set_q(rat, S7_BIG_RATIO(num), GMP_RNDN);
	  mpfr_mul(*n, *n, rat, GMP_RNDN);
	  mpfr_clear(rat);
	  return(big_rationalize(sc, make_list_1(sc, s7_make_object(sc, big_real_tag, (void *)n))));
	}
      
      if (c_object_type(num) == big_real_tag)
	{
	  mpfr_t *n;
	  n = (mpfr_t *)malloc(sizeof(mpfr_t));
	  mpfr_init_set_ui(*n, 1, GMP_RNDN);
#if HAVE_PTHREADS
	  pthread_mutex_lock(&rng_lock);
#endif
	  mpfr_urandomb(*n, r->state);
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&rng_lock);
#endif
	  mpfr_mul(*n, *n, S7_BIG_REAL(num), GMP_RNDN);
	  return(s7_make_object(sc, big_real_tag, (void *)n));
	}
      
      if (c_object_type(num) == big_complex_tag)
	{
	  mpc_t *n;
	  n = (mpc_t *)malloc(sizeof(mpc_t));
	  mpc_init(*n);
#if HAVE_PTHREADS
	  pthread_mutex_lock(&rng_lock);
#endif
	  mpc_urandom(*n, r->state);
#if HAVE_PTHREADS
	  pthread_mutex_unlock(&rng_lock);
#endif
	  mpfr_mul(mpc_realref(*n), mpc_realref(*n), mpc_realref(S7_BIG_COMPLEX(num)), GMP_RNDN);
	  mpfr_mul(mpc_imagref(*n), mpc_imagref(*n), mpc_imagref(S7_BIG_COMPLEX(num)), GMP_RNDN);
	  return(s7_make_object(sc, big_complex_tag, (void *)n));
	}
    }

  return(g_random(sc, args));
}


static void s7_gmp_init(s7_scheme *sc)
{
  big_integer_tag = s7_new_type_x("<big-integer>", print_big_integer, free_big_integer, equal_big_integer, NULL, NULL, NULL, NULL, copy_big_integer, NULL);
  big_ratio_tag =   s7_new_type_x("<big-ratio>",   print_big_ratio,   free_big_ratio,   equal_big_ratio,   NULL, NULL, NULL, NULL, copy_big_ratio, NULL);
  big_real_tag =    s7_new_type_x("<big-real>",    print_big_real,    free_big_real,    equal_big_real,    NULL, NULL, NULL, NULL, copy_big_real, NULL);
  big_complex_tag = s7_new_type_x("<big-complex>", print_big_complex, free_big_complex, equal_big_complex, NULL, NULL, NULL, NULL, copy_big_complex, NULL);

  s7_define_function(sc, "+",                   big_add,              0, 0, true,  H_add);
  s7_define_function(sc, "-",                   big_subtract,         1, 0, true,  H_subtract);
  s7_define_function(sc, "*",                   big_multiply,         0, 0, true,  H_multiply);
  s7_define_function(sc, "/",                   big_divide,           1, 0, true,  H_divide);

  s7_define_function(sc, "max",                 big_max,              1, 0, true,  H_max);
  s7_define_function(sc, "min",                 big_min,              1, 0, true,  H_min);
  s7_define_function(sc, "<",                   big_less,             2, 0, true,  H_less);
  s7_define_function(sc, "<=",                  big_less_or_equal,    2, 0, true,  H_less_or_equal);
  s7_define_function(sc, ">",                   big_greater,          2, 0, true,  H_greater);
  s7_define_function(sc, ">=",                  big_greater_or_equal, 2, 0, true,  H_greater_or_equal);
  s7_define_function(sc, "=",                   big_equal,            2, 0, true,  H_equal);

  s7_define_function(sc, "numerator",           big_numerator,        1, 0, false, H_numerator);
  s7_define_function(sc, "denominator",         big_denominator,      1, 0, false, H_denominator);
  s7_define_function(sc, "rationalize",         big_rationalize,      1, 1, false, H_rationalize);
  s7_define_function(sc, "exact->inexact",      big_exact_to_inexact, 1, 0, false, H_exact_to_inexact);
  s7_define_function(sc, "inexact->exact",      big_inexact_to_exact, 1, 0, false, H_inexact_to_exact);
  s7_define_function(sc, "floor",               big_floor,            1, 0, false, H_floor);
  s7_define_function(sc, "ceiling",             big_ceiling,          1, 0, false, H_ceiling);
  s7_define_function(sc, "truncate",            big_truncate,         1, 0, false, H_truncate);
  s7_define_function(sc, "round",               big_round,            1, 0, false, H_round);
  s7_define_function(sc, "quotient",            big_quotient,         2, 0, false, H_quotient);
  s7_define_function(sc, "remainder",           big_remainder,        2, 0, false, H_remainder);
  s7_define_function(sc, "modulo",              big_modulo,           2, 0, false, H_modulo);
  s7_define_function(sc, "gcd",                 big_gcd,              0, 0, true,  H_gcd);
  s7_define_function(sc, "lcm",                 big_lcm,              0, 0, true,  H_lcm);

  s7_define_function(sc, "make-rectangular",    big_make_rectangular, 2, 0, false, H_make_rectangular);
  s7_define_function(sc, "make-polar",          big_make_polar,       2, 0, false, H_make_polar);
  s7_define_function(sc, "real-part",           big_real_part,        1, 0, false, H_real_part);
  s7_define_function(sc, "imag-part",           big_imag_part,        1, 0, false, H_imag_part);
  s7_define_function(sc, "angle",               big_angle,            1, 0, false, H_angle);
  s7_define_function(sc, "magnitude",           big_magnitude,        1, 0, false, H_magnitude);

  s7_define_function(sc, "lognot",              big_lognot,           1, 0, false, H_lognot);
  s7_define_function(sc, "logior",              big_logior,           1, 0, true,  H_logior);
  s7_define_function(sc, "logxor",              big_logxor,           1, 0, true,  H_logxor);
  s7_define_function(sc, "logand",              big_logand,           1, 0, true,  H_logand);
  s7_define_function(sc, "ash",                 big_ash,              2, 0, false, H_ash);
  s7_define_function(sc, "integer-length",      big_integer_length,   1, 0, false, H_integer_length);
  
  s7_define_function(sc, "even?",               big_is_even,          1, 0, false, H_is_even);
  s7_define_function(sc, "odd?",                big_is_odd,           1, 0, false, H_is_odd);
  s7_define_function(sc, "zero?",               big_is_zero,          1, 0, false, H_is_zero);
  s7_define_function(sc, "positive?",           big_is_positive,      1, 0, false, H_is_positive);
  s7_define_function(sc, "negative?",           big_is_negative,      1, 0, false, H_is_negative);

  s7_define_function(sc, "abs",                 big_abs,              1, 0, false, H_abs);
  s7_define_function(sc, "exp",                 big_exp,              1, 0, false, H_exp);
  s7_define_function(sc, "expt",                big_expt,             2, 0, false, H_expt);
  s7_define_function(sc, "log",                 big_log,              1, 1, false, H_log);
  s7_define_function(sc, "sqrt",                big_sqrt,             1, 0, false, H_sqrt);
  s7_define_function(sc, "sin",                 big_sin,              1, 0, false, H_sin);
  s7_define_function(sc, "cos",                 big_cos,              1, 0, false, H_cos);
  s7_define_function(sc, "tan",                 big_tan,              1, 0, false, H_tan);
  s7_define_function(sc, "asin",                big_asin,             1, 0, false, H_asin);
  s7_define_function(sc, "acos",                big_acos,             1, 0, false, H_acos);
  s7_define_function(sc, "atan",                big_atan,             1, 1, false, H_atan);
  s7_define_function(sc, "sinh",                big_sinh,             1, 0, false, H_sinh);
  s7_define_function(sc, "cosh",                big_cosh,             1, 0, false, H_cosh);
  s7_define_function(sc, "tanh",                big_tanh,             1, 0, false, H_tanh);
  s7_define_function(sc, "asinh",               big_asinh,            1, 0, false, H_asinh);
  s7_define_function(sc, "acosh",               big_acosh,            1, 0, false, H_acosh);
  s7_define_function(sc, "atanh",               big_atanh,            1, 0, false, H_atanh);

  big_rng_tag = s7_new_type("<big-random-number-generator>", print_big_rng, free_big_rng, equal_big_rng, NULL, NULL, NULL);
  s7_define_function(sc, "random",              big_random,           1, 1, false, H_random);
  s7_define_function(sc, "make-random-state",   make_big_random_state,1, 0, false, H_make_random_state);

  s7_define_function(sc, "bignum",              g_bignum,             1, 0, false, H_bignum);
  s7_define_function(sc, "bignum?",             g_is_bignum,          1, 0, false, H_is_bignum);
  s7_define_function_with_setter(sc, "bignum-precision", g_get_precision, g_set_precision, 0, 0, H_bignum_precision);

  add_max = (1 << (s7_int_bits - 1));
  mpfr_set_default_prec((mp_prec_t)128); 
  mpc_set_default_precision((mp_prec_t)128);

  s7_symbol_set_value(sc, s7_make_symbol(sc, "pi"), big_pi(sc));
  g_provide(sc, make_list_1(sc, s7_make_symbol(sc, "gmp")));
}

#endif
/* WITH_GMP */



/* -------------------------------- initialization -------------------------------- */

s7_scheme *s7_init(void) 
{
  int i;
  s7_scheme *sc;
  
  init_ctables();
  
  sc = (s7_scheme *)calloc(1, sizeof(s7_scheme)); /* malloc is not recommended here */
#if HAVE_PTHREADS
  sc->orig_sc = sc;
#endif
  
  sc->gc_off = (bool *)calloc(1, sizeof(bool));
  (*(sc->gc_off)) = true;                         /* sc->args and so on are not set yet, so a gc during init -> segfault */
  sc->longjmp_ok = false;
  
  sc->strbuf_size = INITIAL_STRBUF_SIZE;
  sc->strbuf = (char *)calloc(sc->strbuf_size, sizeof(char));
  
  sc->read_line_buf = NULL;
  sc->read_line_buf_size = 0;

  sc->NIL = &sc->_NIL;
  sc->T = &sc->_T;
  sc->F = &sc->_F;
  sc->EOF_OBJECT = &sc->_EOF_OBJECT;
  sc->UNSPECIFIED = &sc->_UNSPECIFIED;  
  sc->UNDEFINED = &sc->_UNDEFINED;

  set_type(sc->NIL, T_NIL | T_ATOM | T_GC_MARK | T_IMMUTABLE | T_SIMPLE | T_DONT_COPY);
  car(sc->NIL) = cdr(sc->NIL) = sc->UNSPECIFIED;
  
  set_type(sc->T, T_BOOLEAN | T_ATOM | T_GC_MARK | T_IMMUTABLE | T_SIMPLE | T_DONT_COPY);
  car(sc->T) = cdr(sc->T) = sc->UNSPECIFIED;
  
  set_type(sc->F, T_BOOLEAN | T_ATOM | T_GC_MARK | T_IMMUTABLE | T_SIMPLE | T_DONT_COPY);
  car(sc->F) = cdr(sc->F) = sc->UNSPECIFIED;
  
  set_type(sc->EOF_OBJECT, T_UNTYPED | T_ATOM | T_GC_MARK | T_IMMUTABLE | T_SIMPLE | T_DONT_COPY);
  car(sc->EOF_OBJECT) = cdr(sc->EOF_OBJECT) = sc->UNSPECIFIED;
  
  set_type(sc->UNSPECIFIED, T_UNTYPED | T_ATOM | T_GC_MARK | T_IMMUTABLE | T_SIMPLE | T_DONT_COPY);
  car(sc->UNSPECIFIED) = cdr(sc->UNSPECIFIED) = sc->UNSPECIFIED;
  
  set_type(sc->UNDEFINED, T_UNTYPED | T_ATOM | T_GC_MARK | T_IMMUTABLE | T_SIMPLE | T_DONT_COPY);
  car(sc->UNDEFINED) = cdr(sc->UNDEFINED) = sc->UNSPECIFIED;
  
  sc->nil_vector = (s7_pointer *)malloc(BLOCK_VECTOR_SIZE * sizeof(s7_pointer));
  sc->unspecified_vector = (s7_pointer *)malloc(BLOCK_VECTOR_SIZE * sizeof(s7_pointer));
  for (i = 0; i < BLOCK_VECTOR_SIZE; i++) 
    {
      sc->nil_vector[i] = sc->NIL;
      sc->unspecified_vector[i] = sc->UNSPECIFIED;
    }

  sc->input_port = sc->NIL;
  sc->input_port_stack = sc->NIL;
  sc->output_port = sc->NIL;
  sc->error_port = sc->NIL;
  
  sc->code = sc->NIL;
  sc->cur_code = ERROR_INFO_DEFAULT;
  sc->args = sc->NIL;
  sc->value = sc->NIL;
  sc->x = sc->NIL;
  sc->y = sc->NIL;
  sc->z = sc->NIL;
  sc->error_exiter = NULL;
  sc->default_rng = NULL;
  
  sc->heap_size = INITIAL_HEAP_SIZE;
  sc->heap = (s7_pointer *)malloc((sc->heap_size + 1) * sizeof(s7_pointer));
  
  sc->free_heap = (s7_cell **)malloc(sc->heap_size * sizeof(s7_cell *));
  sc->free_heap_top = INITIAL_HEAP_SIZE;
  {
    s7_cell *cells = (s7_cell *)calloc(INITIAL_HEAP_SIZE, sizeof(s7_cell));
    for (i = 0; i < INITIAL_HEAP_SIZE; i++)
      {
	sc->heap[i] = &cells[i];
 	sc->free_heap[i] = sc->heap[i];
 	sc->heap[i]->hloc = i;
      }
    sc->heap[sc->heap_size] = NULL;
  }
  
  /* this has to precede s7_make_* allocations */
  sc->temps_size = GC_TEMPS_SIZE;
  sc->temps_ctr = 0;
  sc->temps = (s7_pointer *)malloc(sc->temps_size * sizeof(s7_pointer));
  for (i = 0; i < sc->temps_size; i++)
    sc->temps[i] = sc->NIL;

  sc->circular_refs = (s7_pointer *)calloc(CIRCULAR_REFS_SIZE, sizeof(s7_pointer));
  
  sc->protected_objects_size = (int *)malloc(sizeof(int));
  (*(sc->protected_objects_size)) = INITIAL_PROTECTED_OBJECTS_SIZE;
  sc->protected_objects_loc = (int *)malloc(sizeof(int));
  (*(sc->protected_objects_loc)) = 0;
  sc->protected_objects = s7_make_vector(sc, INITIAL_PROTECTED_OBJECTS_SIZE); /* realloc happens to the embedded array, so this pointer is global */
  set_immutable(sc->protected_objects);
  typeflag(sc->protected_objects) |= T_DONT_COPY;
  
  sc->stack_top = 0;
  sc->stack = s7_make_vector(sc, INITIAL_STACK_SIZE);
  sc->stack_size = INITIAL_STACK_SIZE;
  sc->stack_size2 = sc->stack_size / 2;
  
  /* keep the symbol table out of the heap */
  sc->symbol_table = (s7_pointer)calloc(1, sizeof(s7_cell));
  set_type(sc->symbol_table, T_VECTOR | T_FINALIZABLE | T_DONT_COPY);
  vector_length(sc->symbol_table) = SYMBOL_TABLE_SIZE;
  vector_elements(sc->symbol_table) = (s7_pointer *)malloc(SYMBOL_TABLE_SIZE * sizeof(s7_pointer));
  s7_vector_fill(sc, sc->symbol_table, sc->NIL);
  sc->symbol_table->hloc = NOT_IN_HEAP;
  
  sc->gensym_counter = (long *)calloc(1, sizeof(long));
  sc->tracing = (bool *)calloc(1, sizeof(bool));
  sc->trace_all = (bool *)calloc(1, sizeof(bool));

#if WITH_ENCAPSULATION
  sc->encapsulators = sc->NIL;
#endif

  sc->trace_list = (s7_pointer *)calloc(INITIAL_TRACE_LIST_SIZE, sizeof(s7_pointer));
  sc->trace_list_size = INITIAL_TRACE_LIST_SIZE;
  sc->trace_top = 0;
  sc->trace_depth = 0;

#if HAVE_PTHREADS
  sc->thread_ids = (int *)calloc(1, sizeof(int));
  sc->thread_id = 0;
  sc->key_values = sc->NIL;
#endif
  
  sc->global_env = s7_cons(sc, s7_make_vector(sc, SYMBOL_TABLE_SIZE), sc->NIL);
  sc->envir = sc->global_env;
  
  /* keep the small_ints out of the heap */
  sc->small_ints = (s7_pointer *)malloc((NUM_SMALL_INTS + 1) * sizeof(s7_pointer));
  for (i = 0; i <= NUM_SMALL_INTS; i++) 
    {
      s7_pointer p;
      p = (s7_pointer)calloc(1, sizeof(s7_cell));
      p->flag = T_OBJECT | T_IMMUTABLE | T_ATOM | T_NUMBER | T_SIMPLE | T_DONT_COPY;
      p->hloc = NOT_IN_HEAP;
      number_type(p) = NUM_INT;
      integer(number(p)) = (s7_Int)i;
      sc->small_ints[i] = p;
    }
  
  sc->real_zero = (s7_pointer)calloc(1, sizeof(s7_cell));
  sc->real_zero->flag = T_OBJECT | T_IMMUTABLE | T_ATOM | T_NUMBER | T_SIMPLE | T_DONT_COPY;
  sc->real_zero->hloc = NOT_IN_HEAP;
  number_type(sc->real_zero) = NUM_REAL;
  real(number(sc->real_zero)) = (s7_Double)0.0;

  sc->real_one = (s7_pointer)calloc(1, sizeof(s7_cell));
  sc->real_one->flag = T_OBJECT | T_IMMUTABLE | T_ATOM | T_NUMBER | T_SIMPLE | T_DONT_COPY;
  sc->real_one->hloc = NOT_IN_HEAP;
  number_type(sc->real_one) = NUM_REAL;
  real(number(sc->real_one)) = (s7_Double)1.0;

  /* initialization of global pointers to special symbols */
  assign_syntax(sc, "lambda",            OP_LAMBDA);
  assign_syntax(sc, "lambda*",           OP_LAMBDA_STAR);      /* part of define* */
  assign_syntax(sc, "quote",             OP_QUOTE);
  assign_syntax(sc, "define",            OP_DEFINE0);
  assign_syntax(sc, "define*",           OP_DEFINE_STAR);
  assign_syntax(sc, "define-constant",   OP_DEFINE_CONSTANT0);
  assign_syntax(sc, "if",                OP_IF0);
  assign_syntax(sc, "begin",             OP_BEGIN);
  assign_syntax(sc, "set!",              OP_SET0);
  assign_syntax(sc, "let",               OP_LET0);
  assign_syntax(sc, "let*",              OP_LET_STAR0);
  assign_syntax(sc, "letrec",            OP_LETREC0);
  assign_syntax(sc, "cond",              OP_COND0);
  assign_syntax(sc, "and",               OP_AND0);
  assign_syntax(sc, "or",                OP_OR0);
  assign_syntax(sc, "case",              OP_CASE0);
  assign_syntax(sc, "macro",             OP_MACRO0);           /* r4rs macro syntax, I think */
  assign_syntax(sc, "defmacro",          OP_DEFMACRO);         /* CL-style macro syntax */
  assign_syntax(sc, "defmacro*",         OP_DEFMACRO_STAR);
  assign_syntax(sc, "define-macro",      OP_DEFINE_MACRO);     /* Scheme-style macro syntax */
  assign_syntax(sc, "define-macro*",     OP_DEFINE_MACRO_STAR); 
  assign_syntax(sc, "define-expansion",  OP_DEFINE_EXPANSION); /* read-time (immediate) macro expansion */
  assign_syntax(sc, "do",                OP_DO);
  assign_syntax(sc, "with-environment",  OP_WITH_ENV0);
  
  sc->LAMBDA = s7_make_symbol(sc, "lambda");
  typeflag(sc->LAMBDA) |= T_DONT_COPY; 
  
  sc->LAMBDA_STAR = s7_make_symbol(sc, "lambda*");
  typeflag(sc->LAMBDA_STAR) |= T_DONT_COPY; 
  
  sc->QUOTE = s7_make_symbol(sc, "quote");
  typeflag(sc->QUOTE) |= T_DONT_COPY; 
  
  sc->QUASIQUOTE = s7_make_symbol(sc, "quasiquote");
  typeflag(sc->QUASIQUOTE) |= T_DONT_COPY;  /* add the immutable bit later */
  
  sc->UNQUOTE = s7_make_symbol(sc, "unquote");
  typeflag(sc->UNQUOTE) |= T_DONT_COPY; 
  
  sc->UNQUOTE_SPLICING = s7_make_symbol(sc, "unquote-splicing");
  typeflag(sc->UNQUOTE_SPLICING) |= T_DONT_COPY; 
  
  sc->MACROEXPAND = s7_make_symbol(sc, "macroexpand");
  typeflag(sc->MACROEXPAND) |= T_DONT_COPY; 
  
  sc->FEED_TO = s7_make_symbol(sc, "=>");
  typeflag(sc->FEED_TO) |= T_DONT_COPY; 
  
  #define object_set_name "(generalized set!)"
  sc->OBJECT_SET = s7_make_symbol(sc, object_set_name);   /* will call g_object_set */
  typeflag(sc->OBJECT_SET) |= T_DONT_COPY; 

  sc->APPLY = s7_make_symbol(sc, "apply");
  typeflag(sc->APPLY) |= T_DONT_COPY; 
  
  sc->CONS = s7_make_symbol(sc, "cons");
  typeflag(sc->CONS) |= T_DONT_COPY; 
  
  sc->APPEND = s7_make_symbol(sc, "append");
  typeflag(sc->APPEND) |= T_DONT_COPY; 
  
  sc->CDR = s7_make_symbol(sc, "cdr");
  typeflag(sc->CDR) |= T_DONT_COPY; 
  
  sc->ELSE = s7_make_symbol(sc, "else");
  typeflag(sc->ELSE) |= T_DONT_COPY; 
  add_to_current_environment(sc, sc->ELSE, sc->T); 

  sc->VECTOR = s7_make_symbol(sc, "vector");
  typeflag(sc->VECTOR) |= T_DONT_COPY; 
  
  sc->VALUES = s7_make_symbol(sc, "values");
  typeflag(sc->VALUES) |= T_DONT_COPY; 

  sc->ERROR = s7_make_symbol(sc, "error");
  typeflag(sc->ERROR) |= T_DONT_COPY; 

  sc->WRONG_TYPE_ARG = s7_make_symbol(sc, "wrong-type-arg");
  typeflag(sc->WRONG_TYPE_ARG) |= T_DONT_COPY; 

  sc->WRONG_NUMBER_OF_ARGS = s7_make_symbol(sc, "wrong-number-of-args");
  typeflag(sc->WRONG_NUMBER_OF_ARGS) |= T_DONT_COPY; 

  sc->FORMAT_ERROR = s7_make_symbol(sc, "format-error");
  typeflag(sc->FORMAT_ERROR) |= T_DONT_COPY; 

  sc->OUT_OF_RANGE = s7_make_symbol(sc, "out-of-range");
  typeflag(sc->OUT_OF_RANGE) |= T_DONT_COPY; 

  sc->KEY_KEY = s7_make_keyword(sc, "key");
  typeflag(sc->KEY_KEY) |= T_DONT_COPY; 
  
  sc->KEY_OPTIONAL = s7_make_keyword(sc, "optional");
  typeflag(sc->KEY_OPTIONAL) |= T_DONT_COPY; 
  
  sc->KEY_REST = s7_make_keyword(sc, "rest");
  typeflag(sc->KEY_REST) |= T_DONT_COPY; 

  sc->__FUNC__ = s7_make_symbol(sc, "__func__");
  typeflag(sc->__FUNC__) |= T_DONT_COPY; 

  sc->ERROR_HOOK = s7_make_symbol(sc, "*error-hook*");
  typeflag(sc->ERROR_HOOK) |= T_DONT_COPY; 

  sc->TRACE_HOOK = s7_make_symbol(sc, "*trace-hook*");
  typeflag(sc->TRACE_HOOK) |= T_DONT_COPY; 

  sc->SET = s7_make_symbol(sc, "set!");
  typeflag(sc->SET) |= T_DONT_COPY; 

  (*(sc->gc_off)) = false;


  /* pws first so that make-procedure-with-setter has a type tag */
  s7_define_function(sc, "make-procedure-with-setter",         g_make_procedure_with_setter,         2, 0, false, H_make_procedure_with_setter);
  s7_define_function(sc, "procedure-with-setter?",             g_is_procedure_with_setter,           1, 0, false, H_is_procedure_with_setter);
  s7_define_function(sc, "procedure-with-setter-setter-arity", g_procedure_with_setter_setter_arity, 1, 0, false, "kludge to get setter's arity");
  pws_tag = s7_new_type("<procedure-with-setter>", pws_print, pws_free,	pws_equal, pws_mark, pws_apply,	pws_set);
  

  s7_define_function(sc, "gensym",                  g_gensym,                  0, 1, false, H_gensym);
  s7_define_function(sc, "symbol-table",            g_symbol_table,            0, 0, false, H_symbol_table);
  s7_define_function(sc, "symbol?",                 g_is_symbol,               1, 0, false, H_is_symbol);
  s7_define_function(sc, "symbol->string",          g_symbol_to_string,        1, 0, false, H_symbol_to_string);
  s7_define_function(sc, "string->symbol",          g_string_to_symbol,        1, 0, false, H_string_to_symbol);
  s7_define_function(sc, "symbol->value",           g_symbol_to_value,         1, 1, false, H_symbol_to_value);
#if WITH_PROFILING
  s7_define_function(sc, "symbol-calls",            g_symbol_calls,            1, 0, false, H_symbol_calls);
#endif
  
  s7_define_function(sc, "global-environment",      g_global_environment,      0, 0, false, H_global_environment);
  s7_define_function(sc, "current-environment",     g_current_environment,     0, CURRENT_ENVIRONMENT_OPTARGS, false, H_current_environment);
  s7_define_function(sc, "provided?",               g_is_provided,             1, 0, false, H_is_provided);
  s7_define_function(sc, "provide",                 g_provide,                 1, 0, false, H_provide);
  s7_define_function(sc, "defined?",                g_is_defined,              1, 1, false, H_is_defined);
  s7_define_function(sc, "constant?",               g_is_constant,             1, 0, false, H_is_constant);

  
  s7_define_function(sc, "keyword?",                g_is_keyword,              1, 0, false, H_is_keyword);
  s7_define_function(sc, "make-keyword",            g_make_keyword,            1, 0, false, H_make_keyword);
  s7_define_function(sc, "symbol->keyword",         g_symbol_to_keyword,       1, 0, false, H_symbol_to_keyword);
  s7_define_function(sc, "keyword->symbol",         g_keyword_to_symbol,       1, 0, false, H_keyword_to_symbol);
  

  s7_define_function(sc, "hash-table?",             g_is_hash_table,           1, 0, false, H_is_hash_table);
  s7_define_function(sc, "make-hash-table",         g_make_hash_table,         0, 1, false, H_make_hash_table);
  s7_define_function(sc, "hash-table-ref",          g_hash_table_ref,          2, 0, false, H_hash_table_ref);
  s7_define_set_function(sc, "hash-table-set!",     g_hash_table_set,          3, 0, false, H_hash_table_set);
  s7_define_function(sc, "hash-table-size",         g_hash_table_size,         1, 0, false, H_hash_table_size);
  s7_define_function(sc, "hash-table-for-each",     g_hash_table_for_each,     2, 0, true,  H_hash_table_for_each);  
  
  s7_define_function(sc, "port-line-number",        g_port_line_number,        1, 0, false, H_port_line_number);
  s7_define_function(sc, "port-filename",           g_port_filename,           1, 0, false, H_port_filename);
  
  s7_define_function(sc, "input-port?",             g_is_input_port,           1, 0, false, H_is_input_port);
  s7_define_function(sc, "output-port?",            g_is_output_port,          1, 0, false, H_is_output_port);
  s7_define_function(sc, "char-ready?",             g_is_char_ready,           0, 1, false, H_is_char_ready);
  s7_define_function(sc, "eof-object?",             g_is_eof_object,           1, 0, false, H_is_eof_object);
  
  s7_define_function(sc, "current-input-port",      g_current_input_port,      0, 0, false, H_current_input_port);
  s7_define_function(sc, "set-current-input-port",  g_set_current_input_port,  1, 0, false, H_set_current_input_port);
  s7_define_function(sc, "current-output-port",     g_current_output_port,     0, 0, false, H_current_output_port);
  s7_define_function(sc, "set-current-output-port", g_set_current_output_port, 1, 0, false, H_set_current_output_port);
  s7_define_function(sc, "current-error-port",      g_current_error_port,      0, 0, false, H_current_error_port);
  s7_define_function(sc, "set-current-error-port",  g_set_current_error_port,  1, 0, false, H_set_current_error_port);
  s7_define_function(sc, "close-input-port",        g_close_input_port,        1, 0, false, H_close_input_port);
  s7_define_function(sc, "close-output-port",       g_close_output_port,       1, 0, false, H_close_output_port);
  s7_define_function(sc, "open-input-file",         g_open_input_file,         1, 1, false, H_open_input_file);
  s7_define_function(sc, "open-output-file",        g_open_output_file,        1, 1, false, H_open_output_file);
  s7_define_function(sc, "open-input-string",       g_open_input_string,       1, 0, false, H_open_input_string);
  s7_define_function(sc, "open-output-string",      g_open_output_string,      0, 0, false, H_open_output_string);
  s7_define_function(sc, "get-output-string",       g_get_output_string,       1, 0, false, H_get_output_string);
  
  s7_define_function(sc, "read-char",               g_read_char,               0, 1, false, H_read_char);
  s7_define_function(sc, "peek-char",               g_peek_char,               0, 1, false, H_peek_char);
  s7_define_function(sc, "read",                    g_read,                    0, 1, false, H_read);
  s7_define_function(sc, "newline",                 g_newline,                 0, 1, false, H_newline);
  s7_define_function(sc, "write-char",              g_write_char,              1, 1, false, H_write_char);
  s7_define_function(sc, "write",                   g_write,                   1, 1, false, H_write);
  s7_define_function(sc, "display",                 g_display,                 1, 1, false, H_display);
  s7_define_function(sc, "read-byte",               g_read_byte,               0, 1, false, H_read_byte);
  s7_define_function(sc, "write-byte",              g_write_byte,              1, 1, false, H_write_byte);
  s7_define_function(sc, "read-line",               g_read_line,               0, 2, false, H_read_line);
  
  s7_define_function(sc, "call-with-input-string",  g_call_with_input_string,  2, 0, false, H_call_with_input_string);
  s7_define_function(sc, "call-with-input-file",    g_call_with_input_file,    2, 0, false, H_call_with_input_file);
  s7_define_function(sc, "with-input-from-string",  g_with_input_from_string,  2, 0, false, H_with_input_from_string);
  s7_define_function(sc, "with-input-from-file",    g_with_input_from_file,    2, 0, false, H_with_input_from_file);
  
  s7_define_function(sc, "call-with-output-string", g_call_with_output_string, 1, 0, false, H_call_with_output_string);
  s7_define_function(sc, "call-with-output-file",   g_call_with_output_file,   2, 0, false, H_call_with_output_file);
  s7_define_function(sc, "with-output-to-string",   g_with_output_to_string,   1, 0, false, H_with_output_to_string);
  s7_define_function(sc, "with-output-to-file",     g_with_output_to_file,     2, 0, false, H_with_output_to_file);
  
  
  s7_define_function(sc, "number->string",          g_number_to_string,        1, 2, false, H_number_to_string);
  s7_define_function(sc, "string->number",          g_string_to_number,        1, 2, false, H_string_to_number);
  s7_define_function(sc, "make-polar",              g_make_polar,              2, 0, false, H_make_polar);
  s7_define_function(sc, "make-rectangular",        g_make_rectangular,        2, 0, false, H_make_rectangular);
  s7_define_function(sc, "magnitude",               g_magnitude,               1, 0, false, H_magnitude);
  s7_define_function(sc, "angle",                   g_angle,                   1, 0, false, H_angle);
  s7_define_function(sc, "real-part",               g_real_part,               1, 0, false, H_real_part);
  s7_define_function(sc, "imag-part",               g_imag_part,               1, 0, false, H_imag_part);
  s7_define_function(sc, "numerator",               g_numerator,               1, 0, false, H_numerator);
  s7_define_function(sc, "denominator",             g_denominator,             1, 0, false, H_denominator);
  s7_define_function(sc, "rationalize",             g_rationalize,             1, 1, false, H_rationalize);
  s7_define_function(sc, "abs",                     g_abs,                     1, 0, false, H_abs);
  s7_define_function(sc, "exp",                     g_exp,                     1, 0, false, H_exp);
  s7_define_function(sc, "log",                     g_log,                     1, 1, false, H_log);
  s7_define_function(sc, "sin",                     g_sin,                     1, 0, false, H_sin);
  s7_define_function(sc, "cos",                     g_cos,                     1, 0, false, H_cos);
  s7_define_function(sc, "tan",                     g_tan,                     1, 0, false, H_tan);
  s7_define_function(sc, "asin",                    g_asin,                    1, 0, false, H_asin);
  s7_define_function(sc, "acos",                    g_acos,                    1, 0, false, H_acos);
  s7_define_function(sc, "atan",                    g_atan,                    1, 1, false, H_atan);
  s7_define_function(sc, "sinh",                    g_sinh,                    1, 0, false, H_sinh);
  s7_define_function(sc, "cosh",                    g_cosh,                    1, 0, false, H_cosh);
  s7_define_function(sc, "tanh",                    g_tanh,                    1, 0, false, H_tanh);
  s7_define_function(sc, "asinh",                   g_asinh,                   1, 0, false, H_asinh);
  s7_define_function(sc, "acosh",                   g_acosh,                   1, 0, false, H_acosh);
  s7_define_function(sc, "atanh",                   g_atanh,                   1, 0, false, H_atanh);
  s7_define_function(sc, "sqrt",                    g_sqrt,                    1, 0, false, H_sqrt);
  s7_define_function(sc, "expt",                    g_expt,                    2, 0, false, H_expt);
  s7_define_function(sc, "floor",                   g_floor,                   1, 0, false, H_floor);
  s7_define_function(sc, "ceiling",                 g_ceiling,                 1, 0, false, H_ceiling);
  s7_define_function(sc, "truncate",                g_truncate,                1, 0, false, H_truncate);
  s7_define_function(sc, "round",                   g_round,                   1, 0, false, H_round);
  s7_define_function(sc, "lcm",                     g_lcm,                     0, 0, true,  H_lcm);
  s7_define_function(sc, "gcd",                     g_gcd,                     0, 0, true,  H_gcd);
  s7_define_function(sc, "+",                       g_add,                     0, 0, true,  H_add);
  s7_define_function(sc, "-",                       g_subtract,                1, 0, true,  H_subtract);
  s7_define_function(sc, "*",                       g_multiply,                0, 0, true,  H_multiply);
  s7_define_function(sc, "/",                       g_divide,                  1, 0, true,  H_divide);
  s7_define_function(sc, "max",                     g_max,                     1, 0, true,  H_max);
  s7_define_function(sc, "min",                     g_min,                     1, 0, true,  H_min);
  s7_define_function(sc, "quotient",                g_quotient,                2, 0, false, H_quotient);
  s7_define_function(sc, "remainder",               g_remainder,               2, 0, false, H_remainder);
  s7_define_function(sc, "modulo",                  g_modulo,                  2, 0, false, H_modulo);
  s7_define_function(sc, "=",                       g_equal,                   2, 0, true,  H_equal);
  s7_define_function(sc, "<",                       g_less,                    2, 0, true,  H_less);
  s7_define_function(sc, ">",                       g_greater,                 2, 0, true,  H_greater);
  s7_define_function(sc, "<=",                      g_less_or_equal,           2, 0, true,  H_less_or_equal);
  s7_define_function(sc, ">=",                      g_greater_or_equal,        2, 0, true,  H_greater_or_equal);
  s7_define_function(sc, "number?",                 g_is_number,               1, 0, false, H_is_number);
  s7_define_function(sc, "integer?",                g_is_integer,              1, 0, false, H_is_integer);
  s7_define_function(sc, "real?",                   g_is_real,                 1, 0, false, H_is_real);
  s7_define_function(sc, "complex?",                g_is_complex,              1, 0, false, H_is_complex);
  s7_define_function(sc, "rational?",               g_is_rational,             1, 0, false, H_is_rational);
  s7_define_function(sc, "even?",                   g_is_even,                 1, 0, false, H_is_even);
  s7_define_function(sc, "odd?",                    g_is_odd,                  1, 0, false, H_is_odd);
  s7_define_function(sc, "zero?",                   g_is_zero,                 1, 0, false, H_is_zero);
  s7_define_function(sc, "positive?",               g_is_positive,             1, 0, false, H_is_positive);
  s7_define_function(sc, "negative?",               g_is_negative,             1, 0, false, H_is_negative);
  s7_define_function(sc, "inexact->exact",          g_inexact_to_exact,        1, 0, false, H_inexact_to_exact);
  s7_define_function(sc, "exact->inexact",          g_exact_to_inexact,        1, 0, false, H_exact_to_inexact);
  s7_define_function(sc, "exact?",                  g_is_exact,                1, 0, false, H_is_exact);
  s7_define_function(sc, "inexact?",                g_is_inexact,              1, 0, false, H_is_inexact);

  s7_define_function(sc, "integer-length",          g_integer_length,          1, 0, false, H_integer_length);
  s7_define_function(sc, "logior",                  g_logior,                  1, 0, true,  H_logior);
  s7_define_function(sc, "logxor",                  g_logxor,                  1, 0, true,  H_logxor);
  s7_define_function(sc, "logand",                  g_logand,                  1, 0, true,  H_logand);
  s7_define_function(sc, "lognot",                  g_lognot,                  1, 0, false, H_lognot);
  s7_define_function(sc, "ash",                     g_ash,                     2, 0, false, H_ash);
  
  rng_tag = s7_new_type_x("<random-number-generator>", print_rng, free_rng, equal_rng, NULL, NULL, NULL, NULL, copy_random_state, NULL);
  s7_define_function(sc, "random",                  g_random,                  1, 1, false, H_random);
  s7_define_function(sc, "make-random-state",       g_make_random_state,       1, 0, false, H_make_random_state);

  
  s7_define_function(sc, "char-upcase",             g_char_upcase,             1, 0, false, H_char_upcase);
  s7_define_function(sc, "char-downcase",           g_char_downcase,           1, 0, false, H_char_downcase);
  s7_define_function(sc, "char->integer",           g_char_to_integer,         1, 0, false, H_char_to_integer);
  s7_define_function(sc, "integer->char",           g_integer_to_char,         1, 0, false, H_integer_to_char);
  
  s7_define_function(sc, "char-upper-case?",        g_is_char_upper_case,      1, 0, false, H_is_char_upper_case);
  s7_define_function(sc, "char-lower-case?",        g_is_char_lower_case,      1, 0, false, H_is_char_lower_case);
  s7_define_function(sc, "char-alphabetic?",        g_is_char_alphabetic,      1, 0, false, H_is_char_alphabetic);
  s7_define_function(sc, "char-numeric?",           g_is_char_numeric,         1, 0, false, H_is_char_numeric);
  s7_define_function(sc, "char-whitespace?",        g_is_char_whitespace,      1, 0, false, H_is_char_whitespace);
  s7_define_function(sc, "char?",                   g_is_char,                 1, 0, false, H_is_char);
  
  s7_define_function(sc, "char=?",                  g_chars_are_equal,         2, 0, true,  H_chars_are_equal);
  s7_define_function(sc, "char<?",                  g_chars_are_less,          2, 0, true,  H_chars_are_less);
  s7_define_function(sc, "char>?",                  g_chars_are_greater,       2, 0, true,  H_chars_are_greater);
  s7_define_function(sc, "char<=?",                 g_chars_are_leq,           2, 0, true,  H_chars_are_leq);
  s7_define_function(sc, "char>=?",                 g_chars_are_geq,           2, 0, true,  H_chars_are_geq);
  s7_define_function(sc, "char-ci=?",               g_chars_are_ci_equal,      2, 0, true,  H_chars_are_ci_equal);
  s7_define_function(sc, "char-ci<?",               g_chars_are_ci_less,       2, 0, true,  H_chars_are_ci_less);
  s7_define_function(sc, "char-ci>?",               g_chars_are_ci_greater,    2, 0, true,  H_chars_are_ci_greater);
  s7_define_function(sc, "char-ci<=?",              g_chars_are_ci_leq,        2, 0, true,  H_chars_are_ci_leq);
  s7_define_function(sc, "char-ci>=?",              g_chars_are_ci_geq,        2, 0, true,  H_chars_are_ci_geq);
  
  
  s7_define_function(sc, "string?",                 g_is_string,               1, 0, false, H_is_string);
  s7_define_function(sc, "make-string",             g_make_string,             1, 1, false, H_make_string);
  s7_define_function(sc, "string-length",           g_string_length,           1, 0, false, H_string_length);
  s7_define_function(sc, "string-ref",              g_string_ref,              2, 0, false, H_string_ref);
  s7_define_set_function(sc, "string-set!",         g_string_set,              3, 0, false, H_string_set);
  
  s7_define_function(sc, "string=?",                g_strings_are_equal,       2, 0, true,  H_strings_are_equal);
  s7_define_function(sc, "string<?",                g_strings_are_less,        2, 0, true,  H_strings_are_less);
  s7_define_function(sc, "string>?",                g_strings_are_greater,     2, 0, true,  H_strings_are_greater);
  s7_define_function(sc, "string<=?",               g_strings_are_leq,         2, 0, true,  H_strings_are_leq);
  s7_define_function(sc, "string>=?",               g_strings_are_geq,         2, 0, true,  H_strings_are_geq);
  s7_define_function(sc, "string-ci=?",             g_strings_are_ci_equal,    2, 0, true,  H_strings_are_ci_equal);
  s7_define_function(sc, "string-ci<?",             g_strings_are_ci_less,     2, 0, true,  H_strings_are_ci_less);
  s7_define_function(sc, "string-ci>?",             g_strings_are_ci_greater,  2, 0, true,  H_strings_are_ci_greater);
  s7_define_function(sc, "string-ci<=?",            g_strings_are_ci_leq,      2, 0, true,  H_strings_are_ci_leq);
  s7_define_function(sc, "string-ci>=?",            g_strings_are_ci_geq,      2, 0, true,  H_strings_are_ci_geq);
  
  s7_define_function(sc, "string-append",           g_string_append,           0, 0, true,  H_string_append);
  s7_define_set_function(sc, "string-fill!",        g_string_fill,             2, 0, false, H_string_fill);
  s7_define_function(sc, "string-copy",             g_string_copy,             1, 0, false, H_string_copy);
  s7_define_function(sc, "substring",               g_substring,               2, 1, false, H_substring);
  s7_define_function(sc, "string",                  g_string,                  0, 0, true,  H_string);
  s7_define_function(sc, "list->string",            g_list_to_string,          1, 0, false, H_list_to_string);
  s7_define_function(sc, "string->list",            g_string_to_list,          1, 0, false, H_string_to_list);
  s7_define_function(sc, "object->string",          g_object_to_string,        1, 0, false, H_object_to_string);
  s7_define_function(sc, "format",                  g_format,                  1, 0, true,  H_format);
  s7_define_function(sc, "string-for-each",         g_string_for_each,         2, 0, true,  H_string_for_each);  


  s7_define_function(sc, "null?",                   g_is_null,                 1, 0, false, H_is_null);
  s7_define_function(sc, "list?",                   g_is_list,                 1, 0, false, H_is_list);
  s7_define_function(sc, "pair?",                   g_is_pair,                 1, 0, false, H_is_pair);
  s7_define_function(sc, "reverse",                 g_reverse,                 1, 0, false, H_reverse);
  s7_define_set_function(sc, "reverse!",            g_reverse_in_place,        1, 0, false, H_reverse_in_place); /* used by Snd code */
  s7_define_function(sc, "cons",                    g_cons,                    2, 0, false, H_cons);
  s7_define_function(sc, "car",                     g_car,                     1, 0, false, H_car);
  s7_define_function(sc, "cdr",                     g_cdr,                     1, 0, false, H_cdr);
  s7_define_function(sc, "set-car!",                g_set_car,                 2, 0, false, H_set_car);
  s7_define_function(sc, "set-cdr!",                g_set_cdr,                 2, 0, false, H_set_cdr);
  s7_define_function(sc, "caar",                    g_caar,                    1, 0, false, H_caar);
  s7_define_function(sc, "cadr",                    g_cadr,                    1, 0, false, H_cadr);
  s7_define_function(sc, "cdar",                    g_cdar,                    1, 0, false, H_cdar);
  s7_define_function(sc, "cddr",                    g_cddr,                    1, 0, false, H_cddr);
  s7_define_function(sc, "caaar",                   g_caaar,                   1, 0, false, H_caaar);
  s7_define_function(sc, "caadr",                   g_caadr,                   1, 0, false, H_caadr);
  s7_define_function(sc, "cadar",                   g_cadar,                   1, 0, false, H_cadar);
  s7_define_function(sc, "cdaar",                   g_cdaar,                   1, 0, false, H_cdaar);
  s7_define_function(sc, "caddr",                   g_caddr,                   1, 0, false, H_caddr);
  s7_define_function(sc, "cdddr",                   g_cdddr,                   1, 0, false, H_cdddr);
  s7_define_function(sc, "cdadr",                   g_cdadr,                   1, 0, false, H_cdadr);
  s7_define_function(sc, "cddar",                   g_cddar,                   1, 0, false, H_cddar);
  s7_define_function(sc, "caaaar",                  g_caaaar,                  1, 0, false, H_caaaar);
  s7_define_function(sc, "caaadr",                  g_caaadr,                  1, 0, false, H_caaadr);
  s7_define_function(sc, "caadar",                  g_caadar,                  1, 0, false, H_caadar);
  s7_define_function(sc, "cadaar",                  g_cadaar,                  1, 0, false, H_cadaar);
  s7_define_function(sc, "caaddr",                  g_caaddr,                  1, 0, false, H_caaddr);
  s7_define_function(sc, "cadddr",                  g_cadddr,                  1, 0, false, H_cadddr);
  s7_define_function(sc, "cadadr",                  g_cadadr,                  1, 0, false, H_cadadr);
  s7_define_function(sc, "caddar",                  g_caddar,                  1, 0, false, H_caddar);
  s7_define_function(sc, "cdaaar",                  g_cdaaar,                  1, 0, false, H_cdaaar);
  s7_define_function(sc, "cdaadr",                  g_cdaadr,                  1, 0, false, H_cdaadr);
  s7_define_function(sc, "cdadar",                  g_cdadar,                  1, 0, false, H_cdadar);
  s7_define_function(sc, "cddaar",                  g_cddaar,                  1, 0, false, H_cddaar);
  s7_define_function(sc, "cdaddr",                  g_cdaddr,                  1, 0, false, H_cdaddr);
  s7_define_function(sc, "cddddr",                  g_cddddr,                  1, 0, false, H_cddddr);
  s7_define_function(sc, "cddadr",                  g_cddadr,                  1, 0, false, H_cddadr);
  s7_define_function(sc, "cdddar",                  g_cdddar,                  1, 0, false, H_cdddar);
  s7_define_function(sc, "assq",                    g_assq,                    2, 0, false, H_assq);
  s7_define_function(sc, "assv",                    g_assv,                    2, 0, false, H_assv);
  s7_define_function(sc, "assoc",                   g_assoc,                   2, 0, false, H_assoc);
  s7_define_function(sc, "memq",                    g_memq,                    2, 0, false, H_memq);
  s7_define_function(sc, "memv",                    g_memv,                    2, 0, false, H_memv);
  s7_define_function(sc, "member",                  g_member,                  2, 0, false, H_member);
  s7_define_function(sc, "append",                  g_append,                  0, 0, true,  H_append);
  s7_define_function(sc, "list",                    g_list,                    0, 0, true,  H_list);
  s7_define_function(sc, "list-ref",                g_list_ref,                2, 0, false, H_list_ref);
  s7_define_set_function(sc, "list-set!",           g_list_set,                3, 0, false, H_list_set);
  s7_define_function(sc, "list-tail",               g_list_tail,               2, 0, false, H_list_tail);
  s7_define_function(sc, "make-list",               g_make_list,               1, 1, false, H_make_list);

  s7_define_function(sc, "length",                  g_length,                  1, 0, false, H_length);
  s7_define_function(sc, "copy",                    g_copy,                    1, 0, false, H_copy);
  s7_define_set_function(sc, "fill!",               g_fill,                    2, 0, false, H_fill);
  
  
  s7_define_function(sc, "vector?",                 g_is_vector,               1, 0, false, H_is_vector);
  s7_define_function(sc, "vector->list",            g_vector_to_list,          1, 0, false, H_vector_to_list);
  s7_define_function(sc, "list->vector",            g_list_to_vector,          1, 0, false, H_list_to_vector);
  s7_define_set_function(sc, "vector-fill!",        g_vector_fill,             2, 0, false, H_vector_fill);
  s7_define_function(sc, "vector",                  g_vector,                  0, 0, true,  H_vector);
  s7_define_function(sc, "vector-length",           g_vector_length,           1, 0, false, H_vector_length);
  s7_define_function(sc, "vector-ref",              g_vector_ref,              2, 0, VECTOR_REST_ARGS, H_vector_ref);
  s7_define_set_function(sc, "vector-set!",         g_vector_set,              3, 0, VECTOR_REST_ARGS, H_vector_set);
  s7_define_function(sc, "make-vector",             g_make_vector,             1, 1, false, H_make_vector);
#if WITH_MULTIDIMENSIONAL_VECTORS
  s7_define_function(sc, "vector-dimensions",       g_vector_dimensions,       1, 0, false, H_vector_dimensions);
#endif
  s7_define_function(sc, "vector-for-each",         g_vector_for_each,         2, 0, true,  H_vector_for_each);
  s7_define_function(sc, "vector-map",              g_vector_map,              2, 0, true,  H_vector_map);
  s7_define_set_function(sc, "sort!",               g_sort_in_place,           2, 0, false, H_sort_in_place);
  

  s7_define_function(sc, "call/cc",                 g_call_cc,                 1, 0, false, H_call_cc);
  s7_define_function(sc, "call-with-current-continuation", g_call_cc,          1, 0, false, H_call_cc);
  s7_define_function(sc, "call-with-exit",          g_call_with_exit,          1, 0, false, H_call_with_exit);
  s7_define_function(sc, "continuation?",           g_is_continuation,         1, 0, false, H_is_continuation);

  s7_define_function(sc, "load",                    g_load,                    1, 1, false, H_load);
  s7_define_function(sc, "eval",                    g_eval,                    1, 1, false, H_eval);
  s7_define_function(sc, "eval-string",             g_eval_string,             1, 1, false, H_eval_string);
  s7_define_function(sc, "apply",                   g_apply,                   1, 0, true,  H_apply);
  s7_define_function(sc, "for-each",                g_for_each,                2, 0, true,  H_list_for_each);
  s7_define_function(sc, "map",                     g_map,                     2, 0, true,  H_list_map);

  s7_define_function(sc, "values",                  g_values,                  0, 0, true,  H_values);
  s7_define_function(sc, "dynamic-wind",            g_dynamic_wind,            3, 0, false, H_dynamic_wind);
  s7_define_function(sc, "catch",                   g_catch,                   3, 0, false, H_catch);
  s7_define_function(sc, "error",                   g_error,                   0, 0, true,  H_error);
  
  s7_define_function(sc, "trace",                   g_trace,                   0, 0, true,  H_trace);
  s7_define_function(sc, "untrace",                 g_untrace,                 0, 0, true,  H_untrace);
  s7_define_variable(sc, "*trace-hook*", sc->NIL);

  s7_define_function(sc, "stack",                   g_stack,                   0, 1, false, H_stack);
  s7_define_function(sc, "stacktrace",              g_stacktrace,              0, 1, false, H_stacktrace);

  s7_define_function(sc, "gc",                      g_gc,                      0, 1, false, H_gc);
  s7_define_function(sc, "quit",                    g_quit,                    0, 0, false, H_quit);

  s7_define_function(sc, "procedure?",              g_is_procedure,            1, 0, false, H_is_procedure);
  s7_define_function(sc, "procedure-documentation", g_procedure_documentation, 1, 0, false, H_procedure_documentation);
  s7_define_function(sc, "help",                    g_procedure_documentation, 1, 0, false, H_procedure_documentation);
  s7_define_function(sc, "procedure-arity",         g_procedure_arity,         1, 0, false, H_procedure_arity);
  s7_define_function(sc, "procedure-source",        g_procedure_source,        1, 0, false, H_procedure_source);
  s7_define_function(sc, "procedure-environment",   g_procedure_environment,   1, 0, false, H_procedure_environment);
  
  
  s7_define_function(sc, "not",                     g_not,                     1, 0, false, H_not);
  s7_define_function(sc, "boolean?",                g_is_boolean,              1, 0, false, H_is_boolean);
  s7_define_function(sc, "eq?",                     g_is_eq,                   2, 0, false, H_is_eq);
  s7_define_function(sc, "eqv?",                    g_is_eqv,                  2, 0, false, H_is_eqv);
  s7_define_function(sc, "equal?",                  g_is_equal,                2, 0, false, H_is_equal);
  
  s7_define_function(sc, "s7-version",              g_s7_version,              0, 0, false, H_s7_version);
  s7_define_set_function(sc, object_set_name,       g_object_set,              1, 0, true, "internal setter redirection");
  s7_define_function(sc, "_quasiquote_",            g_quasiquote,              2, 0, false, "internal quasiquote handler");
  
  s7_define_variable(sc, "*features*", sc->NIL);
  s7_define_variable(sc, "*load-path*", sc->NIL);
  s7_define_variable(sc, "*vector-print-length*", sc->small_ints[8]);
  s7_define_variable(sc, "*load-hook*", sc->NIL);

  s7_define_variable(sc, "*error-hook*", sc->NIL);
  sc->error_info = s7_make_and_fill_vector(sc, ERROR_INFO_SIZE, ERROR_INFO_DEFAULT);
  s7_define_constant(sc, "*error-info*", sc->error_info);
  
  g_provide(sc, make_list_1(sc, s7_make_symbol(sc, "s7")));

#if HAVE_PTHREADS
  thread_tag = s7_new_type("<thread>",          thread_print, thread_free, thread_equal, thread_mark, NULL, NULL);
  lock_tag =   s7_new_type("<lock>",            lock_print,   lock_free,   lock_equal,   NULL,        NULL, NULL);
  key_tag =    s7_new_type("<thread-variable>", key_print,    key_free,    key_equal,    NULL,        get_key, set_key);

  s7_define_function(sc, "make-thread",             g_make_thread,             1, 0, false, H_make_thread);
  s7_define_function(sc, "join-thread",             g_join_thread,             1, 0, false, H_join_thread);
  s7_define_function(sc, "thread?",                 g_is_thread,               1, 0, false, H_is_thread);

  s7_define_function(sc, "make-lock",               g_make_lock,               0, 0, false, H_make_lock); /* "mutex" is ugly (and opaque) jargon */
  s7_define_function(sc, "grab-lock",               g_grab_lock,               1, 0, false, H_grab_lock);
  s7_define_function(sc, "release-lock",            g_release_lock,            1, 0, false, H_release_lock);
  s7_define_function(sc, "lock?",                   g_is_lock,                 1, 0, false, H_is_lock);

  s7_define_function(sc, "make-thread-variable",    g_make_thread_variable,    0, 0, false, H_make_thread_variable);
  s7_define_function(sc, "thread-variable?",        g_is_thread_variable,      1, 0, false, H_is_thread_variable);

  g_provide(sc, make_list_1(sc, s7_make_symbol(sc, "threads")));
#endif

#if WITH_ENCAPSULATION
  encapsulator_tag = s7_new_type("<encapsulator>",  encapsulator_print, encapsulator_free, encapsulator_equal, encapsulator_mark, encapsulator_apply, NULL);
  s7_define_function(sc, "open-encapsulator",       g_open_encapsulator,       0, 0, false, H_open_encapsulator);
  s7_define_function(sc, "close-encapsulator",      g_close_encapsulator,      1, 0, false, H_close_encapsulator);
  s7_define_function(sc, "encapsulator?",           g_is_encapsulator,         1, 0, false, H_is_encapsulator);
  s7_define_function(sc, "encapsulator-bindings",   g_encapsulator_bindings,   1, 0, false, H_encapsulator_bindings);
#endif

#if WITH_MULTIDIMENSIONAL_VECTORS
  g_provide(sc, make_list_1(sc, s7_make_symbol(sc, "multidimensional-vectors")));
#endif

  sc->VECTOR_FUNCTION = s7_name_to_value(sc, "vector");

  sc->VECTOR_SET = s7_symbol_value(sc, s7_make_symbol(sc, "vector-set!"));
  typeflag(sc->VECTOR_SET) |= T_DONT_COPY; 

  sc->LIST_SET = s7_symbol_value(sc, s7_make_symbol(sc, "list-set!"));
  typeflag(sc->LIST_SET) |= T_DONT_COPY; 

  sc->HASH_TABLE_SET = s7_symbol_value(sc, s7_make_symbol(sc, "hash-table-set!"));
  typeflag(sc->HASH_TABLE_SET) |= T_DONT_COPY; 

  sc->STRING_SET = s7_symbol_value(sc, s7_make_symbol(sc, "string-set!"));
  typeflag(sc->STRING_SET) |= T_DONT_COPY; 

  {
    int top;
    top = sizeof(s7_Int);
    s7_int_max = (top == 8) ? LONG_MAX : SHRT_MAX;
    s7_int_min = (top == 8) ? LONG_MIN : SHRT_MIN;
    s7_int_bits = (top == 8) ? 63 : 31;
    s7_int_digits = (top == 8) ? 18 : 8;

    s7_define_constant(sc, "most-positive-fixnum", s7_make_integer(sc, (top == 8) ? LLONG_MAX : ((top == 4) ? LONG_MAX : SHRT_MAX)));
    s7_define_constant(sc, "most-negative-fixnum", s7_make_integer(sc, (top == 8) ? LLONG_MIN : ((top == 4) ? LONG_MIN : SHRT_MIN)));

    if (top == 4) default_rationalize_error = 1.0e-6;
    s7_define_constant(sc, "pi", s7_make_real(sc, 3.1415926535897932384626433832795029L)); /* M_PI is not good enough for s7_Double = long double */

    /* for s7_Double, float gives about 9 digits, double 18, long Double claims 28 but I don't see more than about 22? */
  }

  s7_eval_c_string(sc, "(macro quasiquote (lambda (l) (_quasiquote_ 0 (cadr l))))");

#if WITH_GMP
  s7_gmp_init(sc);
#endif

  /* s7_define_function(sc, "dump-heap", g_dump_heap, 0, 0, false, "hiho"); */

  /* macroexpand */
  s7_eval_c_string(sc, "(define-macro (macroexpand mac) `(,(procedure-source (car mac)) ',mac))");

  /* multiple values */
  /* call-with-values is almost a no-op in this context */
  s7_eval_c_string(sc, "(define-macro (call-with-values producer consumer) `(,consumer (,producer)))"); 
  /* (call-with-values (lambda () (values 1 2 3)) +) */

  s7_eval_c_string(sc, "(define-macro (multiple-value-bind vars expression . body) `((lambda ,vars ,@body) ,expression))");
  /* (multiple-value-bind (a b) (values 1 2) (+ a b)) */
  /*   named "receive" in srfi-8 which strikes me as perverse */

  s7_eval_c_string(sc, "(define-macro (multiple-value-set! vars expr . body)\n\
                          (let ((local-vars (map (lambda (n) (gensym)) vars)))\n\
                            `((lambda ,local-vars ,@(map (lambda (n ln) `(set! ,n ,ln)) vars local-vars) ,@body) ,expr)))");

#if 0
  /*
(define-macro (let*-values vals . body)
  (let ((args '())
	(exprs '()))
    (for-each
     (lambda (arg+expr)
       (set! args (cons (car arg+expr) args))
       (set! exprs (cons (cadr arg+expr) exprs)))
     vals)
    (let ((form `((lambda ,(car args) ,@body) ,(car exprs))))
      (if (not (null? (cdr args)))
	  (for-each
	   (lambda (arg expr)
	     (set! form `((lambda ,arg ,form) ,expr)))
	   (cdr args)
	   (cdr exprs)))
      form)))
  */
#endif


#if WITH_ENCAPSULATION
  s7_eval_c_string(sc, "                                 \n\
      (define-macro (encapsulate . body)                 \n\
	(let ((encap (gensym)))                          \n\
	  `(let ((,encap (open-encapsulator)))           \n\
	     (dynamic-wind                               \n\
		 (lambda ()                              \n\
		   #f)                                   \n\
		 (lambda ()                              \n\
		   ,@body)                               \n\
		 (lambda ()                              \n\
		   ((,encap))                            \n\
		   (close-encapsulator ,encap))))))");
#endif

  /* s7_eval_c_string(sc, "(define (ratio? n) (and (rational? n) (not (integer? n))))"); */


#if WITH_FORCE
  s7_eval_c_string("(define (force object) (object))");

  s7_eval_c_string("(define-macro (delay expression)         \n\
                      `(let ((result-ready? #f)              \n\
  	                     (result #f))                    \n\
                         (lambda ()                          \n\
                           (if result-ready?                 \n\
	                       result                        \n\
	                       (let ((x (let ()              \n\
		                          ,expression)))     \n\
	                         (if result-ready?           \n\
		                     result                  \n\
                                    (begin                   \n\
		                      (set! result-ready? #t)\n\
		                      (set! result x)        \n\
		                      result)))))))");
#endif
	   
  return(sc);
}

/* TODO: how to trace setter [s7_object_set?] mus-srate for example */

#if 0
/*  here are the ->* functions, without c-type stuff

(define ->string object->string) ; or perhaps (format #f "~A" x)

(define (->symbol x)
  (string->symbol (object->string x)))

(define (->real x)
  (if (number? x)
      (if (complex? x)
	  (real-part x)
	  (exact->inexact x))
      (if (string? x)
	  (exact->inexact (string->number x))
	  (if (char? x)
	      (exact->inexact (->integer x))
	      (error 'wrong-type-arg "can't convert ~A to real" x)))))

(define (->integer x)
  (if (number? x)
      (if (complex? x)
	  (floor (real-part x))
	  (floor x))
      (if (string? x)
	  (floor (string->number x))
	  (if (char? x)
	      (char->integer x)
	      (error 'wrong-type-arg "can't convert ~A to integer" x)))))  

(define (->ratio x)
  (if (number? x)
      (if (complex? x)
	  (rationalize (real-part x))
	  (rationalize x))
      (if (string? x)
	  (rationalize (string->number x))
	  (if (char? x)
	      (char->integer x)
	      (error 'wrong-type-arg "can't convert ~A to ratio" x)))))

(define (->number x)
  (if (number? x)
      x
      (if (string? x)
	  (string->number x)
	  (if (char? x)
	      (char->integer x)
	      (error 'wrong-type-arg "can't convert ~A to number" x)))))

(define (->character x)
  (if (char? x)
      x
      (if (number? x)
	  (integer->char (->integer x))
	  (if (and (string? x)
		   (= (length x) 1))
	      (string-ref x 0)
	      (error 'wrong-type-arg "can't convert ~A to character" x)))))	      

(define (->list x)
  (if (list? x)
      x
      (if (pair? x)
	  (list (car x) (cdr x))
	  (if (vector? x)
	      (vector->list x)
	      (if (string? x)
		  (string->list x)
		  (error 'wrong-type-arg "can't convert ~A to list" x))))))	      

(define (->vector x)
  (if (vector? x)
      x
      (if (list? x)
	  (list->vector x)
	  (if (string? x)
	      (list->vector (string->list x))
	      (if (pair? x)
		  (list->vector (->list x))
		  (error 'wrong-type-arg "can't convert ~A to vector" x))))))
  */
#endif

