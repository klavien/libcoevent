#
# By klavien on 2021/08/14, email: klavien@163.com
# 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, 
# software distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
#


COMM_MAKE = 1
COMM_ECHO = 1
version=0.5
v=debug
include co.mk

########## options ##########
CFLAGS += -fno-strict-aliasing -Wall -export-dynamic \
	-Wall -pipe  -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64

UNAME := $(shell uname -s)

ifeq ($(UNAME), FreeBSD)
LINKS += -L../lib -lcoevent -lpthread 
else
LINKS += -L../lib -lcoevent -lpthread -ldl -lev
endif

COLIB_OBJS=co_epoll.o co_routine.o co_hook_sys_call.o coctx_swap.o coctx.o co_comm.o
#co_swapcontext.o

PROGS = example_poll example_echosvr example_echocli example_thread  example_cond example_specific example_copystack example_closure example_setenv

all:$(PROGS)

example_echosvr:example_echosvr.o
	$(BUILDEXE) 
example_echocli:example_echocli.o
	$(BUILDEXE) 
example_thread:example_thread.o
	$(BUILDEXE) 
example_poll:example_poll.o
	$(BUILDEXE) 
example_exit:example_exit.o
	$(BUILDEXE) 
example_cond:example_cond.o
	$(BUILDEXE)
example_specific:example_specific.o
	$(BUILDEXE)
example_copystack:example_copystack.o
	$(BUILDEXE)
example_setenv:example_setenv.o
	$(BUILDEXE)
example_closure:example_closure.o
	$(BUILDEXE)

dist: clean

clean:
	$(CLEAN) *.o $(PROGS)
