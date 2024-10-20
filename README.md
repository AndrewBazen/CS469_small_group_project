# CS469_small_group_project

## Project Overview

This project involves building a three-tiered client-server system using a MySQL database server and a command line client. The system will include the following components:

1. **Client**: A command line client that interacts with the application server.
2. **Application Server**: Authenticates the client and transforms RPC messages into SQL commands for the MySQL server. It also returns the results of database queries back to the client. The application server process will be replicated for fault tolerance.
3. **Database Server**: A MySQL database server that stores the data.
4. **Docker Containers**: The application server, and database server will run in separate Docker containers.

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
    complete this step if you have not already cloned the repository or downloaded the project files.
    ```shell
    git clone https://github.com/yourusername/CS469_small_group_project.git
    cd CS469_small_group_project
    ```

2. **Install the dependencies**:
    install the necessary dependencies using the following command:
    ```shell
    sudo apt-get install openssl mysql make gcc docker docker-compose
    ```
    if you are using a different package manager, you can replace `apt-get` with the appropriate package manager. if you already have the dependencies installed, you can skip this step.

3. **Update all your libraries**: 
    ensure that all your libraries are up to date by running the following commands:
    ```shell
    sudo apt-get update
    sudo apt-get upgrade
    ```

## Note: you will need to ensure that you either have docker-desktop running, ## or have the docker daemon running on your system.

4. **compile the files in the project**:
    In the root directory of the project, run the following command:
    ```shell
    make
    ```

4. **Build the Docker Containers**: 
    In the root directory of the project, run the following command:
    ```shell
    docker-compose up --build
    ```

## Note: you may need to adjust the Makefile to include the correct path to the 
## OpenSSL & MYSQL libraries on your system, however, only do this if you 
## encounter an error.

once the docker containers are built, access the shell of the mysql container by running the following command:
```shell
docker exec -it mysql bash
```
or you can access the exec shell through the docker desktop interface.

5. **Create the Database**:
    In the MySQL container, run the following command:
    ```shell
    ./Docker/seed.sh
    ```
    This will create the database and tables. if you want to reset the database, you can run the command again.

6. **Run the client**:
    in a different command line window, run the following command:
    ```shell
    ./repl
    ```
    This will start the client. You can now interact with the database using the client commands.

## Usage

1. **Client Commands**:
    
    - **Get Tables**: List all tables in the database.
        usage:
        ```shell
        get tables my_database
        ```
    - **Get Columns**: List all columns in a table.
        usage:
        ```shell
        get columns my_database <table>
        ```
    - **Insert**: Insert a row into a table.
        usage:
        ```shell
        insert my_database <table> <field1,field2,field3> <value1,value2,value3>
        ```
    - **Update**: Update a cell in a table.
        usage:
        ```shell
        update <table> <field-of-change> <new-value> <field-of-change> <old-value>
        ```
    - **Delete**: Delete a row from a table.
        usage:
        ```shell
        delete <table> <field> <value>
        ```
    - **Select**: Select rows from a table.
        usage:
        ```shell
        select my_database <table>
        ```
    - **Help**: Display help information.
        usage:
        ```shell
        help
        ``` 
    - **Exit**: Exit the client.
        usage:
        ```shell
        exit
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
