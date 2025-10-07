// tictactoe_opengl.cpp
// Simple Tic-Tac-Toe (Three-in-a-row) using GLFW + OpenGL immediate mode.
//
// Controls:
//  - Click on a cell to place X or O (X goes first).
//  - Press R to restart.
//  - Press Esc to quit.

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <array>
#include <string>

const int WIN_W = 600;
const int WIN_H = 600;

int board[9]; // 0 empty, 1 = X, -1 = O
int currentPlayer = 1; // 1 => X, -1 => O
bool gameOver = false;
int winner = 0; // 0 none, 1 X, -1 O, 2 draw

void resetGame() {
    for(int i=0;i<9;i++) board[i] = 0;
    currentPlayer = 1;
    gameOver = false;
    winner = 0;
}

int checkWinner() {
    
    const int lines[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},
        {0,3,6},{1,4,7},{2,5,8},
        {0,4,8},{2,4,6}
    };
    for(int i=0;i<8;i++){
        int a=lines[i][0], b=lines[i][1], c=lines[i][2];
        int sum = board[a] + board[b] + board[c];
        if(sum == 3) return 1;
        if(sum == -3) return -1;
    }
    bool anyEmpty=false;
    for(int i=0;i<9;i++) if(board[i]==0) { anyEmpty=true; break; }
    if(!anyEmpty) return 2; 
    return 0;
}

void drawLine(float x1, float y1, float x2, float y2, float w=6.0f) {
    glLineWidth(w);
    glBegin(GL_LINES);
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glEnd();
}

void drawX(float cx, float cy, float hs) {
    glLineWidth(8.0f);
    glBegin(GL_LINES);
      glVertex2f(cx - hs, cy - hs);
      glVertex2f(cx + hs, cy + hs);
      glVertex2f(cx - hs, cy + hs);
      glVertex2f(cx + hs, cy - hs);
    glEnd();
}

void drawO(float cx, float cy, float r) {
    glLineWidth(8.0f);
    glBegin(GL_LINE_STRIP);
    const int SEG = 40;
    for(int i=0;i<=SEG;i++){
        float a = (float)i / (float)SEG * 2.0f * 3.1415926f;
        glVertex2f(cx + cosf(a)*r, cy + sinf(a)*r);
    }
    glEnd();
}

void renderBoard(int w, int h) {
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glColor3f(0.15f, 0.15f, 0.15f);
    drawLine(-1.0f/3.0f, -1.0f, -1.0f/3.0f, 1.0f, 8.0f);
    drawLine( 1.0f/3.0f, -1.0f,  1.0f/3.0f, 1.0f, 8.0f);

    drawLine(-1.0f, -1.0f/3.0f, 1.0f, -1.0f/3.0f, 8.0f);
    drawLine(-1.0f,  1.0f/3.0f, 1.0f,  1.0f/3.0f, 8.0f);

    for(int r=0;r<3;r++){
        for(int c=0;c<3;c++){
            int idx = r*3 + c;
            int val = board[idx];
            float cellW = 2.0f/3.0f;
            float cx = -1.0f + cellW*(c+0.5f);
            float cy =  1.0f - cellW*(r+0.5f); 
            if(val == 1) {
                glColor3f(0.9f, 0.2f, 0.2f); 
                drawX(cx, cy, cellW*0.28f);
            } else if(val == -1) {
                glColor3f(0.12f, 0.2f, 0.9f); 
                drawO(cx, cy, cellW*0.28f);
            }
        }
    }


    if(gameOver) {
        glColor4f(0.0f,0.0f,0.0f,0.35f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
          glVertex2f(-1,-1); glVertex2f(1,-1);
          glVertex2f(1,1);  glVertex2f(-1,1);
        glEnd();
        glDisable(GL_BLEND);

        if(winner == 1 || winner == -1) {
            const int lines[8][3] = {
                {0,1,2},{3,4,5},{6,7,8},
                {0,3,6},{1,4,7},{2,5,8},
                {0,4,8},{2,4,6}
            };
            for(int i=0;i<8;i++){
                int a=lines[i][0], b=lines[i][1], c=lines[i][2];
                int sum = board[a]+board[b]+board[c];
                if(sum == 3*winner){
                    int rc = c/3, cc = c%3;
                    float cellW = 2.0f/3.0f;
                    float cxa = -1.0f + cellW*(ca+0.5f);
                    float cya =  1.0f - cellW*(ra+0.5f);
                    float cxc = -1.0f + cellW*(cc+0.5f);
                    float cyc =  1.0f - cellW*(rc+0.5f);
                    if(winner==1) glColor3f(1.0f,0.7f,0.2f); else glColor3f(0.2f,0.9f,0.7f);
                    drawLine(cxa,cya,cxc,cyc, 10.0f);
                    break;
                }
            }
        }
    }
}

int windowCoordsToCell(double mx, double my, int w, int h) {

    float nx = (float)(mx / w) * 2.0f - 1.0f;
    float ny = -((float)(my / h) * 2.0f - 1.0f); // invert y

    float cellW = 2.0f/3.0f;
    int col = (int)floor((nx + 1.0f) / cellW);
    int row = (int)floor((1.0f - ny) / cellW); // careful with inversion
    if(col<0 || col>2 || row<0 || row>2) return -1;
    return row*3 + col;
}

void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) {
    if(button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;
    if(gameOver) return; // ignore clicks after game over (press R to restart)
    double mx, my;
    int w,h;
    glfwGetCursorPos(win, &mx, &my);
    glfwGetWindowSize(win, &w, &h);
    int idx = windowCoordsToCell(mx,my,w,h);
    if(idx >= 0 && idx < 9 && board[idx] == 0) {
        board[idx] = currentPlayer;
        int cw = checkWinner();
        if(cw != 0) {
            gameOver = true;
            winner = cw;
        } else {
            currentPlayer = -currentPlayer;
        }
    }
}

void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if(action != GLFW_PRESS) return;
    if(key == GLFW_KEY_R) {
        resetGame();
    } else if(key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
    }
}

int main() {
    if(!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return -1;
    }

    // Try to create a compatibility profile window to allow immediate mode drawing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow* window = glfwCreateWindow(WIN_W, WIN_H, "Three in a Row - Tic Tac Toe", NULL, NULL);
    if(!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    resetGame();

    while(!glfwWindowShouldClose(window)) {
        int w,h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0,0,w,h);
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderBoard(w,h);

        // optional: draw simple HUD (which player turn) using small squares (no real text)
        // show small indicator top-left
        if(!gameOver) {
            if(currentPlayer == 1) glColor3f(0.9f,0.2f,0.2f); else glColor3f(0.12f,0.2f,0.9f);
            glBegin(GL_QUADS);
              glVertex2f(-0.98f,0.92f); glVertex2f(-0.88f,0.92f);
              glVertex2f(-0.88f,0.82f); glVertex2f(-0.98f,0.82f);
            glEnd();
        } else {
            // draw a small green/red based on winner/draw
            if(winner == 2) glColor3f(0.4f,0.5f,0.4f); // draw color
            else if(winner == 1) glColor3f(1.0f,0.7f,0.2f);
            else glColor3f(0.2f,0.9f,0.7f);
            glBegin(GL_QUADS);
              glVertex2f(-0.98f,0.92f); glVertex2f(-0.88f,0.92f);
              glVertex2f(-0.88f,0.82f); glVertex2f(-0.98f,0.82f);
            glEnd();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
