#include <iostream>
#include <conio.h>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>

using namespace std;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum GameMode { NORMAL, CHALLENGE };

Direction dir;

struct Position {
    int x;
    int y;
};

struct PlayerScore {
    string name;
    int score;
};

class SnakeGame {
public:
    SnakeGame(int width = 20, int height = 20, int difficulty = 1, GameMode mode = NORMAL, bool npc = false);
    void Run();
    void DisplayRanking();

private:
    void Setup();
    void Draw();
    void Input();
    void Logic();
    void GenerateItem();
    void GenerateChallengeItems();
    bool IsCollision(int x, int y);
    void SaveScore(int score);
    void NPCMove();

    int width, height, difficulty;
    int score, highScore;
    int appleX, appleY, specialItemX, specialItemY;
    int decreaseAppleX, decreaseAppleY;
    int nTail;
    vector<Position> tail;
    Position head;
    bool gameOver;
    time_t startTime, endTime;
    bool specialItemPresent, decreaseApplePresent;
    int specialItemDuration;
    GameMode mode;
    chrono::time_point<chrono::steady_clock> challengeEndTime;
    vector<PlayerScore> ranking;
    int moves;
    int applesEaten;
    bool useNPC;

    void LoadRanking();
    void UpdateRanking(const string& playerName, int score);
};

SnakeGame::SnakeGame(int w, int h, int diff, GameMode m, bool npc) : width(w), height(h), difficulty(diff), mode(m), useNPC(npc) {
    highScore = 0;
    LoadRanking();
}

void SnakeGame::LoadRanking() {
    ifstream file("ranking.txt");
    if (file.is_open()) {
        PlayerScore playerScore;
        while (file >> playerScore.name >> playerScore.score) {
            ranking.push_back(playerScore);
        }
        file.close();
    }
}

void SnakeGame::UpdateRanking(const string& playerName, int score) {
    PlayerScore playerScore = { playerName, score };
    ranking.push_back(playerScore);

    sort(ranking.begin(), ranking.end(), [](const PlayerScore& a, const PlayerScore& b) {
        return a.score > b.score;
        });

    ofstream file("ranking.txt");
    if (file.is_open()) {
        for (const auto& ps : ranking) {
            file << ps.name << " " << ps.score << endl;
        }
        file.close();
    }
}

void SnakeGame::Setup() {
    gameOver = false;
    dir = RIGHT; // Começa movendo para a direita
    head.x = width / 2;
    head.y = height / 2;
    score = 0;
    nTail = 0; // Inicia com comprimento zero (apenas cabeça)
    tail.clear();
    specialItemPresent = false;
    decreaseApplePresent = false;
    if (mode == NORMAL) {
        GenerateItem();
    }
    else if (mode == CHALLENGE) {
        GenerateChallengeItems();
        challengeEndTime = chrono::steady_clock::now() + chrono::minutes(2);
    }
    startTime = time(0);
    moves = 0;
    applesEaten = 0;
}


void SnakeGame::Draw() {
    // Clear screen without flickering
    cout << "\033[H\033[J";

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) cout << "#";
            if (i == head.y && j == head.x) cout << "O";
            else if (i == appleY && j == appleX) cout << "A";
            else if (specialItemPresent && i == specialItemY && j == specialItemX) cout << "S";
            else if (decreaseApplePresent && i == decreaseAppleY && j == decreaseAppleX) cout << "D";
            else {
                bool printTail = false;
                for (int k = 0; k < nTail; k++) {
                    if (tail[k].x == j && tail[k].y == i) {
                        cout << "o";
                        printTail = true;
                    }
                }
                if (!printTail) cout << " ";
            }
            if (j == width - 1) cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) cout << "#";
    cout << endl;
    cout << "Score: " << score << "  HighScore: " << highScore << endl;
    cout << "Moves: " << moves << "  Apples Eaten: " << applesEaten << endl;
    cout << "Difficulty: " << difficulty << "  Speed: " << 100 / difficulty << endl;

    if (mode == CHALLENGE) {
        auto timeLeft = chrono::duration_cast<chrono::seconds>(challengeEndTime - chrono::steady_clock::now()).count();
        cout << "Time left: " << timeLeft << " seconds" << endl;
        if (timeLeft <= 0) {
            gameOver = true;
        }
    }

    if (applesEaten >= 100) {
        cout << "Congratulations! You've eaten 100 apples!" << endl;
        gameOver = true;
    }
}

void SnakeGame::Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            moves++;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            moves++;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            moves++;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            moves++;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}



