
#ifndef _DEBUG1_H
#define _DEBUG1_H
#ifdef DEBUG1
#define dbg(...) fprintf(stderr, __VA_ARGS__)
#else
#define dbg(...)
#endif
#endif

#ifndef _MYDEBUG_H
#define _MYDEBUG_H
#ifdef MYDEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...)
#endif
#endif

 
#ifndef _EXTRACREDIT_H
#define _EXTRACREDIT_H
#ifdef EXTRACREDIT
#define EC 1
#else
#define EC 0
#endif
#endif 

#define PRINTUSAGE fprintf(stderr, "filesec [-devh] [-D DBGVAL] [-p PASSFILE] [-s] infile outfile \n \
-d: decrypt \n \
-e: encrypt \n \
-v: version \n \
-h: help message \n \
-D [arg]: Debug value \n \
-p [arg]: file containing password \n \
-s: prompts user for password twice \n \
infile: file to read from \n \
outfile: file to write to\n"); 


#define DBG_NONE 0x00

#define DBG_ENTEXIT(x, y) if (x & 0x01) dbg(y) // entry/exit to fxn
#define DBG_LIB(x, y) if (x & 0x02) dbg(y)// before/after call to lib fxn
#define DBG_SYSCALL(x, y) if (x & 0x04) dbg(y)// before/after call to syscall
#define DBG_ARGS(x, y) if (x & 0x10) dbg(y)// print args before calling fxn
#define DBG_RET(x, y) if (x &	0x20) dbg(y)// print retval before return from fxn

#define VERSION "10.2.1"

