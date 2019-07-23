set(SRCS
	Timestap.cpp
	Log.cpp
	Buffer.cpp
	Channel.cpp
	EventLoop.cpp
	Epoller.cpp
	InetAddress.cpp
	Socket.cpp
	Acceptor.cpp
	TcpServer.cpp
	TcpConnection.cpp
)

add_subdirectory(http)

add_library(hquin ${SRCS})

target_link_libraries(hquin hquin_http)
set(LIBRARY_OUTPUT_PATH lib)