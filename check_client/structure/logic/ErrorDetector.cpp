//
// Created by Admin on 09.11.2026.
//

#include "ErrorDetector.h"
#include "../ErrorDetector.h"
#include "../FormalStructures/PrefixTree.h"

ErrorDetector::ErrorDetector() {
    std::vector<std::string> error_word_list;
    int error_word_size;
    std::ifstream file("../get_logs/error_fails.txt");
    if (!file.is_open()) {
        std::cout << "Error detector could not open file";
    }
    std::string line;
    while (std::getline(file, line)) {
        error_word_list.at(error_word_size) = line;
        error_word_size++;
    }
    file.close();
    this->error_tree = PrefixTree();
    for (int i = 0; i<error_word_size; i++) {
        tree.insert(error_word_list.at(i));
    }
}

int ErrorDetector::detect_string(std::string test) {
    //get all words to iterate over every one to check for errors
    std::vector<std::string> word_list;
    int wl_size;
    std::string temp;
    int current_size;
    for (int i = 0; i<test.size(); i++) {
       if (test.at(i) == ' ') {
           word_list[wl_size] = temp;
           wl_size++;
           continue;
       }
        temp[current_size] = test.at(i);
        current_size ++;
    }
    //get all Key words for system errors -> outsourced in Konstruktur für bessere Laufzeit
    //nun unter this->error_tree

    int amounts_of_error_connects = 0;
    for (int i = 0; i<wl_size; i++) {
        bool is_in_error = this->error_tree.starts_with(word_list.at(i));
        if (is_in_error) {
            amounts_of_error_connects++;
        }
    }
    /*
     * 8 = max -> 100%
     * 0 = min -> 0%
     */
    if (amounts_of_error_connects == 0) {
        this->probability = 0.0;
        return 0;
    }else if (amounts_of_error_connects >= 8){
        this->probability = 1.0;
        return 2;
    }
    /*
     * between with an exponantial fuinction:
     *  -> must have a (8 / 1.0) && (0 /0.0)#
     *  => f(x) = (e^kx-1)/(e^k8-1)
     */
    if (amounts_of_error_connects > 0 && amounts_of_error_connects < 8) {
        double k = 0.5;
        double x = amounts_of_error_connects;
        this->probability += (std::exp(k*x)-1)/(std::exp(k*8)-1);
        return 1;
    }
    return -1;
}

//getter && setter
 float get_probability() {
    return this->probability;
}
