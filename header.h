int lkreport(int fd, void* flags);
int lkmalloc(int size, void **ptr, int flags);
int lkfree(void **pts, int flags);

#define LKM_REG 0x0
#define LKM_INIT 0x1
#define LKM_OVER 0x2
#define LKM_UNDER 0x4