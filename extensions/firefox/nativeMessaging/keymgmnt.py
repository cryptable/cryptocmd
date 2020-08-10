#!//usr/bin/python

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
from cryptography.hazmat.primitives.asymmetric import rsa

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
        subject_list.append(x509.NameAttribute(subject_mapper[keyValue[0].upper().strip()], unicode(keyValue[1].strip())))

    private_key = rsa.generate_private_key(public_exponent=65537, key_size=(+bitLength), backend=default_backend())

    csr_builder = x509.CertificateSigningRequestBuilder()
    csr_builder = csr_builder.subject_name(x509.Name(subject_list))
    csr = csr_builder.sign(private_key, hashes.SHA256(), default_backend())

    return csr.public_bytes(serialization.Encoding.PEM)

def import_certificate(pemCertificate):
    cert = x509.load_pem_x509_certificate(pemCertificate, default_backend())
    fname = hashlib.sha224(cert.issuer.rfc4514_string()) + '_' + cert.serial + '.pem'
    f = open(fname,"w+")
    f.write(pemCertificate)
    f.close()
    return

def import_pfx_key(pkcs12, password):
    cert = x509.load_pem_x509_certificate(pemCertificate, default_backend())
    fname = hashlib.sha224(cert.issuer.rfc4514_string()) + '_' + cert.serial + '.pem'
    f = open(fname,"w+")
    f.write(pemCertificate)
    f.close()
    return

def export_pfx_key(issuer, serialnr, password):
    return 'private key'

def get_message():
    raw_length = sys.stdin.read(4)
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
    sys.stdout.write(encoded_message['length'])
    sys.stdout.write(encoded_message['content'])
    sys.stdout.flush()

import_pfx_key

if __name__ == "__main__":
    request = get_message()
    if (request['request'] == 'create_csr'):
        response = {'result':'OK', 'key_id':request['key_id'], 'response':base64.b64encode(generate_csr(request['subjectName'], request['rsaKeyLength'])) }
        send_message(encode_message(response))
    elif (request['request'] == 'import_certificate'):
        import_certificate(base64.b64decode(request['certificate']))
        response = {'result':'OK', 'key_id':request['key_id'], 'response': 'Certificate Imported'}
        send_message(encode_message(response))
    elif (request['request'] == 'import_pfx_key'):
        import_pfx_key(base64.b64decode(request['pkcs12']), request['password'])
        response = {'result':'OK', 'key_id':request['key_id'], 'response': 'PFX Imported'}
        send_message(encode_message(response))
    elif (request['request'] == 'export_pfx_key'):
        export_pfx_key(request['issuer'], request['serial_number'], request['password'])
        response = {'result':'OK', 'key_id':request['key_id'], 'response': base64.b64encode(export_pfx_key(request['issuer'], request['serial_number'], request['password']))}
        send_message(encode_message(response))
    else:
        response = {'result':'NOK', 'key_id':request['key_id'], 'response': 'Unknown request error'}
        send_message(encode_message(response))
