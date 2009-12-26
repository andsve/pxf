#ifndef _S7_CONFIG_H_
#define _S7_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#define snprintf _snprintf
#define strtoll(a, b, c) atoi((a))

#define HAVE_PTHREADS 0
#define WITH_COMPLEX 0
#define HAVE_COMPLEX_TRIG 0
#define HAVE_NESTED_FUNCTIONS 0
#define WITH_GMP 0

#endif // _S7_CONFIG_H_