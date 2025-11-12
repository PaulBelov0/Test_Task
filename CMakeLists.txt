cmake_minimum_required(VERSION 3.19)
project(Test_Task LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Test_Task
    WIN32 MACOSX_BUNDLE
    main.cpp
    ui/main_window.cpp
    ui/main_window.h
    ui/models/tab_item_model.h ui/models/tab_item_model.cpp
    ui/widgets/tree_view_field.h ui/widgets/tree_view_field.cpp
    ui/widgets/table_view_field.h ui/widgets/table_view_field.cpp
    services/app_manager.h services/app_manager.cpp
    services/archive_manager.h services/archive_manager.cpp
    config.json
    terminal/terminal_core.h terminal/terminal_core.cpp
    services/archiver_pipeline.h services/archiver_pipeline.cpp
)

find_package(ZLIB REQUIRED)

target_include_directories(Test_Task PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/services
)

target_link_libraries(Test_Task
    PRIVATE
        Qt::Core
        Qt::Widgets
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    ${CMAKE_CURRENT_BINARY_DIR}/config.json
    COPYONLY
)

include(GNUInstallDirs)

install(TARGETS Test_Task
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_compile_options("/utf-8")

qt_generate_deploy_app_script(
    TARGET Test_Task
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
