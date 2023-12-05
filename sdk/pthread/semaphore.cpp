#include <errno.h>
#include "semaphore.h"

static int handle_errno(int err)
{
    if (err == 0) {
        return 0;
    }
    errno = err;
    return -1;
}

int sem_init(sem_t *semp, int pshared, unsigned int value)
{
    if (pshared != 0) {
        /* Shared memory not supported */
        errno = ENOSYS;
        return -1;
    }
    return handle_errno(sgx_thread_semaphore_init(semp, value));
}

int sem_destroy(sem_t *semp)
{
    return handle_errno(sgx_thread_semaphore_destroy(semp));
}

int sem_wait(sem_t *semp)
{
    return handle_errno(sgx_thread_semaphore_wait(semp));
}

int sem_trywait(sem_t *semp)
{
    return handle_errno(sgx_thread_semaphore_trywait(semp));
}

int sem_post(sem_t *semp)
{
    return handle_errno(sgx_thread_semaphore_post(semp));
}

int sem_getvalue(sem_t *semp, int *sval)
{
    return handle_errno(sgx_thread_semaphore_getvalue(semp, sval));
}
