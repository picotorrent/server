# build server
FROM node:16-alpine AS native-build-env
WORKDIR /app
COPY . .

ENV VCPKG_FORCE_SYSTEM_BINARIES 1

RUN apk add --no-cache \
    build-base \
    cmake \
    ninja \
    zip \
    unzip \
    curl \
    perl \
    linux-headers \
    pkgconfig \
    git

RUN cd lib/native && npm i

FROM node:18-alpine3.16 AS client-build-env
WORKDIR /app
COPY ./client .

RUN npm i && npm run build

FROM node:18-alpine3.16 AS runtime
WORKDIR /app

ENV SKIP_NATIVE_BUILD=1
ENV TS_NODE_TRANSPILE_ONLY=1

COPY . .

COPY --from=client-build-env /app/build ./static
COPY --from=native-build-env /app/lib/native/build/Release ./lib/native/build/Release

RUN npm i --production

CMD [ "npm", "start" ]
