#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <stack>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>

using namespace std;

const int WALL = 1;    // Valor que representa una pared en el laberinto
const int PATH = 0;    // Valor que representa un camino en el laberinto
const int SOLUTION = 3;// Valor que representa el camino de solución

struct Cell {
    int x, y;
    Cell(int x, int y) : x(x), y(y) {}
};

// Genera un laberinto usando el algoritmo de búsqueda en profundidad
vector<vector<int>> generateMaze(int width, int height) {
    vector<vector<int>> maze(height, vector<int>(width, WALL)); // Inicializa el laberinto lleno de paredes
    stack<Cell> cellStack;

    int startX = 1;
    int startY = 1;

    maze[startY][startX] = PATH; // Marca la celda inicial como camino
    cellStack.push(Cell(startX, startY));

    vector<pair<int, int>> directions = {{2, 0}, {-2, 0}, {0, 2}, {0, -2}}; // Posibles direcciones (N, S, E, O) en pasos de dos

    while (!cellStack.empty()) {
        Cell current = cellStack.top();
        cellStack.pop();

        vector<pair<int, int>> neighbors;
        for (const auto& direction : directions) {
            int newX = current.x + direction.first;
            int newY = current.y + direction.second;
            if (newX > 0 && newX < width - 1 && newY > 0 && newY < height - 1 && maze[newY][newX] == WALL) {
                neighbors.push_back({newX, newY}); // Encuentra vecinos no visitados
            }
        }

        if (!neighbors.empty()) {
            cellStack.push(current);

            auto nextCell = neighbors[rand() % neighbors.size()]; // Selecciona un vecino aleatoriamente
            int midX = (current.x + nextCell.first) / 2;
            int midY = (current.y + nextCell.second) / 2;

            maze[midY][midX] = PATH; // Crea un camino al vecino
            maze[nextCell.second][nextCell.first] = PATH;

            cellStack.push(Cell(nextCell.first, nextCell.second)); // Empuja el vecino al stack
        }
    }

    return maze;
}

// Función auxiliar para rastrear el camino de solución
void backtrackPath(vector<vector<int>>& maze, const vector<vector<pair<int, int>>>& parent, int endX, int endY) {
    int x = endX;
    int y = endY;
    while (x != -1 && y != -1) {
        maze[y][x] = SOLUTION; // Marca el camino de solución
        auto p = parent[y][x];
        x = p.first;
        y = p.second;
    }
}

// Resuelve el laberinto usando BFS y rastrea el camino de solución
bool solveMaze(vector<vector<int>>& maze, int startX, int startY, int endX, int endY) {
    queue<pair<int, int>> q;
    q.push({startX, startY});

    vector<vector<int>> visited(maze.size(), vector<int>(maze[0].size(), false));
    vector<vector<pair<int, int>>> parent(maze.size(), vector<pair<int, int>>(maze[0].size(), {-1, -1}));
    visited[startY][startX] = true;

    vector<pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; // Posibles direcciones (N, S, E, O) en pasos de uno

    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        int x = current.first;
        int y = current.second;

        if (x == endX && y == endY) {
            backtrackPath(maze, parent, endX, endY); // Rastrear el camino de solución
            return true; // Se encontró el camino al final
        }

        for (const auto& direction : directions) {
            int newX = x + direction.first;
            int newY = y + direction.second;

            if (newX >= 0 && newX < maze[0].size() && newY >= 0 && newY < maze.size() &&
                maze[newY][newX] == PATH && !visited[newY][newX]) {
                visited[newY][newX] = true;
                parent[newY][newX] = {x, y}; // Guarda el nodo padre para rastrear el camino
                q.push({newX, newY});
            }
        }
    }

    return false;
}

// Dibuja el laberinto en la ventana
void drawMaze(sf::RenderWindow& window, const vector<vector<int>>& maze) {
    int cellSize = 20; // Tamaño de cada celda en píxeles
    sf::RectangleShape wall(sf::Vector2f(cellSize, cellSize));
    wall.setFillColor(sf::Color::Black);
    sf::RectangleShape path(sf::Vector2f(cellSize, cellSize));
    path.setFillColor(sf::Color::White);
    sf::RectangleShape solution(sf::Vector2f(cellSize, cellSize));
    solution.setFillColor(sf::Color::Green);

    for (int y = 0; y < maze.size(); y++) {
        for (int x = 0; x < maze[0].size(); x++) {
            sf::RectangleShape* shape;
            if (maze[y][x] == WALL) shape = &wall;
            else if (maze[y][x] == PATH) shape = &path;
            else shape = &solution;

            shape->setPosition(x * cellSize, y * cellSize); // Posiciona la celda en la ventana
            window.draw(*shape);
        }
    }
}

int main() {
    srand(time(0)); // Inicializa la semilla para los números aleatorios
    int size = 0;

    cout << "Ingrese de cuanto por cuanto desea el laberinto (numero impar) de lo contrario sera de 11 x 11: ";
    string input;
    getline(cin, input);
    
    if (input.empty()) {
        size = 11;
    } else {
        size = stoi(input);
        if (size <= 0 || size % 2 == 0) {
            size++;
        } 
    }
    
    int width = size;
    int height = size;

    vector<vector<int>> maze = generateMaze(width, height);

    int startX = 1, startY = 0; // Inicio en el borde superior (1, 0)
    int endX = width - 2, endY = height - 1; // Fin en el borde inferior (width-2, height-1)

    maze[startY][startX] = PATH; // Marca el inicio como camino
    maze[endY][endX] = PATH; // Marca el final como camino

    // Cierra el perímetro del laberinto excepto en los puntos de entrada y salida
    for (int i = 0; i < width; i++) {
        maze[0][i] = WALL;
        maze[height - 1][i] = WALL;
    }
    for (int i = 0; i < height; i++) {
        maze[i][0] = WALL;
        maze[i][width - 1] = WALL;
    }

    // Abre los puntos de entrada y salida
    maze[startY][startX] = PATH;
    maze[endY][endX] = PATH;

    if (!solveMaze(maze, startX, startY, endX, endY)) {
        cout << "No se pudo encontrar un camino." << endl;
        return 1; // Sale del programa si no se encontró un camino
    }

    sf::RenderWindow window(sf::VideoMode(width * 20, height * 20), "Laberinto");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        drawMaze(window, maze);
        window.display(); // Actualiza la ventana
    }

    return 0;
}