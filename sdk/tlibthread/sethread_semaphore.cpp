#include <errno.h>
#include <limits.h>

#include "sethread_internal.h"

int sgx_thread_semaphore_init(sgx_thread_semaphore_t *sem, uint32_t value)
{
    CHECK_PARAMETER(sem);

    sem->m_value = value;
    sem->m_lock = SGX_SPINLOCK_INITIALIZER;

    QUEUE_INIT(&sem->m_queue);

    return 0;
}

int sgx_thread_semaphore_destroy(sgx_thread_semaphore_t *sem)
{
    CHECK_PARAMETER(sem);

    SPIN_LOCK(&sem->m_lock);
    if (QUEUE_FIRST(&sem->m_queue) != SGX_THREAD_T_NULL) {
        SPIN_UNLOCK(&sem->m_lock);
        return EBUSY;
    }
    SPIN_UNLOCK(&sem->m_lock);

    return 0;
}

int sgx_thread_semaphore_wait(sgx_thread_semaphore_t *sem)
{
    CHECK_PARAMETER(sem);

    sgx_thread_t self = (sgx_thread_t)get_thread_data();

    while (1) {
        SPIN_LOCK(&sem->m_lock);

        if (sem->m_value > 0) {
            QUEUE_REMOVE(&sem->m_queue, self);
            sem->m_value--;
            SPIN_UNLOCK(&sem->m_lock);
            return 0;
        }

        sgx_thread_t waiter = SGX_THREAD_T_NULL;
        QUEUE_FOREACH(waiter, &sem->m_queue) {
            if (waiter == self) break;
        }

        if (waiter == SGX_THREAD_T_NULL)
            QUEUE_INSERT_TAIL(&sem->m_queue, self);

        SPIN_UNLOCK(&sem->m_lock);

        int err = 0;
        sgx_thread_wait_untrusted_event_ocall(&err, TD2TCS(self));
    }

    /* NOTREACHED */
}

int sgx_thread_semaphore_trywait(sgx_thread_semaphore_t *sem)
{
    CHECK_PARAMETER(sem);

    SPIN_LOCK(&sem->m_lock);

    if (sem->m_value > 0) {
        sem->m_value--;
        SPIN_UNLOCK(&sem->m_lock);
        return 0;
    }

    SPIN_UNLOCK(&sem->m_lock);
    return EAGAIN;
}

int sgx_thread_semaphore_post(sgx_thread_semaphore_t *sem)
{
    sgx_thread_t waiter = SGX_THREAD_T_NULL;

    CHECK_PARAMETER(sem);

    SPIN_LOCK(&sem->m_lock);

    if (sem->m_value == UINT32_MAX) {
        SPIN_UNLOCK(&sem->m_lock);
        return EOVERFLOW;
    }

    sem->m_value++;
    waiter = QUEUE_FIRST(&sem->m_queue);

    SPIN_UNLOCK(&sem->m_lock);

    if (waiter != SGX_THREAD_T_NULL) {
        int err = 0;
        sgx_thread_set_untrusted_event_ocall(&err, TD2TCS(waiter));
    }
    return 0;
}

int sgx_thread_semaphore_getvalue(sgx_thread_semaphore_t *sem, uint32_t *sval)
{
    CHECK_PARAMETER(sem);
    CHECK_PARAMETER(sval);

    SPIN_LOCK(&sem->m_lock);
    if (sval != NULL) {
        *sval = sem->m_value;
    }
    SPIN_UNLOCK(&sem->m_lock);

    return 0;
}
