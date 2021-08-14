/*
*
* modified by klavien on 2021/08/14, email: klavien@163.com
*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include "co_routine.h"
#include "ev.h"
using namespace std;
struct stTask_t
{
	int id;
};
struct stEnv_t
{
	stCoCond_t* cond;
	queue<stTask_t*> task_queue;
};
stEnv_t* env=NULL;
// timing
static double seconds()
{
#if defined(_WIN32) || defined(_WIN64)
  LARGE_INTEGER frequency, now;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter  (&now);
  return now.QuadPart / double(frequency.QuadPart);
#else
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
#endif
}
void* Producer(void* args)
{
	co_enable_hook_sys();
	stEnv_t* env=  (stEnv_t*)args;
	int id = 0;
	while (true)
	{
		stTask_t* task = (stTask_t*)calloc(1, sizeof(stTask_t));
		task->id = id++;
		env->task_queue.push(task);
		printf("%s:%d produce task %d\n", __func__, __LINE__, task->id);
		co_cond_signal(env->cond);
		poll(NULL, 0, 1000);
	}
	return NULL;
}
void* Consumer(void* args)
{
	co_enable_hook_sys();
	stEnv_t* env = (stEnv_t*)args;
	while (true)
	{
		if (env->task_queue.empty())
		{
			co_cond_timedwait(env->cond, -1);
			continue;
		}
		stTask_t* task = env->task_queue.front();
		env->task_queue.pop();
		printf("%s:%d consume task %d\n", __func__, __LINE__, task->id);
		free(task);
	}
	return NULL;
}

void timer_func1(EV_P_ ev_timer *et,int revent)
{
	// producer_routine cause mem leak,but we just make a use case here.
	stCoRoutine_t* producer_routine;
	co_create(&producer_routine, NULL, Producer, env);
	co_resume(producer_routine);
	printf("time:%f,this is ev timer_func1!\n",seconds());
}
void timer_func2(EV_P_ ev_timer *et,int revent)
{
	printf("time:%f,this is ev timer_func2!\n",seconds());
}
int main()
{
	EV_P =ev_default_loop(0);
	co_seteventloop(loop); // if your loop already exists, just set it before the first corotine.
    ev_timer et1;
    ev_timer_init(&et1,timer_func1,0,0);
    ev_timer_start(EV_A_ &et1);
	ev_timer et2;
    ev_timer_init(&et2,timer_func2,1,1);
    ev_timer_start(EV_A_ &et2);

	env = new stEnv_t;
	env->cond = co_cond_alloc();

	stCoRoutine_t* consumer_routine;
	co_create(&consumer_routine, NULL, Consumer, env);
	co_resume(consumer_routine);

	stCoRoutine_t* producer_routine;
	co_create(&producer_routine, NULL, Producer, env);
	co_resume(producer_routine);
	
	//co_eventloop(co_get_epoll_ct(), NULL, NULL);
	ev_run(EV_A_ 0);
	return 0;
}
