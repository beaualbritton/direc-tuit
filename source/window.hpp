#pragma once
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

class WindowRender {
public:
  // trigger warning: singleton
  static WindowRender &instance() {
    static WindowRender instance;
    return instance;
  };

  // point to the window that is currently being rendered
  /*std::shared_ptr<Component> */ Component activeWindow;

  // Contains activeWindow
  Component rootWindow;

  void setupWindow();
  void run();
  void setWindow(Component);
  Component getWindow();

private:
  // c++ singleton time
  WindowRender() = default;
  WindowRender(const WindowRender &) = delete;
  WindowRender &operator=(const WindowRender &) = delete;
  // screen (terminal screen)
  inline static ScreenInteractive screen = ScreenInteractive::Fullscreen();
};
