//
// Created by Admin on 09.11.2026.
//

//external
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
//internal
#include "ServerPort.h"
#include "../ServerPort.h"

ServerPort::ServerPort(int port) {
    /*
    * Dear Reader pls excuse the many explainations in this code but this is my first time
    * dealing with raw sockets in C++, so I'm kinda still figuring it out...
    * btw thx Beej's Guide to Network Programming couldn't have done it without you
    */
    this->port = port;
    int status;
    struct addrinfo hints; // hints als Settings -> Übertragen dann auch servinfo
    struct addrinfo *servinfo; // quasi Briefkasten | verkettete Liste mit möglichen Adressen
    memset(&hints, 0, sizeof(hints)); // certifiyes structure is empty
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6 hints.
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets hints.
    hints.ai_flags = AI_PASSIVE; // Ich bin ein Server, nimm meine eigene Adresse. -> NULL

    /*Whats: getaddrinfo? :
     * | takes | a human-readable address (like "[www.example.com](https://www.example.com)" && Port
     *======================================================================================================
     * status = getaddrinfo(NULL, "3490", &hints, &servinfo);
     * //  ^                                          ^
     * //  Rückgabewert (return)              Out-Parameter
     * | 1.returns (via. Out-Parameter) | a linked list of structured address data : {IP4, IP6}
     *               Out-Parameter (ein Zeiger auf einen Zeiger, struct addrinfo **res). Hier schreibt getaddrinfo das eigentliche Ergebnis rein.
     * | 2.returns | int a status Code zum Testen nach init error
     */

     /*1.Parameter:
     *  1. NULL || DNS-Adresse
     *  2. Port
     *  3. Settings_data
     *  4. Aufzufüllender Kasten
     */
    status = getaddrinfo(NULL, this->port.aschar() , &hints, &servinfo);
    if (status != 0) {
        fprintf (stderr, "getaddrinfo error: %s\n", gai_strerror(status) ) ;
        exit(1);
    }

    this->socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    // all return vals fpr socket_fd are diff. types exept "-1" is an Error
    if (socket_fd == -1) {
        perror("Error | socket");
        exit(1);
    }

    if (bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        perror("Error | bind");
        exit(1);
    }
    freeaddrinfo(servinfo);
}

int ServerPort::Listener() {
    int extracted_id = 0;
    int extracted_type = 0;

    // 1. ID extrahieren
    size_t id_pos = raw.find("\"id\"");
    if (id_pos != std::string::npos) {
        size_t colon_pos = raw.find(":", id_pos);
        size_t end_pos = raw.find_first_of(",}", colon_pos);
        if (colon_pos != std::string::npos && end_pos != std::string::npos) {
            std::string val = raw.substr(colon_pos + 1, end_pos - colon_pos - 1);
            extracted_id = val;
        }
    }

    // 2. Type extrahieren
    size_t type_pos = raw.find("\"type\"");
    if (type_pos != std::string::npos) {
        size_t colon_pos = raw.find(":", type_pos);
        size_t end_pos = raw.find_first_of(",}", colon_pos);
        if (colon_pos != std::string::npos && end_pos != std::string::npos) {
            std::string val = raw.substr(colon_pos + 1, end_pos - colon_pos - 1);
            extracted_type = std::stoi(val); // stoi : str -> int
        }
    }

    std::cout << "[C++] Empfangen - ID: " << extracted_id << ", Type: " << extracted_type << std::endl;
    this->buffer = extracted_id;
    return extracted_type;
}

ServerPort::~ServerPort() {
    close(this->socket_fd);
}

void ServerPort::back_to_original_controller(int opp_type, int back_to_server) {
    std::string url = "/server_status/basic/answer_type_" + std::to_string(opp_type) + "/" + std::to_string(back_to_server);
    std::string host = "127.0.0.1";
    std::string outer_port = "9000";
    struct addrinfo hints{}, *servinfo;
    hints.ai_family = AF_UNSPEC;     // IPv4 oder IPv6 egal
    hints.ai_socktype = SOCK_STREAM; // TCP Strea m

    // Wichtig: KEIN AI_PASSIVE, weil wir hier als Client rausfunken!
    int status = getaddrinfo("127.0.0.1", "9000", &hints, &servinfo);
    if (status != 0) {
        std::cerr << "[C++] getaddrinfo Fehler: " << gai_strerror(status) << std::endl;
        return;
    }
    int sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sock < 0) {
        freeaddrinfo(servinfo);
        return;
    }
    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
        std::cerr << "[C++] Verbindung zum FastAPI-Controller fehlgeschlagen!" << std::endl;
        close(sock);
        freeaddrinfo(servinfo);
        return;
    }

    freeaddrinfo(servinfo); // Briefkasten dings ->  wieder aufräumen
    /*
     * Falls mal jemand aus sienen HTB-lessons kommt und socket Programmierung mit curl verwechselt: <- aka ich selbst
    * Das -X (bzw. --request) ist nur eine Kommandozeilen-Option von curl – kein Teil des HTTP-Protokolls selbst!
    * Wenn du im Terminal curl -X POST [http://127.0.0.1:8000/](http://127.0.0.1:8000/)... eintippst, sagt das -X POST dem curl-Programm lediglich:
    * "Schreibe ganz oben in die allererste Zeile des rohen HTTP-Pakets das Wort POST statt GET."
    *
    * Da du in C++ mit Sockets auf der rohen Protokoll-Ebene arbeitest, schreibst du genau diese erste Zeile (die sogenannte Request-Line) selbst
     */
    //Minimalen HTTP POST Request abfeuern
    std::string http_request =
        "POST " + url + " HTTP/1.1\r\n"
        "Host: " + host + ":" + std::to_string(port) + "\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n\r\n";

    send(sock, http_request.c_str(), http_request.size(), 0);
    close(sock);
}

//getter && setter
std::string ServerPort::get_buffer() {
        return this->buffer;
}