#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <atomic>
#include <mutex>
#include <queue>

struct Window{
  sf::RenderWindow window{ sf::VideoMode(800, 800), "My window" };
  std::mutex mutex{};
};

class WindowManager{
public:
  static Window window;
  static std::atomic<bool> exit;
  static std::atomic<bool> pause;
};

#endif // WINDOW_H
