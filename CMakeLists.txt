cmake_minimum_required(VERSION 2.8)

project(lbp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

IF(UNIX)
  # add some standard warnings
  ADD_DEFINITIONS(-Wno-variadic-macros -Wno-long-long -Wall -Wextra -Winit-self -Woverloaded-virtual -Wsign-promo -Wno-unused-parameter -pedantic -Woverloaded-virtual -Wno-unknown-pragmas)
  
  # -ansi does not compile with sjn module
  #ADD_DEFINITIONS(-ansi)

  # if you like to have warinings about conversions, e.g. double->int or double->float etc., or float compare
  #ADD_DEFINITIONS(-Wconversion -Wfloat-equal)
endif(UNIX)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(${OpenCV_VERSION} VERSION_EQUAL 3 OR ${OpenCV_VERSION} VERSION_GREATER 3)
  message (FATAL_ERROR "OpenCV version is not compatible: ${OpenCV_VERSION}")
	message (FATAL_ERROR "Please use OpenCV 2.4.x version.")
endif()

file(GLOB sources histogram.cpp)
file(GLOB main Main.cpp)

file(GLOB_RECURSE lbp package_lbp/*.cpp package_lbp/*.c)
file(GLOB_RECURSE lbp_include package_lbp/*.h package_lbp/*.hpp)

include_directories(${CMAKE_SOURCE_DIR})

add_library(lbp SHARED ${sources} ${lbp})
target_link_libraries(lbp ${OpenCV_LIBS})
set_property(TARGET lbp PROPERTY PUBLIC_HEADER ${lbp_include})

add_executable(lbp_bin ${main})
target_link_libraries(lbp_bin ${OpenCV_LIBS} lbp)
set_target_properties(lbp_bin
  PROPERTIES OUTPUT_NAME lbp)

INSTALL(TARGETS lbp
	lbp_bin
  RUNTIME DESTINATION bin COMPONENT app
  LIBRARY DESTINATION lib COMPONENT runtime
  ARCHIVE DESTINATION lib COMPONENT runtime
  PUBLIC_HEADER DESTINATION include/package_lbp COMPONENT dev
  FRAMEWORK DESTINATION "/Library/Frameworks"
)
