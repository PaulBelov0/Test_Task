cmake_minimum_required(VERSION 3.16)

project(Test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Test)

set(PROJECT_SOURCES
        src/main.cpp
        src/ui/main_window.cpp
        src/ui/main_window.h
        src/archive_core/archive_manager.cpp
        src/archive_core/archive_manager.h
        src/archive_core/archiver_pipeline.cpp
        src/archive_core/archiver_pipeline.h
)

set(TEST_SOURCES
    tests/unit_test_runner.cpp
    tests/test_archive_manager.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent Test)

    qt_add_executable(Test
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${TEST_SOURCES}
        src/resources.qrc
        src/ui/tab_items/summary_tab.h
        src/ui/tab_items/summary_tab.cpp
        src/ui/tab_items/files_tab.h
        src/ui/tab_items/files_tab.cpp
        src/ui/widgets/expandable_widget.h
        src/ui/widgets/expandable_widget.cpp
        src/ui/models/table_model.h
        src/ui/models/table_model.cpp
        src/ui/widgets/table_view.h
        src/ui/widgets/table_view.cpp
        src/structures/file_data.h
        tests/unit_test_runner.h
        tests/test_archive_manager.h
        tests/test_archiver_pipeline.h tests/test_archiver_pipeline.cpp
        tests/test_table_model.h tests/test_table_model.cpp
    )

else()
    if(ANDROID)
        add_library(Test SHARED
            ${PROJECT_SOURCES}
            ${TEST_SOURCES}
        )
    else()
        add_executable(Test
            ${PROJECT_SOURCES}
            ${TEST_SOURCES}
        )
    endif()
endif()

target_link_libraries(Test PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Test
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Test)
endif()

set_target_properties(Test PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Test)
endif()

# Опционально: добавить кастомную цель для запуска тестов
add_custom_target(run_tests
    COMMAND Test
    DEPENDS Test
    COMMENT "Running unit tests"
)
