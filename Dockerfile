# build js client
FROM node:16 AS client-build-env
WORKDIR /app
COPY ./client/package*.json ./
RUN npm install
COPY ./client .
RUN npm run build

# build server
FROM debian:bookworm-slim AS build-env
WORKDIR /app
COPY . .
RUN apt-get update \
    && apt-get install -y build-essential cmake zip unzip curl git ninja-build pkg-config \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=x64-linux-release -G Ninja .. \
    && ninja \
    && ./PicoTorrentTests

# production layer
FROM debian:bookworm-slim
WORKDIR /app
RUN mkdir client
COPY --from=client-build-env /app/dist /app/client
COPY --from=build-env /app/build/PicoTorrentServer /app
ENV PICOTORRENT_HTTP_HOST=0.0.0.0
ENV PICOTORRENT_WEBROOT_PATH=/app/client
ENTRYPOINT [ "./PicoTorrentServer" ]
