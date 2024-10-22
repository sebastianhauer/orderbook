ARG VARIANT="ubuntu-24.04"
FROM mcr.microsoft.com/devcontainers/cpp:${VARIANT}

RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update --quiet --quiet && \
    apt-get upgrade --quiet --quiet --yes --no-install-recommends && \
    apt-get autoremove --quiet --quiet --yes && \
    apt-get clean --quiet --quiet && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /tmp/* /var/tmp/*

RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update --quiet --quiet && \
    apt-get install --quiet --quiet --yes --no-install-recommends \
    vim-nox \
    emacs-nox \
    && \
    apt-get autoremove --quiet --quiet --yes && \
    apt-get clean --quiet --quiet && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /tmp/* /var/tmp/*
