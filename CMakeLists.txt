cmake_minimum_required(VERSION 3.16)

project(DataRelaySever LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    WebSockets
)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    WebSockets
)

# 手动列出文件
set(MAIN_FILE
    main.cpp
)

set(SOURCES
    WidgetClass/Source/DataRelayWidget.cpp
    JsonClass/Source/ATSSessionDefinitionJsonMsg.cpp
    JsonClass/Source/ATSStateControlJsonMsg.cpp
    JsonClass/Source/ATSVirtualTimeJsonMsg.cpp
    JsonClass/Source/ATSATPATOinfoJsonMsg.cpp
    JsonClass/Source/ATSMMIInfoJsonMsg.cpp
    JsonClass/Source/ATSAutomaticTrainInfoJsonMsg.cpp
    JsonClass/Source/ATSLineDeviceDataJsonMsg.cpp
    JsonClass/Source/ATSJsonMsg.cpp
    JsonClass/Source/ReplyJsonMsg.cpp
    JsonClass/Source/SessionTrainData.cpp
    JsonClass/Source/SimulatorJsonMsg.cpp
    JsonClass/Source/ATSControlMsg.cpp
    JsonClass/Source/ATSSIMFaultInfo.cpp
    JsonClass/Source/SimRunningLogMsg.cpp
    JsonClass/Source/SimAtsEventInfoMsg.cpp
    ServiceClass/Source/WebSocket.cpp
    ServiceClass/Source/minetohengnuoserver.cpp
    ServiceClass/Source/logmanager.cpp
    ServiceClass/Source/task_cpp/ATSClientWorker.cpp
    ServiceClass/Source/sessions_cpp/SimulatorSession.cpp
    ServiceClass/Source/sessions_cpp/ATSControlSession.cpp
    ServiceClass/Source/task_cpp/RevSimulatorDataWorker.cpp
    ServiceClass/Source/task_cpp/SendATSDataWorker.cpp
    ServiceClass/Source/task_cpp/DataParseTask.cpp
    ServiceClass/Source/task_cpp/ParseLineDeviceDataTask.cpp
    ServiceClass/Source/task_cpp/LogShowWorker.cpp
    ServiceClass/Source/HttpSever.cpp
    ServiceClass/Source/task_cpp/RevSimLogWorker.cpp
)

set(HEADERS
    WidgetClass/Header/DataRelayWidget.h
    JsonClass/Header/ATSSessionDefinitionJsonMsg.h
    JsonClass/Header/ATSStateControlJsonMsg.h
    JsonClass/Header/ATSVirtualTimeJsonMsg.h
    JsonClass/Header/ATSATPATOinfoJsonMsg.h
    JsonClass/Header/ATSMMIInfoJsonMsg.h
    JsonClass/Header/ATSAutomaticTrainInfoJsonMsg.h
    JsonClass/Header/ATSLineDeviceDataJsonMsg.h
    JsonClass/Header/ATSJsonMsg.h
    JsonClass/Header/ReplyJsonMsg.h
    JsonClass/Header/SessionTrainData.h
    JsonClass/Header/SimulatorJsonMsg.h
    JsonClass/StructDef/StructClsss.h
    JsonClass/Header/ATSConstrolMsg.h
    JsonClass/Header/ATSSIMFaultInfo.h
    JsonClass/Header/SimRunningLogMsg.h
    JsonClass/Header/SimAtsEventInfoMsg.h
    ServiceClass/Header/WebSocket.h
    ServiceClass/Header/minetohengnuoserver.h
    ServiceClass/Header/logmanager.h
    ServiceClass/Header/task_h/ATSClientWorker.h
    ServiceClass/Header/sessions_h/SimulatorSession.h
    ServiceClass/Header/sessions_h/ATSControlSession.h
    ServiceClass/Header/task_h/ParseLineDeviceDataTask.h
    ServiceClass/Header/task_h/DataParseTask.h
    ServiceClass/Header/task_h/RevSimulatorDataWorker.h
    ServiceClass/Header/task_h/SendATSDataWorker.h
    ServiceClass/Header/HttpSever.h
    ServiceClass/Header/task_h/RevSimLogWorker.h
    ServiceClass/Header/task_h/LogShowWorker.h
)

set(UI_FILES
    UI/DataRelayWidget.ui
)

set(RESOURCES
    Asset/Asset.qrc
)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/UI
)

# 分组
source_group("Main" FILES ${MAIN_FILE})
source_group("Sources" FILES ${SOURCES})
source_group("Headers" FILES ${HEADERS})
source_group("UIs" FILES ${UI_FILES})
source_group("Resources" FILES ${RESOURCES})

add_executable(DataRelaySever
    ${MAIN_FILE}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ${UI_FILES}
)

target_link_libraries(DataRelaySever PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
)

target_include_directories(DataRelaySever PRIVATE
    ${CMAKE_SOURCE_DIR}/WidgetClass/Header
    ${CMAKE_SOURCE_DIR}/JsonClass/Header
    ${CMAKE_SOURCE_DIR}/JsonClass/StructDef
    ${CMAKE_SOURCE_DIR}/ServiceClass/Header
)

include(GNUInstallDirs)
install(TARGETS DataRelaySever
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
