#include "Riddle.h"
#include "Console.h"
#include "ReplayIO.h"

void Riddle::onStep(Player& player, Screen& screen)  {
    if (answered)
        return;

    
    screen.showRiddleQuestion(q);
    screen.printBoard();
    const int posY = 15;
    const int posX = 41; // fixed column for answer input

    const int wrongAnswer = 26; 
    

    // Use the answer stored in this Riddle instance (each Riddle keeps its own QA)
    string expected = a;

    while (true) {
        gotoxy(posX, posY);
        set_color(Color::LightYellow);

        // Read user input line until ENTER or ESC
        string input;
        while (true) {
            char ch = static_cast<char>(get_single_char());
            if (ch == 27) { // ESC cancels
                screen.printRoom();
                return;
            }
            if (ch == '\r' || ch == '\n') {
                break;
            }
            if(ch == '\b') { // Backspace
                if (!input.empty()) {
                    input.pop_back();
                    // Move cursor back, print space, move back again
                    cout << "\b \b" << flush;
                }
                continue;
			}
            if (ch >= '0' && ch <= '9' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
                input.push_back(ch);
                cout << ch << flush;
            }
        }

        auto trim = [](string s){
            size_t b = s.find_first_not_of(' ');
            size_t e = s.find_last_not_of(' ');
            if (b == string::npos) return string();
            return s.substr(b, e - b + 1);
        };
        string ans = trim(input);
        string exp = trim(expected);

        // Case-insensitive compare
        auto lower = [](string s){ for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c))); return s; };
        bool correct = (!exp.empty() && lower(ans) == lower(exp));
        // Log riddle via Screen-held ReplayIO (no globals)
        if (screen.getReplayIO()) {
            screen.getReplayIO()->logRiddle(screen.getReplayTick(), screen.getCurrentRoomIndex(), q, ans, correct);
        }
        if (correct) {
            player.addScore(50);
            sleep_ms(500);
            answered = true;
            Point p = pos;
            screen.changePixelInRoom(pos, ' ');
            pos.setCh(' ');
            screen.printRoom();
            screen.removeItemAt(p);
            return;
        } else {
            player.addScore(-20);
            // brief feedback below the answer row
            gotoxy(wrongAnswer, posY+2);
            set_color(Color::LightRed);
            cout << "Wrong answer, try again (ESC to cancel)" << flush;
            sleep_ms(500);
            // clear feedback line
            gotoxy(wrongAnswer, posY+2);
            set_color(Color::Gray);
            cout << string(40, ' ') << flush;
            reset_color();

            // flush only the wrong input from the Answer line (preserve board edges)
            gotoxy(posX, posY);
            set_color(Color::Gray);
            cout << string(input.size(), ' ') << flush;
            reset_color();
            // reposition cursor for next attempt
            gotoxy(posX, posY);
        }
    }
}
