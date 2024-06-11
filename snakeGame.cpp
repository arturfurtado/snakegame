#include <iostream>     
#include <conio.h>      
#include <windows.h>    
#include <vector>       
#include <string>       
#include <fstream>      
#include <ctime>        
#include <algorithm>    

using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100]; 
int nTail; 
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; 
eDirection dir;
string playerName;
time_t startTime, endTime;

struct PlayerScore {
    string name;
    int score;
    double time;
};

vector<PlayerScore> ranking; 

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2; 
    y = height / 2;
    fruitX = (rand() % (width - 2)) + 1; 
    fruitY = (rand() % (height - 2)) + 1;
    score = 0;
    nTail = 0;
    startTime = time(0); 
}

void Draw() {
    system("cls"); 
    cout << "Score: " << score << endl; 

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
}

void Input() {
    if (_kbhit()) { 
        switch (_getch()) {
        case 'a': case 75: 
            dir = LEFT;
            break;
        case 'd': case 77: 
            dir = RIGHT;
            break;
        case 'w': case 72: 
            dir = UP;
            break;
        case 's': case 80: 
            dir = DOWN;
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

    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 10; 
        fruitX = (rand() % (width - 2)) + 1; 
        fruitY = (rand() % (height - 2)) + 1;
        nTail++; 
    }

    if (score >= 100) {
        cout << "Parabéns, você ganhou!" << endl;
        gameOver = true;
    }
}

void SaveRanking() {
    ofstream outfile("ranking.txt");
    for (const auto& entry : ranking) {
        outfile << "Jogador: " << entry.name << ", Pontuação: " << entry.score << ", Tempo: " << entry.time << "s" << endl;
    }
    outfile.close();
}

void LoadRanking() {
    ifstream infile("ranking.txt");
    ranking.clear(); 
    string name;
    int score;
    double time;

    while (infile >> name >> score >> time) {
        ranking.push_back({ name, score, time });
    }
    infile.close();
}

void ShowRanking() {
    system("cls");
    cout << "Ranking de Jogadores:\n";

    sort(ranking.begin(), ranking.end(), [](const PlayerScore& a, const PlayerScore& b) {
        if (a.score == b.score) {
            return a.time < b.time;
        }
        return a.score > b.score;
        });

    for (size_t i = 0; i < ranking.size(); i++) {
        cout << i + 1 << ". " << ranking[i].name << " - Pontos: " << ranking[i].score << " - Tempo: " << ranking[i].time << "s" << endl;
    }
    cout << "\nPressione qualquer tecla para voltar ao menu...\n";
    _getch(); 
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
            Setup(); 
            while (!gameOver) {
                Draw(); 
                Input(); 
                Logic(); 
                Sleep(100); 
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
