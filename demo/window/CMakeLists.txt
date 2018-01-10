SET (window_SRCS test_window.cpp window.rc ${ULIB_SRC_DIR}/uwindow.cpp)

if (MINGW)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif (MINGW)

add_executable (test_window ${window_SRCS})

target_link_libraries (test_window msimg32)