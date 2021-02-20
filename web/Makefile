SHELL := /bin/zsh
.DEFAULT_GOAL := help

###########################
# TARGETS
###########################

.PHONY: help
help: ## Show this help
	@awk 'BEGIN {FS = ":.*##"; printf "Usage: make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

.PHONY: dev-run
dev-run: ## Start the development docker environment
	docker-compose run dev zsh

.PHONY: dev-build
dev-build: ## Build the development docker environment
	docker-compose build

.PHONY: dev-prune
dev-prune: ## Cleanup the development docker environment
	docker-compose down -v --remove-orphans --rmi all
