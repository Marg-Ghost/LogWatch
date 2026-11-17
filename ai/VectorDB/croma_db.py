import chromadb
import json

# initialize the client with a persistent path
client = chromadb.PresistantClient(path="./croma_db")

collection1 = client.create_collection("error_messages_and_solutions")
collection2 = client.create_collection("general_intel")

#load json intel
errmsg_solution, id1 = json.load(open("error_messages_and_solution.json", "r"))
intel, id2 = json.load(open("general_intel.json", "r"))

if collection1.count() == 0:
    collection1.add(
        documents=errmsg_solution,
        ids=id1
    )
if collection2.count() == 0:
    collection2.add(
        documents=intel,
        ids=id2
    )

collection1.upsert(
    documents=errmsg_solution,
    ids=id1
)
collection2.upsert(
    documents=intel,
    ids=id2
)


def get_general_intel(type : int, data : str ) -> str:
    match(type):
        case 1:
            if data == None:
                raise ValueError("Error | No error entry provided")
            results = collection1.query(query_texts = [data], n_results = 1)
            documents = results.get("documents", [[]])[0]
            if not documents:
                return "No previous entries to this specific Error"
            return documents
        case 2:
            # Retrieve general intelligence
            if data == None:
                raise ValueError("Error | No error entry provided")
            results = collection2.query(query_texts = [data], n_restults = 3)
            documents = results.get("documents", [[]])
            if not documents:
                return "No extra Information for this argument"
            return documents