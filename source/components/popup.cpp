#include "../foperation.hpp"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

Component horizontalPopup(std::string message, bool *modalFlag,
                          std::function<void()> onConfirm) {
  Component confirmButton, cancelButton;
  confirmButton = Button("[Y]es", onConfirm, ButtonOption::Ascii());
  cancelButton = Button(
      "[N]o", [modalFlag] { *modalFlag = false; }, ButtonOption::Ascii());
  Component popupBody = Container::Horizontal({confirmButton, cancelButton});
  Component popup = Renderer(popupBody, [popupBody, message] {
    return window(text(message), popupBody->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 5) |
           border | center;
  });

  return popup;
}
Component deleteOptionPopUp(bool *modalFlag, std::filesystem::path pPath,
                            std::function<void()> refresh) {
  // Originally was going to using ftxui::Menu but it has limited support for
  // onEnter events

  Component deleteButton = Button("[D]elete", refresh, ButtonOption::Ascii());
  Component optionContainer = Container::Vertical({deleteButton});
  Component popup = Renderer(optionContainer, [pPath, optionContainer] {
    return window(text(pPath.filename().string()), optionContainer->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 5) |
           center;
  });
  return popup;
}

/* TODO: These should have corresponding keybinds in filexplorer
  Component openButton, openWithButton, previewButton,
   TODO: more button options. viewInfo, rename, copy, paste,
*/
