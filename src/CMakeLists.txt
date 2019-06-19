set(SRCS
	EventLoop.cpp
	Channel.cpp
	Epoller.cpp
	InetAddress.cpp
	Acceptor.cpp
)

add_library(hquin ${SRCS})
set(LIBRARY_OUTPUT_PATH lib)