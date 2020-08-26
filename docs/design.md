Design
======

Interface to the extension
--------------------------

To talk to the extension, a message needs to be posted to the extension. You need to add a callback function for the reponse.

For example to create a csr:

```
    function create_csr( subjectName, bitLength, callback) {
        window.postMessage({
            direction: "from-page-script",
            message: { 
                request:"create_csr",
                subject_name: subjectName,
                rsa_key_length: bitLength
            }
        }, "*")
        window.addEventListener("message", callback, false);
    }
```

and the response will be handled with the callback function:

```
    function receiveMessage(event) {
        if (event.source == window &&
            event.data &&
            event.data.direction == "from-content-script") {
            var response = event.data.message
            $( "#csr_placeholder").html("<pre>" + atob(response.response) + "</pre>")
        }
    }
```

The response will be in the event.data.message.

Following sections describe thje javascript objects which need to be send and are received.

### Generate CSR

Fill in the message following information

```
{ 
    request:"create_csr",
    subject_name: "cn=John Doe,o=Company,c=US",
    rsa_key_length: 2048
}
```

subject_name: is the distinguished name which will be put in the CSR
rsa_key_length: the bit length of the RSA key

The response:

```
{
    result: "OK",
    response: "<base64 encoded PEM version of the CSR>"
}
```

### Import Certificate

Fill in the message following information

```
{ 
    request:"import_certificate",
    "certificate": "<base64 encoded PEM certificate>"
}
```

subject_name: is the distinguished name which will be put in the CSR
certificate: is a openssl PEM encoded certificate which is then transformed in base64 without carriage returns

The response:

```
{
    result: "OK",
    response: "import certificate successful"
}
```

### Import PKCS12

Fill in the message following information

```
{ 
    request: "import_pfx_key",
    pkcs12: "<base64 encoded pkcs12>",
    password: "system33"
}
```

pkcs12: this is the pkcs12 structure with private key and protected with the password in base64 without carriage returns
password: the password to import the p12 file

The response:

```
{
    result: "OK",
    response: "import pfx successful"
}
```

### Export PKCS12

When the pkcs12 is exported the use need to give his password to export the key, this is his own chosen password. The password here is a password chosen by the server and will be different from the user's password.

Fill in the message following information

```
{ 
    request:"export_pfx_key",
    "issuer": "cn=RootCA,o=Company,c=US",
    "serial_number": "0x0763",
    "password": "system33"
}
```

issuer: the distinguished name of the certification authority who issued the certificate
serial_number: the serial number of the certificate in hex format
password: the password to export the p12 file


The response:
```
{
    result: "OK",
    response: "<base64 encoded pkcs12 protected with the password>"
}
```

### Error

The response:
```
{
    result: "NOK",
    respônse: "<error message>"
}
```
The event viewer will also contain more information what went wrong.

Interface to native message executable
--------------------------------------

### Generate CSR

```
{
    "request":"create_csr",
    "request_id":"XH45E45MLk0",
    "subject_name":"cn=John Doe,o=Company,c=US",
    "rsa_key_length":2048
}
```

request: is to request the desired action of the extension
request_id: is the identifier which will be returned in the response
subject_name: is the distinguished name which will be put in the CSR
rsa_key_length: the bit length of the RSA key

### Import Certificate

```
{
    "request":"import_certificate",
    "request_id":"XH45E45MLk0",
    "certificate": "<base64 encoded PEM certificate>"
}
```

request: is to request the desired action of the extension
request_id: is the identifier which will be returned in the response
certificate: is a openssl PEM encoded certificate which is then transformed in base64 without carriage returns

### Import PKCS12

```
{
    "request":"import_pfx_key",
    "request_id":"XH45E45MLk0",
    "pkcs12": "<base64 encoded pkcs12>",
    "password": "system33"
}
```

request: is to request the desired action of the extension
request_id: is the identifier which will be returned in the response
pkcs12: this is the pkcs12 structure with private key and protected with the password in base64 without carriage returns
password: the password to import the p12 file

### Export PKCS12

```
{
    "request":"export_pfx_key",
    "request_id":"XH45E45MLk0",
    "issuer": "cn=RootCA,o=Company,c=US",
    "serial_number": "0x0763",
    "password": "system33"
}
```

request: is to request the desired action of the extension
request_id: is the identifier which will be returned in the response
issuer: the distinguished name of the certification authority who issued the certificate
serial_number: the serial number of the certificate
password: the password to export the p12 file