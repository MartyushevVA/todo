# =========================
# Этап сборки
# =========================
FROM debian:bookworm-slim AS builder

# Установка только необходимых инструментов для сборки
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libpqxx-dev \
    && rm -rf /var/lib/apt/lists/*

# Копируем исходники
WORKDIR /server
COPY . .

# Сборка проекта
RUN cmake -S . -B build && cmake --build build

# =========================
# Финальный минимальный образ
# =========================
FROM debian:bookworm-slim

# Установка только необходимых runtime-библиотек
RUN apt-get update && apt-get install -y --no-install-recommends \
    libpqxx-dev \
    && rm -rf /var/lib/apt/lists/*

# Копируем собранный сервер из builder
WORKDIR /server
COPY --from=builder /server/build/server .

# Запуск приложения
CMD ["./server"]
