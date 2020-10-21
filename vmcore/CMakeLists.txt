CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(vmcore C)
SET(CMAKE_C_STANDARD 11)

SET(VMCORE_INCLUDE_DICT
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ofunc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/md5/include
        )

SET(VMCORE_SRC_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/memory
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/gc
        ${CMAKE_CURRENT_SOURCE_DIR}/file
        ${CMAKE_CURRENT_SOURCE_DIR}/parser
        ${CMAKE_CURRENT_SOURCE_DIR}/ofunc/src
        ${CMAKE_CURRENT_SOURCE_DIR}/ofunc/clib
        ${CMAKE_CURRENT_SOURCE_DIR}/signalhandler
        ${CMAKE_CURRENT_SOURCE_DIR}/md5
        )

# 添加头文件
INCLUDE_DIRECTORIES(${VMCORE_INCLUDE_DICT} BEFORE)

# 搜索源文件
FOREACH(DIR ${VMCORE_SRC_DIR})
    SET(TMP)  # 清空
    AUX_SOURCE_DIRECTORY(${DIR} TMP)
    SET(SRC ${SRC} ${TMP})  # 列表追加
ENDFOREACH()
MESSAGE(STATUS "vmcore src list = ${SRC}")

# 处理依赖库
FIND_PATH(ffih ffi.h REQUIRED)
INCLUDE_DIRECTORIES(${ffih})

FIND_LIBRARY(libffi NAMES ffi REQUIRED)
FIND_LIBRARY(libdl NAMES dl REQUIRED)

IF (libffi AND ffih)
    MESSAGE(STATUS "libffi = ${libffi} ffi.h in ${ffih}")
ELSE()
    MESSAGE(FATAL_ERROR "not found libffi or ffi.h")
ENDIF()

IF (libdl)
    MESSAGE(STATUS "libdl = ${libdl}")
ELSE()
    MESSAGE(FATAL_ERROR "not found libdl")
ENDIF()

ADD_COMPILE_OPTIONS(-pg)
ADD_LINK_OPTIONS(-pg)
ADD_LIBRARY(vmcore SHARED ${SRC})
TARGET_LINK_LIBRARIES(vmcore ${libffi} ${libdl} m)
TARGET_INCLUDE_DIRECTORIES(vmcore PRIVATE VMCORE_INCLUDE_DICT)
SET_TARGET_PROPERTIES(vmcore PROPERTIES
        PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/virtualmath.h  # 设置public的头文件
        OUTPUT_NAME "vmcore"  # 设置输出名
        VERSION 2.1  # 设置版本号
        )
INSTALL(TARGETS vmcore)
