#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <algorithm>
using namespace std;

const int width = 20;
const int height = 17;

struct Node {

    int x, y;
    int cost, priority;
    bool operator<(const Node& other) const { return priority > other.priority; }
    bool operator==(const Node& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Node& other) const { return !(*this == other); }
};

struct NodeHash {
    size_t operator()(const Node& n) const { return n.x * 31 + n.y; }
};

bool gameOver;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirecton { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirecton dir;
int ranking[5];
string rankingName[5];
char playerName[10];

void setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void draw() {
    system("cls");
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";
            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "F";
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
    cout << "Score: " << score << endl;
}

void input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

bool isOccupied(int x, int y) {
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y) return true;
    }
    return false;
}

vector<Node> getNeighbors(Node node) {
    vector<Node> neighbors;
    vector<pair<int, int>> directions = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
    for (auto d : directions) {
        int nx = node.x + d.first;
        int ny = node.y + d.second;
        if (nx >= 0 && nx < width && ny >= 0 && ny < height && !isOccupied(nx, ny)) {
            neighbors.push_back({ nx, ny, node.cost + 1 });
        }
    }
    return neighbors;
}

int heuristic(Node a, Node b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

vector<Node> aStar(Node start, Node goal) {
    priority_queue<Node> openSet;
    unordered_set<Node, NodeHash> closedSet;
    unordered_map<Node, Node, NodeHash> cameFrom;

    start.priority = start.cost + heuristic(start, goal);
    openSet.push(start);

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current == goal) {
            vector<Node> path;
            while (current != start) {
                path.push_back(current);
                current = cameFrom[current];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        closedSet.insert(current);

        for (auto neighbor : getNeighbors(current)) {
            if (closedSet.find(neighbor) != closedSet.end()) continue;
            neighbor.priority = neighbor.cost + heuristic(neighbor, goal);
            if (cameFrom.find(neighbor) == cameFrom.end() || neighbor.cost < cameFrom[neighbor].cost) {
                cameFrom[neighbor] = current;
                openSet.push(neighbor);
            }
        }
    }

    return vector<Node>();
}

void logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    if (x >= width)
        x = 0;
    else if (x < 0)
        x = width - 1;
    if (y >= height)
        y = 0;
    else if (y < 0)
        y = height - 1;

    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }

    if (x == fruitX && y == fruitY) {
        nTail++;
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
    }
}

void saveRanking() {
    ofstream file;
    file.open("ranking.txt");
    for (int i = 0; i < 5; i++) {
        file << rankingName[i] << endl;
        file << ranking[i] << endl;
    }
    file.close();
}

void loadRanking() {
    ifstream file;
    file.open("ranking.txt");
    for (int i = 0; i < 5; i++) {
        file >> rankingName[i];
        file >> ranking[i];
    }
    file.close();
}

void mainMenu() {
    loadRanking();
    cout << "1. Start Normal Game" << endl;
    cout << "2. Start NPC Game" << endl;
    cout << "3. Show Ranking" << endl;
    cout << "4. Quit" << endl;
    char choice;
    cin >> choice;
    switch (choice) {
    case '1':
        setup();
        break;
    case '2':
        setup();
        while (!gameOver) {
            draw();
            Node start = { x, y, 0 };
            Node goal = { fruitX, fruitY, 0 };
            vector<Node> path = aStar(start, goal);
            if (!path.empty()) {
                Node nextStep = path.front();
                if (nextStep.x > x) dir = RIGHT;
                else if (nextStep.x < x) dir = LEFT;
                else if (nextStep.y > y) dir = DOWN;
                else if (nextStep.y < y) dir = UP;
            }
            logic();
            Sleep(100); // Slow down the game loop
        }
        ranking[4] = score;
        rankingName[4] = playerName;
        for (int i = 4; i > 0; i--) {
            if (ranking[i] > ranking[i - 1]) {
                swap(ranking[i], ranking[i - 1]);
                swap(rankingName[i], rankingName[i - 1]);
            }
        }
        saveRanking();
        break;
    case '3':
        for (int i = 0; i < 5; i++) {
            cout << rankingName[i] << ": " << ranking[i] << endl;
        }
        mainMenu();
        break;
    case '4':
        exit(0);
    }
}

int main() {
    cout << "Enter your name: ";
    cin >> playerName;
    mainMenu();
    while (!gameOver) {
        draw();
        input();
        logic();
        Sleep(100); // Slow down the game loop
    }
    ranking[4] = score;
    rankingName[4] = playerName;
    for (int i = 4; i > 0; i--) {
        if (ranking[i] > ranking[i - 1]) {
            swap(ranking[i], ranking[i - 1]);
            swap(rankingName[i], rankingName[i - 1]);
        }
    }
    saveRanking();
    return 0;
}




