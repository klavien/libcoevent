/*
*
* Created by klavien on 2021/08/14, email: klavien@163.com.
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

#ifdef USE_LIBEV
#undef USE_LIBCO_LOOP
#define USE_LIBCO_LOOP 0

#include "ev.h"

void *createEventLoop()
{
    unsigned int flag = 0;
    flag |= EVFLAG_AUTO;
    flag |= EVFLAG_NOENV;
    flag |= EVFLAG_FORKCHECK;
    EV_P = ev_loop_new(flag);
    return loop;
}
static void dummyProc(EV_P_ ev_timer *ev, int events)
{
    UNUSED(loop);
    UNUSED(ev);
    UNUSED(events);
}
int eventLoopRun(stCoEpoll_t *ctx)
{
    /*
    // Sometimes,you need this,
    // it just keep liev alive when there is no event on loop.
    ev_timer et;
    ev_timer_init(&et,dummyProc,86400,86400);
    ev_timer_start((struct ev_loop*)ctx->eventLoop,&et);
    */
    return ev_run((struct ev_loop*)ctx->eventLoop,0);
}
uint32_t pollEvent2Me( short events )
{
	uint32_t e = 0;	
	if( events & POLLIN ) 	e |= EV_READ;
	if( events & POLLOUT )  e |= EV_WRITE;
	//if( events & POLLHUP ) 	e |= (EV_WRITE|EV_READ);
	//if( events & POLLERR )	e |= (EV_WRITE|EV_READ);
	return e;
}
short myEvent2Poll( uint32_t events )
{
	short e = 0;	
	if( events & EV_READ ) 	e |= POLLIN;
	if( events & EV_WRITE ) e |= POLLOUT;
	//if( events & (EV_WRITE|EV_READ) ) e |= POLLHUP;
	//if( events & (EV_WRITE|EV_READ) ) e |= POLLERR;
	return e;
}
int deleteTimeEvent(void *eventLoop,stTimeoutItem_t *clientData);
static void fileProc(EV_P_ ev_io *watcher, int mask)
{
    stPollItem_t *lp = (stPollItem_t *)(watcher->data);
	lp->pSelf->revents = myEvent2Poll(mask);
	stPoll_t *pPoll = lp->pPoll;
	pPoll->iRaiseCnt++;

	if( !pPoll->iAllEventDetach )
	{
		pPoll->iAllEventDetach = 1;
        deleteTimeEvent(EV_A_ (stTimeoutItem_t*)pPoll);
	}
    if(pPoll->pfnProcess) pPoll->pfnProcess(pPoll);
}
static void timeProc(EV_P_ ev_timer *ev, int events)
{
    UNUSED(loop);
    UNUSED(events);
    stPoll_t *pPoll = (stPoll_t*)ev->data;
    if(pPoll->pfnProcess) pPoll->pfnProcess(pPoll);
}
int createFileEvent(stCoEpoll_t *ctx,stTimeoutItem_t *clientData,size_t idx)
{
    ev_io *ev=(ev_io*)malloc(sizeof(*ev));
    if(!ev) return -1;
    stPoll_t *arg=(stPoll_t *)clientData;
    arg->pPollItems[idx].handler=ev;
    ev->data=&arg->pPollItems[idx];
    ev_io_init(ev,fileProc,arg->pPollItems[idx].pSelf->fd,
        pollEvent2Me(arg->pPollItems[idx].pSelf->events));
    ev_io_start((struct ev_loop*)ctx->eventLoop,ev);
    return 0;
}
void deleteFileEvent(stCoEpoll_t *ctx, stTimeoutItem_t *clientData,size_t idx)
{
    stPoll_t *arg=(stPoll_t*)clientData;
    ev_io *ev=(ev_io*)arg->pPollItems[idx].handler;
    if (ev_is_active(ev)) {
        ev_io_stop((struct ev_loop*)ctx->eventLoop,ev);
    }
    free(ev);
}
long long createTimeEvent(stCoEpoll_t *ctx, long long milliseconds, stTimeoutItem_t *clientData)
{
    ev_timer *et=(ev_timer*)malloc(sizeof(*et));
    if(!et) return -1;
    clientData->handler=et;
    et->data=clientData;
    ev_timer_init(et,timeProc,(1.0*milliseconds)/1000,0);
    ev_timer_start((struct ev_loop*)ctx->eventLoop,et);
    return 0;
}
int deleteTimeEvent(void *eventLoop, stTimeoutItem_t *clientData)
{
    if(!clientData->handler) return 0; // it is important here!
    ev_timer *et=(ev_timer*)clientData->handler;
    if (ev_is_active(et)) {
        ev_timer_stop((struct ev_loop*)eventLoop,et);
    }
    free(et);
    clientData->handler=NULL;
    return 0;
}

#endif