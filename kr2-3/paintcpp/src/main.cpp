#include "Canvas.h"
#include "PaintContext.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void drawCursor(Canvas& canvas, PaintContext& ctx, int cursorX, int cursorY) {
    gotoxy(cursorX + 1, cursorY + 1);
    cout << ctx.getCursorSymbol();
    gotoxy(0, canvas.getHeight() + 8);
}

void drawScreen(Canvas& canvas, PaintContext& ctx, int cursorX, int cursorY) {
    canvas.display();
    
    cout << "\n";
    cout << "===============================================================================\n";
    cout << "  UPRAVLENIE:\n";
    cout << "  STRELKI    - dvizhenie kursora\n";
    cout << "  PROBEL     - risovat/stirat\n";
    cout << "  R          - rezhim (seichas: " << ctx.getModeName() << ")\n";
    cout << "  L          - narisovat liniu\n";
    cout << "  F          - zalit oblast\n";
    cout << "  C          - ochistit holst\n";
    cout << "  Ctrl+Z     - otmena\n";
    cout << "  S          - sohranit\n";
    cout << "  O          - zagruzit\n";
    cout << "  Q          - vyhod\n";
    cout << "===============================================================================\n";
    cout << "  Positsiya: X=" << cursorX << " Y=" << cursorY << " | Simvol: " << canvas.getPixel(cursorX, cursorY) << "\n";
    cout << "  > ";
}

void drawLineMode(Canvas& canvas, int& cursorX, int& cursorY, PaintContext& context) {
    cout << "\n  [REZHIM LINII]\n";
    cout << "  Peremestite kursor v NACHALNUYU tochku i nazhmite PROBEL\n";
    cout << "  Dlya otmeny nazhmite ESC\n\n";
    
    bool firstPointSet = false;
    int x1 = 0, y1 = 0;
    
    drawScreen(canvas, context, cursorX, cursorY);
    drawCursor(canvas, context, cursorX, cursorY);
    
    while (!firstPointSet) {
        int key = _getch();
        if (key == 224) {
            key = _getch();
            switch (key) {
                case 72: if (cursorY > 0) cursorY--; break;
                case 80: if (cursorY < canvas.getHeight() - 1) cursorY++; break;
                case 75: if (cursorX > 0) cursorX--; break;
                case 77: if (cursorX < canvas.getWidth() - 1) cursorX++; break;
            }
            drawScreen(canvas, context, cursorX, cursorY);
            drawCursor(canvas, context, cursorX, cursorY);
            cout << "\n  [REZHIM LINII] Nachalnaya tochka: X=" << cursorX << " Y=" << cursorY << "\n";
            cout << "  Nazhmite PROBEL dlya vybora ili ESC dlya otmeny\n";
            cout << "  > ";
        }
        else if (key == 32) {
            x1 = cursorX;
            y1 = cursorY;
            firstPointSet = true;
            cout << "\n  Nachalnaya tochka: (" << x1 << "," << y1 << ")\n";
            cout << "  Teper peremestite kursor v KONECHNUYU tochku i nazhmite PROBEL\n";
        }
        else if (key == 27) {
            cout << "\n  [LINIYA OTMENENA]\n";
            Sleep(800);
            return;
        }
    }
    
    int x2 = 0, y2 = 0;
    bool secondPointSet = false;
    
    drawScreen(canvas, context, cursorX, cursorY);
    drawCursor(canvas, context, cursorX, cursorY);
    
    while (!secondPointSet) {
        int key = _getch();
        if (key == 224) {
            key = _getch();
            switch (key) {
                case 72: if (cursorY > 0) cursorY--; break;
                case 80: if (cursorY < canvas.getHeight() - 1) cursorY++; break;
                case 75: if (cursorX > 0) cursorX--; break;
                case 77: if (cursorX < canvas.getWidth() - 1) cursorX++; break;
            }
            drawScreen(canvas, context, cursorX, cursorY);
            drawCursor(canvas, context, cursorX, cursorY);
            cout << "\n  [REZHIM LINII] Konechnaya tochka: X=" << cursorX << " Y=" << cursorY << "\n";
            cout << "  Nazhmite PROBEL dlya risovaniya linii ili ESC dlya otmeny\n";
            cout << "  > ";
        }
        else if (key == 32) {
            x2 = cursorX;
            y2 = cursorY;
            secondPointSet = true;
        }
        else if (key == 27) {
            cout << "\n  [LINIYA OTMENENA]\n";
            Sleep(800);
            return;
        }
    }
    
    canvas.drawLine(x1, y1, x2, y2);
    cout << "\n  [LINIYA NARISOVANA]\n";
    Sleep(800);
}

