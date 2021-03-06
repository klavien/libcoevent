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


#ifdef USE_LIBAE
#undef USE_LIBCO_LOOP
#define USE_LIBCO_LOOP 0

#include "ae.h"

void *createEventLoop()
{
	// note!!!    1024*10 may not enouth to use,
	// then you should adjust it manually by aeResizeSetSize().
	return aeCreateEventLoop(stCoEpoll_t::_EPOLL_SIZE);
}
int co_eventloop( stCoEpoll_t *ctx,pfn_co_eventloop_t pfn,void *arg )
{
	UNUSED(pfn);
    UNUSED(arg);
	aeMain((aeEventLoop *)ctx->eventLoop);
	return 0;
}
uint32_t PollEvent2Me( short events )
{
	uint32_t e = 0;	
	if( events & POLLIN ) 	e |= AE_READABLE;
	if( events & POLLOUT )  e |= AE_WRITABLE;
	//if( events & POLLHUP ) 	e |= (AE_WRITABLE|AE_READABLE);
	//if( events & POLLERR )	e |= (AE_WRITABLE|AE_READABLE);
	return e;
}
short MyEvent2Poll( uint32_t events )
{
	short e = 0;	
	if( events & AE_READABLE ) 	e |= POLLIN;
	if( events & AE_WRITABLE ) e |= POLLOUT;
	//if( events & (AE_WRITABLE|AE_READABLE) ) e |= POLLHUP;
	//if( events & (AE_WRITABLE|AE_READABLE) ) e |= POLLERR;
	return e;
}
int deleteTimeEvent(void *eventLoop, stTimeoutItem_t *clientData);
static void fileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
    UNUSED(fd);
    stPollItem_t *lp = (stPollItem_t *)clientData;
	lp->pSelf->revents = MyEvent2Poll(mask);


	stPoll_t *pPoll = lp->pPoll;
	pPoll->iRaiseCnt++;

	if( !pPoll->iAllEventDetach )
	{
		pPoll->iAllEventDetach = 1;
        deleteTimeEvent(eventLoop,pPoll);
	}
    if(pPoll->pfnProcess) pPoll->pfnProcess(pPoll);
}
static int timeProc(struct aeEventLoop *eventLoop, long long id, void *clientData)
{
    UNUSED(eventLoop);
    UNUSED(id);
    stTimeoutItem_t *pPoll = (stTimeoutItem_t*)clientData;
    if(pPoll->pfnProcess) pPoll->pfnProcess(pPoll);
    return 0;
}
int createFileEvent(stCoEpoll_t *ctx,stTimeoutItem_t *clientData,size_t idx)
{
	stPoll_t *arg=(stPoll_t*)clientData;
    return aeCreateFileEvent((aeEventLoop*)ctx->eventLoop,
		arg->pPollItems[idx].pSelf->fd,
		PollEvent2Me(arg->pPollItems[idx].pSelf->events),
		fileProc,&arg->pPollItems[idx]);
}
void deleteFileEvent(stCoEpoll_t *ctx, stTimeoutItem_t *clientData,size_t idx)
{
	stPoll_t *arg=(stPoll_t*)clientData;
    aeDeleteFileEvent((aeEventLoop*)ctx->eventLoop,
		arg->pPollItems[idx].pSelf->fd, 
		PollEvent2Me(arg->pPollItems[idx].pSelf->events));
}
int createTimeEvent(stCoEpoll_t *ctx, long long milliseconds, stTimeoutItem_t *clientData)
{
	clientData->timerId=aeCreateTimeEvent((aeEventLoop*)ctx->eventLoop,milliseconds, timeProc, clientData, 0);
	return 0;
}
int deleteTimeEvent(void *eventLoop, stTimeoutItem_t *clientData)
{
    return aeDeleteTimeEvent((aeEventLoop*)eventLoop,clientData->timerId);
}
#endif