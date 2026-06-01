#include <gtest/gtest.h>
#include "Canvas.h"
#include <fstream>

// Canvas — Конструктор

// Тест 1: холст по умолчанию должен быть 80x24 — это жёстко заданный размер
// редактора. Если кто-то случайно поменяет константы в коде — тест сразу упадёт
TEST(CanvasConstructor, DefaultSize) {
    Canvas c;
    EXPECT_EQ(c.getWidth(), 80);
    EXPECT_EQ(c.getHeight(), 24);
}

// Тест 2: можно создать холст произвольного размера — это нужно для тестов,
// чтобы не гонять каждый раз полноразмерный 80x24 и не писать циклы на 1920 итераций
TEST(CanvasConstructor, CustomSize) {
    Canvas c(40, 10);
    EXPECT_EQ(c.getWidth(), 40);
    EXPECT_EQ(c.getHeight(), 10);
}

// Тест 3: сразу после создания все клетки должны быть '.' — без этого
// непонятно с какого состояния начинает пользователь
TEST(CanvasConstructor, InitiallyEmpty) {
    Canvas c(10, 5);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 10; x++)
            EXPECT_EQ(c.getPixel(x, y), '.');
}

// Тест 4: холст 1x1 — минимально возможный случай, просто проверяю что не падает
TEST(CanvasConstructor, MinimalCanvas) {
    Canvas c(1, 1);
    EXPECT_EQ(c.getWidth(), 1);
    EXPECT_EQ(c.getHeight(), 1);
    EXPECT_EQ(c.getPixel(0, 0), '.');
}

// Canvas — setPixel / getPixel

// Тест 5: ставим '#' в (0,0) и проверяем что он там оказался —
// базовая проверка что set и get работают в связке
TEST(CanvasPixel, SetTopLeft) {
    Canvas c(10, 10);
    c.setPixel(0, 0, '#');
    EXPECT_EQ(c.getPixel(0, 0), '#');
}

// Тест 6: правый нижний угол (9,9) при размере 10x10 — граничная координата,
// именно здесь чаще всего прячутся off-by-one ошибки
TEST(CanvasPixel, SetBottomRight) {
    Canvas c(10, 10);
    c.setPixel(9, 9, '#');
    EXPECT_EQ(c.getPixel(9, 9), '#');
}

// Тест 7: пытаемся нарисовать за пределами холста — ничего не должно измениться
// и программа не должна падать. setPixel молча игнорирует невалидные координаты
TEST(CanvasPixel, OutOfBoundsIgnored) {
    Canvas c(10, 10);
    c.setPixel(-1, 0, '#');
    c.setPixel(10, 0, '#');
    c.setPixel(0, -1, '#');
    c.setPixel(0, 10, '#');
    EXPECT_EQ(c.getPixel(0, 0), '.');
}

// Тест 8: getPixel за границей должен вернуть '\0' — это важно для floodFill,
// который проверяет соседей и должен корректно останавливаться на краю холста
TEST(CanvasPixel, GetOutOfBoundsReturnsNull) {
    Canvas c(10, 10);
    EXPECT_EQ(c.getPixel(-1, 0), '\0');
    EXPECT_EQ(c.getPixel(0, -1), '\0');
    EXPECT_EQ(c.getPixel(10, 10), '\0');
}

// Тест 9: ставим '#', потом поверх пишем '.' — старое значение должно затереться.
// Так работает стирание в редакторе
TEST(CanvasPixel, OverwritePixel) {
    Canvas c(10, 10);
    c.setPixel(5, 5, '#');
    c.setPixel(5, 5, '.');
    EXPECT_EQ(c.getPixel(5, 5), '.');
}

// Тест 10: setPixel принимает любой char, не только '#' — ставим '*'
// и проверяем что именно он там и оказался, без искажений
TEST(CanvasPixel, SetArbitraryChar) {
    Canvas c(10, 10);
    c.setPixel(3, 3, '*');
    EXPECT_EQ(c.getPixel(3, 3), '*');
}

// Canvas — clear()

// Тест 11: рисуем два пикселя, чистим — оба должны стать '.'
TEST(CanvasClear, ClearAfterDraw) {
    Canvas c(10, 10);
    c.setPixel(3, 3, '#');
    c.setPixel(5, 5, '#');
    c.clear();
    EXPECT_EQ(c.getPixel(3, 3), '.');
    EXPECT_EQ(c.getPixel(5, 5), '.');
}

// Тест 12: очистка уже пустого холста — проверяю что не падает в этом случае
TEST(CanvasClear, ClearEmptyCanvas) {
    Canvas c(10, 10);
    EXPECT_NO_THROW(c.clear());
}

// Тест 13: заполняем весь холст '#', чистим и проходим по каждой клетке —
// хочу убедиться что clear() не пропускает ни одну строку
TEST(CanvasClear, AllDotsAfterClear) {
    Canvas c(5, 5);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
            c.setPixel(x, y, '#');
    c.clear();
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
            EXPECT_EQ(c.getPixel(x, y), '.');
}

