CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(vm_md5 C)
SET(CMAKE_C_STANDARD 11)

INCLUDE_DIRECTORIES(${VMCORE_INCLUDE_DICT} BEFORE)
INCLUDE_DIRECTORIES(${vm_md5_SOURCE_DIR}/include BEFORE)
AUX_SOURCE_DIRECTORY(${vm_md5_SOURCE_DIR} VM_MD5_SRC)

ADD_LIBRARY(vm_md5 SHARED ${VM_MD5_SRC})
SET_TARGET_PROPERTIES(vm_md5 PROPERTIES OUTPUT_NAME "md5")
INSTALL(TARGETS vm_md5)
