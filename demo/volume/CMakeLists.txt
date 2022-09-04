#/*
# * ALGO     : PROJ. : Volume
# * RESEARCH : File  : CMakeLists.txt
# *          : Date  : 20100531.0725UTC
# *          : Email : mail@algoresearch.net
# */

cmake_minimum_required(VERSION 2.8)

#	 set(CMAKE_C_COMPILER clang)
#	 set(CMAKE_CXX_COMPILER clang)

project(template)

	aux_source_directory(src SRC_LIST)
		#set(SRC_LIST src/main.c src/test.c)

	include_directories(include)

	add_compile_options(-Wall -O2)
	add_executable(main ${SRC_LIST})
	target_link_libraries(main X11 m pthread)

	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#EOF
