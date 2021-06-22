FROM node:15 AS client-build-env

WORKDIR /app

COPY ./client/package*.json ./

RUN npm install

COPY ./client .

RUN npm run build

FROM alpine:3.14 AS build-env

WORKDIR /app

RUN apk update --no-cache && apk add curl g++ ninja cmake

COPY . .

RUN mkdir build && cd build && cmake --version && cmake -G Ninja .. && ninja

FROM alpine:3.14

RUN apk update --no-cache && apk add boost openssl

WORKDIR /app

RUN mkdir client

COPY --from=client-build-env /app/dist /app/client

COPY --from=build-env /app/build/PicoTorrentServer /usr/bin

ENV PICOTORRENT_HTTP_HOST=0.0.0.0

ENV PICOTORRENT_WEBROOT_PATH=/app/client

ENTRYPOINT [ "PicoTorrentServer" ]
