#include<bits/stdc++.h>
using namespace std;

enum PieceType {
    KING, 
    QUEEN, 
    KNIGHT,
    BISHOP,
    ROOK,
    PAWN
}

class Piece {
protected:
    Color color;
    PieceType ptype;
    bool hasMoved;
public:
    Piece(Color col, PieceType pt) : color(col), ptype(pt) {
        hasMoved = false;
    }

    virtual vector<Position*> getPossibleMoves(Position* currPos, Board* b) = 0;
    virtual string getSymbol() = 0;
};

class King : public Piece {
public:
    King(Color col) : Piece(col, KING) {}
    
}

class Queen : public Piece {
public:
    Queen(Color col) : Piece(col, QUEEN) {}
    
}

class Rook : public Piece {
public:
    Rook(Color col) : Piece(col, ROOK) {}
    
}

class Knight : public Piece {
public:
    Knight(Color col) : Piece(col, KNIGHT) {}
    
}

class Bishop : public Piece {
public:
    Bishop(Color col) : Piece(col, BISHOP) {}
    
}

class Pawn : public Piece {
public:
    Pawn(Color col) : Piece(col, PAWN) {}
    
}

class Board {
private:
    Piece* board[8][8];
    map<Position*, Piece> piecePos;
public:

};

class ChatMediator {
public:
    virtual void send(const string &msg, User* user) = 0;
    virtual void addUser(User* user) = 0;
    virtual void removeUser(User* user) = 0;
};

class Match : public ChatMediator {
private:
    string id;
    User white, black;
    Board* board;
    ChessRules* rule;
    GameStatus status;
    vector<Message*> messageHistory;
    vector<Move*> moveHistory;
public:

};