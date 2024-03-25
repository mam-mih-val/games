#include "window.h"

Window WindowManager::window = Window{};
std::atomic<bool> WindowManager::exit = false;
std::atomic<bool> WindowManager::pause = false;
