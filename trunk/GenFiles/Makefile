login_client_file = login_client_send.h login_client_send.cpp login_client_recv.h login_client_recv.cpp
login_server_file = login_server_send.h login_server_send.cpp login_server_recv.h login_server_recv.cpp

all: $(login_client_file) $(login_server_file)

clean:
	del /Q $(login_client_file) $(login_server_file)
	
$(login_client_file): login_client.ft
	$(ftgen) $?
	
$(login_server_file): login_server.ft
	$(ftgen) $?