import json
import struct
from ca import certify_p10, generate_p12
from binascii import b2a_base64, a2b_base64
from subprocess import Popen, PIPE, TimeoutExpired

def certify_csr(csr):
    result = certify_p10(a2b_base64(csr))
    return b2a_base64(result.encode('ascii')).decode('ascii')

def gen_pkcs12(common_name, organization, country, password):
    return b2a_base64(generate_p12(common_name, organization, country, password)).decode('ascii')

def encode_message_raw(message_content):
    encoded_length = struct.pack('=I', len(message_content))
    return (encoded_length, message_content.encode('utf-8'))

def encode_message_json(message_content):
    encoded_content = json.dumps(message_content).encode('utf-8')
    encoded_length = struct.pack('=I', len(encoded_content))
    return (encoded_length, encoded_content)

def decode_message(data):
    print (len(data))
    decode_length = struct.unpack('=I',data[:4])
    print (decode_length)
    return json.loads(data[4:].decode('utf-8'))

def call_native_messaging(exec, msg_tuple):
    proc = Popen(exec, stdin=PIPE, stdout=PIPE, stderr=PIPE, shell=True)
    try:
        proc.stdin.write(msg_tuple[0])
        proc.stdin.write(msg_tuple[1])
        outs, errs = proc.communicate(timeout=60)
        print (errs)
        return decode_message(outs)
    except TimeoutExpired:
        proc.kill()
        outs, errs = proc.communicate()
        print (outs)
        print (errs)
        print("Timeout of application")
        raise 

def ok_test(data):
    response = call_native_messaging(r"C:\Program Files\Cryptable\Key Management Web Extension\ksmgmnt.exe", encode_message_json(data))
    if (response["result"] != "OK"):
        print(response["response"])
        raise
    if (response["request_id"] != data['request_id']):
        print("request_id is not equal")
        raise
    return response['response']

def nok_test(data, reason):
    response = call_native_messaging(r"C:\Program Files\Cryptable\Key Management Web Extension\ksmgmnt.exe", encode_message_json(data))
    if (response['result'] != "NOK"):
        print("Successful call not expected:" + response['response'])
        raise
    if (response["request_id"] != data['request_id']):
        print("request_id is not equal")
        raise 
    if (response['response'] != reason):
        print("Response was unexpected, so not normal (investigation necessary)")
        raise

def nok_test_raw(data, reason):
    response = call_native_messaging(r"C:\Program Files\Cryptable\Key Management Web Extension\ksmgmnt.exe", encode_message_raw(data))
    if (response['result'] != "NOK"):
        print("Successful call not expected:" + response['response'])
        raise
    if (response['response'] != reason):
        print("Response was unexpected, so not normal (investigation necessary)")
        raise

def illegal_data_test(data_tuple, reason):
    response = call_native_messaging(r"C:\Program Files\Cryptable\Key Management Web Extension\ksmgmnt.exe", data_tuple)
    if (response['result'] != "NOK"):
        print("Successful call not expected:" + response['response'])
        raise
    if (response['response'] != reason):
        print("Response was unexpected, so not normal (investigation necessary)")
        raise

def scenario1_create_csr_certificate():
    print('success scenario 1 request certificate')
    data = {
        'request': 'create_csr',
        'request_id': '0x030405',
        'subject_name': 'cn=John Doe,o=Company,c=US',
        'rsa_key_length': 2048
    }
    response = ok_test(data)
    print(response)
    certificate = certify_csr(response);
    print(certificate)
    cert_data = {
        'request': 'import_certificate',
        'request_id': '0x030406',
        'certificate': certificate,
    }
    response = ok_test(cert_data)
    if (response != "import certificate successful"):
        print("Incorrect success message")
        raise 

def scenario2_import_pfx():
    print('Success scenario 2 import pfx file')
    pkcs12 = gen_pkcs12('John Doe Encryption', 'Company', 'US', 'system88')
    data = {
        'request': 'import_pfx_key',
        'request_id': '0x030407',
        'pkcs12':pkcs12,
        'password':'system88',
    }
    response = ok_test(data)
    print(response)
    if (response != "import pfx successful"):
        print("Incorrect success message")
        raise 

def scenario3_garbage():
    print('scenario3_garbage test')
    data = "dfhsdkjlfhsjdkqlhfjklsqhfjksldqhfkljqsdhfjkldsqfhjqklsdfh"
    response = nok_test_raw(data, "Bad Request")

def scenario4_garbage():
    print('scenario4_garbage test')
    data = ""
    response = nok_test_raw(data, "Bad Request")

def scenario5_garbage():
    print('scenario5_garbage test')
    encoded_length = struct.pack('=I', 0)
    data_tuple = (encoded_length, b'njnjkhjhhklkm')
    response = illegal_data_test(data_tuple, "Bad Request")

def scenario6_garbage():
    print('scenario6_garbage test')
    data = {
        'request': 'create_csr',
        'request_id': '0x030405',
        'subject_name': 'cn=John Doe,o=Company,c=US',
        'rsa_key_length': 2048,
    }
    good_data = encode_message_json(data)
    encoded_length = struct.pack('=I', 0)
    data_tuple = (encoded_length, good_data[1])
    response = illegal_data_test(data_tuple, "Bad Request")

def scenario7_garbage():
    print('scenario7_garbage test')
    data = {
        'request': 'create_csr',
        'request_id': '0x030408',
        'subject_name': 'cn=John Doe,o=Company,c=US',
        'rsa_key_length': 56887,
    }
    response = nok_test(data, "Bad Request")

def scenario8_garbage():
    print('scenario8_garbage test')
    data = {
        'request': 'create_csr',
        'request_id': '0x030409',
        'subject_name': 'dsdqsdqsdsDoe,o=Company,c=US',
        'rsa_key_length': 2048,
    }
    response = nok_test(data, "Bad Request")

def main():
    # scenario1_create_csr_certificate()
    # scenario2_import_pfx()
    scenario3_garbage()
    scenario4_garbage()
    scenario5_garbage()
    scenario6_garbage()
    scenario7_garbage()
    scenario8_garbage()
    # scenario9_garbage()

if __name__ == '__main__':
    main()