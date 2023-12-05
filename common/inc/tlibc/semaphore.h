#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <sgx_defs.h>
#include <sgx_thread.h>

typedef struct  _sgx_thread_semaphore_t     sem_t;

#ifdef __cplusplus
extern "C" {
#endif

int SGXAPI sem_init(sem_t *, int, unsigned int);
int SGXAPI sem_destroy(sem_t *);
int SGXAPI sem_wait(sem_t *);
int SGXAPI sem_trywait(sem_t *);
int SGXAPI sem_post(sem_t *);
int SGXAPI sem_getvalue(sem_t *, int *);

#ifdef __cplusplus
}
#endif

#endif  //_SEMAPHORE_H_
