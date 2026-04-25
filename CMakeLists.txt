cmake_minimum_required(VERSION 3.19)
project(HookOfTheReaper LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


#find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort Network Multimedia Bluetooth)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets SerialPort Network Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets SerialPort Network Multimedia)


#qt_add_executable(HookOfTheReaper
add_executable(HookOfTheReaper
    WIN32 MACOSX_BUNDLE
    main.cpp ${app_icon_resource_windows}
    hookOfTheReaper.cpp
    hookOfTheReaper.h
    hookOfTheReaper.ui
    Global.h

    README.md
    data/icons/icons.rc
    data/icons/hOTRIcon256.ico
    data/iniDefault.hor data/lgDefault.hor data/settings.hor
    data/mx24.hor data/rs3Reaper.hor
    data/jbgun4ir.hor
    data/nonDefaultLG.hor
    data/fusion.hor
    data/blamcon.hor
    data/alienUSB.hor data/openFire.hor data/xGunner.hor
    data/sounds/sounds.qrc
    data/icons/icons.qrc

    COMDeviceList/ComDeviceList.cpp COMDeviceList/ComDeviceList.h
    COMDeviceList/LightBackground.h COMDeviceList/LightBackground.cpp
    COMDeviceList/LightController.h COMDeviceList/LightController.cpp
    COMDeviceList/LightExecution.h COMDeviceList/LightExecution.cpp
    COMDeviceList/LightGun.cpp COMDeviceList/LightGun.h
    COMDeviceList/PacDriveControl.h COMDeviceList/PacDriveControl.cpp

    HookerEngine/GamePlayer.h HookerEngine/GamePlayer.cpp
    HookerEngine/HookCOMPort.cpp HookerEngine/HookCOMPort.h
    HookerEngine/HookCOMPortWin.h HookerEngine/HookCOMPortWin.cpp
    HookerEngine/HookerEngine.cpp HookerEngine/HookerEngine.h
    HookerEngine/HookLight.h HookerEngine/HookLight.cpp
    HookerEngine/HookTCPSocket.cpp HookerEngine/HookTCPSocket.h
    HookerEngine/LightCommand.h HookerEngine/LightCommand.cpp

    Windows/aboutWindow.cpp Windows/aboutWindow.h Windows/aboutWindow.ui
    Windows/addLightControllerWindow.h Windows/addLightControllerWindow.cpp Windows/addLightControllerWindow.ui
    Windows/addLightGunWindowV2.h Windows/addLightGunWindowV2.cpp Windows/addLightGunWindowV2.ui
    Windows/editLightControllerWindow.h Windows/editLightControllerWindow.cpp Windows/editLightControllerWindow.ui
    Windows/editLightGunWindowV2.h Windows/editLightGunWindowV2.cpp Windows/editLightGunWindowV2.ui
    Windows/playerAssignWindow.cpp Windows/playerAssignWindow.h Windows/playerAssignWindow.ui
    Windows/settingsWindow.cpp Windows/settingsWindow.h Windows/settingsWindow.ui
    Windows/testComPortWindow.h Windows/testComPortWindow.cpp Windows/testComPortWindow.ui
    Windows/testLightController.h Windows/testLightController.cpp Windows/testLightController.ui

    ini/MAME/digdug.ini ini/default.ini
    defaultLG/hotd4.txt defaultLG/jp.txt defaultLG/rambo.txt
    defaultLG/lgi.txt defaultLG/lgi3d.txt defaultLG/lgj.txt
    defaultLG/alien3.txt defaultLG/confmiss.txt defaultLG/ptblank2.txt
    COMDeviceList/UltimarcLC.h COMDeviceList/UltimarcLC.cpp
    COMDeviceList/ALEDStripLC.h COMDeviceList/ALEDStripLC.cpp
)



target_include_directories(HookOfTheReaper
    PRIVATE
    D:/QtProjects/HookOfTheReaperGit/HIDAPI/include
    D:/QtProjects/HookOfTheReaperGit/UltimarcSDK
)
target_link_directories(HookOfTheReaper
    PRIVATE
    D:/QtProjects/HookOfTheReaperGit/HIDAPI/x64
    D:/QtProjects/HookOfTheReaperGit/UltimarcSDK
)

target_link_libraries(HookOfTheReaper
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::SerialPort
        Qt::Network
        Qt::Multimedia
        hidapi
        PacDrive64
)


include(GNUInstallDirs)





set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/data/icons/icons.rc")

target_link_options(HookOfTheReaper PRIVATE "-Wl,--start-group")


