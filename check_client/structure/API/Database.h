//
// Created by Admin on 12.11.2026.
//

#ifndef AWS_HACKATHAN_DATABASE_H
#define AWS_HACKATHAN_DATABASE_H



class Database {
private:
    std::stirng database_name;
    std::staring table_name;
public:
    Database(std::stirng database_name, std::stirng table_name);
    ~Database();
    std::string get_name_from_id(std::string id);
    void save_error_entry(std::string id, std::vector<std::string> error);

};



#endif //AWS_HACKATHAN_DATABASE_H
