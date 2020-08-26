from datetime import datetime, timedelta
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.serialization import load_pem_private_key, pkcs12
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID
from cryptography import x509

ca_cert = '''-----BEGIN CERTIFICATE-----
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
GHdRbVL198G65g3tx8YZU7nsN6QzhLyHPQ==
-----END CERTIFICATE-----'''

ca_private = '''-----BEGIN ENCRYPTED PRIVATE KEY-----
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
VBk=
-----END ENCRYPTED PRIVATE KEY-----'''

ca_password = 'system'


def certify_p10(request):
    # Load CA certificate and private key
    key = load_pem_private_key(ca_private.encode('ascii'), ca_password.encode('ascii'), default_backend())
    cacert = x509.load_pem_x509_certificate(ca_cert.encode('ascii'), default_backend())

    # Load Request
    x509req = x509.load_pem_x509_csr(request, default_backend())

    # Create certificate for it
    sernum = x509.random_serial_number()
    cert = x509.CertificateBuilder()\
        .issuer_name(cacert.subject)\
        .subject_name(x509req.subject)\
        .public_key(x509req.public_key())\
        .serial_number(sernum)\
        .not_valid_before(datetime.now())\
        .not_valid_after(datetime.now() + timedelta(days=366))\
        .add_extension(x509.KeyUsage(digital_signature=True
                                     , content_commitment=True
                                     , key_encipherment=False
                                     , data_encipherment=False
                                     , key_agreement=False
                                     , key_cert_sign=False
                                     , crl_sign=False
                                     , encipher_only=False
                                     , decipher_only = False), critical=True)\
        .sign(key, hashes.SHA256(), default_backend())

    pemCert = cert.public_bytes(serialization.Encoding.PEM).decode('ascii')
    return pemCert


def generate_p12(common_name, organization_name, country, password):
    # Load CA certificate and private key
    cakey = load_pem_private_key(ca_private.encode('ascii'), ca_password.encode('ascii'), default_backend())
    cacert = x509.load_pem_x509_certificate(ca_cert.encode('ascii'), default_backend())

    subject = x509.Name([
        x509.NameAttribute(NameOID.COMMON_NAME, common_name),
        x509.NameAttribute(NameOID.ORGANIZATION_NAME, organization_name),
        x509.NameAttribute(NameOID.COUNTRY_NAME, country),])

    sernum = x509.random_serial_number()
    key = rsa.generate_private_key(public_exponent=65537,
                                   key_size=2048,
                                   backend=default_backend())
    cert = x509.CertificateBuilder().subject_name(subject) \
        .issuer_name(cacert.subject) \
        .serial_number(sernum) \
        .public_key(key.public_key()) \
        .not_valid_before(datetime.now())\
        .not_valid_after(datetime.now() + timedelta(days=366))\
        .add_extension(x509.KeyUsage(digital_signature=False
                                     , content_commitment=False
                                     , key_encipherment=True
                                     , data_encipherment=True
                                     , key_agreement=False
                                     , key_cert_sign=False
                                     , crl_sign=False
                                     , encipher_only=False
                                     , decipher_only = False), critical=True)\
        .sign(cakey, hashes.SHA256(), default_backend())

    cas = [cacert]

    p12 = pkcs12.serialize_key_and_certificates(common_name.encode('utf-8'),
                                                key,
                                                cert,
                                                cas,
                                                serialization.BestAvailableEncryption(password.encode('ascii')))

    return p12