#include <gtest/gtest.h>
#include "Canvas.h"
#include "PaintContext.h"

// Сценарий 1: рисуем пару точек, потом жмём 'C' (clear).
// Холст должен полностью сброситься — пользователь начинает заново с чистого листа
TEST(Scenario, DrawAndClear) {
    Canvas c(20, 10);
    c.setPixel(3, 3, '#');
    c.setPixel(7, 7, '#');
    ASSERT_EQ(c.getPixel(3, 3), '#');
    ASSERT_EQ(c.getPixel(7, 7), '#');
    c.clear();
    EXPECT_EQ(c.getPixel(3, 3), '.');
    EXPECT_EQ(c.getPixel(7, 7), '.');
}

// Сценарий 2: поставили точку не там — переключаем режим на стирание и убираем её.
// Типичный случай: нарисовал, понял что промахнулся, нажал 'R' и стёр
TEST(Scenario, ToggleModeAndErase) {
    Canvas c(10, 10);
    PaintContext ctx;
    c.setPixel(5, 5, '#');
    ASSERT_EQ(c.getPixel(5, 5), '#');
    ctx.toggleMode(); // переходим в режим стирания
    ASSERT_FALSE(ctx.isDrawMode());
    c.setPixel(5, 5, '.'); // стираем
    EXPECT_EQ(c.getPixel(5, 5), '.');
}

// Сценарий 3: режим линии, рисуем горизонталь с x=2 до x=10.
// Все точки внутри отрезка должны быть '#', а соседние (x=1 и x=11) — нетронуты
TEST(Scenario, DrawHorizontalLine) {
    Canvas c(20, 10);
    c.drawLine(2, 4, 10, 4);
    for (int x = 2; x <= 10; x++)
        EXPECT_EQ(c.getPixel(x, 4), '#') << "x=" << x;
    EXPECT_EQ(c.getPixel(1, 4), '.');
    EXPECT_EQ(c.getPixel(11, 4), '.');
}

// Сценарий 4: заливка пустого холста — нажимаем 'F' стоя на клетке '.'.
// Весь холст должен заполниться '#', ни одна клетка не должна остаться пустой
TEST(Scenario, FloodFillArea) {
    Canvas c(5, 5);
    c.floodFill(0, 0);
    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
            EXPECT_EQ(c.getPixel(x, y), '#');
}

// Сценарий 5: рисуем, сохраняем ('S'), загружаем ('O') в другой холст и проверяем
// что оба пикселя на своих местах — данные не потерялись при записи в файл
TEST(Scenario, SaveAndLoad) {
    Canvas c1(10, 5);
    c1.setPixel(2, 2, '#');
    c1.setPixel(7, 3, '#');
    bool saved = c1.save("scenario_save.txt");
    ASSERT_TRUE(saved);
    Canvas c2(10, 5);
    bool loaded = c2.load("scenario_save.txt");
    ASSERT_TRUE(loaded);
    EXPECT_EQ(c2.getPixel(2, 2), '#');
    EXPECT_EQ(c2.getPixel(7, 3), '#');
    EXPECT_EQ(c2.getPixel(0, 0), '.'); // незатронутые клетки остались точками
}

// Сценарий 6: Ctrl+Z — сохраняем состояние, рисуем пиксель, нажимаем undo.
// После отмены холст должен быть точно таким же, каким был до действия
TEST(Scenario, UndoDrawAction) {
    Canvas c(10, 10);
    c.saveToHistory();     // запоминаем чистый холст
    c.setPixel(4, 4, '#');
    ASSERT_EQ(c.getPixel(4, 4), '#');
    c.undo();
    EXPECT_EQ(c.getPixel(4, 4), '.');
}

// Сценарий 7: полный рабочий цикл — ставим точку, рисуем линию,
// сохраняем в файл, загружаем в новый холст, чистим.
// Проверяю что все операции в цепочке не мешают друг другу
TEST(Scenario, FullWorkflow) {
    Canvas c(10, 5);
    PaintContext ctx;

    // ставим точку в (1,1)
    c.setPixel(1, 1, '#');
    EXPECT_EQ(c.getPixel(1, 1), '#');

    // рисуем горизонтальную линию по нижней строке
    c.drawLine(0, 4, 9, 4);
    EXPECT_EQ(c.getPixel(5, 4), '#');

    // сохраняем и загружаем в новый холст
    c.save("workflow.txt");
    Canvas c2(10, 5);
    c2.load("workflow.txt");
    EXPECT_EQ(c2.getPixel(1, 1), '#');
    EXPECT_EQ(c2.getPixel(5, 4), '#');

    // чистим и убеждаемся что всё сброшено
    c2.clear();
    EXPECT_EQ(c2.getPixel(1, 1), '.');
    EXPECT_EQ(c2.getPixel(5, 4), '.');
}
