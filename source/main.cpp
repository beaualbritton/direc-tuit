#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int main()
{
  //Displaying fullscreen in terminal interface. Takes width/height of current viewport. 
  //See ftxui/component/screen_interactive.hpp
  auto screen = ScreenInteractive::Fullscreen();
  //Renderer component that will be passed to screen.loop()
  //Within that renderer is a single window (with just text for now)
  auto verticalContainer = vbox({text("Press 'q' to quit.")}) | border;
  auto introText = vbox({paragraph("welcome to produc-tuity")});
  auto openWindow = window(text("open text") | italic, introText);
  auto infoWindow = window(text("info-mation") | italic, verticalContainer); 
  auto focusedWindow = openWindow;
  bool focused = false;
  Component component = Renderer([&] {
    if(!focused)
    {
      focusedWindow = openWindow;
    }
    else
    {
      focusedWindow = infoWindow;
    }

      return window(
          text("produc-tuity") | bold,
          focusedWindow 
      );
  });

  // Wrap with quit-on-'q' behavior
  component = CatchEvent(component, [&](Event event) {
      if (event == Event::Character('q'))
         focused = !focused; 
      return false;
  });

  //Rendering loop, everything within window.
  screen.Loop(component);
  return 0;
}
