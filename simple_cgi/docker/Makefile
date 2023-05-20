GREEN := "\033[32m"
YELLOW := "\033[33m"
BLUE := "\033[34m"
NC := "\033[0m"

all:
	@mkdir -p "$(PWD)/data"
	@docker compose -f ./docker-compose.yml up -d --build

up:
	@docker compose -f ./docker-compose.yml up -d --build

down:
	@docker compose -f ./docker-compose.yml down

re: prune
	make all

stop:
	@if [ $$(docker ps -q | wc -l) -gt 0 ]; then \
		echo ${BLUE}"Stopping containers..."${NC}; \
		docker stop $$(docker ps -q); \
	else \
		echo ${YELLOW}"No running containers found."${NC}; \
	fi

prune: stop
	@echo ${BLUE}"docker system prune will be executed"${NC}; \
	sudo docker system prune -a -f
	docker volume rm $(docker volume ls  -q --filter dangling=true)
	@echo ${BLUE}"Bind mounted volumes will be deleted"${NC};
	@rm -rf "$(PWD)/data"