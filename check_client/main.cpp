//external
#include <iostream>
#include<fstream>
#incldue<iostream>
//internal
#include "structure/API/Database.h"
#include "structure/logic/ErrorDetector.h"
#include "structure/logic/DockerSystem.h"
#include "structure/FormalStructures/Stack.h"
#include "structure/FormalStructures/Item.h"
#include "structure/API/ServerPort.h"

std::vector<std::string> get_container();
std::vector<DockerSystem> get_error_messages(std::vector<std::string> docker_deamon);
int main() {
    std::cout<<"Init | cpp-Server Deamon";
    ServerPort server = ServerPort(9876);
    Database database = Database("Docker.db");
    while (true) {
        //Reqeust to server from server
        int opp_type = server.Listener();
        std::string id = server.get_buffer();

        switch (opp_type) {
            case 1:
                //json mit der DB vergleichen ID -> In Docker
                std::string docker_name = database->get_name_from_id();
                //Error-Detector for server
                DockerSystem sytstem = get_error_messages(docker_name);
                std::vector<std::string> docker_error = get_error_messages(docker_name);
        }




        //return to http server python
    }












}
std::vector<std::string> get_container() {
    std::vector<std::string> docker_deamon;
    int dd_len = 0;
    std::system("../get_logs/get_container.sh");
    std::ifstream conainer_list("../get_logs/container_list");
    if (!conainer_list.is_open()) {
        std::count << "[Error] : couldn't open the file";
    }

    std::string line;
    while (std::getline(conainer_list, line)) {
        docker_deamon[dd_len] = line;
        dd_len++;
    }
    conainer_list.close();
    return docker_deamon;
}

DockerSystem get_error_messages(std::vector<std::string> docker_deamon) {
    DockerSystem docker_return

    std::system("../get_logs/get_logs.sh %s %i", docker_deamon[i].c_str(), 1);
    std::string filename = "logs_" + docker_deamon[i] + "_data.txt";

    std::ifstream log_list(filename);
    if (!log_list.is_open()) {
        std::count << "[Error] : couldn't open the file";
    }
    /*
    *Every line is a singular feedback - Bsp.:
    *INFO:     127.0.0.1:47126 - "GET / HTTP/1.1" 200 OK
    *ConnectionError: Failed to connect to Ollama. Please check that Ollama is downloaded, running and accessible. https://ollama.com/download
    *---------------------------------------------------------------
    *If current Error ? -> Check bottom message
    *=> Stack and if top = Error -> Return as Docker its false
    */
    Stack error_entires = Stack(10);
    std::line;
    while (std::getline(log_list, line)) {
        Item new_entry = Item(line);
        error_entires.push(new_entry);
    }
    log_list.close();

    //Error Detector : if Error ? 1 : 0
    ErrorDetector detector;
    int probability = detector->detect_string(error_entires.pop());
    if (probability == 0) {
        error_entires.set_max(0);
        error_entires.clean();
        //Docker noch initialisieren
        docker_return = DockerSystem(error_entires, 0);
        return docker_return;
    }else if (probability == 1) {
        error_entires.set_max(3);
        error_entires.clean();
        docker_return = DockerSystem(error_entires, 1);
    }else if (probability == 2) {
        error_entires.set_max(5);
        error_entires.clean();
        docker_return = DockerSystem(error_entires, 2);
    }else if (probability == 3) {
        docker_return = DockerSystem(error_entires, 3);
    }else {
        return NULL;
    }
}