#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;
// Returns navigable terminal-based file explorer
Component fileExplorer();
void populate(std::shared_ptr<ComponentBase> pContainer,
              const std::filesystem::path &pPath);
void getUserPinned(Component pContainer, Component fileContainer);
std::filesystem::path homeDir();
