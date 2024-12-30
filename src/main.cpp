#include <raylib.hpp>
#include "berzerk.hpp"
#include "renderer.hpp"

const double TICK_RATE = 1.0 / 30.0;

int main() {
    using namespace bm;

    berzerk::World world {};
    berzerk::Renderer renderer(&world);

    world.nextLayout();
    renderer.initialize();

    double previousTime = GetTime();
    double lag = 0.0;

    while (!bm::berzerk::shouldStop) {
        double currentTime = GetTime();
        double deltaTime = currentTime - previousTime;

        previousTime = currentTime;
        lag += deltaTime;

        while (lag >= TICK_RATE) {
            world.tick(TICK_RATE);
            lag -= TICK_RATE;
        }

        renderer.render();
    }

    renderer.deinitialize();
}
