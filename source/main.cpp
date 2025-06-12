#include "user_config.hpp"
#include "window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
using namespace ftxui;

int main() {
  // testing user_config stuff
  std::cout << "checkConfig returns: " << checkConfig();

  // Displaying fullscreen in terminal interface. Takes width/height of current
  // viewport. See ftxui/component/screen_interactive.hpp
  WindowRender::instance().setupWindow();

  // Renderer component that will be passed to screen.loop()
  // Within that renderer is a single window (with just text for now)
  return 0;
}
