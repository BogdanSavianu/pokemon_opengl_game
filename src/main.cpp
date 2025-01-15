#include "core/Engine.hpp"

int main(int argc, const char * argv[]) {
    Engine engine;
    
    if (!engine.init()) {
        return 1;
    }
    
    engine.run();
    return 0;
}
