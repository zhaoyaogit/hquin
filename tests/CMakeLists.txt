add_executable(event_test event_test.cpp)
target_link_libraries(event_test hquin)

add_executable(acceptor_test acceptor_test.cpp)
target_link_libraries(acceptor_test hquin)

add_executable(tcpserver_test tcpserver_test.cpp)
target_link_libraries(tcpserver_test hquin)

add_executable(timestap_test timestap_test.cpp)
target_link_libraries(timestap_test hquin)

add_executable(buffer_test buffer_test.cpp)
target_link_libraries(buffer_test hquin)

add_executable(log_test log_test.cpp)
target_link_libraries(log_test hquin)

add_executable(tcpserver_send_test tcpserver_send_test.cpp)
target_link_libraries(tcpserver_send_test hquin)