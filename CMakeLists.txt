cmake_minimum_required(VERSION 3.19)
project(HookOfTheReaper LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)


qt_add_executable(HookOfTheReaper
    WIN32 MACOSX_BUNDLE
    main.cpp ${app_icon_resource_windows}
    hookOfTheReaper.cpp
    hookOfTheReaper.h
    hookOfTheReaper.ui
    Global.h

    data/icons/icons.rc
    data/icons/hOTRIcon256.ico
    data/iniDefault.hor data/lgDefault.hor data/settings.hor
    ini/default.ini
    ini/MAME/digdug.ini ini/default.ini
    defaultLG/hotd4.txt defaultLG/jp.txt defaultLG/rambo.txt
    COMDeviceList/ComDeviceList.cpp COMDeviceList/ComDeviceList.h COMDeviceList/ComPortDevice.cpp COMDeviceList/ComPortDevice.h COMDeviceList/LightGun.cpp COMDeviceList/LightGun.h
    HookerEngine/HookCOMPort.cpp HookerEngine/HookCOMPort.h HookerEngine/HookerEngine.cpp HookerEngine/HookerEngine.h HookerEngine/HookTCPSocket.cpp HookerEngine/HookTCPSocket.h
    Windows/addComDeviceWindow.cpp Windows/addComDeviceWindow.h Windows/addComDeviceWindow.ui
    Windows/addLightGunWindow.cpp Windows/addLightGunWindow.h Windows/addLightGunWindow.ui
    Windows/editComDeviceWindow.cpp Windows/editComDeviceWindow.h Windows/editComDeviceWindow.ui
    Windows/editLightGunWindow.cpp Windows/editLightGunWindow.h Windows/editLightGunWindow.ui
    Windows/playerAssignWindow.cpp Windows/playerAssignWindow.h Windows/playerAssignWindow.ui
    Windows/settingsWindow.cpp Windows/settingsWindow.h Windows/settingsWindow.ui
    Windows/aboutWindow.cpp Windows/aboutWindow.h Windows/aboutWindow.ui
    data/mx24.hor data/rs3Reaper.hor
    Windows/testComPortWindow.h Windows/testComPortWindow.cpp Windows/testComPortWindow.ui
    data/jbgun4ir.hor
    data/nonDefaultLG.hor
    README.md
    defaultLG/lgi.txt defaultLG/lgi3d.txt defaultLG/lgj.txt
    defaultLG/alien3.txt defaultLG/confmiss.txt defaultLG/ptblank2.txt
    data/fusion.hor
    data/blamcon.hor
    HookerEngine/HookCOMPortWin.h HookerEngine/HookCOMPortWin.cpp
    data/sounds/sounds.qrc
    data/icons/icons.qrc

)



target_include_directories(HookOfTheReaper
    PRIVATE
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6SerialPort_INCLUDE_DIRS}
    ${Qt6Network_INCLUDE_DIRS}
    ${Qt6Multimedia_INCLUDE_DIRS}
    D:/QtProjects/HookOfTheReaperGit/HIDAPI/include
)
target_link_directories(HookOfTheReaper
    PRIVATE
    D:/QtProjects/HookOfTheReaperGit/HIDAPI/x64
)


target_link_libraries(HookOfTheReaper
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::SerialPort
        Qt6::Network
        Qt6::Multimedia
        hidapi
)

include(GNUInstallDirs)

install(TARGETS HookOfTheReaper
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET HookOfTheReaper
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/icons.rc")



#target_link_libraries(HookOfTheReaper
#    PRIVATE
#    hidapi
#)
