FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-base-dev-tools \
    libqt6sql6-psql \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

ENV QT_QPA_PLATFORM=offscreen

WORKDIR /server
COPY . .

RUN cmake -S . -B build && cmake --build build

# Final image
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    qt6-base-dev \
    libqt6sql6-psql \
    libpq-dev \
    netcat-openbsd \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /server

COPY --from=builder /server/build/server .
COPY wait-and-run-server.sh /server/wait-and-run-server.sh
RUN chmod +x /server/wait-and-run-server.sh

CMD ["./wait-and-run-server.sh"]
