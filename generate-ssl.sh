# Description: Generate a self-signed SSL certificate and key for the C HTTP server
# Paths to the certificate and key files
CERT_FILE="cert.pem"
KEY_FILE="key.pem"

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
