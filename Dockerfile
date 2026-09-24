# syntax=docker/dockerfile:1.7

# =============================================================================
# queue-traffic — build multi-stágio
#
#   runtime (default) : imagem final, binário estático em `scratch` (~2 MB)
#   dev               : toolchain completo (gdb/valgrind) para desenvolver
# =============================================================================

ARG GCC_VERSION=14
ARG DEBIAN_RELEASE=bookworm


# -----------------------------------------------------------------------------
# Stage: toolchain — base comum de compilação (com ccache)
# -----------------------------------------------------------------------------
FROM gcc:${GCC_VERSION}-${DEBIAN_RELEASE} AS toolchain

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    rm -f /etc/apt/apt.conf.d/docker-clean \
 && apt-get update \
 && apt-get install -y --no-install-recommends ccache

ENV CCACHE_DIR=/ccache \
    CCACHE_MAXSIZE=200M

WORKDIR /app


# -----------------------------------------------------------------------------
# Stage: builder — compila e linka estaticamente
# -----------------------------------------------------------------------------
FROM toolchain AS builder

# Flags exportadas como ARG para permitir override:
#   docker build --build-arg CXXFLAGS="-std=c++20 -O0 -g -fsanitize=address" .
ARG CXXFLAGS="-std=c++20 -O2 -pipe -DNDEBUG -Wall -Wextra"
ARG LDFLAGS="-static -s"

COPY Makefile .
COPY include/ ./include/
COPY src/ ./src/
COPY main.cpp .

# Mesmo build do ambiente local (Makefile), só trocando o compilador por ccache.
RUN --mount=type=cache,target=/ccache,sharing=locked \
    make CXX="ccache g++" CXXFLAGS="${CXXFLAGS}" LDFLAGS="${LDFLAGS}" \
 && ccache --show-stats


# -----------------------------------------------------------------------------
# Stage: runtime — imagem final mínima, sem shell, sem package manager
# -----------------------------------------------------------------------------
FROM scratch AS runtime

LABEL org.opencontainers.image.title="queue-traffic" \
      org.opencontainers.image.description="Simulacao de fila de trafego em C++20" \
      org.opencontainers.image.source="https://github.com/matheus-lursy/queue-traffic" \
      org.opencontainers.image.licenses="MIT"

# Binário estático: não precisa de libc, usuário ou /etc na imagem.
USER 10001:10001

COPY --from=builder /app/app /app

ENTRYPOINT ["/app"]


# -----------------------------------------------------------------------------
# Stage: dev — ambiente de desenvolvimento (código montado via volume)
# -----------------------------------------------------------------------------
FROM toolchain AS dev

RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update \
 && apt-get install -y --no-install-recommends gdb valgrind less

# Lido pelo Makefile: `make` dentro do contêiner gera um build de debug.
ENV CXXFLAGS="-std=c++20 -O0 -g -Wall -Wextra"

CMD ["bash"]
