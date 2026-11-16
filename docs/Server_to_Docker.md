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