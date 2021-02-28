####
# Development
####

FROM node:14.15-alpine AS dev

RUN apk add --update --no-cache \
  curl \
  git \
  zsh \
  make

RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"

WORKDIR /workspace
