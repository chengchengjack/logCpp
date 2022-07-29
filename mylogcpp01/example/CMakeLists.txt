cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
#SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}")


#查找目录下面所有源文件 aux_source_directory(< dir > < variable >)
#将输出结果列表储存在指定的变量中
aux_source_directory(${PROJECT_SOURCE_DIR}/example TEST_SRC)

include_directories("${PROJECT_SOURCE_DIR}/src/lib/include")

#link_libraries("${PROJECT_BINARY_DIR}/lib")

#将aux中文件变成可执行文件
add_executable(test ${TEST_SRC})

target_link_libraries(test mylogcpp ${THIRD_PARTY_LIBS})

#源文件定义的
#target_link_libraries(test liblog.a)

#target_link_libraries(test libmylogcpp.so)
