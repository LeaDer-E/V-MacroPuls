cmake_minimum_required(VERSION 3.20)
project(V-MacroPlus VERSION 3.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(VMACRO_SOURCES
    src/main.cpp
    src/core/I18n.h
    src/core/I18n.cpp
    src/core/Theme.h
    src/core/Theme.cpp
    src/core/Paths.h
    src/core/Paths.cpp
    src/core/Models.h
    src/core/Models.cpp
    src/core/AppState.h
    src/core/AppState.cpp
    src/platform/Win32Input.h
    src/platform/Win32Input.cpp
    src/platform/MicrocontrollerBridge.h
    src/platform/MicrocontrollerBridge.cpp
    src/engine/PythonScriptRunner.h
    src/engine/PythonScriptRunner.cpp
    src/engine/MacroEngine.h
    src/engine/MacroEngine.cpp
    src/engine/AutoPresserEngine.h
    src/engine/AutoPresserEngine.cpp
    src/ui/Icons.h
    src/ui/Icons.cpp
    src/ui/Widgets.h
    src/ui/Widgets.cpp
    src/ui/AboutDialog.h
    src/ui/AboutDialog.cpp
    src/ui/OnboardingOverlay.h
    src/ui/OnboardingOverlay.cpp
    src/ui/SettingsPage.h
    src/ui/SettingsPage.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/pages/CombosPage.h
    src/ui/pages/CombosPage.cpp
    src/ui/pages/AutoPresserPage.h
    src/ui/pages/AutoPresserPage.cpp
    src/ui/pages/RecorderPage.h
    src/ui/pages/RecorderPage.cpp
    src/ui/pages/TestPage.h
    src/ui/pages/TestPage.cpp
    resources.qrc
)

if(WIN32)
    set(APP_ICON_RC "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
    file(COPY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.ico" "${CMAKE_CURRENT_BINARY_DIR}/icon.ico")
    file(WRITE "${APP_ICON_RC}" "IDI_ICON1 ICON \"icon.ico\"\n")
    if(MINGW)
        # Override RC compile rule to omit <INCLUDES> so windres.exe never invokes gcc/cc1.exe on paths with parentheses like (1) or (2)
        set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff --use-temp-file -i <SOURCE> -o <OBJECT>")
    endif()
    qt_add_executable(V-MacroPlus WIN32 ${VMACRO_SOURCES} "${APP_ICON_RC}")
    target_link_libraries(V-MacroPlus PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets user32 kernel32 advapi32 shell32)
    if(MINGW)
        target_link_options(V-MacroPlus PRIVATE -static-libgcc -static-libstdc++)
    endif()
else()
    qt_add_executable(V-MacroPlus ${VMACRO_SOURCES})
    target_link_libraries(V-MacroPlus PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets pthread)
    find_package(X11)
    if(X11_FOUND)
        target_include_directories(V-MacroPlus PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(V-MacroPlus PRIVATE ${X11_LIBRARIES})
        if(X11_XTest_FOUND)
            target_link_libraries(V-MacroPlus PRIVATE ${X11_XTest_LIB})
            target_compile_definitions(V-MacroPlus PRIVATE VMACRO_HAS_XTEST=1)
        endif()
    endif()
endif()

target_include_directories(V-MacroPlus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Copy runtime assets, firmware, and drivers next to executable
add_custom_command(TARGET V-MacroPlus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/firmware" "$<TARGET_FILE_DIR:V-MacroPlus>/firmware"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/drivers" "$<TARGET_FILE_DIR:V-MacroPlus>/drivers"
)
