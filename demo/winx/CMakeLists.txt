SET (winx_SRCS test_winx.cpp winx.rc)

if (MINGW)
  ADD_DEFINITIONS("-DBUILD_EXE")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif (MINGW)

add_executable (test_winx ${winx_SRCS})

target_link_libraries (test_winx uwin)
