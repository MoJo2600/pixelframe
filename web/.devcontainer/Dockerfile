# Use modern Node.js LTS (20) with development tools
ARG VARIANT="20-noble"
FROM mcr.microsoft.com/devcontainers/typescript-node:1-${VARIANT}

# Install additional development tools
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    git \
    build-essential \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install global Node packages
RUN su node -c "npm install -g npm@latest yarn pnpm"
