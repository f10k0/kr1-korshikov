#include "Canvas.h"
#include "PaintContext.h"
#include <string>

#define K_UP    1001
#define K_DOWN  1002
#define K_LEFT  1003
#define K_RIGHT 1004
#define K_ESC   27
#define K_SPACE 32
#define K_CTRLZ 26

#ifdef _WIN32
#include <conio.h>
#include <windows.h>

void platformInit()        {}
void platformCleanup()     { system("cls"); }
void platformSleep(int ms) { Sleep(ms); }

void gotoxy(int x, int y) {
    COORD c; c.X = x; c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void hideCursor() {
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
    ci.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}
int readKey() {
    int k = _getch();
    if (k == 224) {
        int k2 = _getch();
        if (k2 == 72) return K_UP;
        if (k2 == 80) return K_DOWN;
        if (k2 == 75) return K_LEFT;
        if (k2 == 77) return K_RIGHT;
    }
    return k;
}

#else
#include <ncurses.h>
#include <signal.h>
#include <iostream>

void platformInit() {
    signal(SIGTSTP, SIG_IGN);
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}
void platformCleanup() { endwin(); }
void platformSleep(int) {}

int readKey() {
    int k = getch();
    if (k == KEY_UP)    return K_UP;
    if (k == KEY_DOWN)  return K_DOWN;
    if (k == KEY_LEFT)  return K_LEFT;
    if (k == KEY_RIGHT) return K_RIGHT;
    return k;
}
#endif

// Состояния программы
enum AppState {
    STATE_NORMAL,       // обычный режим
    STATE_LINE_FIRST,   // режим линии: ждём первую точку
    STATE_LINE_SECOND   // режим линии: ждём вторую точку
};

// Отрисовка — одна функция для обеих платформ
void render(Canvas& canvas, PaintContext& ctx,
            int cx, int cy,
            AppState state, int x1, int y1,
            const std::string& msg)
{
#ifdef _WIN32
    canvas.display();
    std::cout << "\n";
    std::cout << "=======================================================================\n";
    if (state == STATE_NORMAL) {
        std::cout << "  STRELKI-dvizh | PROBEL-pix | R-rezhim | L-liniya | F-fill\n";
        std::cout << "  C-clear | Ctrl+Z/U-undo | S-save | O-load | Q-quit\n";
    } else if (state == STATE_LINE_FIRST) {
        std::cout << "  [LINIYA] Peremesti kursor i nazmi PROBEL - pervaya tochka\n";
        std::cout << "  ESC - otmena\n";
    } else {
        std::cout << "  [LINIYA] Nachalo:(" << x1 << "," << y1
                  << ") Nazmi PROBEL - vtoraya tochka  ESC - otmena\n";
        std::cout << "\n";
    }
    std::cout << "  Rezhim: " << ctx.getModeName()
              << "  X=" << cx << " Y=" << cy << "\n";
    if (!msg.empty()) std::cout << "  >> " << msg << "\n";
    std::cout << "=======================================================================\n> ";
    gotoxy(cx + 1, cy + 1);
    std::cout << ctx.getCursorSymbol();
    gotoxy(0, canvas.getHeight() + 8);
#else
    clear();
    // Верхняя граница
    mvaddch(0, 0, '+');
    for (int x = 0; x < canvas.getWidth(); x++) mvaddch(0, x+1, '-');
    mvaddch(0, canvas.getWidth()+1, '+');
    // Холст
    for (int y = 0; y < canvas.getHeight(); y++) {
        mvaddch(y+1, 0, '|');
        for (int x = 0; x < canvas.getWidth(); x++) {
            char ch;
            if (x == cx && y == cy)
                ch = ctx.getCursorSymbol();
            else if (state == STATE_LINE_SECOND && x == x1 && y == y1)
                ch = '*'; // первая точка линии
            else
                ch = canvas.getPixel(x, y);
            mvaddch(y+1, x+1, ch);
        }
        mvaddch(y+1, canvas.getWidth()+1, '|');
    }
    // Нижняя граница
    int br = canvas.getHeight() + 1;
    mvaddch(br, 0, '+');
    for (int x = 0; x < canvas.getWidth(); x++) mvaddch(br, x+1, '-');
    mvaddch(br, canvas.getWidth()+1, '+');
    // Панель управления
    int r = br + 2;
    if (state == STATE_NORMAL) {
        mvprintw(r,   0, "  STRELKI-dvizh | PROBEL-pix | R-rezhim | L-liniya | F-fill");
        mvprintw(r+1, 0, "  C-clear | U-undo | S-save | O-load | Q-quit");
    } else if (state == STATE_LINE_FIRST) {
        mvprintw(r,   0, "  [LINIYA] Pervaya tochka: nazmi PROBEL");
        mvprintw(r+1, 0, "  ESC - otmena rezhima linii");
    } else {
        mvprintw(r,   0, "  [LINIYA] Nachalo:(%d,%d)  Vtoraya tochka: nazmi PROBEL", x1, y1);
        mvprintw(r+1, 0, "  ESC - otmena rezhima linii");
    }
    mvprintw(r+2, 0, "  Rezhim: %-22s  X=%-3d Y=%-3d",
             ctx.getModeName().c_str(), cx, cy);
    if (!msg.empty())
        mvprintw(r+3, 0, "  >> %-50s", msg.c_str());
    else
        mvprintw(r+3, 0, "%s", "");
    refresh();
#endif
}

// main
int main() {
    platformInit();

#ifdef _WIN32
    hideCursor();
    std::cout << "\n\n\n";
    std::cout << "  ================================================================\n";
    std::cout << "         ASCII PAINT - KONSOLE EDITOR\n";
    std::cout << "  ================================================================\n\n\n";
    std::cout << "  Nazhmite lubuyu klavishu dlya nachala...";
    readKey();
#endif

    Canvas canvas;
    PaintContext context;
    int cx = canvas.getWidth()  / 2;
    int cy = canvas.getHeight() / 2;

    AppState state = STATE_NORMAL;
    int lineX1 = 0, lineY1 = 0;
    std::string msg;

    canvas.saveToHistory();
    render(canvas, context, cx, cy, state, lineX1, lineY1, msg);

    bool running = true;
    while (running) {
        int key = readKey();
        msg = "";

        // Движение курсора работает в любом состоянии
        if (key == K_UP    && cy > 0)                    { cy--; }
        else if (key == K_DOWN  && cy < canvas.getHeight()-1) { cy++; }
        else if (key == K_LEFT  && cx > 0)                    { cx--; }
        else if (key == K_RIGHT && cx < canvas.getWidth()-1)  { cx++; }

        // ESC — выход из любого режима обратно в normal
        else if (key == K_ESC) {
            if (state != STATE_NORMAL) {
                state = STATE_NORMAL;
                msg = "[OTMENENO]";
            }
        }

        // Действия зависят от текущего состояния
        else if (state == STATE_NORMAL) {
            switch (key) {
                case K_SPACE:
                    canvas.saveToHistory();
                    canvas.setPixel(cx, cy, context.isDrawMode() ? '#' : '.');
                    break;
                case K_CTRLZ:
                case 'u': case 'U':
                    if (!canvas.isHistoryEmpty()) {
                        canvas.undo();
                        msg = "[OTMENA]";
                    } else {
                        msg = "[NET DEYSTVIY]";
                    }
                    break;
                case 'r': case 'R':
                    context.toggleMode();
                    break;
                case 'c': case 'C':
                    canvas.saveToHistory();
                    canvas.clear();
                    msg = "[OCHISCHENO]";
                    break;
                case 'f': case 'F':
                    canvas.saveToHistory();
                    canvas.floodFill(cx, cy);
                    break;
                case 'l': case 'L':
                    // Переходим в режим линии — ждём первую точку
                    state = STATE_LINE_FIRST;
                    break;
                case 's': case 'S':
                    canvas.save("drawing.txt");
                    msg = "[SOHRANENO]";
                    break;
                case 'o': case 'O':
                    canvas.load("drawing.txt");
                    msg = "[ZAGRUZHENO]";
                    break;
                case 'q': case 'Q':
                    running = false;
                    break;
            }
        }

        else if (state == STATE_LINE_FIRST) {
            if (key == K_SPACE) {
                // Запомнили первую точку, ждём вторую
                lineX1 = cx;
                lineY1 = cy;
                state = STATE_LINE_SECOND;
            }
        }

        else if (state == STATE_LINE_SECOND) {
            if (key == K_SPACE) {
                // Рисуем линию и выходим из режима
                canvas.saveToHistory();
                canvas.drawLine(lineX1, lineY1, cx, cy);
                state = STATE_NORMAL;
                msg = "[LINIYA NARISOVANA]";
            }
        }

        if (running) {
            render(canvas, context, cx, cy, state, lineX1, lineY1, msg);
        }
    }

    platformCleanup();
    std::cout << "\n\n  Spasibo za ispolzovanie ASCII Paint!\n\n";
    return 0;
}
