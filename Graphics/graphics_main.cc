#include "polygon.h"
#include "shape.h"
#include "window.h"
#include "render_engine.h"

int main(){
  auto w = Window(800, 600);
  auto vis = RenderEngine{};
  vis.RegisterWindow(w);
  auto p1 = Polygon( {400, 300}, 20.0, 20.0 );
  auto p2 = Polygon( {400, 500}, 30.0, 30.0 );
  auto shape = Shape{};
  shape.AddPolygon(p1);
  shape.AddPolygon(p2);
  while (!w.Exit()) {
    shape.RotateCM(0.01);
    vis.AddToQueue(shape);
    vis.Render();
    vis.Notify();
    w.Update();
  }
  return 0;
}