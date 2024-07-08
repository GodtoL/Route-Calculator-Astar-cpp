#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <Windows.h>

// Clase para crear el mapa
class MapBoard {
public:
    MapBoard(const int boardHeight, const int boardWidth);
    void printBoard();
    void addObstaclesToBoard();
    void addObstaclesUser();
    void delObstaclesUser();
    const std::vector<std::vector<int>>& getBoard() const { return board; }
    void updateBoard(const std::vector<std::vector<int>>& newBoard);
    void isAccessible(int x , int y){std::cout << ((board[y][x] == 0) ? "Si es accesible" : "No es accesible");}

private:
    const int boardHeight;
    const int boardWidth;
    std::vector<std::vector<int>> board;
    bool validCoodinates(int x, int y) { return x < boardWidth && x >= 0 && y < boardHeight && y >= 0; }
};

MapBoard::MapBoard(const int boardHeight, const int boardWidth) : boardHeight(boardHeight), boardWidth(boardWidth) {
    board.resize(boardHeight, std::vector<int>(boardWidth, 0));
    srand(time(nullptr));
}

void MapBoard::addObstaclesToBoard() {
    int amountObstacles;
    int amountSug = (boardWidth * boardHeight) / 2;
    std::cout << "Ingrese la cantidad de obstáculos iniciales que desea (recomendable " << amountSug << " como máximo)\n";
    std::cin >> amountObstacles;
    for (int i = 1; i <= amountObstacles; i++) {
        int xrand = rand() % boardWidth;
        int yrand = rand() % boardHeight;
        board[yrand][xrand] = 1;
    }
}

void MapBoard::printBoard() {
    std::cout << "  ";
    for (int i = 0; i < boardWidth; ++i) {
        std::cout << i << " ";
    }
    std::cout << "\n";
    for (int y = 0; y < boardHeight; ++y) {
        std::cout << y << " ";
        for (int x = 0; x < boardWidth; ++x) {
            if (board[y][x] == 0)
                std::cout << ". ";
            else if (board[y][x] == 1)
                std::cout << "# ";
            else if (board[y][x] == 2)
                std::cout << "* ";
            else if (board[y][x] == 5)
                std::cout << "I ";
            else if (board[y][x] == 6)
                std::cout << "F ";
        }
        std::cout << '\n';
    }
}

void MapBoard::addObstaclesUser() {
    int x, y;
    while (true) {
        std::cout << "Ingrese coordenadas para los obstáculos (o -1 -1 para finalizar): ";
        std::cin >> x >> y;
        if (x == -1 && y == -1) break;
        if (validCoodinates(x, y)) {
            board[y][x] = 1;
        } else {
            std::cout << "Las coordenadas " << x << "," << y << " no son válidas\n";
        }
    }
}

void MapBoard::delObstaclesUser() {
    int x, y;
    while (true) {
        std::cout << "Ingrese coordenadas que desee eliminar obstáculos (o -1 -1 para finalizar): ";
        std::cin >> x >> y;
        if (x == -1 && y == -1) break;
        if (validCoodinates(x, y)) {
            board[y][x] = 0;
        } else {
            std::cout << "Las coordenadas " << x << "," << y << " no son válidas\n";
        }
    }
}

void MapBoard::updateBoard(const std::vector<std::vector<int>>& newBoard) {
    board = newBoard;
}

class AStar {
public:
    AStar(std::vector<std::vector<int>> map) : board(map), numRows(map.size()), numCols(map[0].size()) {}
    const std::vector<std::vector<int>>& getSolvedBoard() const { return board; }
    bool searchRoute();
    void getInitialCoords();
    void isAccessible(int x , int y){std::cout << ((board[y][x] == 0) ? "Si es accesible\n" : "No es accesible\n");}

private:
    std::vector<std::vector<int>> board;
    int numRows, numCols;
    int startX, startY, endX, endY;
    bool validCoordinates(int x, int y) { return x < numCols && x >= 0 && y < numRows && y >= 0; }

    struct Node {
        int x, y;
        int g, h, f;
        Node* parent;

