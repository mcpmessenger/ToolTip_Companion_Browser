cmake_minimum_required(VERSION 3.16)
project(NaviGrabTooltipLib VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find Chromium dependencies (if available)
find_path(CHROMIUM_SRC_DIR "base" PATHS 
    "C:/chromium/src/src"
    "C:/chromium/src"
    ENV CHROMIUM_SRC
    NO_DEFAULT_PATH
)

if(CHROMIUM_SRC_DIR)
    message(STATUS "Found Chromium source at: ${CHROMIUM_SRC_DIR}")
    include_directories(${CHROMIUM_SRC_DIR})
    include_directories(${CHROMIUM_SRC_DIR}/base)
    include_directories(${CHROMIUM_SRC_DIR}/ui)
    include_directories(${CHROMIUM_SRC_DIR}/content)
    include_directories(${CHROMIUM_SRC_DIR}/chrome)
    set(HAVE_CHROMIUM TRUE)
else()
    message(STATUS "Chromium source not found - building standalone version")
    set(HAVE_CHROMIUM FALSE)
endif()

# Source files
set(SOURCES
    src/navigrab_core.cpp
    src/proactive_scraper.cpp
    src/tooltip_service.cpp
    src/element_detector.cpp
    src/screenshot_capture.cpp
    src/dark_mode_manager.cpp
    src/tooltip_view.cpp
    src/navigrab_integration.cpp
    src/tooltip_browser_integration.cpp
    src/tooltip_prefs.cpp
    src/ai_integration.cpp
    src/fresh_crawl_button.cpp
    src/tooltip_toolbar_integration.cpp
)

# Header files
set(HEADERS
    include/navigrab_core.h
    include/proactive_scraper.h
    include/tooltip_service.h
    include/element_detector.h
    include/screenshot_capture.h
    include/dark_mode_manager.h
    include/tooltip_view.h
    include/navigrab_integration.h
    include/tooltip_browser_integration.h
    include/tooltip_prefs.h
    include/ai_integration.h
    include/fresh_crawl_button.h
    include/tooltip_toolbar_integration.h
)

# Create library
add_library(NaviGrabTooltipLib SHARED ${SOURCES} ${HEADERS})

# Set properties
set_target_properties(NaviGrabTooltipLib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "navigrab_tooltip"
)

# Compiler flags
if(MSVC)
    target_compile_options(NaviGrabTooltipLib PRIVATE /W4)
else()
    target_compile_options(NaviGrabTooltipLib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link libraries
if(HAVE_CHROMIUM)
    # Link with Chromium libraries if available
    target_link_libraries(NaviGrabTooltipLib PRIVATE
        # Add Chromium libraries here if needed
    )
else()
    # Standalone mode - minimal dependencies
    message(STATUS "Building in standalone mode")
endif()

# Install targets
install(TARGETS NaviGrabTooltipLib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include/navigrab_tooltip)

# Create example executable
add_executable(tooltip_example examples/basic_usage.cpp)
target_link_libraries(tooltip_example PRIVATE NaviGrabTooltipLib)

# Create pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/navigrab_tooltip.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/navigrab_tooltip.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/navigrab_tooltip.pc
    DESTINATION lib/pkgconfig
)
