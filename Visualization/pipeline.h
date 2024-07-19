#ifndef PIPELINE_H
#define PIPELINE_H

#include "window.h"
#include "visualizer.h"

namespace Visualization {

template<typename Func>
class Pipeline{
public:
  Pipeline<Func>(Func func) : visualizer_( std::move(func) ) { visualizer_.RegisterWindow(window_); }
  auto GetVisualizer() -> Visualizer<Func>& { return visualizer_; }
  auto GetWindow() -> Window& {return window_;}
  auto Draw() -> void {
    visualizer_.Visualize();
    window_.Update();
  }


private:
  Window window_{800, 600};
  Visualizer<Func> visualizer_;
};

}

#endif