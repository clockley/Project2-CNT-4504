/*
 * Copyright 2015 Christian Lockley
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may 
 * not use this file except in compliance with the License. You may obtain 
 * a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License. 
 */
 
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <unistd.h>

#define MAX_WORKERS 100

static 	_Atomic(bool) canceled = false;

struct threadpool
{
	pthread_t thread;
	sem_t sem;
	void *(*func)(void*);
	void * arg;
	_Atomic(int) active;
};

static struct threadpool threads[MAX_WORKERS] = {0};

static void *Worker(void *arg)
{
	struct threadpool * t = (struct threadpool *)arg;
	pthread_detach(pthread_self());
	while (true) {
		sem_wait(&t->sem);
		__sync_synchronize();
		t->func(t->arg);
		t->active = 0;
		__sync_synchronize();
	}
}

bool ThreadPoolNew(void)
{
	for (size_t i = 0; i < MAX_WORKERS; i++) {
		sem_init(&threads[i].sem, 0, 0);
		pthread_create(&threads[i].thread, NULL, Worker, &threads[i]);
	}
	return true;
}

bool ThreadPoolCancel(void)
{
	canceled = true;
	for (size_t i = 0; i < MAX_WORKERS; i++) {
		pthread_cancel(threads[i].thread);
	}
}

bool ThreadPoolAddTask(void *(*entry)(void*), void * arg, bool retry)
{
	if (canceled == true) {
		return false;
	}

	do {
		for (size_t i = 0; i < MAX_WORKERS; i++) {
			if (__sync_val_compare_and_swap(&threads[i].active, 0, 1) == 0) {
				threads[i].func = entry;
				threads[i].arg = arg;
				__sync_synchronize();
				sem_post(&threads[i].sem);
				return true;
			}
		}
	} while (retry);

	return false;
}