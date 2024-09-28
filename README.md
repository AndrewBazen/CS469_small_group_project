# CS469_small_group_project

## Project Overview

This project involves building a three-tiered client-server system using a MySQL database server and a command line client. The system will include the following components:

1. **Client**: A command line client that interacts with the application server.
2. **Application Server**: Authenticates the client and transforms RPC messages into SQL commands for the MySQL server. It also returns the results of database queries back to the client. The application server process will be replicated for fault tolerance.
3. **Database Server**: A MySQL database server that stores the data.

## Features

- **Client Authentication**: The client must be authenticated using cryptographic methods without storing passwords (either encrypted or plain text).
- **RPC to SQL Transformation**: The application server will transform RPC messages from the client into SQL commands.
- **Fault Tolerance**: The application server process will be replicated to ensure fault tolerance.

## Getting Started

### Prerequisites

- MySQL Server
- Command Line Interface
- Cryptographic Libraries (e.g., OpenSSL)

### Installation and Setup

1. **Clone the repository**:
    ```shell
    git clone https://github.com/yourusername/CS469_small_group_project.git
    cd CS469_small_group_project
    ```

2. **Set up the MySQL database**:
    Create the necessary database and tables.
    ```shell
    mysql -u root -p < database.sql
    ```

3. **Configure the application server**:
    Set up the server to handle RPC messages and interact with the MySQL database.
    build and compile the server using the include make file:
    ```shell
    make
    ```

4. **Run the client**:
    Use the command line client to interact with the application server.
    ```shell
    ./client
    ```
    Follow the prompts to authenticate and interact with the server.

## Usage

1. **Start the MySQL server**:
    ```shell
    sudo service mysql start
    ```

2. **Run the application server**:
    ```sh
    gcc -o server server.c -lssl -lcrypto
    ```

3. **Run the client**:
    ```sh
    python client.py
    ```

## Contributing

1. Fork the repository.
2. Create your feature branch 
    ```shell
    git checkout -b feature/AmazingFeature
    ```

3. Commit your changes 
    ```shell
    git commit -m 'Add some AmazingFeature'
    ```

4. Push to the branch 
    ```shell
    git push origin feature/AmazingFeature
    ```

5. Open a pull request.


## License

Distributed under the MIT License. See `LICENSE` for more information.

## Developers

- **Andrew Bazen** - [andrew.bazen@gmail.com](mailto:andrew.bazen@gmail.com)
- **Ben Merritt** - [bmerritt001@regis.edu](mailto:bmerritt001@regis.edu)
- **Austen Jarosz** - [ajarosz@regis.edu](mailto:ajarosz@regis.edu)
