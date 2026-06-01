#include <gtest/gtest.h>
#include "PaintContext.h"

// Тест 1: сразу после создания контекст должен быть в режиме рисования —
// при запуске программы пользователь сразу должен мочь ставить точки
TEST(PaintContextTest, DefaultIsDrawMode) {
    PaintContext ctx;
    EXPECT_TRUE(ctx.isDrawMode());
}

// Тест 2: одно нажатие 'R' переключает в режим стирания — проверяю
// что isDrawMode стал false
TEST(PaintContextTest, ToggleToEraseMode) {
    PaintContext ctx;
    ctx.toggleMode();
    EXPECT_FALSE(ctx.isDrawMode());
}

// Тест 3: два нажатия 'R' подряд — должны вернуться обратно в рисование
TEST(PaintContextTest, DoubleToggleRestoresDraw) {
    PaintContext ctx;
    ctx.toggleMode();
    ctx.toggleMode();
    EXPECT_TRUE(ctx.isDrawMode());
}

// Тест 4: в режиме рисования getCursorSymbol должен вернуть '#' —
// этот же символ потом ставится на холст при нажатии пробела
TEST(PaintContextTest, CursorSymbolDraw) {
    PaintContext ctx;
    EXPECT_EQ(ctx.getCursorSymbol(), '#');
}

// Тест 5: в режиме стирания getCursorSymbol должен вернуть '%' —
// чтобы пользователь видел в какой режим переключился прямо по курсору
TEST(PaintContextTest, CursorSymbolErase) {
    PaintContext ctx;
    ctx.toggleMode();
    EXPECT_EQ(ctx.getCursorSymbol(), '%');
}

// Тест 6: проверяю что getModeName возвращает нужную строку для режима рисования —
// именно этот текст выводится в нижней части экрана редактора
TEST(PaintContextTest, ModeNameDraw) {
    PaintContext ctx;
    EXPECT_EQ(ctx.getModeName(), "RISOVANIE [#]");
}

// Тест 7: то же самое для режима стирания
TEST(PaintContextTest, ModeNameErase) {
    PaintContext ctx;
    ctx.toggleMode();
    EXPECT_EQ(ctx.getModeName(), "STIRANIE [%]");
}

// Тест 8: 10 переключений подряд — чётное число, значит должны вернуться
// в рисование. Проверяю что toggle не накапливает лишнего состояния
TEST(PaintContextTest, EvenTogglesRestoresDraw) {
    PaintContext ctx;
    for (int i = 0; i < 10; i++) ctx.toggleMode();
    EXPECT_TRUE(ctx.isDrawMode());
}

// Тест 9: переключаю режим 6 раз и каждый раз сверяю символ курсора с флагом режима —
// они не должны рассинхронизироваться ни на одном шаге
TEST(PaintContextTest, CursorSymbolConsistentWithMode) {
    PaintContext ctx;
    for (int i = 0; i < 6; i++) {
        if (ctx.isDrawMode())
            EXPECT_EQ(ctx.getCursorSymbol(), '#');
        else
            EXPECT_EQ(ctx.getCursorSymbol(), '%');
        ctx.toggleMode();
    }
}
