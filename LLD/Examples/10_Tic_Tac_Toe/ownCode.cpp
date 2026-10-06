#include<bits/stdc++.h>
using namespace std;

class Symbol {
private:
    char ch;
public:
    Symbol(char ch) : ch(ch) {}
    char getCh() const { return ch; }
    void setCh(char ch_) { ch = ch_; }
};

class Board {
private:
    vector<vector<Symbol*>> grid;
    Symbol* emptyCell;
    int size;
    bool check(int row, int col, int n){
        return row>=0 and col>=0 and row<n and col<n;
    }
public:
    Board(int n){
        size = n;
        grid.resize(n);

        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                grid[i].push_back(new Symbol('.'));
            }
        }
    }

    ~Board() {
        for(auto &row: grid) {
            for(auto &col: row) {
                delete col;
            }
        }
    }

    bool isEmptyCell(int row, int col) {
        if(check(row, col, size))
            return grid[row][col]->getCh() == '.';
        return false;
    }

    bool placeMark(int row, int col, Symbol* mark) {
        if(check(row, col, size) and isEmptyCell(row, col)) {
            grid[row][col]->setCh(mark->getCh());
            return true;
        }
        return false;
    }

    char getCellSymbol(int row, int col) {
        if(check(row, col, size))
            return grid[row][col]->getCh();
        return '#';
    }

    void display() {
        cout << "\n  ";
        for(int i = 0; i < size; i++) {
            cout << i << " ";
        }
        cout << endl;
        
        for(int i = 0; i < size; i++) {
            cout << i << " ";
            for(int j = 0; j < size; j++) {
                cout << grid[i][j]->getCh() << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    vector<vector<Symbol*>> getGrid() const { return grid; }
};

class Rule {
public:
    virtual bool isValidMove(Board* b, int row, int col) = 0;
    virtual bool checkWin(Board* b, Symbol* s) = 0;
    virtual bool checkDraw(Board* b) = 0;
};

class StandardGame: public Rule {
public:
    bool isValidMove(Board* b, int row, int col) override {
        return b->isEmptyCell(row, col);
    }

    bool checkWin(Board* b, Symbol* s) {
        auto grid = b->getGrid();
        int n = grid.size();
        
        // row check;
        for(int i=0; i<n; i++){
            bool rowWin = true;
            for(int j=0; j<n; j++){
                auto curr = grid[i][j];
                if(curr->getCh() != s->getCh()){
                    rowWin = false;
                    break;
                }
            }
            if(rowWin)
                return true;
        }

        // col check;
        for(int i=0; i<n; i++){
            bool colWin = true;
            for(int j=0; j<n; j++){
                auto curr = grid[j][i];
                if(curr->getCh() != s->getCh()){
                    colWin = false;
                    break;
                }
            }
            if(colWin)
                return true;
        }    
        
        // Daigonal win
        bool daignoalWin = true, antiWin = true;
        for(int i=0; i<n; i++) {
            if(grid[i][i]->getCh() != s->getCh()){
                daignoalWin = false;
            }
            if(grid[i][n-1-i]->getCh() != s->getCh()) {
                antiWin = false;
            }
        }

        if(daignoalWin or antiWin) return true;
        return false;
    }

    bool checkDraw(Board* b) {
        auto grid = b->getGrid();
        int n = grid.size();
        
        for(int i=0; i<n; i++) {
            for(int j=0; j<n; j++) {
                if(grid[i][j]->getCh() == '.')
                    return false;
            }
        }

        return true;
    }
};

class Player {
private:
    int id;
    string name;
    Symbol *s;
    int score;
public:
    Player(int id, const string& name, Symbol* sym) {
        this->id = id;
        this->name = name;
        this->s = sym;
        score = 0;
    }

    string getName() const { return name; }
    Symbol *getS() const { return s; }
    int getScore() const { return score; }

    void incrScore() {
        this->score ++;
    }

    ~Player() {
        delete s;
    }
};

class Iobserver{
public:
    virtual void update(const string& msg) = 0;
};

class ConsoleNotifier : public Iobserver {
public:
    void update(const string& msg) override {
        cout << "[Notification] " << msg << endl;
    }
};

class Game{
private:
    Board* b;
    Rule* rule;
    deque<Player*> turn;
    vector<Iobserver*> obs;
    bool gameOver = false;
public:
    Game(int boardSize) {
        b = new Board(boardSize);
        rule = new StandardGame();
        gameOver = false;
    }

    void addPlayer(Player* player) {
        turn.push_back(player);
    }

    void addObserver(Iobserver* ob){
        obs.push_back(ob);
    }

    void notify(const string& msg) {
        for(auto &ob: obs) {
            ob->update(msg);
        }
    }

    void play() {
        while(!gameOver) {
            b->display();
            auto &currPlayer = turn.front();
            cout << currPlayer->getName() << " (" << currPlayer->getS()->getCh() << ") - Enter row and column: ";
            
            int row, col;
            cin >> row >> col;
            if(rule->isValidMove(b, row, col)) {
                b->placeMark(row, col, currPlayer->getS());
                notify(currPlayer->getName() + " played (" + to_string(row) + "," + to_string(col) + ")");
                
                if(rule->checkWin(b, currPlayer->getS())){
                    b->display();
                    cout << currPlayer->getName() << " wins!" << endl;
                    currPlayer->incrScore();

                    notify(currPlayer->getName() + " wins!");
                    gameOver = true;
                }
                else if(rule->checkDraw(b)) {
                    b->display();
                    
                    cout << "It's a draw!" << endl;
                    notify("Game is Draw!");

                    gameOver = true;
                }

                else {
                    // Move player to back of queue
                    turn.pop_front();
                    turn.push_back(currPlayer);
                }
            }
            else{
                cout<<"Try Again, Invalid Move"<<endl;
            }
        }
    }

    ~Game() {
        delete b;
        delete rule;
        while(!turn.empty()){
            auto& f = turn.front();
            delete f;
            turn.pop_front();
        }

        for(auto &it: obs){
            delete it;
        }
    }
};

enum GameType {
    STANDARD
};

class TicTacToeGameFactory {
public:
    static Game* createGame(GameType gt, int boardSize) {
        if(GameType::STANDARD == gt) {
            return new Game(boardSize);
        }
        return nullptr;
    }
};

int main() {
    int boardSize;
    cout << "Enter board size (e.g., 3 for 3x3): ";
    cin >> boardSize;
    Game* game = TicTacToeGameFactory::createGame(GameType::STANDARD, boardSize);

    Iobserver* notifier = new ConsoleNotifier();
    game->addObserver(notifier);
    
    Player* tej = new Player(1, "Tejas", new Symbol('#'));
    Player* darshan = new Player(2, "Darshan", new Symbol('O'));

    game->addPlayer(tej);
    game->addPlayer(darshan);

    game->play();

    delete game;
}