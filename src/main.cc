#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <memory>

#ifdef WEB_VERSION
    #include <emscripten.h>
    #include "web_interface.h"
    // Web version doesn't use traditional main loop
    extern "C" {
        EMSCRIPTEN_KEEPALIVE
        void init_chess_engine() {
            // Web interface initialization is handled by JavaScript
        }
    }
    int main() { return 0; } // Minimal main for web version
#else
    #ifndef NO_GRAPHICS
        #include "graphics/window.h"
    #endif
    #include "game.h"
    #include "ai_factory.h"
    using namespace std;

    int main(int argc, char *argv[]) {
#ifdef NO_GRAPHICS
        Game game {nullptr}; // Console mode
#else
        unique_ptr<Xwindow> window = make_unique<Xwindow>(680, 680);
        Game game {window.get()};
#endif

        cout << "Chess Engine v2.0 - Advanced AI Edition" << endl;
        cout << "Commands: game [white] [black], setup, quit, algorithms" << endl;
        cout << "Players: human, computer1-8" << endl;
        cout << "Levels 1-4: Classic algorithms | Levels 5-8: Advanced AI" << endl;
        cout << "Example: game human computer6" << endl;
        cout << "Type 'algorithms' for detailed AI information" << endl << endl;

        string inputLine;
        while (getline(cin, inputLine)) {
            istringstream iss{inputLine};
            string command, whitePlayer, blackPlayer; 
            iss >> command >> whitePlayer >> blackPlayer;

            if (command == "game") {
                bool whiteIsHuman = whitePlayer == "human";
                bool blackIsHuman = blackPlayer == "human";

                if (whiteIsHuman && blackIsHuman) {
                    game.startGame(true, true, -1, -1);
                } else if (whiteIsHuman && !blackIsHuman) {
                    int difficulty = static_cast<int>(blackPlayer.back()) - '0';
                    if (difficulty < 1 || difficulty > 8) {
                        cerr << "Invalid difficulty level. Use 1-8." << endl;
                        continue;
                    }
                    cout << "Black AI: " << AIFactory::getAIDescription(difficulty) << endl;
                    game.startGame(true, false, -1, difficulty);
                } else if (!whiteIsHuman && blackIsHuman) {
                    int difficulty = static_cast<int>(whitePlayer.back()) - '0';
                    if (difficulty < 1 || difficulty > 8) {
                        cerr << "Invalid difficulty level. Use 1-8." << endl;
                        continue;
                    }
                    cout << "White AI: " << AIFactory::getAIDescription(difficulty) << endl;
                    game.startGame(false, true, difficulty, -1);
                } else {
                    int whiteDifficulty = static_cast<int>(whitePlayer.back()) - '0';
                    int blackDifficulty = static_cast<int>(blackPlayer.back()) - '0';
                    if (whiteDifficulty < 1 || whiteDifficulty > 8 || 
                        blackDifficulty < 1 || blackDifficulty > 8) {
                        cerr << "Invalid difficulty level. Use 1-8." << endl;
                        continue;
                    }
                    cout << "White AI: " << AIFactory::getAIDescription(whiteDifficulty) << endl;
                    cout << "Black AI: " << AIFactory::getAIDescription(blackDifficulty) << endl;
                    game.startGame(false, false, whiteDifficulty, blackDifficulty);
                }

            } else if (command == "setup") {
                game.setupBoard();
            } else if (command == "algorithms" || command == "ai") {
                printAIAlgorithmInfo();
            } else if (command == "quit" || command == "exit") {
                break;
            } else {
                cerr << "Invalid command. Use 'game', 'setup', 'algorithms', or 'quit'." << endl;
            }
        }

        game.renderScore();
        return 0;
    }
#endif
