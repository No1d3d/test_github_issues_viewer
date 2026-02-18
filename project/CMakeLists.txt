cmake_minimum_required(VERSION 3.22)
project(GithubIssuesViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Qml
    Network
)

add_executable(app
    main.cpp
    resources.qrc
    src/GithubClient.cpp
    src/IssuesModel.cpp
    src/OfflineCache.cpp
)

target_include_directories(app PRIVATE src)

target_link_libraries(app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Network
)
