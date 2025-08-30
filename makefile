.PHONY: server server-log server-run client client-log client-run

server:
	@-cd server ; \
	  platformio run --environment server --target compiledb ; \
	  platformio run --environment server ; \
	  cd ..

server-log:
	@-cd server ; \
	  platformio device monitor --environment server ; \
	  cd ..

server-run: server
	@-cd server ; \
	  platformio run --environment server --target upload ; \
	  platformio device monitor --environment server ; \
	  cd ..

client:
	@-cd client ; \
	  platformio run --environment client --target compiledb ; \
	  platformio run --environment client ;
	  cd ..

client-log:
	@-cd client ; \
	  platformio device monitor --environment client
	  cd ..

client-run: client
	@-cd client ; \
	  platformio run --environment client --target upload ; \
	  platformio device monitor --environment client
	  cd ..
