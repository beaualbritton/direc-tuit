#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

Component fileExplorer() {
  Component explorer = Renderer([=] {
    return window(text("file explorer place holder"),
                  vbox(text("file explorer"))) |
           bold;
  });
  return explorer;
}
