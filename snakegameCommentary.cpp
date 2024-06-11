#include <iostream>     // Biblioteca padrão para entrada e saída
#include <conio.h>      // Biblioteca para manipulação de teclado (_kbhit() e _getch())
#include <windows.h>    // Biblioteca para funções do Windows (Sleep())
#include <vector>       // Biblioteca para uso do vetor
#include <string>       // Biblioteca para manipulação de strings
#include <fstream>      // Biblioteca para manipulação de arquivos
#include <ctime>        // Biblioteca para manipulação de tempo
#include <algorithm>    // Biblioteca para funções de algoritmos (sort)

using namespace std;

// Variáveis globais e constantes
bool gameOver;
const int width = 20;
const int height = 20;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100]; // Arrays para a posição da cauda da cobra
int nTail; // Tamanho da cauda
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; // Direções possíveis
eDirection dir;
string playerName;
time_t startTime, endTime;

// Estrutura para armazenar pontuação e tempo do jogador
struct PlayerScore {
    string name;
    int score;
    double time;
};

vector<PlayerScore> ranking; // Vetor para armazenar o ranking dos jogadores

// Função para configuração inicial do jogo
void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2; // Posição inicial da cabeça da cobra
    y = height / 2;
    fruitX = (rand() % (width - 2)) + 1; // Gera posição aleatória para a fruta
    fruitY = (rand() % (height - 2)) + 1;
    score = 0;
    nTail = 0;
    startTime = time(0); // Inicia o temporizador
}

// Função para desenhar o campo de jogo
void Draw() {
    system("cls"); // Limpa a tela
    cout << "Score: " << score << endl; // Exibe a pontuação na parte superior

    // Desenha a borda superior
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    // Desenha o campo de jogo
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#"; // Borda esquerda
            if (i == y && j == x)
                cout << "O"; // Cabeça da cobra
            else if (i == fruitY && j == fruitX)
                cout << "F"; // Fruta
            else {
                bool print = false;
                // Desenha a cauda da cobra
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print)
                    cout << " "; // Espaço vazio
            }
            if (j == width - 1)
                cout << "#"; // Borda direita
        }
        cout << endl;
    }

    // Desenha a borda inferior
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
}

// Função para capturar entrada do teclado
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
        case 'x': // Tecla 'x' para sair do jogo
            gameOver = true;
            break;
        }
    }
}

// Função para atualizar a lógica do jogo
void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    // Atualiza a posição da cauda
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Atualiza a posição da cabeça da cobra
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

    // Verifica colisão com as bordas
    if (x >= width || x < 0 || y >= height || y < 0)
        gameOver = true;

    // Verifica colisão com a cauda
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;

    // Verifica se a cobra comeu a fruta
    if (x == fruitX && y == fruitY) {
        score += 10; // Aumenta a pontuação
        fruitX = (rand() % (width - 2)) + 1; // Gera nova fruta em posição aleatória
        fruitY = (rand() % (height - 2)) + 1;
        nTail++; // Aumenta o tamanho da cauda
    }

    // Condição de vitória
    if (score >= 100) {
        cout << "Parabéns, você ganhou!" << endl;
        gameOver = true;
    }
}

// Função para salvar o ranking em um arquivo
void SaveRanking() {
    ofstream outfile("ranking.txt");
    for (const auto& entry : ranking) {
        outfile << "Jogador: " << entry.name << ", Pontuação: " << entry.score << ", Tempo: " << entry.time << "s" << endl;
    }
    outfile.close();
}

// Função para carregar o ranking de um arquivo
void LoadRanking() {
    ifstream infile("ranking.txt");
    ranking.clear(); // Limpa o ranking atual
    string name;
    int score;
    double time;
    // Carrega os dados do arquivo
    while (infile >> name >> score >> time) {
        ranking.push_back({ name, score, time });
    }
    infile.close();
}

// Função para mostrar o ranking
void ShowRanking() {
    system("cls");
    cout << "Ranking de Jogadores:\n";
    // Ordena o ranking primeiro por pontuação, depois por tempo
    sort(ranking.begin(), ranking.end(), [](const PlayerScore& a, const PlayerScore& b) {
        if (a.score == b.score) {
            return a.time < b.time;
        }
        return a.score > b.score;
        });
    // Exibe o ranking
    for (size_t i = 0; i < ranking.size(); i++) {
        cout << i + 1 << ". " << ranking[i].name << " - Pontos: " << ranking[i].score << " - Tempo: " << ranking[i].time << "s" << endl;
    }
    cout << "\nPressione qualquer tecla para voltar ao menu...\n";
    _getch(); // Espera que o usuário pressione uma tecla
}

// Função para exibir o menu principal
void MainMenu() {
    LoadRanking(); // Carrega o ranking do arquivo
    bool exitProgram = false;
    while (!exitProgram) {
        system("cls"); // Limpa a tela
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
            Setup(); // Configura o jogo
            while (!gameOver) {
                Draw(); // Desenha o campo de jogo
                Input(); // Captura a entrada do teclado
                Logic(); // Atualiza a lógica do jogo
                Sleep(100); // Controla a velocidade do jogo
            }
            endTime = time(0); // Registra o tempo de fim
            double timeTaken = difftime(endTime, startTime); // Calcula o tempo total
            ranking.push_back({ playerName, score, timeTaken }); // Adiciona ao ranking
            SaveRanking(); // Salva o ranking no arquivo
            break;
        }
        case '2':
            ShowRanking(); // Mostra o ranking
            break;
        case '3':
            exitProgram = true; // Sai do programa
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
