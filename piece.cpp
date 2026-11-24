
enum class PieceType { I = 0, O, T, S, Z, J, L };

class Piece {
public:
    explicit Piece(PieceType type = PieceType::I);

    PieceType type() const { return type_; }
    int x() const { return x_; }
    int y() const { return y_; }
    int rotation() const { return rotation_; }

    int colorId() const { return static_cast<int>(type_) + 1; }

    void setPosition(int x, int y);
    void move(int dx, int dy);
    void rotateClockwise();
    void rotateCounterClockwise();

    bool isBlock(int row, int col) const;

    static PieceType randomType();

private:
    PieceType type_;
    int x_;         
    int y_;         
    int rotation_;  
};

#include <algorithm>
#include <random>
#include <vector>

namespace {
const char* const SHAPES[7][4] = {
    // I
    {"...." "XXXX" "...." "....",
     "..X." "..X." "..X." "..X.",
     "...." "...." "XXXX" "....",
     ".X.." ".X.." ".X.." ".X.."},
    // O
    {"...." ".XX." ".XX." "....",
     "...." ".XX." ".XX." "....",
     "...." ".XX." ".XX." "....",
     "...." ".XX." ".XX." "...."},
    // T
    {".X.." "XXX." "...." "....",
     ".X.." ".XX." ".X.." "....",
     "...." "XXX." ".X.." "....",
     ".X.." "XX.." ".X.." "...."},
    // S
    {".XX." "XX.." "...." "....",
     ".X.." ".XX." "..X." "....",
     "...." ".XX." "XX.." "....",
     "X..." "XX.." ".X.." "...."},
    // Z
    {"XX.." ".XX." "...." "....",
     "..X." ".XX." ".X.." "....",
     "...." "XX.." ".XX." "....",
     ".X.." "XX.." "X..." "...."},
    // J
    {"X..." "XXX." "...." "....",
     ".XX." ".X.." ".X.." "....",
     "...." "XXX." "..X." "....",
     ".X.." ".X.." "XX.." "...."},
    // L
    {"..X." "XXX." "...." "....",
     ".X.." ".X.." ".XX." "....",
     "...." "XXX." "X..." "....",
     "XX.." ".X.." ".X.." "...."},
};

}

Piece::Piece(PieceType type)
    : type_(type), x_(0), y_(0), rotation_(0) {}

void Piece::setPosition(int x, int y) {
    x_ = x;
    y_ = y;
}

void Piece::move(int dx, int dy) {
    x_ += dx;
    y_ += dy;
}

void Piece::rotateClockwise() {
    rotation_ = (rotation_ + 1) % 4;
}

void Piece::rotateCounterClockwise() {
    rotation_ = (rotation_ + 3) % 4;
}

bool Piece::isBlock(int row, int col) const {
    if (row < 0 || row >= 4 || col < 0 || col >= 4) return false;
    return SHAPES[static_cast<int>(type_)][rotation_][row * 4 + col] == 'X';
}

PieceType Piece::randomType() {
    static std::mt19937 rng{std::random_device{}()};
    static std::vector<int> bag;

    if (bag.empty()) {
        for (int i = 0; i < 7; ++i) bag.push_back(i);
        std::shuffle(bag.begin(), bag.end(), rng);
    }
    int value = bag.back();
    bag.pop_back();
    return static_cast<PieceType>(value);
}
