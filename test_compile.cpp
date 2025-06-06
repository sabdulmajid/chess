#ifdef NO_GRAPHICS
#include <iostream>
using namespace std;

// Forward declaration
class Xwindow;

class Game {
public:
    Game(void* unused = nullptr) {
        cout << "Console constructor called" << endl;
    }
};

int main() {
    Game game{nullptr};
    return 0;
}
#else
#error "NO_GRAPHICS should be defined"
#endif
