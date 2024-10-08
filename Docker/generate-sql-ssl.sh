# Paths to certificate files
CERT_DIR="/etc/mysql/certs"

# Check if the directory exists, create if not
if [ ! -d "$CERT_DIR" ]; then
    mkdir -p "$CERT_DIR"
fi

# Generate the SSL certificates if they do not already exist
if [ ! -f "$CERT_DIR/server.crt" ] || [ ! -f "$CERT_DIR/server.key" ] || [ ! -f "$CERT_DIR/rootCA.pem" ]; then
    echo "Generating SSL certificates..."

    # Generate the CA key and certificate
    openssl genrsa -out rootCA.key 2048
    openssl req -x509 -new -nodes -key rootCA.key -sha256 -days 1024 \
     -out rootCA.pem -subj "/C=US/ST=State/L=City/O=MyOrg/OU=OrgUnit/CN=MyRootCA"

    # Generate the server key and CSR (Certificate Signing Request)
    openssl genrsa -out server.key 2048
    openssl req -new -key server.key -out server.csr \
    -subj "/C=US/ST=State/L=City/O=MyOrg/OU=OrgUnit/CN=server.example.com"

    # Sign the server certificate with the CA key
    openssl x509 -req -in server.csr -CA rootCA.pem \
    -CAkey rootCA.key -CAcreateserial -out server.crt -days 500 -sha256

    echo "SSL certificate and key generated."

    # Move the files to the certs directory
    mv rootCA.pem "$CERT_DIR"
    mv server.crt "$CERT_DIR"
    mv server.key "$CERT_DIR"

    # Ensure the correct permissions
    chmod 600 "$CERT_DIR/server.key"
    chmod 644 "$CERT_DIR/rootCA.pem"
    chmod 644 "$CERT_DIR/server.crt"
else
    echo "SSL certificate and key already exist."
fi

# Path to your MySQL configuration file (my.cnf or mysql.conf)
MY_CNF_PATH="/etc/my.cnf"

# Ensure the configuration file exists
if [ ! -f "$MY_CNF_PATH" ]; then
    touch "$MY_CNF_PATH"
fi

# Append SSL settings to the MySQL configuration file if not already present
if ! grep -q "ssl-ca=$CERT_DIR/rootCA.pem" "$MY_CNF_PATH"; then
    echo "[mysqld]" >> $MY_CNF_PATH
    echo "ssl-ca=$CERT_DIR/rootCA.pem" >> $MY_CNF_PATH
    echo "ssl-cert=$CERT_DIR/server.crt" >> $MY_CNF_PATH
    echo "ssl-key=$CERT_DIR/server.key" >> $MY_CNF_PATH
fi

# Start MySQL
exec mysqld
