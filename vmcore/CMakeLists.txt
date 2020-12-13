CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(vmcore C)
SET(CMAKE_C_STANDARD 11)

SET(VMCORE_INCLUDE_DICT
        ${vmcore_SOURCE_DIR}/include
        ${vmcore_SOURCE_DIR}/src/include
        ${vmcore_SOURCE_DIR}/ofunc/include
        )

SET(VMCORE_SRC_DIR
        ${vmcore_SOURCE_DIR}/src
        )

# 搜索源文件
FOREACH(DIR ${VMCORE_SRC_DIR})
    SET(TMP)  # 清空
    AUX_SOURCE_DIRECTORY(${DIR} TMP)
    SET(SRC ${SRC} ${TMP})  # 列表追加
ENDFOREACH()
MESSAGE(STATUS "vmcore src list = ${SRC}")

IF (PG)
    ADD_COMPILE_OPTIONS(-pg)
    ADD_LINK_OPTIONS(-pg)
ENDIF()

ADD_COMPILE_OPTIONS(-fPIC)
# 处理内部依赖库
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/signalhandler)
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/memory)
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/md5)
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/file)

# 添加子目录(没有生成新的lib)
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/parser)
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/ofunc)
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/gc)

# 添加头文件
INCLUDE_DIRECTORIES(${VMCORE_INCLUDE_DICT} BEFORE)
INCLUDE_DIRECTORIES(${vm_ofunc_include} BEFORE)
INCLUDE_DIRECTORIES(${vm_parser_include} BEFORE)
MESSAGE(STATUS "VMCORE_INCLUDE_DICT = ${VMCORE_INCLUDE_DICT}")

# 编译和链接vmcore
ADD_SUBDIRECTORY(${vmcore_SOURCE_DIR}/src)
TARGET_LINK_LIBRARIES(vmcore_src vm_signal vm_memory vm_md5 vm_file)

# 安装vmlib
SET(VMLIB_PATH ${CMAKE_INSTALL_PREFIX}/vmlib)
SET(CLIB_PATH ${CMAKE_INSTALL_PREFIX}/lib)
INSTALL(DIRECTORY ${vmcore_SOURCE_DIR}/vmlib/ DESTINATION ${VMLIB_PATH})
INSTALL(CODE "SET(ENV{VIRTUALMATHPATH} \"${VMLIB_PATH},${CLIB_PATH}\")")  # 设置系统环境变量(安装时候执行)
INSTALL(CODE "MESSAGE(STATUS \"Now Install\")")  # 安装时运行

# 安装clib
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/clib)

# 结束语
MESSAGE(STATUS "vmcore cmake configure finish")