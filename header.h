int lkreport(int fd, int flags);
int lkmalloc(int size, void **ptr, int flags);
int lkfree(void **pts, int flags);
void create_first_csv();
void log_a(char* a, char* b, int c);

#define LKM_REG 0x0
#define LKM_INIT 0x1
#define LKM_OVER 0x2
#define LKM_UNDER 0x4

#define LKF_REG 0x0
#define LKF_APPROX 0x1
#define LKF_WARN 0x2
#define LKF_UNKNOWN 0x4
#define LKF_ERROR 0x8

#define LKR_NONE 0x0
#define LKR_SERIOUS 0x1
#define LKR_MATCH 0x2
#define LKR_BAD_FREE 0x4
#define LKR_ORPHAN_FREE 0x8
#define LKR_DOUBLE_FREE 0x10
#define LKR_APPROX 0x20

#define lk_foo(arg1,arg2,arg3) __lk_foo_internal((arg1),(arg2),(arg3),__FILE__,__func__,__LINE__)
#define log_info(arg1,arg2,arg3) arg1 = __FILE__; arg2 = __func__; arg3 = __LINE__
//#define log_info(arg1,arg2,arg3) printf("[INFO] (%s:%d) \n", __FILE__,__LINE__)