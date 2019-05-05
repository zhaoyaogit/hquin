set(SRCS
	EventLoop.cpp
	Channel.cpp
	Epoll.cpp
)

add_library(hquin ${SRCS})
set(LIBRARY_OUTPUT_PATH lib)