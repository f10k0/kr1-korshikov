#include "Canvas.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

const char EMPTY = '.';
const char DRAW  = '#';

Canvas::Canvas(int w, int h) : width(w), height(h), grid(h, string(w, EMPTY)) {}

void Canvas::setPixel(int x, int y, char ch) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        grid[y][x] = ch;
}

char Canvas::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height)
        return grid[y][x];
    return '\0';
}

void Canvas::drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2-x1), dy = abs(y2-y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (true) {
        setPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

void Canvas::floodFill(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    char target      = getPixel(x, y);
    char replacement = (target == EMPTY) ? DRAW : EMPTY;
    if (target == replacement) return;
    queue<pair<int,int>> q;
    q.push({x, y});
    while (!q.empty()) {
        auto [cx, cy] = q.front(); q.pop();
        if (cx < 0 || cx >= width || cy < 0 || cy >= height) continue;
        if (getPixel(cx, cy) != target) continue;
        setPixel(cx, cy, replacement);
        q.push({cx+1,cy}); q.push({cx-1,cy});
        q.push({cx,cy+1}); q.push({cx,cy-1});
    }
}

void Canvas::clear() {
    for (auto& row : grid)
        fill(row.begin(), row.end(), EMPTY);
}

bool Canvas::save(const string& filename) const {
    ofstream out(filename);
    if (!out) return false;
    out << width << " " << height << "\n";
    for (const auto& row : grid)
        out << row << "\n";
    return true;
}

bool Canvas::load(const string& filename) {
    ifstream in(filename);
    if (!in) return false;
    int w, h;
    in >> w >> h;
    if (w != width || h != height) return false;
    in.ignore();
    grid.clear();
    string line;
    for (int i = 0; i < height; ++i) {
        getline(in, line);
        if ((int)line.length() < width)
            line.append(width - line.length(), EMPTY);
        grid.push_back(line);
    }
    return true;
}

// display() используется только в Windows-версии main.cpp
// В Linux рисует напрямую ncurses — этот метод там не вызывается
void Canvas::display() const {
#ifdef _WIN32
    system("cls");
    cout << "+" << string(width, '-') << "+\n";
    for (const auto& row : grid)
        cout << "|" << row << "|\n";
    cout << "+" << string(width, '-') << "+\n";
#endif
}

void Canvas::saveToHistory() {
    string saved;
    for (const auto& row : grid)
        saved += row + "\n";
    history.push_back(saved);
    if (history.size() > 20)
        history.erase(history.begin());
}

void Canvas::undo() {
    if (!history.empty()) {
        string lastState = history.back();
        history.pop_back();
        stringstream ss(lastState);
        for (int y = 0; y < height; y++)
            getline(ss, grid[y]);
    }
}

bool Canvas::isHistoryEmpty() const { return history.empty(); }

vector<string> Canvas::getFullState() const { return grid; }

void Canvas::restoreState(const vector<string>& state) {
    if ((int)state.size() == height)
        grid = state;
}
