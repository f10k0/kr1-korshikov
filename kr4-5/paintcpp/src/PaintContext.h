#ifndef PAINT_CONTEXT_H
#define PAINT_CONTEXT_H

#include <string>

class PaintContext {
private:
    bool drawMode;  //true - risovanie, false - stiranie
    
public:
    PaintContext();
    
    void toggleMode();
    bool isDrawMode() const;
    char getCursorSymbol() const;
    std::string getModeName() const;
};

#endif