int main() {
    system("cls");
    hideCursor();
    
    cout << "\n\n\n";
    cout << "  ================================================================\n";
    cout << "         ASCII PAINT - KONSOLE EDITOR\n";
    cout << "  ================================================================\n\n\n";
    cout << "  Nazhmite lubuyu klavishu dlya nachala...";
    cin.get();
    
    Canvas canvas;
    PaintContext context;
    int cursorX = canvas.getWidth() / 2;
    int cursorY = canvas.getHeight() / 2;
    bool running = true;
    
    canvas.saveToHistory();
    drawScreen(canvas, context, cursorX, cursorY);
    drawCursor(canvas, context, cursorX, cursorY);
    
    while (running) {
        int key = _getch();
        
        if (key == 26) {
            canvas.undo();
            drawScreen(canvas, context, cursorX, cursorY);
            drawCursor(canvas, context, cursorX, cursorY);
        }
        else if (key == 224) {
            key = _getch();
            switch (key) {
                case 72: if (cursorY > 0) cursorY--; break;
                case 80: if (cursorY < canvas.getHeight() - 1) cursorY++; break;
                case 75: if (cursorX > 0) cursorX--; break;
                case 77: if (cursorX < canvas.getWidth() - 1) cursorX++; break;
            }
            drawScreen(canvas, context, cursorX, cursorY);
            drawCursor(canvas, context, cursorX, cursorY);
        }
        else if (key == 32) {
            canvas.saveToHistory();
            if (context.isDrawMode()) {
                canvas.setPixel(cursorX, cursorY, '#');
            } else {
                canvas.setPixel(cursorX, cursorY, '.');
            }
            drawScreen(canvas, context, cursorX, cursorY);
            drawCursor(canvas, context, cursorX, cursorY);
        }
        else {
            char ch = tolower(key);
            switch (ch) {
                case 'r':
                    context.toggleMode();
                    drawScreen(canvas, context, cursorX, cursorY);
                    drawCursor(canvas, context, cursorX, cursorY);
                    break;
                case 'c':
                    canvas.saveToHistory();
                    canvas.clear();
                    drawScreen(canvas, context, cursorX, cursorY);
                    drawCursor(canvas, context, cursorX, cursorY);
                    break;
                case 'f':
                    canvas.saveToHistory();
                    canvas.floodFill(cursorX, cursorY);
                    drawScreen(canvas, context, cursorX, cursorY);
                    drawCursor(canvas, context, cursorX, cursorY);
                    break;
                case 'l':
                    canvas.saveToHistory();
                    drawLineMode(canvas, cursorX, cursorY, context);
                    drawScreen(canvas, context, cursorX, cursorY);
                    drawCursor(canvas, context, cursorX, cursorY);
                    break;
                case 's':
                    if (canvas.save("drawing.txt"))
                        cout << "\n  [SOHRANENO]";
                    Sleep(800);
                    drawScreen(canvas, context, cursorX, cursorY);
                    drawCursor(canvas, context, cursorX, cursorY);
                    break;
                case 'o':
                    if (canvas.load("drawing.txt"))
                        cout << "\n  [ZAGRUZHENO]";
                    Sleep(800);
                    drawScreen(canvas, context, cursorX, cursorY);
                    drawCursor(canvas, context, cursorX, cursorY);
                    break;
                case 'q':
                    running = false;
                    break;
            }
        }
    }
    
    system("cls");
    cout << "\n\n  Spasibo za ispolzovanie ASCII Paint!\n\n";
    
    return 0;
}