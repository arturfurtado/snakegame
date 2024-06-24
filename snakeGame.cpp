#include <iostream>
#include <conio.h>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <windows.h>

using namespace std;

struct Player {
    string name;
    int score;
    double time;
};

vector<Player> ranking;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

int speed = 100;
bool wallsKill = true;
bool increaseSpeed = false;
time_t startTime, endTime;
string playerName;

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;  // Começar com três unidades
}


void Draw() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
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
                if (!print) cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
    cout << "Pontuacao: " << score << endl;
    cout << "Macas: " << score / 10 << endl;  // Cada maçã vale 10 pontos
    cout << "Velocidade: " << speed << endl;
    cout << "Tempo: " << difftime(time(0), startTime) << " seconds" << endl;
}


void Input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}


void Logic() {
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

    if (wallsKill) {
        if (x >= width) gameOver = true;
        else if (x < 0) gameOver = true;
        if (y >= height) gameOver = true;
        else if (y < 0) gameOver = true;
    }
    else {
        if (x >= width) x = 0; else if (x < 0) x = width - 1;
        if (y >= height) y = 0; else if (y < 0) y = height - 1;
    }

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
        if (increaseSpeed) {
            speed -= 5; // Aumenta a velocidade
        }
    }
}

void LoadRanking() {
    ifstream inFile("ranking.txt");
    if (inFile.is_open()) {
        string name;
        int score;
        double time;
        while (inFile >> name >> score >> time) {
            ranking.push_back({ name, score, time });
        }
        inFile.close();
    }
}

void SaveRanking() {
    ofstream outFile("ranking.txt");
    if (outFile.is_open()) {
        for (const auto& player : ranking) {
            outFile << player.name << " " << player.score << " " << player.time << endl;
        }
        outFile.close();
    }
}

void ShowRanking() {
    system("cls");
    cout << "Ranking:\n";
    sort(ranking.begin(), ranking.end(), [](const Player& a, const Player& b) {
        return a.score > b.score;
        });
    for (const auto& player : ranking) {
        cout << player.name << " " << player.score << " " << player.time << endl;
    }
    system("pause");
}

void SelectDifficulty() {
    cout << "Selecione uma dificuldade.:\n";
    cout << "1. Facil (Paredes nao matam)\n";
    cout << "2. Medio (Paredes matam)\n";
    cout << "3. Dificil (Paredes matam e aumenta a velocidade da cobra a cada maca que come)\n";
    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        wallsKill = false;
        speed = 100;
        break;
    case 2:
        wallsKill = true;
        speed = 100;
        break;
    case 3:
        wallsKill = true;
        speed = 80;
        increaseSpeed = true;
        break;
    default:
        cout << "Escolha invalida, aperte outra tecla.\n";
        wallsKill = false;
        speed = 100;
        break;
    }
}

void MainMenu() {
    LoadRanking();
    bool exitProgram = false;
    while (!exitProgram) {
        system("cls");
        cout << "Snake Game\n";
        cout << "1. Jogar\n";
        cout << "2. Ver Ranking\n";
        cout << "3. Sair\n";
        cout << "Escolha uma opção: ";
        char choice = _getch();
        switch (choice) {
        case '1': {
            system("cls");
            cout << "Digite seu nome: ";
            cin >> playerName;
            SelectDifficulty();
            Setup();
            while (!gameOver) {
                Draw();
                Input();
                Logic();
                Sleep(speed);
            }
            endTime = time(0);
            double timeTaken = difftime(endTime, startTime);
            ranking.push_back({ playerName, score, timeTaken });
            SaveRanking();
            break;
        }
        case '2':
            ShowRanking();
            break;
        case '3':
            exitProgram = true;
            break;
        default:
            cout << "\nOpção inválida!\n";
            Sleep(1000);
            break;
        }
    }
}


int main() {
    MainMenu();
    return 0;
}