        Node(int x, int y, int g, int h, Node* parent = nullptr)
            : x(x), y(y), g(g), h(h), f(g + h), parent(parent) {}
    };

    int heuristic(int x1, int y1, int x2, int y2) { return abs(x1 - x2) + abs(y1 - y2); }
    std::vector<Node*> getNeighbors(Node* current, int endX, int endY, const std::vector<std::vector<int>>& board);
    void reconstructPath(Node* endNode, std::vector<std::vector<int>>& board);
};

void AStar::getInitialCoords() {
    while (true) {
        std::cout << "\nIngrese las coordenadas de inicio: ";
        std::cin >> startX >> startY;
        std::cout << "Ingrese las coordenadas del final: ";
        std::cin >> endX >> endY;

        if (validCoordinates(startX, startY) && validCoordinates(endX, endY)) {
            break;
        } else {
            std::cout << "\nCoordenadas fuera de rango o en un obstáculo. Vuelva a intentarlo.\n";
        }
    }
}

bool AStar::searchRoute() {
    auto cmp = [](Node* left, Node* right) { return left->f > right->f; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> openSet(cmp);
    std::unordered_set<int> closedSet;

    Node* startNode = new Node(startX, startY, 0, heuristic(startX, startY, endX, endY));
    openSet.push(startNode);

    while (!openSet.empty()) {
        Node* current = openSet.top();
        openSet.pop();

        if (current->x == endX && current->y == endY) {
            reconstructPath(current, board);
            return true;
        }

        int currentHash = current->y * numCols + current->x;
        closedSet.insert(currentHash);

        for (Node* neighbor : getNeighbors(current, endX, endY, board)) {
            int neighborHash = neighbor->y * numCols + neighbor->x;
            if (closedSet.find(neighborHash) != closedSet.end()) {
                delete neighbor;
                continue;
            }
            openSet.push(neighbor);
        }
    }

    return false;
}

std::vector<AStar::Node*> AStar::getNeighbors(Node* current, int endX, int endY, const std::vector<std::vector<int>>& board) {
    std::vector<Node*> neighbors;
    std::vector<std::pair<int, int>> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
    for (const auto& dir : directions) {
        int newX = current->x + dir.first;
        int newY = current->y + dir.second;
        if (newX >= 0 && newX < numCols && newY >= 0 && newY < numRows && board[newY][newX] != 1) {
            int g = current->g + 1;
            int h = heuristic(newX, newY, endX, endY);
            neighbors.push_back(new Node(newX, newY, g, h, current));
        }
    }
    return neighbors;
}

void AStar::reconstructPath(Node* endNode, std::vector<std::vector<int>>& board) {
    Node* current = endNode;
    board[current->y][current->x] = 6;
    current = current->parent;

    while (current->parent != nullptr) {
        board[current->y][current->x] = 2;
        current = current->parent;
    }
    board[current->y][current->x] = 5;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    int rows, cols;
    std::cout << "Ingrese el tamaño del tablero (ancho altura): ";
    std::cin >> cols >> rows;

    MapBoard board1(rows, cols);
    board1.printBoard();
    board1.addObstaclesToBoard();
    board1.printBoard();
    board1.addObstaclesUser();
    board1.printBoard();
    board1.delObstaclesUser();
    board1.printBoard();

    AStar astar1(board1.getBoard());

    while(true){
        int checkX;
        int checkY;
        std::cout << "Ingrese las coordenadas que desea comprobar su accesibilidad (o -1 -1 para finalizar): ";
        std::cin >> checkX >> checkY;
        if (checkX == -1 && checkY == -1)break;
        if (checkX < cols && checkX >= 0 && checkY < rows && checkY >= 0){
            astar1.isAccessible(checkX , checkY);
        }else{
            std::cout << "Coordenadas fuera de rango. Vuelva a intentarlo.\n";
        }
    }

    astar1.getInitialCoords();
    if (astar1.searchRoute()) {
        board1.updateBoard(astar1.getSolvedBoard());
        board1.printBoard();
    } else {
        std::cout << "No se encontró ruta\n";
    }

    return 0;
}
