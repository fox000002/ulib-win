SET (button_SRCS test_button.cpp color_button.cpp button.rc)

if (MINGW)
  if (BUILD_SHARED_LIBS)
	  ADD_DEFINITIONS("-DBUILD_EXE")
  else (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("-DBUILD_STATIC")
  endif (BUILD_SHARED_LIBS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif (MINGW)

if (MSVC)
  if (BUILD_SHARED_LIBS)
	  ADD_DEFINITIONS("-DBUILD_EXE")
  else (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS("-DBUILD_STATIC")
  endif (BUILD_SHARED_LIBS)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
endif (MSVC)

add_executable (test_button WIN32 ${button_SRCS})

target_link_libraries (test_button msimg32 uwin)