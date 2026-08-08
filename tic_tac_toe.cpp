/*
    Tic-Tac-Toe (Console Mini Game) - C++
    ======================================
    Demonstrates loops, arrays, and conditional logic through a classic
    2-player Tic-Tac-Toe game with an optional simple computer opponent.

    Features:
        - 3x3 board stored as a 2D char array
        - Dynamic board redraw after every move
        - Input validation (rejects out-of-range / already-taken cells)
        - Win detection (rows, columns, diagonals) and draw detection
        - Choice of Player vs Player or Player vs Computer
        - Simple computer AI: win if possible, block if needed, else
          take center/corner/random
        - Replay option with a running score tally across rounds

    Compile:
        g++ -std=c++17 -O2 -o tictactoe tic_tac_toe.cpp
    Run:
        ./tictactoe
*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

const int SIZE = 3;

// ---------------------------------------------------------------------------
// Board utilities
// ---------------------------------------------------------------------------

// Initialize the board with '1'..'9' as placeholders (so players can see
// which number to type for each cell).
void initBoard(char board[SIZE][SIZE]) {
    int num = 1;
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            board[r][c] = char('0' + num);
            num++;
        }
    }
}

void printBoard(const char board[SIZE][SIZE]) {
    cout << "\n";
    for (int r = 0; r < SIZE; r++) {
        cout << " ";
        for (int c = 0; c < SIZE; c++) {
            cout << board[r][c];
            if (c < SIZE - 1) cout << " | ";
        }
        cout << "\n";
        if (r < SIZE - 1) cout << "---+---+---\n";
    }
    cout << "\n";
}

// Converts a 1-9 cell number into row/col indices.
bool cellToRowCol(int cellNum, int& row, int& col) {
    if (cellNum < 1 || cellNum > SIZE * SIZE) return false;
    row = (cellNum - 1) / SIZE;
    col = (cellNum - 1) % SIZE;
    return true;
}

bool isCellFree(const char board[SIZE][SIZE], int row, int col) {
    char c = board[row][col];
    return c != 'X' && c != 'O';
}

bool boardFull(const char board[SIZE][SIZE]) {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (isCellFree(board, r, c)) return false;
    return true;
}

// Returns 'X' or 'O' if that player has won, or '\0' if no winner yet.
char checkWinner(const char board[SIZE][SIZE]) {
    // Rows and columns
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] &&
            (board[i][0] == 'X' || board[i][0] == 'O'))
            return board[i][0];
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] &&
            (board[0][i] == 'X' || board[0][i] == 'O'))
            return board[0][i];
    }
    // Diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] &&
        (board[0][0] == 'X' || board[0][0] == 'O'))
        return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] &&
        (board[0][2] == 'X' || board[0][2] == 'O'))
        return board[0][2];

    return '\0';
}

// ---------------------------------------------------------------------------
// Input handling
// ---------------------------------------------------------------------------

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) return value;
        if (cin.eof()) {
            cout << "\nInput closed. Exiting.\n";
            exit(0);
        }
        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Prompts the human player for a move and applies it to the board.
void humanMove(char board[SIZE][SIZE], char symbol) {
    while (true) {
        int cellNum = readInt("Player " + string(1, symbol) +
                               ", choose a cell (1-9): ");
        int row, col;
        if (!cellToRowCol(cellNum, row, col)) {
            cout << "Invalid cell number, choose between 1 and 9.\n";
            continue;
        }
        if (!isCellFree(board, row, col)) {
            cout << "That cell is already taken. Try again.\n";
            continue;
        }
        board[row][col] = symbol;
        return;
    }
}

// ---------------------------------------------------------------------------
// Simple computer AI
// ---------------------------------------------------------------------------

// Searches for a cell where placing `symbol` would immediately win.
// Tests each free cell without leaving any permanent change to the board.
// Returns true and sets row/col if such a cell exists.
bool findWinningMove(char board[SIZE][SIZE], char symbol, int& outRow, int& outCol) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (!isCellFree(board, r, c)) continue;
            char original = board[r][c];
            board[r][c] = symbol;
            bool wins = (checkWinner(board) == symbol);
            board[r][c] = original; // always revert - this is just a test
            if (wins) {
                outRow = r;
                outCol = c;
                return true;
            }
        }
    }
    return false;
}

void computerMove(char board[SIZE][SIZE], char computerSymbol, char humanSymbol) {
    int row, col;

    // 1. Win if possible
    if (findWinningMove(board, computerSymbol, row, col)) {
        board[row][col] = computerSymbol;
        return;
    }

    // 2. Block the human's winning move
    if (findWinningMove(board, humanSymbol, row, col)) {
        board[row][col] = computerSymbol;
        return;
    }

    // 3. Take the center if free
    if (isCellFree(board, 1, 1)) {
        board[1][1] = computerSymbol;
        return;
    }

    // 4. Take a free corner
    int corners[4][2] = {{0, 0}, {0, 2}, {2, 0}, {2, 2}};
    for (auto& corner : corners) {
        if (isCellFree(board, corner[0], corner[1])) {
            board[corner[0]][corner[1]] = computerSymbol;
            return;
        }
    }

    // 5. Otherwise take the first free cell
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (isCellFree(board, r, c)) {
                board[r][c] = computerSymbol;
                return;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Game loop
// ---------------------------------------------------------------------------

// Plays a single round. Returns 'X', 'O' for a winner, or 'D' for a draw.
char playRound(bool vsComputer) {
    char board[SIZE][SIZE];
    initBoard(board);

    char currentSymbol = 'X'; // X always starts
    printBoard(board);

    while (true) {
        bool computersTurn = vsComputer && currentSymbol == 'O';

        if (computersTurn) {
            cout << "Computer (O) is making a move...\n";
            computerMove(board, 'O', 'X');
        } else {
            humanMove(board, currentSymbol);
        }

        printBoard(board);

        char winner = checkWinner(board);
        if (winner != '\0') {
            cout << "*** Player " << winner << " wins! ***\n";
            return winner;
        }
        if (boardFull(board)) {
            cout << "*** It's a draw! ***\n";
            return 'D';
        }

        currentSymbol = (currentSymbol == 'X') ? 'O' : 'X';
    }
}

bool askYesNo(const string& prompt) {
    while (true) {
        cout << prompt << " (y/n): ";
        string answer;
        if (!(cin >> answer)) {
            cout << "\nInput closed. Exiting.\n";
            exit(0);
        }
        if (!answer.empty() && (answer[0] == 'y' || answer[0] == 'Y')) return true;
        if (!answer.empty() && (answer[0] == 'n' || answer[0] == 'N')) return false;
        cout << "Please answer y or n.\n";
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    cout << "=========================================\n";
    cout << "           TIC-TAC-TOE\n";
    cout << "=========================================\n";
    cout << "Cells are numbered 1-9, left to right, top to bottom:\n";
    cout << " 1 | 2 | 3\n---+---+---\n 4 | 5 | 6\n---+---+---\n 7 | 8 | 9\n";

    int scoreX = 0, scoreO = 0, scoreDraw = 0;

    do {
        bool vsComputer = askYesNo("\nPlay against the computer?");
        char result = playRound(vsComputer);

        if (result == 'X') scoreX++;
        else if (result == 'O') scoreO++;
        else scoreDraw++;

        cout << "\n--- Score ---\n";
        cout << "Player X: " << scoreX << "\n";
        cout << "Player O" << (vsComputer ? " (Computer)" : "") << ": " << scoreO << "\n";
        cout << "Draws: " << scoreDraw << "\n";

    } while (askYesNo("\nPlay again?"));

    cout << "\nThanks for playing! Final score - X: " << scoreX
         << " | O: " << scoreO << " | Draws: " << scoreDraw << "\n";

    return 0;
}