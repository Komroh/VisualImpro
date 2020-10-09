/**
 *  \file    Connection.cpp
 *  \author  Jérémy LIXANDRE
 *  \date    July 2017
 *
 *  \brief Connection object to send data on a web page.
 *
 *  The Connection object is used, after finishing the processing
 *  operations, to send the processed data to a web page on Firefox.
 *
 */


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Connection.hpp"

/**
 * \def SYSCALL(call, val, msg)
 * Check if the the return value of the call is equals to val, returns -1 if so.
 */
#define SYSCALL(call, val, msg) if ((call) == (val)) {perror(msg); return -1;}
#define TIMEOUT 1

#define LEN 256

#define PORTNO 9090
#define ADDR "192.168.7.1"



static sig_atomic_t s_signal_received = 0;
static const char *s_http_port = "9090";
static struct mg_serve_http_opts s_http_server_opts;


static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;
}


static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
//	bool connected = false;
    switch (ev)
    {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            cout << "Bonjour" << endl;
            string str = "Salut !";
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
            break;
        }
       
         case MG_EV_HTTP_REQUEST:
        {
            mg_serve_http(nc, (struct http_message *) ev_data, s_http_server_opts);
            break;
        }
        
        /*case MG_EV_POLL:
        {
        	string str = "Yo";//(char *)nc->user_data;
        	//cout << str << endl;
        	mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
        	break;
        }*/

        case MG_EV_CLOSE:
        {
            break;
        }
    }
}

int Connection::init(){

//UDP

/*
sock = UdpClient(PORTNO, ADDR);
sock.setPort(PORTNO);
sock.setServer(ADDR);*/
	

  /*this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  SYSCALL(this->sockfd, -1, "ERROR OPENING SOCKET");

  string addr(ADDR);
  int portno = PORTNO;
  struct in_addr inp;
  SYSCALL(inet_aton(addr.c_str(), &inp), 0, "ERROR copying address");
  //struct sockaddr_in serv_addr;
  bzero((char*) &serv_addr, sizeof(serv_addr)); //useful ?
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr = inp;
  serv_addr.sin_port = htons(portno); // Get port number
  return 0;*/

//TCP
	
  //this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    setvbuf(stdout, NULL, _IOLBF, 0);
	setvbuf(stderr, NULL, _IOLBF, 0);
	mg_mgr_init(&mgr, NULL);
	stop = false;
	this->nc = mg_bind(&mgr,std::to_string(_port).c_str() ,ev_handler);
	mg_set_protocol_http_websocket(nc);
  s_http_server_opts.document_root = ".";  // Serve current directory
  s_http_server_opts.enable_directory_listing = "yes";
	
  	printf("Started on port %s\n", s_http_port);
  	while (/*s_signal_received == 0*/!stop) {
    mg_mgr_poll(&mgr, 200); }
    cout << "Je passe à travers"  << endl;
	return 0;
  /*SYSCALL(this->sockfd, -1, "ERROR OPENING SOCKET");

  string addr(_addr);
  int portno = _port;
  struct in_addr inp;
  SYSCALL(inet_aton(addr.c_str(), &inp), 0, "ERROR copying address");
  struct sockaddr_in serv_addr;
  bzero((char*) &serv_addr, sizeof(serv_addr)); //useful ?
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr = inp;
  serv_addr.sin_port = htons(portno); // Get port number
  if (connect(this->sockfd,
      (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == 0){
    this->_isConnected = true;
    return 0;
  }
  printf("Pas connecté\n");
  return -1;*/

}


int Connection::send( string &msg){

//TCP
/*cout << "Message sent" <<endl;
string str = "Toujours Debout";
mg_send_websocket_frame(this->nc, WEBSOCKET_OP_TEXT, msg.c_str(), msg.size());*/
nc->user_data = const_cast<char *>(msg.c_str());
return 1;
/*
  string tosend = msg;
  int n = write(this->sockfd, tosend.c_str(), tosend.length());
  SYSCALL(n, -1, "ERROR WRITE SOCKET");
  return n;
  */
//UDP

/*
	socklen_t addrlen = sizeof(serv_addr);
	sendto(this->sockfd, msg.c_str(), msg.length()*sizeof(char), 0,
         (struct sockaddr *) &(this->serv_addr), addrlen);
	//printf("sent ? %d\n", sent);

  return 0;
 */


}



int Connection::end(){

//TCP
mg_mgr_free(&mgr);

 /* if (close(sockfd) == 0){
    this->_isConnected = false;
    return 0;
  }
  return -1;*/
return 0;
}

