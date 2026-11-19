//external
#include <iostream>
#include<fstream>
#include <iostream>
#include <vector>
//internal
#include "structure/API/Database.h"
#include "structure/logic/ErrorDetector.h"
#include "structure/logic/DockerSystem.h"
#include "structure/FormalStructures/Stack.h"
#include "structure/FormalStructures/Item.h"
#include "structure/API/ServerPort.h"

std::vector<std::string> get_container();
float get_error_eval_on_container(std::string docker_name, DockerSystem docker);


int main() {
    std::cout<<"Init | cpp-Server Deamon"<<std::endl;
    ServerPort server = ServerPort(9876);
    Database database = Database("../data/Docker.db", "docker_deamons");
    while (true) {
        //Reqeust to server from server
        int opp_type = server.Listener();
        std::string id = server.get_buffer();

        /*
         * 1 = handels one Checkup of one Docker Container and sents back the result
         */
        int back_to_server = 0; //every type sends it's type with an according error code back

        switch (opp_type) {
            /*
             * Hier wird über die Reqeust Typen aus -> Server_to_Docker.md entschieden
             * somit docht #1-Type ist hier case 1
             * ...
             */
            case 1:
                //json mit der DB vergleichen ID -> In Docker
                std::string docker_name = database.get_name_from_id(id);
                //get error probability
                //Error-Detector for server
                DockerSystem docker = DockerSystem(docker_name);
                docker.fill_stack_with_log();
                float error_probability = get_error_eval_on_container(docker_name, docker);

                int number_message;
                if (error_probability < 0.3) {
                    docker.crop_stack(0);
                    number_message = 0;
                    back_to_server = 0;
                }else if (error_probability < 0.5) {
                    docker.crop_stack(3);
                    number_message = 3;
                    back_to_server = 1;
                }else if (error_probability < 0.8) {
                    docker.crop_stack(5);
                    number_message = 5;
                    back_to_server = 1;
                }else if (error_probability > 0.8) {
                    docker.crop_stack(10);
                    number_message = 10;
                    back_to_server = 1;
                }

                 //if error vorhanden wird er in der DB gespeichert, stack übergabe
                std::unordered_map<int, std::string> errors;
                for (int i=number_message; i != 0; i--) {
                    errors[number_message] = docker.get_stack().pop().get_data();
                }
                // in errors kommt dann noch der str rein!
                //return to http server python
                database.save_error_entry(id, errors);
                server.back_to_original_controller(opp_type, back_to_server);
            case 2:
                std::vector<std::string> docker_deamon = get_container();
                //database
                database.save_docker_name(docker_deamon);
                server.back_to_original_controller(opp_type, 0);
        }
        return 0;
    }
}
float get_error_eval_on_container(std::string docker_name, DockerSystem docker) {
        ErrorDetector detector;
    int probability_eval = detector.detect_string(docker.get_stack().pop().get_data());
    float error_probability = 0.0;
    switch (probability_eval) {
        case -1:
            std::cout<<"ErrorDetector | Error couldn't evaluate Probability"<<std::endl;
            break;
        case 0 && 2:
            error_probability = detector.get_probability();
            break;
        case 1:
            while (docker.get_stack().empty() != true || detector.get_probability() < 1.0) {
                detector.detect_string(docker.get_stack().pop().get_data());
            }
            //just as a safe case
            if (detector.get_probability() < 1.0) {
                error_probability = detector.get_probability();
            }else {
                error_probability = 1.0;
            }
            break;
    }
    return error_probability;
}

std::vector<std::string> get_container_names() {
    std::vector<std::string> docker_deamon;
    int dd_len = 0;
    std::system("../get_logs/get_container.sh");
    std::ifstream conainer_list("../get_logs/container_list");
    if (!conainer_list.is_open()) {
        std::cout << "[Error] : couldn't open the file";
    }

    std::string line;
    while (std::getline(conainer_list, line)) {
        if (dd_len == 0) {
            continue;
        }
        std::vector<std::string> parts;
        std::string temp;
        //Structure ist : CONTAINER ID   IMAGE
        // somit brauchen wir nur solange zu iteriren bis IMAGE auftaucht
        for (int i = 0; i < 1; i++) {
            temp += line[i];
            if (line[i] == ' ') {
                parts.push_back(temp);
                temp = "";
            }
        }
        docker_deamon[dd_len] = parts[1];
        dd_len++;
    }

    conainer_list.close();
    return docker_deamon;
}
