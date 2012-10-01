login_client_file = login_client_send.h login_client_send.cpp login_client_recv.h login_client_recv.cpp
login_server_file = login_server_send.h login_server_send.cpp login_server_recv.h login_server_recv.cpp

master_peer_file = master_peer_send.h master_peer_send.cpp master_peer_recv.h master_peer_recv.cpp
session_peer_file = session_peer_send.h session_peer_send.cpp session_peer_recv.h session_peer_recv.cpp
gate_peer_file = gate_peer_send.h gate_peer_send.cpp gate_peer_recv.h gate_peer_recv.cpp

all: $(login_client_file) $(login_server_file) $(master_peer_file) $(session_peer_file) $(gate_peer_file)

clean:
	del /Q $(login_client_file) $(login_server_file) $(master_peer_file) $(session_peer_file) $(gate_peer_file)
	
$(login_client_file): login_client.ft
	$(ftgen) $?
	
$(login_server_file): login_server.ft
	$(ftgen) $?
	
$(master_peer_file): master_peer.pr
	$(prgen) $?
	
$(session_peer_file): session_peer.pr
	$(prgen) $?

$(gate_peer_file): gate_peer.pr
	$(prgen) $?