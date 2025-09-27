#include<bits/stdc++.h>
using namespace std;

const char PLAYER1 = 'X';
const char PLAYER2 = 'O';

class DSU {
private:
    vector<int> parent, size;
public:
    DSU(int n) {
        parent.resize(n);
        size.resize(n, 1);
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);
        return parent[x];
    }

    void unite(int x, int y) {
        int px = find(x);
        int py = find(y);
        if (px == py) return;

        if (size[px] < size[py]) swap(px, py);
        parent[py] = px;
        size[px] += size[py];
    }
    
    int getSize(int x) {
        return size[find(x)];
    }
};

class Game {
private:
    int n;
    vector<vector<char>> board;
    DSU dsu;
    unordered_map<int, int> componentOwner; // DSU root -> owner (1 or 2)
    int turn; // 1 for Player1, 2 for Player2
    int player1Territories = 0, player2Territories = 0;

    int toId(int r, int c) {
        return r * n + c;
    }

    void displayBoard() {
        cout << "\n\n  ";
        for (int i = 0; i < n; i++) cout << setw(3) << i;
        cout << "\n";
        for (int i = 0; i < n; i++) {
            cout << setw(2) << i << " ";
            for (int j = 0; j < n; j++) {
                char sym = board[i][j];
                if (sym == PLAYER1) cout << "\033[1;34m" << sym << " \033[0m"; // Blue
                else if (sym == PLAYER2) cout << "\033[1;31m" << sym << " \033[0m"; // Red
                else cout << ". ";
            }
            cout << "\n";
        }
    }

    void checkAndMerge(int r, int c, char playerChar) {
        vector<pair<int, int>> dirs = {{0,1},{1,0},{0,-1},{-1,0}};
        int id = toId(r, c);
        int currentOwner = turn;
        componentOwner[dsu.find(id)] = currentOwner;

        // Check all adjacent cells
        for (auto dir : dirs) {
            int nr = r + dir.first, nc = c + dir.second;
            if (nr < 0 || nr >= n || nc < 0 || nc >= n) continue;
            if (board[nr][nc] == '.') continue;

            int neighborId = toId(nr, nc);
            int rootA = dsu.find(id);
            int rootB = dsu.find(neighborId);

            if (rootA == rootB) continue; // Already in same component

            int ownerA = componentOwner[rootA];
            int ownerB = componentOwner[rootB];

            // Get sizes before union
            int sizeA = dsu.getSize(rootA);
            int sizeB = dsu.getSize(rootB);

            // Unite the components
            dsu.unite(id, neighborId);
            int newRoot = dsu.find(id);

            // Determine new owner based on Island capture rules
            if (ownerA == ownerB) {
                // Same owner - just merge normally
                componentOwner[newRoot] = ownerA;
            } else {
                // Different owners - larger Island captures smaller
                if (sizeA >= sizeB) {
                    componentOwner[newRoot] = ownerA;
                    cout << "\n Player " << ownerA << " captures Player " << ownerB << "'s Island! (Size " << sizeA << " vs " << sizeB << ")\n";
                } else {
                    componentOwner[newRoot] = ownerB;
                    cout << "\n Player " << ownerB << " captures Player " << ownerA << "'s Island! (Size " << sizeB << " vs " << sizeA << ")\n";
                }
            }
        }
    }

    void updateBoardVisuals() {
        // Update board display based on component ownership
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (board[i][j] == '.') continue;
                
                int root = dsu.find(toId(i, j));
                int owner = componentOwner[root];
                
                if (owner == 1) {
                    board[i][j] = PLAYER1;
                } else if (owner == 2) {
                    board[i][j] = PLAYER2;
                }
            }
        }
    }

    void claimCell(int r, int c) {
        if (board[r][c] != '.') {
            cout << "Cell already taken. Choose another.\n";
            return;
        }
        
        char playerChar = (turn == 1 ? PLAYER1 : PLAYER2);
        board[r][c] = playerChar;
        
        checkAndMerge(r, c, playerChar);
        updateBoardVisuals();
        
        turn = 3 - turn; // Switch player
    }

    void countTerritories() {
        player1Territories = 0;
        player2Territories = 0;
        unordered_map<int, bool> visited;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (board[i][j] == '.') continue;
                
                int root = dsu.find(toId(i, j));
                if (visited[root]) continue;
                
                visited[root] = true;
                int owner = componentOwner[root];
                
                if (owner == 1) player1Territories++;
                else if (owner == 2) player2Territories++;
            }
        }
    }

    void displayGameStats() {
        countTerritories();
        
        // Count total cells owned by each player
        int player1Cells = 0, player2Cells = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == PLAYER1) player1Cells++;
                else if (board[i][j] == PLAYER2) player2Cells++;
            }
        }
        
        cout << "\n Game Statistics\n";
        cout << "Player 1 (X): " << player1Territories << " islands, " << player1Cells << " cells\n";
        cout << "Player 2 (O): " << player2Territories << " islands, " << player2Cells << " cells\n";
    }

public:
    Game(int size) : n(size), dsu(size*size), turn(1) {
        board.assign(n, vector<char>(n, '.'));
    }

    bool isBoardFull() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == '.') return false;
            }
        }
        return true;
    }

public:
    void play() {
        cout << "\n Welcome to Island Conquest! \n";
        cout << "Rules:\n";
        cout << "- Players take turns claiming cells\n";
        cout << "- Adjacent cells of the same player form islands\n";
        cout << "- When different players' islands touch, the larger one captures the smaller!\n";
        cout << "- Goal: Have the most islands at the end\n\n";
        
        while (!isBoardFull()) {
            displayBoard();
            displayGameStats();
            
            cout << "\nPlayer " << (turn == 1 ? "1 (X)" : "2 (O)") << ", enter coordinates to claim (row col): ";
            int r, c;
            
            if (!(cin >> r >> c)) {
                cout << "Invalid input. Please enter two numbers.\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            
            if (r < 0 || r >= n || c < 0 || c >= n) {
                cout << "Invalid coordinates. Row and column must be between 0 and " << (n-1) << ".\n";
                continue;
            }
            
            if (board[r][c] != '.') {
                cout << "Cell already taken. Choose another.\n";
                continue;
            }
            
            claimCell(r, c);
        }

        displayBoard();
        displayGameStats();

        cout << "\n GAME OVER!\n";
        
        if (player1Territories > player2Territories) {
            cout << " Player 1 (X) Wins with " << player1Territories << " islands! \n";
        } else if (player2Territories > player1Territories) {
            cout << "Player 2 (O) Wins with " << player2Territories << " islands! \n";
        } else {
            cout << " It's a Tie! Both players have " << player1Territories << " islands! \n";
        }
    }
};

int main() {
    srand(time(0));
    int size;
    cout << "\n3[=========== Island Conquest Game ===========\n";
    cout << "Enter grid size (n x n): ";
    cin >> size;
    
    if (size < 2 || size > 20) {
        cout << "Grid size should be between 2 and 20. Setting to 5.\n";
        size = 5;
    }
    
    Game game(size);
    game.play();
    return 0;
}