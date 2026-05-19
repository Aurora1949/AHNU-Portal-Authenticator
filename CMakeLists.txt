cmake_minimum_required(VERSION 3.30)
project(AHNU-Portal-Authenticator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(APP_NAME AHNU)

if (WIN32)
    set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")
endif ()

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED
)

include_directories(include)

add_executable(${APP_NAME}
        WIN32
        main.cpp
        src/view/mainwindow.cpp
        include/view/mainwindow.h
        src/view/mainwindow.ui
        src/model/auth_service.cpp
        include/model/auth_service.h
        src/model/config_manager.cpp
        include/model/config_manager.h
        src/controller/app_controller.cpp
        include/controller/app_controller.h
        include/common/constants.h
        res.qrc
        ${app_icon_resource_windows}
)

target_compile_definitions(${APP_NAME} PRIVATE PRIVATE APPNAME="${APP_NAME}")

target_link_libraries(${APP_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
)

# --- Packaging Configuration ---
if (WIN32)
    set(CPACK_PACKAGE_NAME "AHNU-Portal-Authenticator")
    set(CPACK_PACKAGE_VENDOR "AHNU")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AHNU Portal Authenticator")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "AHNU Portal Authenticator")
    
    # MSI specific settings
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "D672E15C-3D2F-4E9D-A92B-E5497A81A4B0") # Example GUID
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/app.ico")
    
    # Executable and Start Menu Shortcut
    install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
    
    # Shortcuts and Menu
    set(CPACK_PACKAGE_EXECUTABLES "${APP_NAME}" "AHNU Portal Authenticator")
    set(CPACK_CREATE_DESKTOP_LINKS "${APP_NAME}")
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "AHNU")

    # Install Qt dependencies
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    
    if (WINDEPLOYQT_EXECUTABLE)
        install(CODE "
            execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" 
                            --release 
                            --no-translations 
                            --compiler-runtime 
                            \"\${CMAKE_INSTALL_PREFIX}/bin/${APP_NAME}.exe\")
        ")
    endif ()
    
    include(CPack)
endif ()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${APP_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Network)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${APP_NAME}>")
    endforeach (QT_LIB)
endif ()
