# Diffrent Types of  Adresses are specified in main.cpp - Diagramm for each :
# case : 1
````mermaid
---
config:
  htmlLabels: false
---
flowchart LR
    %% Knoten definieren
    Python[Python Controller\nserver.py]
    DB_Top[(Containers DB\nName / ID)]
    CPP[C++ Backend\nmain.cpp]
    Docker([Docker Daemon\nLogs])
    Detector[Error Detector\nDocker.h]
    Score["Log Score / Lines\n[10 ... 1]\n(0 - 10)"]
    DB_Bottom[(Errors DB\nDocker.db)]

    %% 1. Namens-Auflösung & Trigger
    Python -->|ID| DB_Top
    DB_Top -->|Name| Python
    Python ==>|"Type = 1, ID"| CPP
    CPP -->|ID| DB_Top
    DB_Top -->|Name| CPP

    %% 2. Log-Abruf & Analyse
    CPP -->|Name| Docker
    Docker -->|last 10 log lines| Detector
    CPP -->|Docker.h| Detector
    Detector -.-> Score

    %% 3. Rückmeldung & Error-Handling
    CPP ==>|"0 / 1"| Python
    Python -->|"if return 1"| DB_Bottom
    CPP -.-> DB_Bottom
    DB_Bottom -.->|"if Return server == 1 -> get {error}"| Python

````
# case 2
The following diagram describes the planned C++ inventory-refresh flow. The
current `GET /load_all_servers` endpoint reads the container and error tables
directly; it does not start this C++ flow. The TCP implementation needed for
the planned flow is not complete yet (see "Start- und Socket-Aufbau" below).
````mermaid
flowchart LR
    UI["Web UI<br/>index.html / main.js"] -->|"Button-Klick / Load<br/>GET /load_all_servers"| API["FastAPI Endpoint<br/>server.py"]
    API -->|"check_all_server()"| Ctrl["Python Controller<br/>server.py"]
    Ctrl -->|"opp_type = 2"| CPP["C++ Backend<br/>main.cpp (case 2)"]
    CPP -->|"get_container_names()"| Daemon(["Docker Daemon<br/>Container-Liste"])
    Daemon -->|"Namen / IDs"| CPP
    CPP -->|"save_docker_name()"| DB[("SQLite DB<br/>database_deamon / AllDockerContainer")]
    CPP -.->|"back_to_original_controller(2, 0)"| Ctrl
    Ctrl -->|"fetch_containers_from_db()"| DB
    DB -->|"Container-Liste"| Ctrl
    Ctrl -->|"JSON (containers)"| API
    API -->|"Response"| UI
````

## Start- und Socket-Aufbau

### Aktueller Stand

Debian Linux ist die vorgesehene Laufzeitumgebung für den C++-Socket und die
Docker-Skripte. Der Browser spricht HTTP mit FastAPI auf Port `9000`;
`controller/check_client.py` soll eine TCP-Verbindung zu `127.0.0.1:9876`
öffnen. `Listener()` ist der vorgesehene Einstiegspunkt für die
Auftragsverarbeitung. Der vollständige Request-Ablauf muss auf dem
Debian-Zielsystem getestet werden: Dazu gehören das Annehmen der Verbindung,
das Lesen des Requests und die Rückgabe des Ergebnisses. Diese Punkte
beschreiben den Implementierungs- und Teststand, nicht die Eignung von Debian
oder des gewählten Socket-Ansatzes.

Außerdem schließt der Python-Client die Verbindung direkt nach `sendall()`, ohne
eine Antwort abzuwarten. C++ versucht seinerseits, Ergebnisse über einen
separaten HTTP-Aufruf an FastAPI zurückzusenden. Für den ersten funktionierenden
Durchlauf sollte nur eine dieser Rückgabevarianten übrig bleiben.

### Empfohlener erster Ablauf

1. FastAPI und C++ auf demselben Debian-Host starten. Die
   C++-Socket-Implementierung und Docker-Skripte sind für diesen Linux-Host
   vorgesehen.
2. FastAPI bleibt der HTTP-Einstiegspunkt auf `9000`; C++ läuft als dauerhaft
   aktiver TCP-Server auf `9876`.
3. Für jede TCP-Verbindung sendet Python genau eine JSON-Zeile, zum Beispiel
   `{"id": 0, "type": 2}\n`. C++ liest bis zum Zeilenumbruch, bearbeitet den
   Auftrag und antwortet mit einer JSON-Zeile. Python liest die Antwort, bevor
   die Verbindung geschlossen wird.
4. C++ muss in einer Schleife `listen()` und `accept()` verwenden und pro
   Verbindung vollständig lesen und antworten. Danach erst den HTTP-Endpunkt
   für den Auftrag freigeben. So gibt FastAPI Erfolg oder Fehler des C++-Auftrags
   an den Browser zurück, ohne einen zweiten Callback-Port.
5. Für den lokalen Web-Teil genügen die Befehle in `Web-app.md`; die
   Containerliste kann dort bereits aus SQLite geladen werden. Die C++-Prüfung
   bleibt davon getrennt, bis der TCP-Ablauf fertig ist.

### Wenn die Prozesse später in Docker laufen

Für den Zielbetrieb sind zwei Compose-Services sinnvoll: `controller` für
FastAPI und `checker` für den C++-TCP-Server. Zwischen ihnen wird Port `9876`
nicht auf den Host veröffentlicht; der Controller verbindet sich mit
`checker:9876`. `127.0.0.1` innerhalb eines Containers bezeichnet nur genau
diesen Container, nicht den jeweils anderen Service. Nur der Web-Port `9000`
muss für den Browser veröffentlicht werden.

Die SQLite-Datei sollte über ein gemeinsames Volume erreichbar sein, bis eine
eigene Datenbank eingeführt wird. Langfristig sollte Python allein in SQLite
schreiben und C++ Ergebnisse an Python zurückgeben. Falls C++ den Docker-Daemon
über `/var/run/docker.sock` anspricht, braucht der Checker Zugriff auf diesen
Socket; dieser Zugriff kann dem Container weitreichende Kontrolle über den
Host-Docker geben und sollte nur für einen vertrauenswürdigen lokalen Betrieb
aktiviert werden. `sudo` aus den aktuellen Bash-Skripten ist dafür kein
geeigneter Container-Startmechanismus.

Compose mit Healthchecks und Restart-Policies ist für den dauerhaften Betrieb
passender als ein Bash-Skript, das mehrere Hintergrundprozesse startet. Die
CMake-Build-Konfiguration ist in `check_client/CMakeLists.txt` festgelegt.
Compose oder ein Startskript braucht weiterhin die endgültigen
Arbeitsverzeichnisse und das TCP-Nachrichtenformat.

### C++-Build auf Debian

Im Repository-Root:

```bash
sudo apt install build-essential cmake libsqlite3-dev
cmake -S check_client -B check_client/build
cmake --build check_client/build --parallel
```

Die Konfiguration enthält alle C++-Quelldateien und bindet SQLite ein. Sie
beschreibt den Build, bestätigt aber noch keinen erfolgreichen Build oder
End-to-End-Test auf dem Debian-Zielsystem. Zum Bauen sollte der CMake-Target
verwendet werden, nicht `g++ main.cpp` allein: Der Target kompiliert auch die
Implementierungsdateien und bindet SQLite ein.