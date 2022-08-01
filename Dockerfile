# build server
FROM debian:bookworm-slim AS build-env
WORKDIR /app
COPY . .
RUN apt-get update \
    && apt-get install -y build-essential cmake zip unzip curl git ninja-build pkg-config python3 \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=x64-linux-release -G Ninja .. \
    && ninja pika

# production layer
FROM debian:bookworm-slim
WORKDIR /app
COPY --from=build-env /app/build/pika /app
ENTRYPOINT [ "./pika" ]
