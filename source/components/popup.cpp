#include "../foperation.hpp"
#include "../user_config.hpp"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
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
Component renameOptionPopUp(bool *modalFlag, std::filesystem::path pPath,
                            std::shared_ptr<std::string> renameString,
                            std::function<void()> refresh) {
  Component renameInput, extensionText, renameConfirm, renameCancel, errorText;
  shared_ptr<bool> pathExistsFlag = make_shared<bool>(false);

  shared_ptr<string> titleString =
      make_shared<string>("[R]enaming: " + pPath.filename().string());
  InputOption renameInputOpts = InputOption();
  renameInputOpts.multiline = false;
  renameInputOpts.on_enter = [=] {
    *titleString = "[R]ename " + pPath.filename().string() +
                   " to: " + *renameString + getExtension(pPath) + "?";

    std::filesystem::path parentPath;
    if (pPath.has_parent_path())
      parentPath = pPath.parent_path();
    if (exists(parentPath / (*renameString + getExtension(pPath)))) {
      *renameString = "";
      *pathExistsFlag = true;
    } else {
      *pathExistsFlag = false;
    }
  };
  renameInput = Input(renameString.get(), "filename", renameInputOpts);
  extensionText = Renderer([pPath] { return text(getExtension(pPath)); });
  renameConfirm = Button("[Y]es", refresh, ButtonOption::Ascii());
  renameCancel = Button(
      "[N]o", [modalFlag] { *modalFlag = false; }, ButtonOption::Ascii());
  errorText = Renderer([] { return text("path exists!"); });

  renameConfirm = Maybe(renameConfirm, [=] { return !*pathExistsFlag; });
  renameCancel = Maybe(renameCancel, [=] { return !*pathExistsFlag; });
  errorText = Maybe(errorText, [=] { return *pathExistsFlag; });

  Component buttonContainer =
      Container::Horizontal({errorText, renameConfirm, renameCancel});
  Component renameContainer =
      Container::Horizontal({renameInput, extensionText});
  Component bodyContainer =
      Container::Vertical({renameContainer, buttonContainer});

  Component popup = Renderer(bodyContainer, [titleString, bodyContainer] {
    return window(text(*titleString), bodyContainer->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 5) |
           center;
  });
  return popup;
}

Component newOptionPopUp(bool *modalFlag, std::filesystem::path pPath,
                         std::shared_ptr<std::string> fileName,
                         std::function<void()> refresh, bool *dirFlag) {

  Component fileNameInput, fileConfirm, fileCancel;
  shared_ptr<string> titleString = make_shared<string>("[N]ew file.");

  InputOption fileInputOpts = InputOption();
  fileInputOpts.multiline = false;
  fileInputOpts.on_enter = [=] {
    *titleString = "[N]ew file. Create:  " + *fileName + "?";
    // not a directory
    if (!(pPath / *fileName).extension().empty()) {
      *dirFlag = false;
    } else {
      *dirFlag = true;
    }
  };

  fileNameInput = Input(fileName.get(), "filename", fileInputOpts);
  fileConfirm = Button("[Y]es", refresh, ButtonOption::Ascii());
  fileCancel = Button(
      "[N]o", [modalFlag] { *modalFlag = false; }, ButtonOption::Ascii());

  Component buttonContainer = Container::Horizontal({fileConfirm, fileCancel});
  Component bodyContainer =
      Container::Vertical({fileNameInput, buttonContainer});

  Component popup = Renderer(bodyContainer, [titleString, bodyContainer] {
    return window(text(*titleString), bodyContainer->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 5) |
           center;
  });
  return popup;
}

Component viewInfoPopUp(bool *modalFlag, std::filesystem::path pPath,
                        std::string permString,
                        std::function<void()> closeLambda) {
  Component previewRender = Renderer([pPath] {
    return paragraph(getPreviewString(pPath)) | size(HEIGHT, EQUAL, 5) | frame |
           vscroll_indicator;
  });
  Component renderSeparator = Renderer([] { return separator(); });
  Component textRender = Button(permString, [] {}, ButtonOption::Ascii());

  Component bodyContainer = Container::Vertical({});
  if (!std::filesystem::is_directory(pPath)) {
    bodyContainer->Add(previewRender);
    bodyContainer->Add(renderSeparator);
  }
  bodyContainer->Add(textRender);
  Component bodyContainerEvents =
      CatchEvent(bodyContainer, [modalFlag, closeLambda](Event event) {
        if (event == Event::Character('x')) {
          closeLambda();
          return true;
        }
        return false;
      });
  shared_ptr<string> titleString =
      make_shared<string>("[V]iew info: " + pPath.filename().string());
  Component popup = Renderer(bodyContainerEvents, [titleString, bodyContainer] {
    return window(text(*titleString), bodyContainer->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 10) |
           center;
  });
  return popup;
}

Component setEditorPopUp(bool *modalFlag) {
  Component setEditorInput, setConfirm, setCancel;
  shared_ptr<string> titleString = make_shared<string>("Set your editor!");

  InputOption setEditorOptions = InputOption();
  setEditorOptions.multiline = false;
  shared_ptr<string> stringInput = make_shared<string>();

  setEditorInput = Input(stringInput.get(), "editor path", setEditorOptions);
  setConfirm = Button(
      "[Y]es",
      [stringInput, modalFlag] {
        setPathToEditor(std::filesystem::path(*stringInput));
        *modalFlag = false;
      },
      ButtonOption::Ascii());
  setCancel = Button(
      "[N]o", [modalFlag] { *modalFlag = false; }, ButtonOption::Ascii());

  Component buttonContainer = Container::Horizontal({setConfirm, setCancel});
  Component bodyContainer =
      Container::Vertical({setEditorInput, buttonContainer});

  Component popup = Renderer(bodyContainer, [titleString, bodyContainer] {
    return window(text(*titleString), bodyContainer->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 5) |
           center;
  });
  return popup;
}

Component openWithPopUp(bool *modalFlag, std::filesystem::path pPath,
                        std::function<void()> openLambda) {
  // Check if .config editor is set. If not, ask for it to be input.
  if (!externalEditorSet()) {
    return setEditorPopUp(modalFlag);
  }
  Component openText, openConfirm, openCancel;
  std::filesystem::path editorPath = getExternalEditorPath();
  openText = Renderer([=] {
    return text("Open " + pPath.string() + " with: " + editorPath.string() +
                "?");
  });
  openConfirm = Button("[Y]es", openLambda, ButtonOption::Ascii());
  openCancel =
      Button("[N]o", [=] { *modalFlag = false; }, ButtonOption::Ascii());
  Component buttonContainer = Container::Horizontal({openConfirm, openCancel});
  Component bodyContainer = Container::Vertical({openText, buttonContainer});

  Component popup = Renderer(bodyContainer, [bodyContainer] {
    return window(text(""), bodyContainer->Render()) |
           size(WIDTH, GREATER_THAN, 30) | size(HEIGHT, GREATER_THAN, 5) |
           center;
  });
  return popup;
}
