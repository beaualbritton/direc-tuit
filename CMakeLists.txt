cmake_minimum_required(VERSION 3.11.0)
project(produc-tuity VERSION 0.0.1 LANGUAGES CXX)

#c23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui

  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)
add_executable(produc-tuity source/main.cpp
source/window.cpp)
target_link_libraries(produc-tuity
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)
