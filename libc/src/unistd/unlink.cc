#include <unistd.h>
#include <sys/syscall.h>
#include <sys/syscall_num.h>
#include <sys/types.h>

int unlink(const char *path)
{
    return syscall1(long(path), SYS_unlink);
}
