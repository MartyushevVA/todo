FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libpq-dev \
    libboost-all-dev \
    && apt-get clean

WORKDIR /server
COPY . .

RUN cmake -S . -B build && cmake --build build

CMD ["./build/server"]
