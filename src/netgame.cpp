
#include "netgame.h"

NetGame::NetGame ()
{
	connected=false;
}

bool NetGame::runServer()
{
	try {
		// Create a new netsession
		netgame = new CL_NetSession("spacewar");
		// connect the connect and disconnect signals to some slots
		slot_connect = netgame->sig_computer_connected().connect(this, 
			&NetGame::on_server_connect);
		slot_disconnect = netgame->sig_computer_disconnected().connect(this, 
			&NetGame::on_server_disconnect);
		// Start the server listening for client activity
		netgame->start_listen("7667");
		isServer=true;
		
		return true;
		
	} catch(CL_Error err) {
		std::cout << "Could not create server: " << err.message.c_str() << std::endl;
	}
	return false;
}


bool NetGame::runClient()
{
	try {
		// create a new netsession
		netgame = new CL_NetSession("spacewar");
		// connect the disconnect and receive signals to some slots
		slot_disconnect = netgame->sig_computer_disconnected().connect(this, 
			&NetGame::on_client_disconnect);
		
		// we have two different slots depending on the packet channel received
		slot_receive_message = netgame->sig_netpacket_receive("message").connect(this, 
			&NetGame::on_message_receive);

		// connect to the server (running on the local machine in this case)
		CL_IPAddress server_ip;
		server_ip.set_address("localhost", "7667");
		netgame->connect(server_ip);
		connected=true;
		isServer=false;
		return true;

	} catch(CL_Error err) {
		std::cout << "Could not create client: " << err.message.c_str() << std::endl;
		return false;
	}
}

void NetGame::on_server_connect(CL_NetComputer &computer)
{
	std::cout << "Client joined." << std::endl;
	connected=true;
}

void NetGame::on_server_disconnect(CL_NetComputer &computer)
{
	std::cout << "Client disconnected." << std::endl;
	connected=false;
}

void NetGame::on_client_disconnect(CL_NetComputer &computer)
{
	std::cout << "Lost connection to server." << std::endl;
	connected=false;
}


bool NetGame::send ()
{
	//compile a new net packet with various forms of data
	msg = new CL_NetPacket();
	//msg->output.write_int32(rand());
	msg->output.write_uchar8('x');
	//msg->output.write_float32(3.14159f);

	//do the actual sending, to all the computers connected
	netgame->get_all().send("message", *msg);
	delete msg;	// discard sent message		
				
	std::cout << "Sent message" << std::endl;

	return true;
}

void NetGame::on_message_receive(CL_NetPacket &packet, CL_NetComputer &computer)
{
	std::cout << "Recibiendo..." << std::endl;
	//std::string message = packet.input.read_string();
	//int mnum = packet.input.read_int32();
	unsigned char mchar = packet.input.read_uchar8();
	//float mfloat = packet.input.read_float32();
	std::cout << "Server message: " << mchar << std::endl;
}

