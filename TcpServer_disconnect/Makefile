MUDUO_DIRECTORY ?= $(HOME)/build/release-install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib

CXXFLAGS = -g -O0 -Wall -Wextra -Werror \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I$(MUDUO_INCLUDE)

LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_net -lmuduo_base -lpthread -lrt

all: test 
clean:
	rm -f test core 

test: test.cc channel.cc eventloop.cc poller.cc timer_queue.cc eventloop_thread.cc\
	  sockets_ops.cc socket.cc inet_address.cc acceptor.cc tcp_server.cc tcp_connection.cc

	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
