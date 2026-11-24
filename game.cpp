#include "board.cpp"

class Game {
public:
    Game();

    void run();

    int score() const { return score_; }
    int lines() const { return lines_; }
    int level() const { return level_; }
    bool isGameOver() const { return gameOver_; }

private:
    void reset();
    void spawnPiece();
    bool tryMove(int dx, int dy);
    void tryRotate();
    void hardDrop();
    void lockCurrentPiece();
    void tick();
    int dropIntervalMs() const; 
    void draw() const;

    Board board_;
    Piece current_;
    Piece next_;

    int score_;
    int lines_;
    int level_;
    int lockTicks_ = 0;
    bool gameOver_;
    bool quit_;
    bool paused_;
};


#include <algorithm>
#include <chrono>
#include <curses.h>

namespace {

constexpr int MIN_ROWS = 25;
constexpr int MIN_COLS = 60;

constexpr int BOARD_TOP = 1;
constexpr int BOARD_LEFT = 1;
constexpr int INFO_X = BOARD_LEFT + 2 * Board::WIDTH + 4;

struct CursesGuard {
    CursesGuard() {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);
        timeout(10);

        if (has_colors()) {
            start_color();
            const short orange = (COLORS >= 256) ? 208 : COLOR_WHITE;
            const short colors[7] = {COLOR_CYAN, COLOR_YELLOW, COLOR_MAGENTA,
                                     COLOR_GREEN, COLOR_RED, COLOR_BLUE, orange};
            for (short i = 0; i < 7; ++i)
                init_pair(i + 1, COLOR_BLACK, colors[i]);
        }
    }
    ~CursesGuard() { endwin(); }
};

void drawCell(int y, int x, int colorId) {
    if (colorId == 0) {
        attron(A_DIM);
        mvaddstr(y, x, " .");
        attroff(A_DIM);
    } else if (has_colors()) {
        attron(COLOR_PAIR(colorId));
        mvaddstr(y, x, "  ");
        attroff(COLOR_PAIR(colorId));
    } else {
        mvaddstr(y, x, "[]");
    }
}

}