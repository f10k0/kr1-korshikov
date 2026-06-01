#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <string>

class Canvas {
private:
    int width, height;
    std::vector<std::string> grid;
    std::vector<std::string> history;
    
public:
    Canvas(int w = 80, int h = 24);
    
    void setPixel(int x, int y, char ch = '#');
    char getPixel(int x, int y) const;
    void drawLine(int x1, int y1, int x2, int y2);
    void floodFill(int x, int y);
    void clear();
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    void display() const;
    
    void saveToHistory();
    void undo();
    bool isHistoryEmpty() const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    std::vector<std::string> getFullState() const;
    void restoreState(const std::vector<std::string>& state);
};

#endif