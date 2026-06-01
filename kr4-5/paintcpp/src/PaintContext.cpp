#include "PaintContext.h"

PaintContext::PaintContext() : drawMode(true) {}

void PaintContext::toggleMode() {
    drawMode = !drawMode;
}

bool PaintContext::isDrawMode() const {
    return drawMode;
}

char PaintContext::getCursorSymbol() const {
    return drawMode ? '#' : '%';
}

std::string PaintContext::getModeName() const {
    return drawMode ? "RISOVANIE [#]" : "STIRANIE [%]";
}