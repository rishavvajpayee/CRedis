# Basic Redis Implementation in C

## Overview

This project is a basic implementation of Redis, developed to deepen understanding of systems programming, network communication, and socket programming. It includes core features like a simple in-memory key-value store, command parsing, and client handling using multiplexing.

## Features

- **Key-Value Store**: Set, get, and delete key-value pairs.
- **Multiplexing**: Efficient handling of multiple client connections using the `select()` system call.
- **Command Parsing**: Supports basic Redis commands such as SET, GET, and DEL.
- **Graceful Shutdown**: A special "nuke" command to gracefully shut down the server.
- **User-Friendly Prompt**: Displays a prompt for client interactions similar to the Redis CLI.

## Getting Started

### Prerequisites

- GCC compiler
- Make (optional, but recommended for easy build)

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/rishavvajpayee/CRedis.git
   cd Credis/
