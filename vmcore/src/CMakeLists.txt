CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(vmcore_src C)
SET(CMAKE_C_STANDARD 11)

# 搜索源文件
AUX_SOURCE_DIRECTORY(${vmcore_src_SOURCE_DIR} SRC)
MESSAGE(STATUS "vmcore src list = ${SRC}")

# 处理外部依赖库
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
    ADD_LIBRARY(vmcore_src STATIC ${SRC})  ## debug模式下, 以静态库形式链接
ELSE()
    ADD_LIBRARY(vmcore_src SHARED ${SRC})
ENDIF()

TARGET_LINK_LIBRARIES(vmcore_src ${libffi} ${libdl} m)
TARGET_INCLUDE_DIRECTORIES(vmcore_src PRIVATE VMCORE_INCLUDE_DICT)
SET_TARGET_PROPERTIES(vmcore_src PROPERTIES
        PUBLIC_HEADER ${vmcore_SOURCE_DIR}/include/virtualmath.h  # 设置public的头文件
        OUTPUT_NAME "vmcore"  # 设置输出名
        VERSION 3.1  # 设置版本号
        )
INSTALL(TARGETS vmcore_src)
