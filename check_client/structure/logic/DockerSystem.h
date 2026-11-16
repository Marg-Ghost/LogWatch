//
// Created by Admin on 12.11.2026.
//

#ifndef AWS_HACKATHAN_DOCKERSYSTEM_H
#define AWS_HACKATHAN_DOCKERSYSTEM_H

#include "../FormalStructures/Stack.h"


class DockerSystem {
private:
    std::string docker_name;
    Stack stack;
public:
    DockerSystem(std::string docker_name);

    void fill_stack_with_log();
    void crop_stack(int max);

    //getter && setter
    std::string get_docker_name();
    Stack get_stack();
};



#endif //AWS_HACKATHAN_DOCKERSYSTEM_H
