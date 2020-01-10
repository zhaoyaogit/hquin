set(SRCS
	HTTPRequest.cpp
	HTTPResponse.cpp
	HTTPServer.cpp
)

add_library(hquin_http ${SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
