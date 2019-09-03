set(SRCS
	Util.cpp
	Timestamp.cpp
	Log.cpp
	Buffer.cpp
	Channel.cpp
	EventLoop.cpp
	EventLoopThread.cpp
	EventLoopThreadPool.cpp
	Epoller.cpp
	InetAddress.cpp
	Socket.cpp
	Acceptor.cpp
	TcpServer.cpp
	TcpConnection.cpp
)

add_subdirectory(http)
add_subdirectory(j4on)

add_library(hquin ${SRCS})

target_link_libraries(hquin hquin_http j4on)
set(LIBRARY_OUTPUT_PATH lib)