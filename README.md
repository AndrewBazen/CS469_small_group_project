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
- Docker
- Docker Compose
- Make
- GCC

### Installation and Setup

1. **Clone the repository**:
    ```shell
    git clone https://github.com/yourusername/CS469_small_group_project.git
    cd CS469_small_group_project
    ```

2. **Build the Docker Containers**: 
    ```shell
    docker-compose up --build
    ```

## Note: you may need to adjust the Makefile to include the correct path to the 
## OpenSSL & MYSQL libraries on your system.

4. **Run the client**:
    in a different command line window, run the following command:
    ```shell
    ./repl localhost:8443
    ```
## Usage

1. **Client Commands**:
    
    - **Login**: Authenticate the client.
    - **Logout**: De-authenticate the client.
    - **Get Tables**: List all tables in the database.
    - **Show Table "X"**: Display the contents of a table.
    - **Insert Into "X"**: Insert a row into a table.
    - **Update "X"**: Update a row in a table.
    - **Delete From "X"**: Delete a row from a table.
    - **Exit**: Exit the client.

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
