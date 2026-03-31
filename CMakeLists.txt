cmake_minimum_required(VERSION 3.16)
project(EasyMail VERSION 0.1 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/Qt/6.10.2/msvc2022_64" CACHE PATH "" FORCE)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 COMPONENTS Concurrent REQUIRED)
find_package(Qt6 COMPONENTS Core Gui Widgets Network Sql Quick Qml WebEngineQuick REQUIRED)

set(VMIME_ROOT "C:/vmime/install")

find_library(VMIME_LIB
    NAMES vmime
    PATHS "${VMIME_ROOT}/lib"
    REQUIRED
)

set(CURL_INCLUDE_DIR "C:/msys64/mingw64/include")
set(CURL_LIBRARY "C:/msys64/mingw64/lib/libcurl.dll.a")

if(NOT VMIME_LIB)
    message(FATAL_ERROR "VMime library not found at ${VMIME_ROOT}/lib")
endif()

# Opcjonalnie - sprawdź czy pliki istnieją
if(NOT EXISTS ${CURL_INCLUDE_DIR}/curl/curl.h)
    message(FATAL_ERROR "CURL headers not found at ${CURL_INCLUDE_DIR}")
endif()

if(NOT EXISTS ${CURL_LIBRARY})
    message(FATAL_ERROR "CURL library not found at ${CURL_LIBRARY}")
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EasyMail
        MANUAL_FINALIZATION

        main.cpp

        Model/message.h Model/message.cpp
        Model/email.h Model/email.cpp
        Infrastructure/imapclient.h Infrastructure/imapclient.cpp
        Service/service.h Service/service.cpp
        View/emailmodel.h View/emailmodel.cpp
        Repository/repositoryemail.h Repository/repositoryemail.cpp
        main.qml

    )
else()
    if(ANDROID)
        add_library(EasyMail SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(EasyMail
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Dodaj include directory
target_include_directories(EasyMail PRIVATE ${CURL_INCLUDE_DIR})

target_link_libraries(EasyMail
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${CURL_LIBRARY}
        Qt6::Network
        Qt6::Concurrent
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Quick
        Qt6::Qml
        Qt6::WebEngineQuick
)
target_link_libraries(EasyMail PRIVATE Qt${QT_VERSION_MAJOR}::Core)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EasyMail)
endif()

set_target_properties(EasyMail PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_add_qml_module(EasyMail
    URI EmailClient
    VERSION 1.0
    QML_FILES main.qml
    SOURCES
)

include(GNUInstallDirs)
install(TARGETS EasyMail
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EasyMail)
endif()
