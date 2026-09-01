\# Distributed Log Monitoring System



A distributed client-server log monitoring system developed in C using TCP sockets and Windows Winsock. The system allows multiple clients to transmit application logs to a centralized server, where logs are processed, classified, monitored, and stored.



\## Features



\- TCP-based client-server communication

\- Multi-client support

\- Multithreaded server architecture

\- Unique client identification

\- Centralized log collection

\- INFO, WARNING, and ERROR log classification

\- Real-time ERROR alerts

\- Per-client log statistics

\- Heartbeat-based client monitoring

\- Client online/offline health tracking

\- Timestamped log processing

\- Persistent server-side log storage

\- Mutex-based synchronization for shared data

\- Tested with multiple concurrent clients



\## System Architecture



```text

&#x20;            +----------------+

&#x20;            |    Client 01   |

&#x20;            |   TCP Socket   |

&#x20;            +-------+--------+

&#x20;                    |

&#x20;                    |

&#x20;            +-------v--------+

&#x20;            |                |

&#x20;            | Monitoring     |

&#x20;            | Server         |

&#x20;            |                |

&#x20;            | TCP Sockets    |

&#x20;            | Multithreading |

&#x20;            | Log Processing |

&#x20;            | Error Alerts   |

&#x20;            | Statistics     |

&#x20;            | Health Monitor |

&#x20;            +---+--------+---+

&#x20;                |        |

&#x20;         +------+        +------+

&#x20;         |                     |

&#x20;  +------v------+       +------v------+

&#x20;  |   Client 02 |       |   Client 03 |

&#x20;  +-------------+       +-------------+

&#x20;                \\

&#x20;                 \\ 

&#x20;              +--v---------+

&#x20;              |  Client 04 |

&#x20;              +------------+

