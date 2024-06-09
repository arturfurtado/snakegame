#include <iostream>
#include <conio.h>
#include <windows.h>
using namespace std;

bool gameOver;
const int width = 20;  // Largura do campo de jogo
const int height = 20; // Altura do campo de jogo
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100]; // Arrays para armazenar as posições da cauda da cobra
int nTail; // Tamanho da cauda
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; // Direções possíveis da cobra
eDirection dir;

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2; // Posição inicial da cobra
    y = height / 2;
    // Gera a fruta em uma posição aleatória, mas não nas bordas
    fruitX = (rand() % (width - 2)) + 1;
    fruitY = (rand() % (height - 2)) + 1;
    score = 0;
}

void Draw() {
    system("cls"); // Limpa a tela
    cout << "Score: " << score << endl; // Exibe a pontuação na parte superior

    for (int i = 0; i < width + 2; i++)
        cout << "#"; // Desenha a borda superior
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#"; // Desenha a borda esquerda
            if (i == y && j == x)
                cout << "O"; // Desenha a cabeça da cobra
            else if (i == fruitY && j == fruitX)
                cout << "F"; // Desenha a fruta
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o"; // Desenha a cauda da cobra
                        print = true;
                    }
                }
                if (!print)
                    cout << " "; // Espaço vazio
            }
            if (j == width - 1)
                cout << "#"; // Desenha a borda direita
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#"; // Desenha a borda inferior
    cout << endl;
}

void Input() {
    if (_kbhit()) { // Verifica se uma tecla foi pressionada
        switch (_getch()) {
        case 'a': case 75: // Tecla 'a' ou seta esquerda
            dir = LEFT;
            break;
        case 'd': case 77: // Tecla 'd' ou seta direita
            dir = RIGHT;
            break;
        case 'w': case 72: // Tecla 'w' ou seta para cima
            dir = UP;
            break;
        case 's': case 80: // Tecla 's' ou seta para baixo
            dir = DOWN;
            break;
        case 'x':
            gameOver = true; // Encerra o jogo
            break;
        }
    }
}

void Logic() {
    // Atualiza a posição da cauda
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

    // Atualiza a posição da cobra com base na direção
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

    // Verifica colisão com as paredes
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    // Verifica colisão com a própria cauda
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    // Verifica se a cobra comeu a fruta
    if (x == fruitX && y == fruitY) {
        score += 10; // Incrementa a pontuação
        // Gera uma nova fruta em uma posição aleatória, mas não nas bordas
        fruitX = (rand() % (width - 2)) + 1;
        fruitY = (rand() % (height - 2)) + 1;
        nTail++; // Aumenta o tamanho da cauda
    }

    // Condição de vitória
    if (score >= 100) {
        cout << "Parabéns, você ganhou!" << endl;
        gameOver = true;
    }
}

int main() {
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(100); // Pausa para reduzir a velocidade do jogo
    }
    return 0;
}
