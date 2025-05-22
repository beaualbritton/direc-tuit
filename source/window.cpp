#include "window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

WindowRender::WindowRender() { setupWindow(); }

void WindowRender::setupWindow() {
  Element title, content;

  title = text("produc-tuity");
  content = vbox({paragraph("welcome to produc-tuity")});

  currentWindow = window(title | bold, content);

  windowComponent = Renderer([&] { return currentWindow; });
}

void WindowRender::run() {
  windowComponent = CatchEvent(windowComponent, [&](Event event) {
    if (event == Event::Character('q')) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });
  screen.Loop(windowComponent);
}
