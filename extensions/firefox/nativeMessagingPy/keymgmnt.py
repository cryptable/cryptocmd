#!/usr/bin/python3

import sys
import shlex
import json
import base64
import struct
import hashlib
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.serialization import load_pem_private_key
from cryptography.hazmat.primitives.serialization.pkcs12 import serialize_key_and_certificates, load_key_and_certificates
from cryptography.hazmat.primitives.asymmetric import rsa


def log(data):
    f = open('/tmp/keymgmnt.log', 'a+')
    f.write(data)
    f.close()


def create_filename(data):
    m = hashlib.sha224()
    m.update(data)
    return m.hexdigest()


def create_privkey_filename(key):   
    return create_filename(key.public_key().public_bytes(encoding=serialization.Encoding.DER, format=serialization.PublicFormat.SubjectPublicKeyInfo))


def write_private_key(key):
    fname = "/tmp/" + create_privkey_filename(key) + ".key"
    fpriv = open(fname, "wb+")
    fpriv.write(key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.BestAvailableEncryption(b'mypassword')))
    fpriv.close()

def load_private_key(key):
    fname = "/tmp/" + create_privkey_filename(key) + ".key"
    fpriv = open(fname, "rb")
    priv = fpriv.read()
    fpriv.close()
    return load_pem_private_key(priv, b'mypassword')


def generate_csr(subject, bitLength):
    subject_mapper = {
        'CN':NameOID.COMMON_NAME,
        'L':NameOID.LOCALITY_NAME,
        'ST':NameOID.STATE_OR_PROVINCE_NAME,
        'O':NameOID.ORGANIZATION_NAME,
        'OU':NameOID.ORGANIZATIONAL_UNIT_NAME,
        'C':NameOID.COUNTRY_NAME,
        'STREET':NameOID.STREET_ADDRESS,
        'DC':NameOID.DOMAIN_COMPONENT,
        'UID':NameOID.USER_ID,
    }
    lexer = shlex.shlex(subject, posix=True)
    lexer.whitespace_split = True
    lexer.whitespace = ','
    subject_list = []
    for pair in lexer:
        keyValue = pair.split('=', 1)
        subject_list.append(x509.NameAttribute(subject_mapper[keyValue[0].upper().strip()], keyValue[1].strip()))

    private_key = rsa.generate_private_key(public_exponent=65537, key_size=(+bitLength), backend=default_backend())

    csr_builder = x509.CertificateSigningRequestBuilder()
    csr_builder = csr_builder.subject_name(x509.Name(subject_list))
    csr = csr_builder.sign(private_key, hashes.SHA256(), default_backend())
    write_private_key(private_key)

    return csr.public_bytes(serialization.Encoding.PEM)


def import_certificate(pemCertificate):
    cert = x509.load_pem_x509_certificate(pemCertificate, default_backend())
    cas = []
    priv_filename = "/tmp/" + create_privkey_filename(cert) + ".key"
    priv = load_private_key(cert)
    cn = cert.subject.get_attributes_for_oid(NameOID.COMMON_NAME)
    p12_data = serialize_key_and_certificates(cn[0].value.encode(), priv, cert, cas, serialization.BestAvailableEncryption(b'mypassword'))
    issuer = ",".join(attr.rfc4514_string() for attr in cert.issuer)
    fname = "/tmp/" + create_filename(issuer.encode()) + '_' + str(cert.serial_number) + '.p12'
    f = open(fname,"wb+")
    f.write(p12_data)
    f.close()
    return


def import_pfx_key(pkcs12, password):
    (priv, cert, cas) = load_key_and_certificates(pkcs12, password.encode())
    cn = cert.subject.get_attributes_for_oid(NameOID.COMMON_NAME)
    p12_data = serialize_key_and_certificates(cn[0].value.encode(), priv, cert, cas, serialization.BestAvailableEncryption(b'mypassword'))
    issuer = ",".join(attr.rfc4514_string() for attr in cert.issuer)
    fname = "/tmp/" + create_filename(issuer.encode()) + '_' + str(cert.serial_number) + '.p12'
    f = open(fname,"wb+")
    f.write(p12_data)
    f.close()
    return


def export_pfx_key(issuer, serialnr, password):
    fname = "/tmp/" + create_filename(issuer.encode("utf-8")) + '_' + str(int(serialnr, 0)) + '.p12'
    f = open(fname,"rb")
    p12_data = f.read()
    f.close()
    (priv, cert, cas) = load_key_and_certificates(p12_data, b'mypassword')
    cn = cert.subject.get_attributes_for_oid(NameOID.COMMON_NAME)
    return serialize_key_and_certificates(cn[0].value.encode(), priv, cert, cas, serialization.BestAvailableEncryption(b'mypassword'))


def get_message():
    raw_length = sys.stdin.buffer.read(4)
    if not raw_length:
        sys.exit(0)
    message_length = struct.unpack('=I', raw_length)[0]
    message = sys.stdin.read(message_length)
    return json.loads(message)


# Encode a message for transmission, given its content.
def encode_message(message_content):
    encoded_content = json.dumps(message_content)
    encoded_length = struct.pack('=I', len(encoded_content))
    return {'length': encoded_length, 'content': encoded_content}


# Send an encoded message to stdout.
def send_message(encoded_message):
    sys.stdout.buffer.write(encoded_message['length'])
    sys.stdout.write(encoded_message['content'])
    sys.stdout.flush()


if __name__ == "__main__":
    request = get_message()
    # log(request)

    if (request['request'] == 'create_csr'):
        response = {'result':'OK', 'request_id':request['request_id'], 'response':base64.b64encode(generate_csr(request['subject_name'], request['rsa_key_length'])).decode("utf-8") }
        send_message(encode_message(response))
    elif (request['request'] == 'import_certificate'):
        import_certificate(base64.b64decode(request['certificate']))
        response = {'result':'OK', 'request_id':request['request_id'], 'response': 'import certificate successful'}
        send_message(encode_message(response))
    elif (request['request'] == 'import_pfx_key'):
        import_pfx_key(base64.b64decode(request['pkcs12']), request['password'])
        response = {'result':'OK', 'request_id':request['request_id'], 'response': 'import pfx successful'}
        send_message(encode_message(response))
    elif (request['request'] == 'export_pfx_key'):
        export_pfx_key(request['issuer'], request['serial_number'], request['password'])
        response = {'result':'OK', 'request_id':request['request_id'], 'response': base64.b64encode(export_pfx_key(request['issuer'], request['serial_number'], request['password'])).decode("utf-8") }
        send_message(encode_message(response))
    else:
        response = {'result':'NOK', 'request_id':request['request_id'], 'response': 'Unknown request error'}
        send_message(encode_message(response))
