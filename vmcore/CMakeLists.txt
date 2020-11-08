CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(vmcore C)
SET(CMAKE_C_STANDARD 11)

SET(VMCORE_INCLUDE_DICT
        ${vmcore_SOURCE_DIR}/include
        ${vmcore_SOURCE_DIR}/parser/include
        ${vmcore_SOURCE_DIR}/src/include
        ${vmcore_SOURCE_DIR}/ofunc/include
        ${vmcore_SOURCE_DIR}/md5/include
        ${vmcore_SOURCE_DIR}/ofunc/clib/include
        )

SET(VMCORE_SRC_DIR
        ${vmcore_SOURCE_DIR}/memory
        ${vmcore_SOURCE_DIR}/src
        ${vmcore_SOURCE_DIR}/gc
        ${vmcore_SOURCE_DIR}/file
        ${vmcore_SOURCE_DIR}/parser
        ${vmcore_SOURCE_DIR}/ofunc/src
        ${vmcore_SOURCE_DIR}/ofunc/clib
        ${vmcore_SOURCE_DIR}/signalhandler
        ${vmcore_SOURCE_DIR}/md5
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

IF (PG)
    ADD_COMPILE_OPTIONS(-pg)
    ADD_LINK_OPTIONS(-pg)
    ADD_LIBRARY(vmcore STATIC ${SRC})  ## debug模式下, 以静态库形式链接
ELSE()
    ADD_LIBRARY(vmcore SHARED ${SRC})
ENDIF()

TARGET_LINK_LIBRARIES(vmcore ${libffi} ${libdl} m)
TARGET_INCLUDE_DIRECTORIES(vmcore PRIVATE VMCORE_INCLUDE_DICT)
SET_TARGET_PROPERTIES(vmcore PROPERTIES
        PUBLIC_HEADER ${vmcore_SOURCE_DIR}/include/virtualmath.h  # 设置public的头文件
        OUTPUT_NAME "vmcore"  # 设置输出名
        VERSION 3.1  # 设置版本号
        )
INSTALL(TARGETS vmcore)
