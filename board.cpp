#include "piece.cpp"

class Board {
public:
static constexpr int WIDTH = 10;
    static constexpr int HEIGHT = 20;
    Board();

void clear();

bool isValidPosition(const Piece& piece) const;

void lockPiece(const Piece& piece);
int clearFullLines();
int cell(int row, int col) const { return grid_[row][col]; }

private:
int grid_[HEIGHT][WIDTH];
};

Board::Board() {
    clear();
}

void Board::clear() {
    for (int r = 0; r < HEIGHT; ++r)
        for (int c = 0; c < WIDTH; ++c)
            grid_[r][c] = 0;
}

bool Board::isValidPosition(const Piece& piece) const {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!piece.isBlock(r, c)) continue;

int gx = piece.x() + c;
int gy = piece.y() + r;

if (gx < 0 || gx >= WIDTH || gy >= HEIGHT) return false;
if (gy >= 0 && grid_[gy][gx] != 0) return false;
}
    }
    return true;
}
void Board::lockPiece(const Piece& piece) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!piece.isBlock(r, c)) continue;

            int gx = piece.x() + c;
            int gy = piece.y() + r;
            if (gx >= 0 && gx < WIDTH && gy >= 0 && gy < HEIGHT)
                grid_[gy][gx] = piece.colorId();
        }
    }
}

int Board::clearFullLines() {
    int cleared = 0;

    // Periksa dari baris paling bawah ke atas
    for (int r = HEIGHT - 1; r >= 0; ) {
        bool full = true;
        for (int c = 0; c < WIDTH; ++c) {
            if (grid_[r][c] == 0) { full = false; break; }
        }

        if (!full) {
            --r;
            continue;
        }
