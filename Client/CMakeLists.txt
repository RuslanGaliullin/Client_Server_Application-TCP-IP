cmake_minimum_required(VERSION 3.22)
project(CSAClient)

set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME} client.cpp)

FIND_PACKAGE( Boost 1.76 COMPONENTS system filesystem thread REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES} )
target_include_directories(${PROJECT_NAME}
        PUBLIC
        CSANetwork
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        CSANetwork
        )