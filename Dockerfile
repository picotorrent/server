# build server
FROM node:16-alpine AS build-env
WORKDIR /app
COPY . .
RUN apk add --no-cache cmake g++ ninja && npm i
