
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