// Canvas — drawLine()

// Тест 14: горизонтальная линия от x=2 до x=8 — все точки на одной высоте
// должны стать '#' без пробелов, иначе алгоритм Брезенхема сломан
TEST(CanvasDrawLine, HorizontalLine) {
    Canvas c(20, 20);
    c.drawLine(2, 5, 8, 5);
    for (int x = 2; x <= 8; x++)
        EXPECT_EQ(c.getPixel(x, 5), '#');
}

// Тест 15: вертикальная линия — фиксируем x и идём по y.
// Проверяю отдельно потому что у Брезенхема разные ветки для dx > dy и наоборот
TEST(CanvasDrawLine, VerticalLine) {
    Canvas c(20, 20);
    c.drawLine(5, 2, 5, 8);
    for (int y = 2; y <= 8; y++)
        EXPECT_EQ(c.getPixel(5, y), '#');
}

// Тест 16: оба конца совпадают — должен поставиться ровно один пиксель
TEST(CanvasDrawLine, SinglePoint) {
    Canvas c(20, 20);
    c.drawLine(5, 5, 5, 5);
    EXPECT_EQ(c.getPixel(5, 5), '#');
}

// Тест 17: диагональ под 45 градусов — каждый шаг по x должен давать шаг по y,
// пиксели идут строго по диагонали без пропусков
TEST(CanvasDrawLine, DiagonalLine) {
    Canvas c(20, 20);
    c.drawLine(0, 0, 4, 4);
    for (int i = 0; i <= 4; i++)
        EXPECT_EQ(c.getPixel(i, i), '#');
}

// Тест 18: передаём точки в обратном порядке (x2 < x1) — линия должна
// нарисоваться та же самая, направление не важно
TEST(CanvasDrawLine, ReverseDirection) {
    Canvas c(20, 20);
    c.drawLine(8, 5, 2, 5);
    for (int x = 2; x <= 8; x++)
        EXPECT_EQ(c.getPixel(x, 5), '#');
}

// Тест 19: линия от (0,0) до (9,9) через весь холст — проверяю
// что не выходит за границы и не ломается
TEST(CanvasDrawLine, DoesNotExceedBounds) {
    Canvas c(10, 10);
    EXPECT_NO_THROW(c.drawLine(0, 0, 9, 9));
}

// Canvas — floodFill()

// Тест 20: заливаем пустой холст 5x5 начиная с угла — весь должен стать '#'.
// Если BFS работает правильно, ни одна клетка не останется точкой
TEST(CanvasFloodFill, FillEmptyCanvas) {
    Canvas c(5, 5);
    c.floodFill(0, 0);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
            EXPECT_EQ(c.getPixel(x, y), '#');
}

// Тест 21: заливаем холст полностью заполненный '#' — он должен перевернуться в '.',
// потому что floodFill инвертирует текущий символ
TEST(CanvasFloodFill, FillHashToDot) {
    Canvas c(5, 5);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
            c.setPixel(x, y, '#');
    c.floodFill(0, 0);
    EXPECT_EQ(c.getPixel(0, 0), '.');
}

// Тест 22: рисуем рамку '#' по периметру и заливаем центр — заливка не должна
// перепрыгнуть через стенку, внешняя рамка остаётся нетронутой
TEST(CanvasFloodFill, FillBoundedByWall) {
    Canvas c(5, 5);
    for (int x = 0; x < 5; x++) { c.setPixel(x, 0, '#'); c.setPixel(x, 4, '#'); }
    for (int y = 0; y < 5; y++) { c.setPixel(0, y, '#'); c.setPixel(4, y, '#'); }
    c.floodFill(2, 2);
    EXPECT_EQ(c.getPixel(2, 2), '#');
    EXPECT_EQ(c.getPixel(0, 0), '#');
}

// Тест 23: вызываем floodFill с координатами за пределами холста —
// функция должна молча выйти, ничего не сломав
TEST(CanvasFloodFill, OutOfBoundsNoCrash) {
    Canvas c(5, 5);
    EXPECT_NO_THROW(c.floodFill(-1, 0));
    EXPECT_NO_THROW(c.floodFill(0, -1));
    EXPECT_NO_THROW(c.floodFill(5, 0));
}

// Тест 24: одна пустая клетка, со всех сторон окружённая '#' —
// заливка должна закрасить только её, не вырываясь наружу
TEST(CanvasFloodFill, FillSingleCell) {
    Canvas c(3, 3);
    c.setPixel(1, 0, '#'); c.setPixel(0, 1, '#');
    c.setPixel(2, 1, '#'); c.setPixel(1, 2, '#');
    c.floodFill(1, 1);
    EXPECT_EQ(c.getPixel(1, 1), '#');
}

// Canvas — saveToHistory / undo()

