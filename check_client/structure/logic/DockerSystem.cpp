//
// Created by Admin on 12.11.2026.
//

#include <iostream>
#include<fstream>

#include "DockerSystem.h"
DockerSystem::DockerSystem(std::string docker_name ) {
    this->docker_name = docker_name;
    this->stack = Stack(10);
}

void DockerSystem::fill_stack_with_log() {
    std::system("../get_logs/get_logs.sh %s %i", this->docker_name.c_str(), 1);
    std::string filename = "logs_" + this->docker_name + "_data.txt";

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
    std::line;
    while (std::getline(log_list, line)) {
        Item new_entry = Item(line);
        this->stack.push(new_entry);
    }
    log_list.close();
}

std::string DockerSystem::pop_first() {
    return this->stack.get_top();
}

void crop_stack(int max) {

}


//getter && setter
std::string DockerSystem::get_docker_name() {
    return this->docker_name;
}
Stack DockerSystem::get_stack() {
    return this->stack;
}