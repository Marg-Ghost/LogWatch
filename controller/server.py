#external
#Server
import html

from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles
import uvicorn
#System
import pathlib


ROOT = pathlib.Path(__file__).resolve().parent.parent
WEBDIR = ROOT / "web"

app = FastAPI()

app.mount("/web", StaticFiles(directory=str(WEBDIR), html=True), name="web")

#################################
# DB anbindung
################################
from data.get_data import Database
COMMON_DOCKER = Database(str(ROOT / "data" / "Docker.db"), "database_deamon")
ERROR_DOCKER = Database(str(ROOT / "data" / "Docker.db"), "database_deamon_errors")

#########################################################
#Frontend
#########################################################
# pages
@app.get("/")
async def main_monitor():
    file_name = "index.html" 
    direction = WEBDIR / file_name
    if not direction.exists():
        return HTTPException(status_code=404, detail=f"File {file_name} not found")
    return FileResponse(str(direction))

# Intel-getter
@app.get("/load_all_servers")
async def get_all_servers() -> dict[str, list[dict]]:
    return COMMON_DOCKER.get_docker_container_data()

# setter
def set_all_servers():
    dict_return = COMMON_DOCKER.get_data_from_database("*")
    names = dict_return["data"]
    return names

##########################################################
#Backend
##########################################################





############################################################
# Connection to secound cpp-Server
############################################################

#cpp Befahlsaufrufe
app.get("/server_status/basic/reqeust_type_1")
async def reqeust_type_1():
    from check_client import reqeust_type_1
    reqeust_type_1()
    return

# app.get("/server_status/settings/refresh_total_server")
def check_all_server():
    from check_client import reqeust_type_2
    reqeust_type_2()
    return

#cpp Answers
app.post("/server_status/basic/answer_type_1{id,return_code}")
async def answer_type_1(id: int, return_code: int):
    id = html.escape(str(id))
    return_code = html.escape(str(return_code))
    if return_code == "0":
        return
    
    error_json = ERROR_DOCKER.get_data_from_database("error_messages", "id", id) #<-Returns a json
    # {get the error message from the database and handle it accordingly} 
    # Kommt nun in die RAG Pipline...#
    import ai.ai_connect as AI
    solution = AI.send_error_message(error_json)
    return {"id": id,"solution": solution}


app.post("/server_satus/basic/settings/get_total_server{return_code}")
async def answer_type2(return_code: int) -> None:
    return_code = int(html.escape(str(return_code)))
    match(return_code):
        case 0: 
            return {"status": "no_changes"}
        case 1:
            names = set_all_servers()
            return {"status", names}



    


if __name__ == "__main__":
    print (WEBDIR / "index.html" )
    uvicorn.run(app,host="127.0.0.1",port=9000)
