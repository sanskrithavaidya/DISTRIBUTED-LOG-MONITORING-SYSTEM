# Distributed Log Monitoring System

A **multithreaded distributed log monitoring system** built in **C** using **TCP socket programming and Windows Winsock**. Multiple clients can connect to a centralized server and transmit application logs in real time. The server processes logs, detects errors, monitors client health, and maintains per-client statistics.

## 🚀 Features

- TCP-based client-server communication
- Multithreaded server for multiple concurrent clients
- Unique client identification
- Real-time log collection and processing
- INFO, WARNING, and ERROR classification
- Real-time ERROR alerts
- Heartbeat-based client monitoring
- Client health and online/offline status
- Per-client log statistics
- Timestamped logs
- Persistent server-side log storage
- Mutex-based synchronization

## 🏗️ Architecture

```text
        CLIENT-01 ───┐
        CLIENT-02 ───┤
        CLIENT-03 ───┼── TCP ──► CENTRAL SERVER
        CLIENT-04 ───┘             │
                                   ├── Log Processing
                                   ├── Error Detection
                                   ├── Heartbeat Monitoring
                                   ├── Client Statistics
                                   └── Persistent Logging
```

The server uses a **thread-per-client architecture**, allowing multiple clients to send logs concurrently.

## 🛠️ Technologies

- **Language:** C
- **Networking:** TCP/IP, Socket Programming
- **API:** Windows Winsock2
- **Concurrency:** Multithreading
- **Synchronization:** Mutex
- **Platform:** Windows
- **Version Control:** Git & GitHub

## 📊 Sample Output

```text
========================================
   DISTRIBUTED LOG MONITORING SERVER
========================================
Server Status : RUNNING
Port          : 5000
Multi-client  : ENABLED
Monitoring    : ENABLED
Statistics    : ENABLED
Heartbeat     : ENABLED
========================================

Client connected. Thread ID: 21704

[2026-09-01 21:52:29] [Thread 21704] [CLIENT-01] INFO: Application started
[2026-09-01 21:52:31] [Thread 21704] [CLIENT-01] WARNING: Memory usage is high

************ ERROR ALERT ************
[2026-09-01 21:52:33] [Thread 21704] [CLIENT-01] ERROR: Database connection failed
Client CLIENT-01 generated an ERROR.
*************************************

[2026-09-01 21:52:35] [Thread 21704] [CLIENT-01] INFO: Application stopped

Client disconnected. Thread ID: 21704
```

### Client Statistics

```text
====================================================
                CLIENT STATISTICS
====================================================
Client          Total    INFO     WARNING    ERROR
----------------------------------------------------
CLIENT-01       4        2        1          1
CLIENT-02       4        2        1          1
CLIENT-03       4        2        1          1
CLIENT-04       4        2        1          1
====================================================
```

## ⚙️ Setup

### Prerequisites

- Windows
- GCC / MinGW
- Git

### Clone

```bash
git clone https://github.com/sanskrithavaidya/DISTRIBUTED-LOG-MONITORING-SYSTEM.git
cd DISTRIBUTED-LOG-MONITORING-SYSTEM
```

### Compile Server

```powershell
gcc server.c -o server.exe -lws2_32
```

### Compile Client

```powershell
gcc client.c -o client.exe -lws2_32
```

## ▶️ Run

### Start Server

```powershell
.\server.exe
```

The server listens on **TCP port 5000**.

### Start Clients

Open separate terminals and run:

```powershell
.\client.exe CLIENT-01
```

```powershell
.\client.exe CLIENT-02
```

```powershell
.\client.exe CLIENT-03
```

```powershell
.\client.exe CLIENT-04
```

Multiple clients can connect simultaneously and transmit logs to the server.

## 📁 Project Structure

```text
DISTRIBUTED-LOG-MONITORING-SYSTEM/
│
├── client.c
├── server.c
├── README.md
└── .gitignore
```

Generated executables and runtime log files are excluded from Git using `.gitignore`.

## 🔮 Future Enhancements

- Web-based monitoring dashboard
- Database-backed log storage
- Advanced log filtering and search
- Configurable alert thresholds
- Authentication and authorization
- Cloud deployment
- Docker support
- Log analytics and visualization

## 🎯 Key Concepts

This project demonstrates practical knowledge of:

**Computer Networks • TCP/IP • Socket Programming • Client-Server Architecture • Multithreading • Concurrency • Synchronization • Distributed Systems • Log Monitoring**

## 👩‍💻 Author

**Sanskritha Vaidya**

B.Tech — Artificial Intelligence & Machine Learning

---

⭐ If you find this project useful, consider giving it a star.