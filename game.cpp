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

Game::Game()
    : current_(), next_(), score_(0), lines_(0), level_(1),
      gameOver_(false), quit_(false), paused_(false) {
    reset();
}

void Game::reset() {
    board_.clear();
    score_ = 0;
    lines_ = 0;
    level_ = 1;
    gameOver_ = false;
    quit_ = false;
    paused_ = false;
    next_ = Piece(Piece::randomType());
    spawnPiece();
}

void Game::spawnPiece() {
    current_ = next_;
    current_.setPosition(3, 0);  
    lockTicks_ = 0;
    next_ = Piece(Piece::randomType());

    if (!board_.isValidPosition(current_)) gameOver_ = true;
}

bool Game::tryMove(int dx, int dy) {
    Piece moved = current_;
    moved.move(dx, dy);
    if (!board_.isValidPosition(moved)) return false;
    current_ = moved;
    return true;
}

void Game::tryRotate() {
    Piece rotated = current_;
    rotated.rotateClockwise();

    for (int shift : {0, -1, 1, -2, 2}) {
        Piece candidate = rotated;
        candidate.move(shift, 0);
        if (board_.isValidPosition(candidate)) {
            current_ = candidate;
            return;
        }
    }
}

void Game::hardDrop() {
    int distance = 0;
    while (tryMove(0, 1)) ++distance;
    score_ += distance * 2;  
    lockCurrentPiece();
}

void Game::lockCurrentPiece() {
    board_.lockPiece(current_);

    int cleared = board_.clearFullLines();
    if (cleared > 0) {
        static const int POINTS[5] = {0, 100, 300, 500, 800};
        score_ += POINTS[cleared] * level_;
        lines_ += cleared;
        level_ = lines_ / 10 + 1;  
    }
    spawnPiece();
}
