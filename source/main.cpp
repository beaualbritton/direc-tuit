#include "user_config.hpp"
#include "window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

int main(int argc, char *argv[]) {
  // Displaying fullscreen in terminal interface. Takes width/height of current
  // viewport. See ftxui/component/screen_interactive.hpp
  //
  if (argc > 1)
    WindowRender::instance().setupWindow(argv[1]);
  else
    // Passing empty arg to setupWindow() which will always get currentPath
    // unless argv[1] path exists
    WindowRender::instance().setupWindow("");

  // Renderer component that will be passed to screen.loop()
  // Within that renderer is a single window (with just text for now)
  return 0;
}