void SnakeGame::NPCMove() {
    // Direções possíveis
    vector<Direction> possibleDirections;

    // Determina a direção oposta à atual
    Direction oppositeDirection;
    switch (dir) {
    case LEFT:
        oppositeDirection = RIGHT;
        break;
    case RIGHT:
        oppositeDirection = LEFT;
        break;
    case UP:
        oppositeDirection = DOWN;
        break;
    case DOWN:
        oppositeDirection = UP;
        break;
    default:
        oppositeDirection = STOP;
        break;
    }

    // Verifica as direções possíveis e prioriza movimentos seguros
    if (!IsCollision(head.x - 1, head.y) && dir != RIGHT) {
        possibleDirections.push_back(LEFT);
    }
    if (!IsCollision(head.x + 1, head.y) && dir != LEFT) {
        possibleDirections.push_back(RIGHT);
    }
    if (!IsCollision(head.x, head.y - 1) && dir != DOWN) {
        possibleDirections.push_back(UP);
    }
    if (!IsCollision(head.x, head.y + 1) && dir != UP) {
        possibleDirections.push_back(DOWN);
    }

    // Escolhe a melhor direção baseada na distância até a comida
    Direction bestDirection = STOP;
    int minDistance = INT_MAX;

    for (auto direction : possibleDirections) {
        int distance = 0;
        int futureCollision = 0;
        Position futureHead = head;

        // Simula o movimento para frente na direção atual
        switch (direction) {
        case LEFT:
            futureHead.x--;
            break;
        case RIGHT:
            futureHead.x++;
            break;
        case UP:
            futureHead.y--;
            break;
        case DOWN:
            futureHead.y++;
            break;
        default:
            break;
        }

        // Verifica se esse movimento levará a uma colisão com o próprio corpo
        for (int i = 0; i < nTail; ++i) {
            if (futureHead.x == tail[i].x && futureHead.y == tail[i].y) {
                futureCollision = 1;
                break;
            }
        }

        // Verifica se o movimento leva à formação de um ciclo ou loop
        bool willFormCycle = false;
        // Implemente sua lógica de detecção de ciclo aqui, por exemplo:
        // Verificar se a nova posição da cabeça é semelhante à uma posição anterior
        for (int i = 1; i < nTail; ++i) {
            if (futureHead.x == tail[i].x && futureHead.y == tail[i].y) {
                willFormCycle = true;
                break;
            }
        }

        // Calcula a distância até a comida
        distance = abs(futureHead.x - appleX) + abs(futureHead.y - appleY);

        // Prioriza direções que não levem a uma colisão imediata ou futura
        if (!futureCollision && !willFormCycle && distance < minDistance) {
            minDistance = distance;
            bestDirection = direction;
        }
    }

    // Define a direção escolhida
    dir = bestDirection;
}


bool SnakeGame::IsCollision(int x, int y) {
    // Verifica se a posição (x, y) colide com qualquer parte do corpo da cobra
    for (int i = 0; i < nTail; i++) {
        if (tail[i].x == x && tail[i].y == y) {
            return true;
        }
    }
    return false;
}

void SnakeGame::Logic() {
    if (tail.size() != nTail) {
        tail.resize(nTail);
    }
    Position prev = head;
    Position prev2;

    for (int i = 0; i < nTail; i++) {
        prev2 = tail[i];
        tail[i] = prev;
        prev = prev2;
    }

    switch (dir) {
    case LEFT:
        head.x--;
        break;
    case RIGHT:
        head.x++;
        break;
    case UP:
        head.y--;
        break;
    case DOWN:
        head.y++;
        break;
    default:
        break;
    }

    if (difficulty == 1) { // Easy: Snake passes through walls
        if (head.x >= width) head.x = 0; else if (head.x < 0) head.x = width - 1;
        if (head.y >= height) head.y = 0; else if (head.y < 0) head.y = height - 1;
    }
    else if (difficulty >= 2) { // Medium and Hard: Snake dies on collision with walls
        if (head.x >= width || head.x < 0 || head.y >= height || head.y < 0) {
            gameOver = true;
        }
    }

    for (int i = 0; i < nTail; i++) {
        if (tail[i].x == head.x && tail[i].y == head.y) gameOver = true;
    }

    if (head.x == appleX && head.y == appleY) {
        score += 10;
        GenerateItem();
        nTail++;
        tail.push_back(prev); // Adiciona a nova posição da cauda
        applesEaten++;
        if (difficulty == 3) { // Hard: Increase speed
            this_thread::sleep_for(chrono::milliseconds(90 / difficulty));
        }
    }

    if (specialItemPresent && head.x == specialItemX && head.y == specialItemY) {
        score += 50;
        specialItemPresent = false;
    }

    if (decreaseApplePresent && head.x == decreaseAppleX && head.y == decreaseAppleY) {
        score -= 20;
        if (nTail > 0) {
            nTail--;
            tail.pop_back(); // Remove a última posição da cauda
        }
        decreaseApplePresent = false;
    }

    if (difftime(time(0), startTime) >= specialItemDuration) {
        specialItemPresent = false;
    }
}

