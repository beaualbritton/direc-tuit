#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

Component horizontalPopup(std::string msg, bool *modalFlag,
                          std::function<void()> onConfirm);
Component verticalPopUp();

Component deleteOptionPopUp(bool *modalFlag, std::filesystem::path pPath,
                            std::function<void()> refresh);

Component renameOptionPopUp(bool *modalFlag, std::filesystem::path pPath,
                            std::shared_ptr<std::string> renameString,
                            std::function<void()> refresh);

Component newOptionPopUp(bool *modalFlag, std::filesystem::path pPath,
                         std::shared_ptr<std::string> fileName,
                         std::function<void()> refresh, bool *dirFlag);

Component viewInfoPopUp(bool *modalFlag, std::filesystem::path pPath,
                        std::string permString,
                        std::function<void()> closeLambda);

Component openWithPopUp(bool *modalFlag, std::filesystem::path pPath,
                        std::function<void()> openLambda);

Component setEditorPopUp(bool *modalFlag);
