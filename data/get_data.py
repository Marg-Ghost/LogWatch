import sqlite3

class Database:
    def __init__(self,server : str, table : str):
        self.servers = server
        self.table = table

    def get_data_from_database(self, select_collums: list, where_collum: str = None, where_value: str = None) -> dict:
        connection = sqlite3.connect(self.servers)
        cursor = connection.cursor()
        query = f"SELECT {', '.join(select_collums)} FROM {self.table}"
        if where_collum and where_value:
            query += f" WHERE {where_collum} = ?"
            cursor.execute(query, (where_value,))
        cursor.execute(query)
        rows = cursor.fetchall()
        connection.close()

        data = []
        for row in rows:
            data.append({
                "id": row[0],
                "name": row[1],
                "status": row[2],
                "last_updated": row[3]
            })
        return {"data": data}

    def inster_data(self) -> None:
        ... # Insert data into the SQLite database

    def update_data(self) -> None:
        ... # Update data in the SQLite database

    def delete_data(self, name: str) -> None:
        ... # Delete data from the SQLite database


    