void SnakeGame::GenerateItem() {
    appleX = rand() % width;
    appleY = rand() % height;
    if (rand() % 5 == 0) {
        specialItemPresent = true;
        specialItemX = rand() % width;
        specialItemY = rand() % height;
        specialItemDuration = 10; // special item lasts 10 seconds
    }
}

void SnakeGame::GenerateChallengeItems() {
    appleX = rand() % width;
    appleY = rand() % height;
    decreaseAppleX = rand() % width;
    decreaseAppleY = rand() % height;
    specialItemPresent = false;
    decreaseApplePresent = true;
}

void SnakeGame::Run() {
    string playerName;
    cout << "Enter your name: ";
    cin >> playerName;
    Setup();
    while (!gameOver) {
        Draw();
        if (useNPC) {
            NPCMove();
        }
        else {
            Input();
        }
        Logic();
        this_thread::sleep_for(chrono::milliseconds(100 / difficulty)); // control game speed
    }
    endTime = time(0);
    int totalTime = difftime(endTime, startTime);
    int finalScore = score + totalTime - nTail;
    if (finalScore > highScore) highScore = finalScore;
    cout << "Game Over! Final Score: " << finalScore << endl;

    SaveScore(finalScore);
    UpdateRanking(playerName, finalScore);
}

void SnakeGame::SaveScore(int score) {
    ofstream file("scores.txt", ios::app);
    if (file.is_open()) {
        file << score << endl;
        file.close();
    }
}

void SnakeGame::DisplayRanking() {
    cout << "===== Ranking =====" << endl;
    for (const auto& ps : ranking) {
        cout << ps.name << " - " << ps.score << endl;
    }
}

void DisplayMenu() {
    cout << "================= Snake Game =================" << endl;
    cout << "1. Easy" << endl;
    cout << "2. Medium" << endl;
    cout << "3. Hard" << endl;
    cout << "4. Challenge Mode" << endl;
    cout << "5. View Ranking" << endl;
    cout << "6. NPC Mode" << endl;
    cout << "Choose an option: ";
}

int main() {
    srand(time(0));
    int choice;
    while (true) {
        DisplayMenu();
        cin >> choice;

        int difficulty;
        GameMode mode = NORMAL;
        bool useNPC = false;

        switch (choice) {
        case 1:
            difficulty = 1;
            break;
        case 2:
            difficulty = 2;
            break;
        case 3:
            difficulty = 3;
            break;
        case 4:
            difficulty = 3; // Challenge mode with highest difficulty
            mode = CHALLENGE;
            break;
        case 5:
        {
            SnakeGame game;
            game.DisplayRanking();
            continue;
        }
        case 6:
            useNPC = true;
            difficulty = 3;
            break;
        default:
            difficulty = 1;
            break;
        }

        SnakeGame game((mode == CHALLENGE ? 40 : 20), (mode == CHALLENGE ? 40 : 20), difficulty, mode, useNPC);
        game.Run();
    }

    return 0;
    while (true) {
        DisplayMenu();
        cin >> choice;

        int difficulty;
        GameMode mode = NORMAL;
        bool useNPC = false;

        switch (choice) {
        case 1:
            difficulty = 1;
            break;
        case 2:
            difficulty = 2;
            break;
        case 3:
            difficulty = 3;
            break;
        case 4:
            difficulty = 3; // Challenge mode with highest difficulty
            mode = CHALLENGE;
            break;
        case 5:
        {
            SnakeGame game;
            game.DisplayRanking();
            continue;
        }
        case 6:
            useNPC = true;
            break;
        default:
            difficulty = 1;
            break;
        }

        SnakeGame game((mode == CHALLENGE ? 40 : 20), (mode == CHALLENGE ? 40 : 20), difficulty, mode, useNPC);
        game.Run();
    }

    return 0;
}
