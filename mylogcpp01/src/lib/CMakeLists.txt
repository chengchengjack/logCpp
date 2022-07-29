cmake_minimum_required(VERSION 3.15)

#设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

#设置编译库输出的位置
#SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
#
#查找目录下面所有源文件 aux_source_directory(< dir > < variable >)
#将输出结果列表储存在指定的变量中
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src/lib/src SRC)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/lib/include")

ADD_LIBRARY(mylogcpp SHARED ${SRC})

target_link_libraries(mylogcpp ${THIRD_PARTY_LIBS})

#ADD_LIBRARY(mylogcpp_static STATIC ${SRC})


# 指定动态库的输出名称
#set_target_properties(mylogcpp_static PROPERTIES OUTPUT_NAME "log")

# 获取相关属性也是通过set_target_properties
#get_target_property(OUTPUT_VALUE mylogcpp_static OUTPUT_NAME)
#message(STATUS "This is the mylogcpp_static OUTPUT_NAME:"${OUTPUT_VALUE})

# 动态库版本号设置 , VERSION指代动态库版本，SOVERSION 指代 API 版本。
#set_target_properties(mylogcpp PROPERTIES VERSION 1.2 SOVERSION 1)
# 其中，libhello.so.1.2为动态库的文件名(realname)，libhello.so.1为动态库的别名(soname)，
# libhello.so为动态库的链接名（linkname）。

#message(STATUS ${DIR_LIB_SRCS}"--构建库完成")

# 使动态库和静态库同时存在
#set_target_properties(mylogcpp PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#set_target_properties(mylogcpp_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 安装共享库和头文件
# 将动态库和静态库安装到<prefix>/lib
#install(TARGETS mylogcpp mylogcpp_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
# 安装头文件
#install(FILES logger.h DESTINATION include/logger)







