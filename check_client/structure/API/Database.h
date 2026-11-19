//
// Created by Admin on 12.11.2026.
//

#ifndef AWS_HACKATHAN_DATABASE_H
#define AWS_HACKATHAN_DATABASE_H


class Database {
private:
    std::string database_name;
    std::string table_name;
public:
    Database(std::string database_name, std::string table_name);
    ~Database();
    std::string get_name_from_id(std::string id);
    void save_error_entry(std::string id, std::unorderd_map<int, std::string> error);
    void save_docker_name(std::vector<std::string> docker_names);

};



#endif //AWS_HACKATHAN_DATABASE_H
