#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

Component horizontalPopup(std::string msg, bool *modalFlag,
                          std::function<void()> onConfirm);
Component verticalPopUp();
Component fileOptionPopUp(bool *modalFlag, std::filesystem::path pPath);
