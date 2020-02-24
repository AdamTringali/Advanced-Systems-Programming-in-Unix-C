#ifndef _MYDEBUG_H
 
#define _MYDEBUG_H
 
#ifdef MYDEBUG
 
#define debug(...) printf(__VA_ARGS__)
 
#else
 
#define debug
 
#endif
 
#endif