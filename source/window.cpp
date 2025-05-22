#include "window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
using namespace ftxui;
using std::string;

Element userDialogueWindow();
Element introWindow();

WindowRender::WindowRender() { setupWindow(); }

void WindowRender::setupWindow() {
  Element title, content;

  title = text("produc-tuity");
  content = vbox({paragraph("welcome to produc-tuity")});
  currentWindow = userDialogueWindow(); // window(title | bold, content);
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
Element WindowRender::getStartMenu(bool userSet) {
  Element startWindow;
  if (!userSet) {
    startWindow = userDialogueWindow();

  } else {
    startWindow = introWindow();
  }
  return startWindow;
}
Element userDialogueWindow() {
  Element cWindow;
  string userName;
  Component inputName = Input(&userName, "Name");

  Element content = vbox(text("hello ☺ " + userName), inputName->Render());

  Element title = text("produc-tuity");

  cWindow = window(title | bold, content);

  return cWindow;
}
Element introWindow() {
  Element e;
  return e;
}
