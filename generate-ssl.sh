# Paths to the certificate and key files
CERT_FILE="/etc/ssl/certs/server.crt"
KEY_FILE="/etc/ssl/private/server.key"

# Check if the SSL certificate and key already exist
if [ ! -f "$CERT_FILE" ] || [ ! -f "$KEY_FILE" ]; then
    echo "Generating SSL certificate and key..."
    
    # Generate a self-signed certificate with OpenSSL
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
        -keyout "$KEY_FILE" \
        -out "$CERT_FILE" \
        -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=localhost"
    
    echo "SSL certificate and key generated."
else
    echo "SSL certificate and key already exist."
fi

# Start the C HTTP server with SSL
./server