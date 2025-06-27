FROM debian:bookworm-slim AS builder
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libpqxx-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /server
COPY . .

RUN cmake -S . -B build && cmake --build build

FROM debian:bookworm-slim
RUN apt-get update && apt-get install -y --no-install-recommends \
    libpqxx-dev \
    netcat-openbsd \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /server
COPY --from=builder /server/build/server .
COPY wait-and-run-server.sh /server/wait-and-run-server.sh
RUN chmod +x /server/wait-and-run-server.sh