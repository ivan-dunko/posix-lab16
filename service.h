#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <pthread.h>

#define ERROR_CODE -1
#define SUCCESS_CODE 0

#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock

#define TRUE 1
#define FALSE 0

void exitWithFailure(const char *msg, int err);
void assertSuccess(const char *msg, int errcode);
void lockSuccessAssertion(pthread_mutex_t *mtx, const char *msg);
void unlockSuccessAssertion(pthread_mutex_t *mtx, const char *msg);
void initMutexSuccessAssertion(
    pthread_mutex_t *mtx, 
    pthread_mutexattr_t *mtx_attr,
    const char *msg);

#endif