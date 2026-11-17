import os
from google import genai
from  google.genai import types
import VectorDB.croma_db as croma_db

"""
Die .env ist diesesmal nicht beigelegt, da dort meien persönliche Anbindung an gemini geegben ist.
Also bitte Erstelle dir eine eigenen DOTENV mit einem selbstgenereirten API KEY (zum ersten testen geht das alles auch kostenlos!)
"""
from dotenv import load_dotenv
load_dotenv()  # Lädt automatisch die .env aus dem Projekt-Root
client = genai.Client(api_key=os.getenv("GEMINI_API_KEY"))

def send_error_message(error_json : dict) -> str:
    """
    Takes every error messages + adds vector db
    sends to API
    """
    if not error_json:
        return "No errors provided."
    #aufbauen des gesamten Prompts
    gemeral_str = ""
    for _ , error in error_json.items():
        gemeral_str += f"{error} << Solution : {croma_db.add_vector_to_msg(1, error)};;"
    first_error = next(iter(error_json.values()))
    gemeral_str += str(croma_db.get_general_intel(2, first_error))
    #error_message wird dann an die API geschickt
    solution = request_ai_gemini()
    save_entry_history(first_error, solution)
    return solution
"""
We will use GEMINI als unsere Main AI
"""
def request_ai_gemini(prompt : str) -> str:
    response = client.models.generate_content(
        model="gemini-2.5-flash",
        contents=prompt,
        config=types.GenerateContentConfig(
            system_instruction="You are a senior DevOps & FinOps SRE assistant. Be concise, technical, and actionable.",
            temperature=0.2,
        )
    )
    solution = response.text # da json format abgraiefen von nur text
    return solution

def save_entry_history(error : str, solution : str) -> None:
   ...