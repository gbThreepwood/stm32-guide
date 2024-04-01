cmake_minimum_required(VERSION 3.22)

# Debug message
message(STATUS "Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")


add_library(qpc INTERFACE)

#
#set(LIBCLI_SOURCE_FILES
#    src/libcli.c
#    src/libcli_queue.c
#    #src/libcli_stach.c # We are not using the stack anymore
#)

# Register core library
# target_sources(qpc PUBLIC
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qep_hsm.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qep_msm.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_act.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_actq.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_defer.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_dyn.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_mem.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_ps.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_qact.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_qeq.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_qmact.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_time.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qv/qv.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/ports/arm-cm/qv/gnu/qv_port.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qs.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qs_rx.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qs_fp.c
# 	${CMAKE_CURRENT_LIST_DIR}/qpc/include/qstamp.c # qpc build timestamp
#     )

set(QPC_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qep_hsm.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qep_msm.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_act.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_actq.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_defer.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_dyn.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_mem.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_ps.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_qact.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_qeq.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_qmact.c
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qf/qf_time.c
 	#${CMAKE_CURRENT_LIST_DIR}/qpc/src/qv/qv.c
 	#${CMAKE_CURRENT_LIST_DIR}/qpc/ports/arm-cm/qv/gnu/qv_port.c
 	#${CMAKE_CURRENT_LIST_DIR}/qpc/include/qstamp.c # qpc build timestamp
 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qstamp.c # qpc build timestamp
)

if (SPY_CONFIGURATION)
	message ("Spy configuration enabled")
    list(APPEND QPC_SOURCES
	 	${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qs.c
 		${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qs_rx.c
 		${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qs_fp.c
	)
else ()
	message ("Spy configuration disabled")
endif ()

if (TEST_CONFIGURATION)
	message ("QUTest configuration enabled")
    add_definitions(-DQ_UTEST)
	# You have to choose if you want the kernel or the test harness
	list(APPEND QPC_SOURCES
		${CMAKE_CURRENT_LIST_DIR}/qpc/src/qs/qutest.c
		#${CMAKE_SOURCE_DIR}/src/bsp/qpc/qutest_port.c # This file is included in the application CMakeLists.txt instead of here
	)
else ()
	message ("QUTest configuration disabled")
	# You have to choose if you want the kernel or the test harness
	list(APPEND QPC_SOURCES
 		${CMAKE_CURRENT_LIST_DIR}/qpc/src/qv/qv.c
 		${CMAKE_CURRENT_LIST_DIR}/qpc/ports/arm-cm/qv/gnu/qv_port.c
	)
endif ()

#message(${QPC_SOURCES})
target_sources(qpc PUBLIC ${QPC_SOURCES})

target_include_directories(qpc INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/qpc/include
    ${CMAKE_CURRENT_LIST_DIR}/qpc/ports/arm-cm/qv/gnu
    ${CMAKE_CURRENT_LIST_DIR}/qpc/ports/arm-cm/qutest
    )

#add_library(${PROJECT_NAME} STATIC ${SOURCES})
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)


# Register other modules

# Debug message
message(STATUS "Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

