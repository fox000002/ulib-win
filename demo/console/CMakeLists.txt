SET (console_SRCS test_console.cpp console.rc)

if (MINGW)
  if (BUILD_SHARED_LIBS)
	  ADD_DEFINITIONS("-DBUILD_EXE")
  else (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("-DBUILD_STATIC")
  endif (BUILD_SHARED_LIBS)
endif (MINGW)

if (MSVC)
  if (BUILD_SHARED_LIBS)
	ADD_DEFINITIONS("-DBUILD_EXE")
  else (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("-DBUILD_STATIC")
  endif (BUILD_SHARED_LIBS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif (MSVC)

add_executable (test_console ${console_SRCS})
target_link_libraries (test_console uwin)