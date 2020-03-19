int lkreport(int fd, int flags);
int lkmalloc(int size, void **ptr, int flags);
int lkfree(void **pts, int flags);

#define LKM_REG(x) if(x & 0x0) 1
#define LKM_INIT(x) if(x & 0x1) 1
#define LKM_OVER(x) if(x & 0x2) 1
#define LKM_UNDER 0x4