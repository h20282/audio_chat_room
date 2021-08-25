CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

tcp_server: main.cpp  ./src/mysql_server.cpp ./src/tcp_net.cpp ./src/tcp_kernel.cpp ./src/thread_pool.cpp ./src/udp_net.cpp
	$(CXX) -o tcp_server  $^ $(CXXFLAGS) -lpthread -lmysqlclient

clean:
	rm  -r tcp_server
