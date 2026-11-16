import sqlite3

def init_table_database_deamon() -> None:
        conn = sqlite3.connect("data/Docker.db")
        cursor = conn.cursor()
        cursor.execute(f"""
            CREATE TABLE IF NOT EXISTS database_deamon (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                status TEXT NOT NULL,
                importance INTEGER NOT NULL,
                type TEXT NOT NULL,
                last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                version TEXT
            )
    """)   
        conn.commit()
        conn.close()

def init_table_database_deamon_errors() -> None:
        conn = sqlite3.connect("data/Docker.db")
        cursor = conn.cursor()
        # Fremdschlüssel-Prüfung in SQLite aktivieren -> for foreign key's 
        cursor.execute("PRAGMA foreign_keys = ON;")
        # In Error messages wird eine Hashtable gespeichert mit allen Errors asu dem Stack
        cursor.execute(f"""
            CREATE TABLE IF NOT EXISTS database_deamon_errors (
                id INTEGER PRIMARY KEY NOT NULL,
                error_messages TEXT NOT NULL,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (id) REFERENCES database_deamon(id) ON DELETE CASCADE
            )
        """)
        conn.commit()
        conn.close() 

if __name__ == "__main__":
    init_table_database_deamon()
    init_table_database_deamon_errors()