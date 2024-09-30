#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>  // Pour utiliser _kbhit() et _getch() sur Windows
#include <windows.h>  // Pour utiliser Sleep()

using namespace std;

// Constantes de la carte
const int MAP_WIDTH = 30;
const int MAP_HEIGHT = 30;
const char EMPTY = '.';
const char OBSTACLE = '#'; // Mur
const char ROBOT = 'R';

// Classe pour la carte
class Map {
public:
    Map();
    void generateMap(int obstacleDensity);
    void printMap() const;
    char getCell(int x, int y) const;
    void setCell(int x, int y, char value);

private:
    vector<vector<char>> grid;
};

// Classe pour le robot
class Robot {
public:
    Robot(int startX, int startY);
    void moveManually(Map& map, char direction);
    void moveAutomatically(Map& map, int targetX, int targetY);

private:
    int x, y;
};

// Constructeur de la carte
Map::Map() : grid(MAP_HEIGHT, vector<char>(MAP_WIDTH, EMPTY)) {}

// Generation de la carte avec obstacles aleatoires
void Map::generateMap(int obstacleDensity) {
    srand(time(0));
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            grid[y][x] = (rand() % 100 < obstacleDensity) ? OBSTACLE : EMPTY;
        }
    }
}

// Affichage de la carte
void Map::printMap() const {
    system("cls");  // Efface l'affichage console pour une mise a jour propre
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
}

// Recuperation du contenu d'une cellule de la carte
char Map::getCell(int x, int y) const {
    return grid[y][x];
}

// Modification du contenu d'une cellule de la carte
void Map::setCell(int x, int y, char value) {
    grid[y][x] = value;
}

// Constructeur du robot
Robot::Robot(int startX, int startY) : x(startX), y(startY) {}

// Mouvement manuel du robot avec ZQSD
void Robot::moveManually(Map& map, char direction) {
    map.setCell(x, y, EMPTY);  // Efface l'ancienne position du robot

    switch (direction) {
    case 'z': // Haut
        if (y > 0 && map.getCell(x, y - 1) != OBSTACLE) y--;
        break;
    case 's': // Bas
        if (y < MAP_HEIGHT - 1 && map.getCell(x, y + 1) != OBSTACLE) y++;
        break;
    case 'q': // Gauche
        if (x > 0 && map.getCell(x - 1, y) != OBSTACLE) x--;
        break;
    case 'd': // Droite
        if (x < MAP_WIDTH - 1 && map.getCell(x + 1, y) != OBSTACLE) x++;
        break;
    default:
        break;
    }

    map.setCell(x, y, ROBOT);  // Place le robot a la nouvelle position
    map.printMap();  // Mise a jour de l'affichage
}

// Mouvement automatique vers une cible specifique
void Robot::moveAutomatically(Map& map, int targetX, int targetY) {
    while (x != targetX || y != targetY) {
        map.setCell(x, y, EMPTY);  // Efface l'ancienne position du robot

        // Deplacement optimisé vers la cible
        if (x < targetX && map.getCell(x + 1, y) != OBSTACLE) {
            x++;  // Se déplace vers la droite
        }
        else if (x > targetX && map.getCell(x - 1, y) != OBSTACLE) {
            x--;  // Se déplace vers la gauche
        }
        else if (y < targetY && map.getCell(x, y + 1) != OBSTACLE) {
            y++;  // Se déplace vers le bas
        }
        else if (y > targetY && map.getCell(x, y - 1) != OBSTACLE) {
            y--;  // Se déplace vers le haut
        }
        else {
            cout << "Un obstacle bloque le chemin !" << endl;
            break;
        }

        map.setCell(x, y, ROBOT);  // Place le robot a la nouvelle position
        map.printMap();  // Mise a jour de l'affichage
        Sleep(500);  // Pause pour ralentir l'animation du mouvement (500 ms)
    }

    if (x == targetX && y == targetY) {
        cout << "Le robot a atteint sa destination !" << endl;
    }
}

// Fonction principale
int main() {
    Map map;
    Robot robot(0, 0);

    // Generation de la carte avec densite d'obstacles definie par l'utilisateur
    cout << "Choisissez la densite des obstacles (0-100) : ";
    int density;
    cin >> density;
    map.generateMap(density);

    // Placement initial du robot
    map.setCell(0, 0, ROBOT);
    map.printMap();

    // Choix du mode de deplacement
    cout << "Choisissez le mode de deplacement : 1 = manuel, 2 = automatique : ";
    int mode;
    cin >> mode;

    if (mode == 1) {
        // Mode manuel avec capture des touches ZQSD
        while (true) {
            if (_kbhit()) {  // Verifie si une touche a ete pressee
                char direction = _getch();  // Recupere la touche pressee
                robot.moveManually(map, direction);  // Deplace le robot
            }
        }
    }
    else if (mode == 2) {
        // Mode automatique vers des coordonnees donnees
        int targetX, targetY;
        cout << "Entrez les coordonnees cibles (x y) : ";
        cin >> targetX >> targetY;

        // Verifie si la cible est valide
        if (targetX < 0 || targetX >= MAP_WIDTH || targetY < 0 || targetY >= MAP_HEIGHT) {
            cout << "Coordonnees invalides. Terminaison du programme." << endl;
        }
        else {
            robot.moveAutomatically(map, targetX, targetY);  // Deplacement automatique
        }
    }
    else {
        cout << "Mode invalide." << endl;
    }

    return 0;
}
