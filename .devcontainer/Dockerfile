# Use modern Ubuntu LTS base image
FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies for ESP32/PlatformIO and Node.js development
RUN apt-get update && apt-get -y install --no-install-recommends \
    # Build essentials
    build-essential \
    cmake \
    # Python for PlatformIO
    python3 \
    python3-pip \
    python3-venv \
    python3-dev \
    # Git and utilities
    git \
    curl \
    wget \
    ca-certificates \
    # Serial/USB support for ESP32
    udev \
    usbutils \
    # Additional tools
    jq \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install Node.js LTS (v20.x)
RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get install -y nodejs \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create Python virtual environment for PlatformIO
ENV PLATFORMIO_VENV=/opt/platformio-venv
RUN python3 -m venv ${PLATFORMIO_VENV} \
    && ${PLATFORMIO_VENV}/bin/pip install --upgrade pip setuptools wheel \
    && ${PLATFORMIO_VENV}/bin/pip install platformio

# Add PlatformIO to PATH
ENV PATH="${PLATFORMIO_VENV}/bin:${PATH}"

# Setup udev rules for ESP32 devices (to be run with appropriate permissions)
RUN mkdir -p /etc/udev/rules.d

# Set the default shell to bash
SHELL ["/bin/bash", "-c"]
