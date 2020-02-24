#ifndef _MYDEBUG_H
 
#define _MYDEBUG_H
 
#ifdef MYDEBUG
 
#include <stdio.h>
 
#define debug(...) printf(__VA_ARGS__)
 
#else
 
#define log_printf
 
#endif
 
#endif