// Тест 25: у нового холста история пустая — иначе undo с самого старта
// будет делать что-то непредсказуемое
TEST(CanvasHistory, InitiallyEmpty) {
    Canvas c(10, 10);
    EXPECT_TRUE(c.isHistoryEmpty());
}

// Тест 26: после saveToHistory флаг isHistoryEmpty должен стать false
TEST(CanvasHistory, NotEmptyAfterSave) {
    Canvas c(10, 10);
    c.saveToHistory();
    EXPECT_FALSE(c.isHistoryEmpty());
}

// Тест 27: сохраняем чистый холст, ставим пиксель, нажимаем undo —
// пиксель должен исчезнуть. Это основная цель Ctrl+Z
TEST(CanvasHistory, UndoRestoresState) {
    Canvas c(10, 10);
    c.saveToHistory();
    c.setPixel(5, 5, '#');
    EXPECT_EQ(c.getPixel(5, 5), '#');
    c.undo();
    EXPECT_EQ(c.getPixel(5, 5), '.');
}

// Тест 28: делаем два действия, отменяем одно — второй пиксель исчезает,
// первый остаётся. История работает как стек: последним пришёл — первым ушёл
TEST(CanvasHistory, MultipleUndos) {
    Canvas c(10, 10);
    c.saveToHistory();
    c.setPixel(1, 1, '#');
    c.saveToHistory();
    c.setPixel(2, 2, '#');
    c.undo();
    EXPECT_EQ(c.getPixel(2, 2), '.');
    EXPECT_EQ(c.getPixel(1, 1), '#');
}

// Тест 29: undo при пустой истории — не должен падать,
// в реализации есть проверка на empty() перед pop_back
TEST(CanvasHistory, UndoOnEmptyNoCrash) {
    Canvas c(10, 10);
    EXPECT_NO_THROW(c.undo());
}

// Тест 30: сохраняем 25 состояний при лимите 20 — старые должны вытесняться,
// и краша быть не должно
TEST(CanvasHistory, HistoryLimit) {
    Canvas c(10, 10);
    for (int i = 0; i < 25; i++)
        c.saveToHistory();
    EXPECT_NO_THROW(c.undo());
}

// Canvas — getFullState / restoreState

// Тест 31: getFullState возвращает вектор строк — проверяю что его размер
// равен высоте холста, то есть соответствует числу строк в grid
TEST(CanvasState, GetFullStateSize) {
    Canvas c(10, 5);
    auto state = c.getFullState();
    EXPECT_EQ(state.size(), 5u);
}

// Тест 32: берём снимок состояния, что-то рисуем, потом откатываемся через restoreState —
// нарисованный пиксель должен исчезнуть, холст вернётся к снимку
TEST(CanvasState, RestoreState) {
    Canvas c(10, 5);
    auto initial = c.getFullState();
    c.setPixel(3, 3, '#');
    c.restoreState(initial);
    EXPECT_EQ(c.getPixel(3, 3), '.');
}

// Тест 33: передаём вектор с неправильным числом строк — функция должна
// проигнорировать его и оставить холст нетронутым
TEST(CanvasState, RestoreWrongSizeIgnored) {
    Canvas c(10, 5);
    std::vector<std::string> bad(3, std::string(10, '.'));
    EXPECT_NO_THROW(c.restoreState(bad));
    EXPECT_EQ(c.getHeight(), 5);
}

// Canvas — save / load

// Тест 34: save() должна вернуть true и реально создать файл на диске
TEST(CanvasSaveLoad, SaveCreatesFile) {
    Canvas c(10, 5);
    bool result = c.save("test_save.txt");
    EXPECT_TRUE(result);
    std::ifstream f("test_save.txt");
    EXPECT_TRUE(f.good());
}

// Тест 35: рисуем пиксель, сохраняем, грузим в другой холст и проверяем
// что пиксель на том же месте — если нет, значит формат файла сломан
TEST(CanvasSaveLoad, LoadRestoresCanvas) {
    Canvas c1(10, 5);
    c1.setPixel(3, 2, '#');
    c1.save("test_load.txt");
    Canvas c2(10, 5);
    c2.load("test_load.txt");
    EXPECT_EQ(c2.getPixel(3, 2), '#');
}

// Тест 36: файла не существует — load() должна вернуть false, а не упасть
TEST(CanvasSaveLoad, LoadNonExistentFile) {
    Canvas c(10, 5);
    bool result = c.load("nonexistent_file_xyz.txt");
    EXPECT_FALSE(result);
}

// Тест 37: сохраняем холст 20x10, пытаемся загрузить в холст 10x5 —
// размеры не совпадают, load() должна вернуть false и не трогать текущий холст
TEST(CanvasSaveLoad, LoadWrongSizeFails) {
    Canvas c1(20, 10);
    c1.save("test_wrong.txt");
    Canvas c2(10, 5);
    bool result = c2.load("test_wrong.txt");
    EXPECT_FALSE(result);
}
