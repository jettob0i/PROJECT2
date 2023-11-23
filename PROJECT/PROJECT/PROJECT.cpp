#include <iostream>
#include <cmath>
#include <glut.h>

const float WinWid = 800.0, WinHei = 600.0;
const float gravity = 2.0;
const float timeStep = 0.1;

class Ballista {
public:
    float angle;
    float x, y;

    Ballista(float x, float y) : x(x), y(y), angle(0.0) {}

    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0.0);
        glRotatef(angle, 0.0, 0.0, 1.0);

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POLYGON);
        glVertex2f(-20.0, -10.0);
        glVertex2f(20.0, -10.0);
        glVertex2f(20.0, 10.0);
        glVertex2f(-20.0, 10.0);
        glEnd();

        glPopMatrix();
    }
};

class Projectile {
public:
    float x, y, angle;
    float speed, mass;
    bool shooting;

    Projectile() : x(0.0), y(0.0), angle(0.0), speed(0.0), mass(1.0), shooting(false) {}

    void updatePosition(float time) {
        float effectiveGravity = gravity * (1.0 + mass / 10.0);

        x += speed * cos(angle * 3.1415 / 180.0) * time;
        y += speed * sin(angle * 3.1415 / 180.0) * time - 0.5 * effectiveGravity * time * time;
    }

    void draw() {
        glPushMatrix();
        glTranslatef(x, y, 0.0);
        glRotatef(angle, 0.0, 0.0, 1.0);

        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_POLYGON);
        glVertex2f(-5.0, -5);
        glVertex2f(5.0, -5);
        glVertex2f(5.0, 5);
        glVertex2f(-5.0, 5);
        glEnd();

        glPopMatrix();
    }

    bool checkCollision(const Ballista& ballista) const {
        float distance = std::sqrt((x - ballista.x) * (x - ballista.x) + (y - ballista.y) * (y - ballista.y));
        return distance < 20.0; // Assuming the ballista has a radius of 20.0
    }
};

void updateGameWrapper(int value);
static void drawWrapper();
void init();

class Game {
    Ballista ballista1, ballista2;
    Projectile projectile1, projectile2;
    int activePlayer;

public:
    Game() : ballista1(-WinWid / 4, 0), ballista2(WinWid / 4, 0), activePlayer(1) {}

    void getPlayerInput() {
        if (activePlayer == 1) {
            std::cout << "Player 1, enter the initial angle for the ballista: ";
            std::cin >> ballista1.angle;
            projectile1.angle = ballista1.angle;

            std::cout << "Player 1, enter the initial speed for the projectile (1-10): ";
            std::cin >> projectile1.speed;

            std::cout << "Player 1, enter the mass for the projectile (1-10): ";
            std::cin >> projectile1.mass;

            projectile1.x = ballista1.x;
            projectile1.y = ballista1.y;
            projectile1.shooting = true;
            activePlayer = 2;

            projectile2.shooting = false;
            projectile2.x = ballista2.x;
            projectile2.y = ballista2.y;
            projectile2.angle = 0.0;
            ballista2.angle = 0.0;

        }
        else {
            std::cout << "Player 2, enter the initial angle for the ballista: ";
            std::cin >> ballista2.angle;
            projectile2.angle = ballista2.angle;

            std::cout << "Player 2, enter the initial speed for the projectile (1-10): ";
            std::cin >> projectile2.speed;

            std::cout << "Player 2, enter the mass for the projectile (1-10): ";
            std::cin >> projectile2.mass;

            projectile2.x = ballista2.x;
            projectile2.y = ballista2.y;
            projectile2.shooting = true;
            activePlayer = 1;

            // Сброс параметров снаряда при смене активного игрока
            projectile1.shooting = false;
            projectile1.x = ballista1.x;
            projectile1.y = ballista1.y;
            projectile1.angle = 0.0;
            ballista1.angle = 0.0;

        }

        if (projectile1.shooting || projectile2.shooting) {
            glutMainLoop();
            init();
            glutDisplayFunc(drawWrapper);
            glutTimerFunc(16, updateGameWrapper, 0);
        }
    }


    void updateGame(int value) {
        if (projectile1.shooting) {
            projectile1.updatePosition(value * timeStep);
            if (projectile1.y < -WinHei / 2) {
                std::cout << "Player 1 misses! Player 2 gets a chance.\n";
                projectile1.shooting = false;
                activePlayer = 2;

                projectile1.x = ballista1.x;
                projectile1.y = ballista1.y;
                projectile1.angle = 0.0;
                ballista1.angle = 0.0;

                getPlayerInput();
                glutTimerFunc(16, updateGameWrapper, value + 1);
            }
            else if (projectile1.checkCollision(ballista2)) {
                std::cout << "Player 1 hits! Player 1 wins!\n";
                exit(0);
            }
        }

        if (projectile2.shooting) {
            projectile2.updatePosition(value * timeStep);
            if (projectile2.y < -WinHei / 2) {
                std::cout << "Player 2 misses! Player 1 gets a chance.\n";
                projectile2.shooting = false;
                activePlayer = 1;

                projectile2.x = ballista2.x;
                projectile2.y = ballista2.y;
                projectile2.angle = 0.0;
                ballista2.angle = 0.0;

                getPlayerInput();
                glutTimerFunc(16, updateGameWrapper, value + 1);
            }
            else if (projectile2.checkCollision(ballista1)) {
                std::cout << "Player 2 hits! Player 2 wins!\n";
                exit(0);
            }
        }

        glutPostRedisplay();
        glutTimerFunc(16, updateGameWrapper, value + 1);
    }

    void draw() {
        glClear(GL_COLOR_BUFFER_BIT);

        ballista1.draw();
        ballista2.draw();

        if (projectile1.shooting) {
            projectile1.draw();
        }

        if (projectile2.shooting) {
            projectile2.draw();
        }

        glutSwapBuffers();

        glutPostRedisplay();
    }
};

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WinWid / 2, WinWid / 2, -WinHei / 2, WinHei / 2, -200.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

static Game game;

static void drawWrapper() {
    game.draw();
}

void updateGameWrapper(int value) {
    game.updateGame(value);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(static_cast<int>(WinWid), static_cast<int>(WinHei));
    glutCreateWindow("Баллистическая игра");

    init();
    glutDisplayFunc(drawWrapper);
    glutTimerFunc(16, updateGameWrapper, 0);

    game.getPlayerInput();

    glutMainLoop();
    return 0;
}