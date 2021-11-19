SHELL := /bin/zsh
.DEFAULT_GOAL := help

###########################
# VARS
###########################

MOCK_API_DIR := mock-api

###########################
# TARGETS
###########################

.PHONY: help
help: ## Show this help
	@awk 'BEGIN {FS = ":.*##"; printf "Usage: make \033[36m<target>\033[0m\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 } /^##@/ { printf "\n\033[1m%s\033[0m\n", substr($$0, 5) } ' $(MAKEFILE_LIST)


## docker container related
.PHONY: dev-run
dev-run: ## Start the development docker environment
	docker-compose run dev zsh

.PHONY: dev-build
dev-build: ## Build the development docker environment
	docker-compose build

.PHONY: dev-prune
dev-prune: ## Cleanup the development docker environment
	docker-compose down -v --remove-orphans --rmi all

.PHONY: dev-run-build
dev-run-build: ## Start the development docker environment and builds front end
	docker-compose run dev make build


## app related
.PHONY: run
run: ## Start up web application locally
	npm run serve

.PHONY: build
build: ## Build web application
	npm run build


## local mock API
.PHONY: mock-api
mock-api: ## Start up mock api
	npx json-server --watch ${MOCK_API_DIR}/db.json --routes ${MOCK_API_DIR}/routes.json --static ${MOCK_API_DIR}

.PHONY: mock-api-delay
mock-api-delay: ## Start up mock api with delay
	npx json-server --watch ${MOCK_API_DIR}/db.json --routes ${MOCK_API_DIR}/routes.json --static ${MOCK_API_DIR} --delay 3000

.PHONY: mock-api-error
mock-api-error: ## Start up mock api with error middleware
	npx json-server --watch ${MOCK_API_DIR}/db.json --routes ${MOCK_API_DIR}/routes.json --static ${MOCK_API_DIR} --middlewares ${MOCK_API_DIR}/middlewares/internalServerError.js
