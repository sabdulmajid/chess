# Multi-stage Dockerfile for Chess Engine

# Stage 1: Build environment
FROM emscripten/emsdk:latest as web-builder

WORKDIR /app
COPY . .

# Build web version
RUN ./build_web.sh

# Stage 2: Desktop build environment  
FROM ubuntu:22.04 as desktop-builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Build desktop version
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Stage 3: Production web server
FROM nginx:alpine as web-production

# Copy web build artifacts
COPY --from=web-builder /app/build/web /usr/share/nginx/html

# Copy nginx configuration
COPY docker/nginx.conf /etc/nginx/nginx.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]

# Stage 4: Desktop runtime
FROM ubuntu:22.04 as desktop-production

RUN apt-get update && apt-get install -y \
    libx11-6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=desktop-builder /app/build/bin/chess-desktop .
COPY --from=desktop-builder /app/build/bin/chess-console .

CMD ["./chess-console"]
