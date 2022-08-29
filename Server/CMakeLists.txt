cmake_minimum_required(VERSION 3.22)
project(Echo_server)

set(CMAKE_CXX_STANDARD 23)
FIND_PACKAGE( Boost 1.76 COMPONENTS system filesystem thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES(Echo_client LINK_PUBLIC ${Boost_LIBRARIES} )
add_executable(Echo_server server.cpp ../Network/src/server/connection.cpp)
