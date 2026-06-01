# Стадия 1: сборка проекта и запуск тестов
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    ninja-build \
    wget \
    unzip \
    ca-certificates \
    libncurses-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY src/       ./src/
COPY tests/     ./tests/
COPY CMakeLists.txt .

RUN cmake -S . -B build -G Ninja \
    && cmake --build build

# Если хоть один тест упадёт — docker build завершится с ошибкой
RUN cd build && ctest --output-on-failure

# Стадия 2: финальный образ без исходников и компилятора
FROM ubuntu:22.04 AS final

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libncurses6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/ascii_paint        ./ascii_paint
COPY --from=builder /app/build/tests/test_canvas        ./tests/
COPY --from=builder /app/build/tests/test_paint_context ./tests/
COPY --from=builder /app/build/tests/test_scenarios     ./tests/

# Запускаем редактор интерактивно
CMD ["./ascii_paint"]
