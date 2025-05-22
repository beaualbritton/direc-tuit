#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

class WindowRender {
public:
  // screen (terminal screen)
  inline static ScreenInteractive screen = ScreenInteractive::Fullscreen();
  // window that is currently being rendered
  Element currentWindow;
  Component windowComponent;

  // initializes Screen and sets currentWindow according to utility functions
  WindowRender();

  void setupWindow();
  void run();
  Element getStartMenu(bool userSet);
};
