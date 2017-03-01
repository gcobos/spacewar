#ifndef _NET_H_
#define _NET_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <ClanLib/core.h>
#include <ClanLib/network.h>
#include <stdlib.h>

class NetGame {

public:

	NetGame ();

	bool connected;
	bool isServer;

	bool runServer();
	bool runClient();
	
	bool send ();
protected:

	void on_client_disconnect (CL_NetComputer &computer);
	void on_server_disconnect (CL_NetComputer &computer);
	void on_server_connect (CL_NetComputer &computer);
	void on_message_receive (CL_NetPacket &packet, CL_NetComputer &computer);

	CL_Slot slot_connect;
	CL_Slot slot_disconnect;
	CL_Slot slot_receive_message;

	CL_NetSession *netgame;
	CL_NetPacket *msg;
};

#endif


