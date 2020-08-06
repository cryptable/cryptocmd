#!//usr/bin/python

import sys
import shlex
import json
import base64
import struct
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa

def generate_signing_csr(keyid, subject):
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

    private_key = rsa.generate_private_key(public_exponent=65537, key_size=2048, backend=default_backend())

    csr_builder = x509.CertificateSigningRequestBuilder()
    csr_builder = csr_builder.subject_name(x509.Name(subject_list))
    csr = csr_builder.sign(private_key, hashes.SHA256(), default_backend())

    return csr.public_bytes(serialization.Encoding.PEM)

def import_signature_certificate(key_id):
    return

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

if __name__ == "__main__":
#    input_data = sys.stdin.read()
    request = get_message()
    if (request['request'] == 'signature_csr'):
        response = {'result':'OK', 'response':base64.b64encode(generate_signing_csr(request['key_id'], request['subjectName'])) }
        send_message(encode_message(response))
    elif (request['request'] == 'import_signature_certificate'):
        import_signature_certificate(request.key_id)
        response = {'result':'OK', 'response': 'Successful Imported'}
        send_message(encode_message(response))
    else:
        response = {'result':'NOK', 'response': 'Unknown Error'}
        send_message(encode_message(response))
