#include "Application.hpp"

int main() {
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) {
        Terminal::printError(std::string("Erreur fatale: ") + e.what());
        return 1;
    }
    return 0;
}
