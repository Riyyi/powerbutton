.PHONY: server server-log server-run client client-log client-run

server:
	@-cd server ; \
		platformio run --target upload ; \
		cd ..

server-log:
	@-cd server ; \
		platformio device monitor ; \
		cd ..

server-run:
	@-cd server ; \
		platformio run --target upload ; \
		platformio device monitor ; \
		cd ..

client:
	@-cd client ; \
		platformio run --target upload ; \
		cd ..

client-log:
	@-cd client ; \
		platformio device monitor ; \
		cd ..

client-run:
	@-cd client ; \
		platformio run --target upload ; \
		platformio device monitor ; \
		cd ..
