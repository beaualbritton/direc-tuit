#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
using namespace ftxui;

int main() {
  //FTXUI element: text with a box border
  auto document = text("Hello, World!") | border;

  //Screen with full width and height fitting the document
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));

  // Render the document to the screen buffer and print it
  Render(screen, document);
  screen.Print();
  return 0;
}
