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

void Game::tick() {
    if (tryMove(0, 1)) lockTicks_ = 0;
    else if (++lockTicks_ >= 2) lockCurrentPiece();  
}

int Game::dropIntervalMs() const {
    return std::max(100, 800 - (level_ - 1) * 70);
}

void Game::draw() const {
    erase();  

    if (getmaxy(stdscr) < MIN_ROWS || getmaxx(stdscr) < MIN_COLS) {
        mvprintw(0, 0, "Terminal terlalu kecil.");
        mvprintw(1, 0, "Perbesar minimal %d x %d.", MIN_COLS, MIN_ROWS);
        refresh();
        return;
    }

    int view[Board::HEIGHT][Board::WIDTH];
    for (int r = 0; r < Board::HEIGHT; ++r)
        for (int c = 0; c < Board::WIDTH; ++c)
            view[r][c] = board_.cell(r, c);

    
for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!current_.isBlock(r, c)) continue;
            int gx = current_.x() + c;
            int gy = current_.y() + r;
            if (gx >= 0 && gx < Board::WIDTH && gy >= 0 && gy < Board::HEIGHT)
                view[gy][gx] = current_.colorId();
        }
    }

    mvaddstr(0, 2, "===== TETRIS =====");

    
    const int right = BOARD_LEFT + 2 * Board::WIDTH + 1;
    const int bottom = BOARD_TOP + Board::HEIGHT + 1;
    mvaddch(BOARD_TOP, BOARD_LEFT, ACS_ULCORNER);
    mvaddch(BOARD_TOP, right, ACS_URCORNER);
    mvaddch(bottom, BOARD_LEFT, ACS_LLCORNER);
    mvaddch(bottom, right, ACS_LRCORNER);
    for (int x = BOARD_LEFT + 1; x < right; ++x) {
        mvaddch(BOARD_TOP, x, ACS_HLINE);
        mvaddch(bottom, x, ACS_HLINE);
    }
    for (int y = BOARD_TOP + 1; y < bottom; ++y) {
        mvaddch(y, BOARD_LEFT, ACS_VLINE);
        mvaddch(y, right, ACS_VLINE);
    }

    for (int r = 0; r < Board::HEIGHT; ++r)
        for (int c = 0; c < Board::WIDTH; ++c)
            drawCell(BOARD_TOP + 1 + r, BOARD_LEFT + 1 + 2 * c, view[r][c]);

    const int y0 = BOARD_TOP + 1;
    mvprintw(y0 + 1, INFO_X, "Skor     : %d", score_);
    mvprintw(y0 + 2, INFO_X, "Garis    : %d", lines_);
    mvprintw(y0 + 3, INFO_X, "Level    : %d", level_);

    mvaddstr(y0 + 5, INFO_X, "Berikutnya:");
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (next_.isBlock(r, c))
                drawCell(y0 + 6 + r, INFO_X + 2 + 2 * c, next_.colorId());

    mvaddstr(y0 + 11, INFO_X, "Kontrol:");
    mvaddstr(y0 + 12, INFO_X, " A / <-  : kiri");
    mvaddstr(y0 + 13, INFO_X, " D / ->  : kanan");
    mvaddstr(y0 + 14, INFO_X, " W / ^   : putar");
    mvaddstr(y0 + 15, INFO_X, " S / v   : turun");
    mvaddstr(y0 + 16, INFO_X, " Spasi   : jatuh langsung");
    mvaddstr(y0 + 17, INFO_X, " P       : jeda");
    mvaddstr(y0 + 18, INFO_X, " Q       : keluar");

    if (gameOver_) {
        attron(A_BOLD);
        mvprintw(bottom + 1, 2, "*** GAME OVER ***  Skor akhir: %d", score_);
        attroff(A_BOLD);
    } else if (paused_) {
        attron(A_BOLD);
        mvaddstr(bottom + 1, 2, "--- JEDA (tekan P untuk lanjut) ---");
        attroff(A_BOLD);
    }

    refresh();
}

void Game::run() {
    using Clock = std::chrono::steady_clock;

    CursesGuard curses;  

    auto lastTick = Clock::now();
    draw();

    while (!quit_ && !gameOver_) {
        bool needRedraw = false;

        int ch = getch();
        if (ch != ERR) {
            needRedraw = true;

            switch (ch) {
                case 'q': case 'Q':
                    quit_ = true;
                    break;

                case 'p': case 'P':
                    paused_ = !paused_;
                    lastTick = Clock::now();
                    break;

                case KEY_RESIZE:  
                    break;

                default:
                    if (paused_) break;  
                    switch (ch) {
                        case 'a': case 'A': case KEY_LEFT:
                            tryMove(-1, 0);
                            break;
                        case 'd': case 'D': case KEY_RIGHT:
                            tryMove(1, 0);
                            break;
                        case 'w': case 'W': case KEY_UP:
                            tryRotate();
                            break;
                        case 's': case 'S': case KEY_DOWN:
                            if (tryMove(0, 1)) ++score_;  
                            lastTick = Clock::now();
                            break;
                        case ' ':
                            hardDrop();
                            lastTick = Clock::now();
                            break;
                        default:
                            needRedraw = false;  
                            break;
                    }
                    break;
            }
        }


        if (!paused_ && !gameOver_ && !quit_) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                Clock::now() - lastTick).count();
            if (elapsed >= dropIntervalMs()) {
                tick();
                lastTick = Clock::now();
                needRedraw = true;
            }
        }

        if (needRedraw) draw();
    }

    if (gameOver_) {
        draw();
        mvaddstr(BOARD_TOP + Board::HEIGHT + 3, 2,
                 "Tekan tombol apa saja untuk lanjut...");
        refresh();
        flushinp();   
        timeout(-1);  
        getch();
    }
}
