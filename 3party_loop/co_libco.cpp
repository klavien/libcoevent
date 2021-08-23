/*
*
* Created by klavien on 2021/08/23, email: klavien@163.com.
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
#ifdef USE_LIBCO
#define USE_LIBCO_LOOP 1
void *createEventLoop()
{
    return (void*)((intptr_t)co_epoll_create( stCoEpoll_t::_EPOLL_SIZE ));
}
/*
 *   EPOLLPRI 		POLLPRI    // There is urgent data to read.  
 *   EPOLLMSG 		POLLMSG
 *
 *   				POLLREMOVE
 *   				POLLRDHUP
 *   				POLLNVAL
 *
 * */
static uint32_t PollEvent2Me( short events )
{
	uint32_t e = 0;	
	if( events & POLLIN ) 	e |= EPOLLIN;
	if( events & POLLOUT )  e |= EPOLLOUT;
	if( events & POLLHUP ) 	e |= EPOLLHUP;
	if( events & POLLERR )	e |= EPOLLERR;
	if( events & POLLRDNORM ) e |= EPOLLRDNORM;
	if( events & POLLWRNORM ) e |= EPOLLWRNORM;
	return e;
}
static short myEvent2Poll( uint32_t events )
{
	short e = 0;	
	if( events & EPOLLIN ) 	e |= POLLIN;
	if( events & EPOLLOUT ) e |= POLLOUT;
	if( events & EPOLLHUP ) e |= POLLHUP;
	if( events & EPOLLERR ) e |= POLLERR;
	if( events & EPOLLRDNORM ) e |= POLLRDNORM;
	if( events & EPOLLWRNORM ) e |= POLLWRNORM;
	return e;
}
void OnPollPreparePfn( stTimeoutItem_t * ap,struct epoll_event &e,stTimeoutItemLink_t *active )
{
	stPollItem_t *lp = (stPollItem_t *)ap;
	lp->pSelf->revents = myEvent2Poll( e.events );


	stPoll_t *pPoll = lp->pPoll;
	pPoll->iRaiseCnt++;

	if( !pPoll->iAllEventDetach )
	{
		pPoll->iAllEventDetach = 1;

		RemoveFromLink<stTimeoutItem_t,stTimeoutItemLink_t>( pPoll );

		AddTail( active,pPoll );

	}
}
int co_eventloop( stCoEpoll_t *ctx,pfn_co_eventloop_t pfn,void *arg )
{
    if( !ctx->result )
	{
		ctx->result =  co_epoll_res_alloc( stCoEpoll_t::_EPOLL_SIZE );
	}
	co_epoll_res *result = ctx->result;


	for(;;)
	{
		int ret = co_epoll_wait( (int)((intptr_t)ctx->eventLoop),result,stCoEpoll_t::_EPOLL_SIZE, 1 );

		stTimeoutItemLink_t *active = (ctx->pstActiveList);
		stTimeoutItemLink_t *timeout = (ctx->pstTimeoutList);

		memset( timeout,0,sizeof(stTimeoutItemLink_t) );

		for(int i=0;i<ret;i++)
		{
			stTimeoutItem_t *item = (stTimeoutItem_t*)result->events[i].data.ptr;
			if( item->pfnPrepare )
			{
				item->pfnPrepare( item,result->events[i],active );
			}
			else
			{
				AddTail( active,item );
			}
		}


		unsigned long long now = GetTickMS();
		TakeAllTimeout( ctx->pTimeout,now,timeout );

		stTimeoutItem_t *lp = timeout->head;
		while( lp )
		{
			//printf("raise timeout %p\n",lp);
			lp->bTimeout = true;
			lp = lp->pNext;
		}

		Join<stTimeoutItem_t,stTimeoutItemLink_t>( active,timeout );

		lp = active->head;
		while( lp )
		{

			PopHead<stTimeoutItem_t,stTimeoutItemLink_t>( active );
            if (lp->bTimeout && now < lp->ullExpireTime) 
			{
				int ret = AddTimeout(ctx->pTimeout, lp, now);
				if (!ret) 
				{
					lp->bTimeout = false;
					lp = active->head;
					continue;
				}
			}
			if( lp->pfnProcess )
			{
				lp->pfnProcess( lp );
			}

			lp = active->head;
		}
		if( pfn )
		{
			if( -1 == pfn( arg ) )
			{
				break;
			}
		}

	}
	co_epoll_res_free(ctx->result);
    return 0;
}

int createFileEvent(stCoEpoll_t *ctx,stTimeoutItem_t *clientData,size_t idx)
{
    stPoll_t *arg=(stPoll_t *)clientData;
    arg->pPollItems[idx].pfnPrepare = OnPollPreparePfn;
    struct epoll_event &ev = arg->pPollItems[idx].stEvent;
    ev.data.ptr = &arg->pPollItems[idx];
    ev.events = PollEvent2Me( arg->fds[idx].events );
    return co_epoll_ctl( (int)((intptr_t)ctx->eventLoop),EPOLL_CTL_ADD, arg->fds[idx].fd, &ev );
}
void deleteFileEvent(stCoEpoll_t *ctx, stTimeoutItem_t *clientData,size_t idx)
{
    stPoll_t *arg=(stPoll_t *)clientData;
    co_epoll_ctl( (int)((intptr_t)ctx->eventLoop),EPOLL_CTL_DEL,arg->fds[idx].fd,&arg->pPollItems[idx].stEvent );
}
int createTimeEvent(stCoEpoll_t *ctx, long long milliseconds, stTimeoutItem_t *clientData)
{
    stPoll_t *arg=(stPoll_t *)clientData;
    unsigned long long now = GetTickMS();
	arg->ullExpireTime = now + milliseconds;
	return AddTimeout( ctx->pTimeout,arg,now );
}
int deleteTimeEvent(void *eventLoop, stTimeoutItem_t *clientData)
{
    RemoveFromLink<stTimeoutItem_t,stTimeoutItemLink_t>( (stPoll_t *)clientData );
    return 0;
}
#endif