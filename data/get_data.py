import json
import sqlite3

class Database:
    def __init__(self,server : str, table : str):
        self.servers = server
        self.table = table

    def get_data_from_database(self, select_collums: list | str, where_collum: str = None, where_value: str = None) -> dict:
        connection = sqlite3.connect(self.servers)
        try:
            cursor = connection.cursor()
            columns = ", ".join(select_collums) if isinstance(select_collums, list) else select_collums
            query = f"SELECT {columns} FROM {self.table}"
            parameters = ()
            if where_collum is not None and where_value is not None:
                query += f" WHERE {where_collum} = ?"
                parameters = (where_value,)
            cursor.execute(query, parameters)
            column_names = [column[0] for column in cursor.description]
            rows = cursor.fetchall()
        finally:
            connection.close()

        data = [dict(zip(column_names, row)) for row in rows]
        return {"data": data}

    def get_docker_container_data(self) -> dict[str, list[dict]]:
        connection = sqlite3.connect(self.servers)
        connection.row_factory = sqlite3.Row
        try:
            rows = connection.execute(f"""
                SELECT d.id, d.name, d.status, d.importance, d.type, d.last_updated, d.version,
                       e.error_messages
                FROM {self.table} AS d
                LEFT JOIN database_deamon_errors AS e ON e.id = d.id
                ORDER BY d.id
            """).fetchall()
        finally:
            connection.close()

        data = []
        for row in rows:
            errors = json.loads(row["error_messages"]) if row["error_messages"] else {}
            data.append({
                "id": row["id"],
                "name": row["name"],
                "status": row["status"],
                "importance": row["importance"],
                "type": row["type"],
                "last_updated": row["last_updated"],
                "version": row["version"],
                "errors": errors,
                "problems": len(errors) if isinstance(errors, (dict, list)) else 0
            })
        return {"data": data}

    def inster_data(self) -> None:
        ... # Insert data into the SQLite database

    def update_data(self) -> None:
        ... # Update data in the SQLite database

    def delete_data(self, name: str) -> None:
        ... # Delete data from the SQLite database


    