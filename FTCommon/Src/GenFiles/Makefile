ftd_file = ftd_define.h

login_client_file = login_client_send.h login_client_send.cpp login_client_recv.h login_client_recv.cpp
login_server_file = login_server_send.h login_server_send.cpp login_server_recv.h login_server_recv.cpp
gate_client_file = gate_client_send.h gate_client_send.cpp gate_client_recv.h gate_client_recv.cpp
gate_server_file = gate_server_send.h gate_server_send.cpp gate_server_recv.h gate_server_recv.cpp
region_client_file = region_client_send.h region_client_send.cpp region_client_recv.h region_client_recv.cpp
region_server_file = region_server_send.h region_server_send.cpp region_server_recv.h region_server_recv.cpp

master_peer_file = master_peer_send.h master_peer_send.cpp master_peer_recv.h master_peer_recv.cpp
session_peer_file = session_peer_send.h session_peer_send.cpp session_peer_recv.h session_peer_recv.cpp
gate_peer_file = gate_peer_send.h gate_peer_send.cpp gate_peer_recv.h gate_peer_recv.cpp
login_peer_file = login_peer_send.h login_peer_send.cpp login_peer_recv.h login_peer_recv.cpp
cache_peer_file = cache_peer_send.h cache_peer_send.cpp cache_peer_recv.h cache_peer_recv.cpp
region_peer_file = region_peer_send.h region_peer_send.cpp region_peer_recv.h region_peer_recv.cpp

lua_file = test.cpp

all: $(ftd_file) $(login_client_file) $(login_server_file) $(gate_client_file) $(gate_server_file) $(region_client_file) $(region_server_file) $(master_peer_file) $(session_peer_file) $(gate_peer_file) $(login_peer_file) $(cache_peer_file) $(region_peer_file) $(lua_file)

clean:
	del /Q $(ftd_file) $(login_client_file) $(login_server_file) $(gate_client_file) $(gate_server_file) $(region_client_file) $(region_server_file) $(master_peer_file) $(session_peer_file) $(gate_peer_file) $(login_peer_file) $(cache_peer_file) $(region_peer_file) $(lua_file)
	
$(ftd_file): ftd.ftd
	$(ftdgen) $?
	
$(login_client_file): login_client.ft
	$(ftgen) $?
	
$(login_server_file): login_server.ft
	$(ftgen) $?
	
$(gate_client_file): gate_client.ft
	$(ftgen) $?
	
$(gate_server_file): gate_server.ft
	$(ftgen) $?
	
$(region_client_file): region_client.ft
	$(ftgen) $?
	
$(region_server_file): region_server.ft
	$(ftgen) $?
	
$(master_peer_file): master_peer.pr
	$(prgen) $?
	
$(session_peer_file): session_peer.pr
	$(prgen) $?

$(gate_peer_file): gate_peer.pr
	$(prgen) $?
	
$(login_peer_file): login_peer.pr
	$(prgen) $?
	
$(cache_peer_file): cache_peer.pr
	$(prgen) $?
	
$(region_peer_file): region_peer.pr
	$(prgen) $?
	
$(lua_file): test.pkg
	$(tolua) -o $(lua_file) $?