set(SRCS
	EventLoop.cpp
	Channel.cpp
	Epoller.cpp
	InetAddress.cpp
	Socket.cpp
	Acceptor.cpp
	TcpServer.cpp
	TcpConnection.cpp
	Timestap.cpp
)

add_library(hquin ${SRCS})
set(LIBRARY_OUTPUT_PATH lib)