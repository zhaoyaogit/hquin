set(SRCS
	Buffer.cpp
	Timestap.cpp
	Channel.cpp
	EventLoop.cpp
	Epoller.cpp
	InetAddress.cpp
	Socket.cpp
	Acceptor.cpp
	TcpServer.cpp
	TcpConnection.cpp
)

add_library(hquin ${SRCS})
set(LIBRARY_OUTPUT_PATH lib)