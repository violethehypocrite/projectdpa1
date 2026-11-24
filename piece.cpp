
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