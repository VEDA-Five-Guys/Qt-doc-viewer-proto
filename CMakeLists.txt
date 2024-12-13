cmake_minimum_required(VERSION 3.16)

project(PdfViewer_with_Qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets PdfWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets PdfWidgets)

# Boost 연결
set(BOOST_ROOT_MSVC "C:/boost_1_86_0_msvc")
find_package(Boost 1.86.0 REQUIRED COMPONENTS system thread chrono)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIR}
)

set(PROJECT_HEADERS
    include/pdf_viewer_widget.h


    include/main_window.h
    include/event_overlay_widget.h
    include/websocket_server.h
)

set(PROJECT_SOURCES
    source/pdf_viewer_widget.cpp


    source/main_window.cpp
    source/event_overlay_widget.cpp
)

set(PROJECT_UI
    # ui/pdf_viewer_widget.ui
    ui/main_window.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PdfViewer_with_Qt
        MANUAL_FINALIZATION
        main.cpp
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        ${PROJECT_UI}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET PdfViewer_with_Qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(PdfViewer_with_Qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(PdfViewer_with_Qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(PdfViewer_with_Qt
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PdfWidgets
    Boost::boost
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PdfViewer_with_Qt)
endif()
set_target_properties(PdfViewer_with_Qt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PdfViewer_with_Qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PdfViewer_with_Qt)
endif()
