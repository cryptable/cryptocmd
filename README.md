Crypto Command Extension
========================

This is a windows only Firefox extension, which allows following actions to the native crypto engine of you OS:
- Generate RSA keypair
- Import certificates for that keypair
- Export the keypair with certificate as a PKCS12
- Import a PKCS12 into the keystore
The project exist of 2 parts:
- A firefox web extension
- A native message handler to communicate from the firefox extension to the native message handler

TODO:
- TPM support: Using a special setting command during the generate RSA keypair the keypair will be generated in the TPM and send back the CSR, signed with the attestation key and the attestation public key certificate.
- Signature support: The extension will be able to put basic RSA signatures (PKCS1.5 and PSS). This can be used in collaboration with JOSE and JWT in the browser
