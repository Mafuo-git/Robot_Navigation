#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>

using namespace std;

const char EMPTY = '.';
const char OBSTACLE = 'X';
const char ROBOT = 'R';       // Définition des différents éléments de la grille
const char GOAL = 'W';
const char BONUS = 'B';

struct Position {  // Structure de données pour stocker les coordonnées d'une cellule
    int x, y;
};

class Game {  // Classe principale du jeu
public:
    Game(int width, int height, int obstacleDensity, int bonusDensity)  // Constructeur de la classe
        : width(width), height(height), score(0) {
        srand(time(0));
        grid.resize(height, vector<char>(width, EMPTY));
        placeObstacles(obstacleDensity);
        placeBonuses(bonusDensity);
        placeRobot();
        placeGoal();
    }

    void play() {  // Méthode pour démarrer le jeu
        char mode;
        cout << "Choisissez le mode: (m)anuel ou (a)utomatique: ";
        cin >> mode;
        if (mode == 'm') {
            playManual();
        }
        else if (mode == 'a') {
            playAutomatic();
        }
        else {
            cout << "Mode invalide" << endl;
        }
    }

private:
    int width, height, score;  // Largeur, hauteur et score du jeu
    vector<vector<char>> grid;  // Grille de jeu
    Position robotPos, goalPos;  // Position du robot et du point d'arrivée

    void placeObstacles(int density) { // Densité des obstacles en %
        int totalCells = width * height;
        int obstacles = totalCells * density / 100;
        while (obstacles > 0) {
            int x = rand() % width;
            int y = rand() % height;
            if (grid[y][x] == EMPTY) {
                grid[y][x] = OBSTACLE;
                obstacles--;
            }
        }
    }

    void placeBonuses(int density) { // Densité des bonus en %
        int totalCells = width * height;
        int bonuses = totalCells * density / 100;
        while (bonuses > 0) {
            int x = rand() % width;
            int y = rand() % height;
            if (grid[y][x] == EMPTY) {
                grid[y][x] = BONUS;
                bonuses--;
            }
        }
    }

    void placeRobot() { // Positionnement aléatoire du robot sur la grille
        while (true) {
            int x = rand() % width;
            int y = rand() % height;
            if (grid[y][x] == EMPTY) {
                grid[y][x] = ROBOT;
                robotPos = { x, y };
                break;
            }
        }
    }

    void placeGoal() {  // Positionnement aléatoire du point d'arrivée sur la grille
        while (true) {
            int x = rand() % width;
            int y = rand() % height;
            if (grid[y][x] == EMPTY) {
                grid[y][x] = GOAL;
                goalPos = { x, y };
                break;
            }
        }
    }

    void displayGrid() {  // Affichage de la grille
        system("cls");
        for (const auto& row : grid) {
            for (char cell : row) {
                cout << cell << ' ';
            }
            cout << endl;
        }
        cout << "Score: " << score << endl;
    }

    void playManual() {  // Mode de jeu manuel
        displayGrid();
        while (true) {
            char move = _getch();
            Position newPos = robotPos;
            if (move == 'z') newPos.y--;        // z: haut, s: bas, q: gauche, d: droite
            else if (move == 's') newPos.y++;
            else if (move == 'q') newPos.x--;
            else if (move == 'd') newPos.x++;
            else continue;

            if (isValidMove(newPos)) {  // Vérification de la validité du mouvement
                moveRobot(newPos);
                displayGrid();
                if (robotPos.x == goalPos.x && robotPos.y == goalPos.y) {
                    cout << "Vous avez atteint le point d'arrivée ! Score final : " << score << endl;
                    break;
                }
            }
        }
    }

    void playAutomatic() {
        priority_queue<Node*, vector<Node*>, CompareNode> openList;  // Liste ouverte
        unordered_set<Position> closedList;                          // Liste fermée
        Node* startNode = new Node(robotPos, 0, heuristic(robotPos, goalPos));  // Création du noeud de départ
        openList.push(startNode);                                   // Ajout du noeud de départ à la liste ouverte

        while (!openList.empty()) {                     // Tant que la liste ouverte n'est pas vide
            Node* currentNode = openList.top();         // Récupère le noeud avec le coût le plus faible
            openList.pop();                             // Supprime le noeud de la liste ouverte

            if (currentNode->pos == goalPos) {          // Vérifie si le robot a atteint le point d'arrivée
                reconstructPath(currentNode);           // Reconstitue le chemin
                cout << "Le robot a atteint le point d'arrivee ! Score final : " << score << endl;   // Affiche le score final
                return;
            }

            closedList.insert(currentNode->pos);        // Ajoute le noeud courant à la liste fermée

            for (const auto& neighbor : getNeighbors(currentNode->pos)) {    // Parcours des voisins du noeud courant
                if (closedList.find(neighbor) != closedList.end()) continue;  // Vérifie si le voisin est dans la liste fermée

                int tentativeG = currentNode->g + 1;        // Coût du chemin actuel
                Node* neighborNode = new Node(neighbor, tentativeG, heuristic(neighbor, goalPos), currentNode);   // Crée un nouveau noeud

                openList.push(neighborNode);      // Ajoute le voisin à la liste ouverte
            }

        }
    }

    bool isValidMove(Position pos) {   // Vérifie si le mouvement est valide
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height && grid[pos.y][pos.x] != OBSTACLE;
    }

    void moveRobot(Position newPos) {   // Déplace le robot vers la nouvelle position
        if (grid[newPos.y][newPos.x] == BONUS) {   // Vérifie si le robot a atteint un bonus
            score += 10;
        }
        grid[robotPos.y][robotPos.x] = EMPTY;    // Met à jour la grille
        robotPos = newPos;                       // Met à jour la position du robot
        grid[robotPos.y][robotPos.x] = ROBOT;    // Met à jour la grille
    }
};



int main() {          // Fonction principale
    char playAgain;
    do {
        int width, height, obstacleDensity, bonusDensity;
        cout << "Indiquez la largeur du plateau de jeu: ";    // Saisie de la largeur
        cin >> width;
        cout << "Indiquez la hauteur du plateau de jeu: ";  // Saisie de la hauteur
        cin >> height;
        cout << "Indiquez la densite d'obstacle (%): ";  // Saisie de la densité des obstacles
        cin >> obstacleDensity;
        cout << "Indiquez la densite de bonus (%): ";   // Saisie de la densité des bonus
        cin >> bonusDensity;

        Game game(width, height, obstacleDensity, bonusDensity);  // Création d'une instance de la classe Game
        game.play();  // Démarrage du jeu

        cout << "Voulez-vous rejouer ? (o/n): ";
        cin >> playAgain;
    } while (playAgain == 'o' || playAgain == 'O');

    return 0;
}