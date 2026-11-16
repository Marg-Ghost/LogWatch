//
// Created by Admin on 09.11.2026.
//

#ifndef AWS_HACKATHAN_SERVERPORT_H
#define AWS_HACKATHAN_SERVERPORT_H

/*
 * We use this Socket to communicate with the main server.py
 * Why not a simple Library- c++ as py subprocess?
 *-> We will send a lot of requests to refresh the website server status and catch every error right away
 */

class ServerPort {
private:
    // status val
    int socket_fd;
    //custom val
    int port;
    std::string buffer;   // sammelt angekommene Bytes

public:
    ServerPort(int port);
    ~ServerPort();

    uint8_t Listener();
    void send_request();
    void back_to_original_controller(int opp_type, int back_to_server);

    //getter && setter
    std::string get_buffer();
};



#endif //AWS_HACKATHAN_SERVERPORT_H
