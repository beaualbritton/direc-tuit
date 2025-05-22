#include "window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

int main() {
  // Displaying fullscreen in terminal interface. Takes width/height of current
  // viewport. See ftxui/component/screen_interactive.hpp
  WindowRender productuityWindow;

  productuityWindow.setupWindow();
  productuityWindow.run();
  // Renderer component that will be passed to screen.loop()
  // Within that renderer is a single window (with just text for now)
  return 0;
}
