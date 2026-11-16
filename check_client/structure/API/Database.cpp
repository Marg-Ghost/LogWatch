//
// Created by Admin on 12.11.2026.
//

//intern
#include "Database.h"

//extern
#include <sqlite3.h>
#include <iostream>

Database::Database(std::stirng database_name, std::stirng table_name) {
    this->database_name = database_name;
    this->table_name = table_name;
}
Database::~Database();

std::string Database::get_name_from_id(std::string id) {
    sqlite3* db;
    char* errMessage = 0;
    // 1. Datenbank öffnen (erstellt die Datei "sentinel.db", falls sie noch nicht existiert)
    int rc = sqlite3_open(this->database_name.c_str(), &db);
    if (rc) {
        std::cerr << "[C++] Fehler beim Öffnen der Datenbank: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }else{
        std::cout << "[C++] Erfolgreich mit SQLite-Datenbank verbunden!" << std::endl;
    }
    std::string deamon_name = "SELECT name FROM %s WHERE id = %s";this->database_name.c_str(), id.c_str();

    rc = sqlite3_exec(db, deamon_name.c_str(), NULL, NULL, &errMessage);
    if (rc != SQLITE_OK) {
        std::cout << "[c++] SQL Fehler: " << errMessage << std::endl;
        sqlite3_free(errMessage);
    }
    sqlite3.close(db);
    return deamon_name;
}

std::string escape_json(const std::string& s);
void Database::save_error_entry(std::string id, std::unordered_map<int, std::string> error) {
    //init db connection
    sqlite3* db;
    char* errMessage = 0;
    int rc = sqlite3_open(this->database_name.c_str(), &db);
    if (rc) {
        std::cout << "[C++] Fehler beim Öffnen der Datenbank: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }else {
        std::cout << "[C++] Erfolgreich mit SQLite-Datenbank verbunden!" << std::endl;
    }
    int deamon_id = std::stoi(id);
    sqlite3_stmt* stmt = nullptr;

    //if der Stack leer ist, dann wir der Entry aus dem Table gelöscht
    if (error.empty()) {
        std::string sql_delete = "DELETE FROM database_deamon_errors WHERE id = ?;";
        rc = sqlite3_prepare_v2(db, sql_delete.c_str(), -1, &stmt, nullptr);
        if (rc == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, deamon_id);
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "[C++] Delete Fehler: " << sqlite3_errmsg(db) << std::endl;
            }
        } else {
            std::cerr << "[C++] Prepare Delete Fehler: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
        return;
    }

    //Hashtable (unordered_map) in einen JSON-String verwandeln: {"1": "err1", "2": "err2"}
    std::string json_str = "{";
    size_t count = 0;
    for (const auto& [id_stack, err_msg] : error) {
        json_str += "\"" + std::to_string(id_stack) + "\": \"" + escape_json(err_msg) + "\"";
        if (++count < error.size()) {
            json_str += ", ";
        }
    }
    json_str += "}";
    //UPSERT direkt auf Spalten (id, error_message)
    std::string sql = "INSERT INTO database_deamon_errors (id, error_message) VALUES (?, ?) "
                      "ON CONFLICT(id) DO UPDATE SET "
                      "error_message = excluded.error_message, "
                      "timestamp = CURRENT_TIMESTAMP;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "[C++] Prepare Fehler: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }
    //Daemon-ID und JSON-String binden
    sqlite3_bind_int(stmt, 1, std::stoi(id));
    sqlite3_bind_text(stmt, 2, json_str.c_str(), -1, SQLITE_STATIC);
    // 4. Ausführen & Aufräumen
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "[C++] Execution Fehler: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

//checht die Error massage nach poentiellen kritschen char ab, sodass es nicht flsch geseichert wird
std::string escape_json(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}