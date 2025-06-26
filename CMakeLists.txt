cmake_minimum_required(VERSION 3.14.0)
project(dir-tuity VERSION 0.0.1 LANGUAGES CXX)

#c23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)
add_executable(dir-tuit source/main.cpp
source/window.cpp
source/user_config.cpp
source/foperation.cpp
source/components/file_explorer.cpp
source/components/popup.cpp)
target_link_libraries(dir-tuit
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE tomlplusplus::tomlplusplus
)
