/*
 * MIT License
 * Author: David Tillemans
 */
#include <windows.h>
#include <wincrypt.h>
#include "GeneralName.hpp"

int create_csr(char *distnm, char *algo, int bitlg, char *csr, size_t *lg) {
	std::string dname(distnm);
	GeneralName subjectName(dname);

    CERT_REQUEST_INFO  CertReqInfo;
    CertReqInfo.Subject = subjectName.getEncodedBlob();
    CertReqInfo.cAttribute = 0;
    CertReqInfo.rgAttribute = NULL;
    CertReqInfo.dwVersion = CERT_REQUEST_V1;

    HCRYPTPROV  hCryptProv;

#if 0
    if(CryptSignAndEncodeCertificate(
              hCryptProv,                      // Crypto provider
              AT_KEYEXCHANGE,                  // Key spec
              MY_ENCODING_TYPE,                // Encoding type
              X509_CERT_REQUEST_TO_BE_SIGNED,  // Structure type
              &CertReqInfo,                    // Structure information
              &SigAlg,                         // Signature algorithm
              NULL,                            // Not used
              NULL,                            // pbSignedEncodedCertReq
              &cbEncodedCertReqSize))          // Size of certificate 
                                               // required
    {
        printf("The size of the encoded certificate is set. \n");
    }
#endif
	return 0;
}


// Sample code generate CSR
#if 0

void ByteToStr(
     DWORD cb, 
     void* pv, 
     LPSTR sz)
{
//-------------------------------------------------------------------
//  Declare and initialize local variables.

BYTE* pb = (BYTE*) pv; // local pointer to a BYTE in the BYTE array
DWORD i;               // local loop counter
int b;                 // local variable

//-------------------------------------------------------------------
//  Begin processing loop.

for (i = 0; i<cb; i++)
{
   b = (*pb & 0xF0) >> 4;
   *sz++ = (b <= 9) ? b + '0' : (b - 10) + 'A';
   b = *pb & 0x0F;
   *sz++ = (b <= 9) ? b + '0' : (b - 10) + 'A';
   pb++;
}
*sz++ = 0;
}


        //       Convert the subject name to an ASN.1 encoded
        //       string and print the octets in that string.

        //       First : Get the number of bytes that must 
        //       be allocated for the string.

        cbSize = CertNameToStr(
            pCertContext->dwCertEncodingType,
            &(pCertContext->pCertInfo->Subject),
            MY_STRING_TYPE,
            NULL,
            0);

        //-----------------------------------------------------------
        //  The function CertNameToStr returns the number
        //  of bytes needed for a string to hold the
        //  converted name, including the null terminator. 
        //  If it returns one, the name is an empty string.

        if (1 == cbSize)
        {
            MyHandleError(TEXT("Subject name is an empty string."));
        }


//-------------------------------------------------------------------
// Copyright (C) Microsoft.  All rights reserved.
// This example demonstrates how to create and encode a 
// certificate request. 
#pragma comment(lib, "crypt32.lib")

#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
void MyHandleError(char *s);

//-------------------------------------------------------------------
//   This program use this additional #define statement. 

#define CERT_SUBJECT_NAME "This certificate user"

//-------------------------------------------------------------------
// This program uses the function ByteToStr to convert an array
// of BYTEs to a char string. 

void main(void)
{
//-------------------------------------------------------------------
// Declare and initialize variables 

// Declare and initialize a CERT_RDN_ATTR array.
// In this code, only one array element is used.

CERT_RDN_ATTR rgNameAttr[] = {
        "2.5.4.3",                             // pszObjId 
        CERT_RDN_PRINTABLE_STRING,             // dwValueType
        strlen(CERT_SUBJECT_NAME),             // value.cbData
        (BYTE*)CERT_SUBJECT_NAME};             // value.pbData

//-------------------------------------------------------------------
// Declare and initialize a CERT_RDN array.
// In this code, only one array element is used.

CERT_RDN rgRDN[] = {
         1,                 // rgRDN[0].cRDNAttr
         &rgNameAttr[0]};   // rgRDN[0].rgRDNAttr

//-------------------------------------------------------------------
// Declare and initialize a CERT_NAME_INFO structure.

CERT_NAME_INFO Name = {
           1,                  // Name.cRDN
           rgRDN};             // Name.rgRDN

//-------------------------------------------------------------------
// Declare and initialize all other variables and structures.

CERT_REQUEST_INFO  CertReqInfo;
CERT_NAME_BLOB  SubjNameBlob;
DWORD  cbNameEncoded;
BYTE*  pbNameEncoded;
HCRYPTPROV  hCryptProv;
DWORD  cbPublicKeyInfo;
CERT_PUBLIC_KEY_INFO*  pbPublicKeyInfo;
DWORD  cbEncodedCertReqSize;
CRYPT_OBJID_BLOB  Parameters;
CRYPT_ALGORITHM_IDENTIFIER  SigAlg;
BYTE*  pbSignedEncodedCertReq;
char*  pSignedEncodedCertReqBlob;

//-------------------------------------------------------------------
//    Begin processing.

if(CryptEncodeObject(
    MY_ENCODING_TYPE,     // Encoding type
    X509_NAME,            // Structure type
    &Name,                // Address of CERT_NAME_INFO structure
    NULL,                 // pbEncoded
    &cbNameEncoded))      // pbEncoded size
{
    printf("The first call to CryptEncodeObject succeeded. \n");
}
else
{
    MyHandleError("The first call to CryptEncodeObject failed. \n" 
        "A public/private key pair may not exit in the container. \n");
}
//-------------------------------------------------------------------
//     Allocate memory for the encoded name.

if(!(pbNameEncoded = (BYTE*)malloc(cbNameEncoded)))
    MyHandleError("The pbNamencoded malloc operation failed. \n");

//-------------------------------------------------------------------
//  Call CryptEncodeObject to do the actual encoding of the name.

if(CryptEncodeObject(
        MY_ENCODING_TYPE,    // Encoding type
        X509_NAME,           // Structure type
        &Name,               // Address of CERT_NAME_INFO structure
        pbNameEncoded,       // pbEncoded
        &cbNameEncoded))     // pbEncoded size
{
    printf("The object is encoded. \n");
}
else
{
    free(pbNameEncoded);
    MyHandleError("The second call to CryptEncodeObject failed. \n");
}
//-------------------------------------------------------------------
// Set the subject member of CertReqInfo to point to 
// a CERT_NAME_INFO structure that 
// has been initialized with the data from cbNameEncoded
// and pbNameEncoded.

SubjNameBlob.cbData = cbNameEncoded;
SubjNameBlob.pbData = pbNameEncoded;
CertReqInfo.Subject = SubjNameBlob;

//-------------------------------------------------------------------
// Generate custom information. This step is not
// implemented in this code.

CertReqInfo.cAttribute = 0;
CertReqInfo.rgAttribute = NULL;
CertReqInfo.dwVersion = CERT_REQUEST_V1;

//-------------------------------------------------------------------
//    Call CryptExportPublicKeyInfo to return an initialized
//    CERT_PUBLIC_KEY_INFO structure.
//    First, get a cryptographic provider.

if(CryptAcquireContext(
    &hCryptProv,        // Address for handle to be returned.
    NULL,               // Use the current user's logon name.
    NULL,               // Use the default provider.
    PROV_RSA_FULL,      // Need to both encrypt and sign.
    NULL))              // No flags needed.
{
     printf("A cryptographic provider has been acquired. \n");
}
else
{
    free(pbNameEncoded);
    MyHandleError("CryptAcquireContext failed. \n");
}
//-------------------------------------------------------------------
// Call CryptExportPublicKeyInfo to get the size of the returned
// information.

if(CryptExportPublicKeyInfo(
          hCryptProv,            // Provider handle
          AT_SIGNATURE,          // Key spec
          MY_ENCODING_TYPE,      // Encoding type
          NULL,                  // pbPublicKeyInfo
          &cbPublicKeyInfo))     // Size of PublicKeyInfo
{
     printf("The keyinfo structure is %d bytes. \n",cbPublicKeyInfo);
}
else
{
    free(pbNameEncoded);
    MyHandleError("The first call to CryptExportPublickKeyInfo failed. \n"
                  "The probable cause is that \n"
                  "there is no key pair in the key container. \n");
}
//-------------------------------------------------------------------
// Allocate the necessary memory.

if(pbPublicKeyInfo = 
   (CERT_PUBLIC_KEY_INFO*)malloc(cbPublicKeyInfo))
{
    printf("Memory is allocated for the public key structure. \n");
}
else
{
    free(pbNameEncoded);
    MyHandleError("Memory allocation failed. \n");
}
//-------------------------------------------------------------------
// Call CryptExportPublicKeyInfo to get pbPublicKeyInfo.

if(CryptExportPublicKeyInfo(
          hCryptProv,            // Provider handle
          AT_SIGNATURE,          // Key spec
          MY_ENCODING_TYPE,      // Encoding type
          pbPublicKeyInfo,       // pbPublicKeyInfo
          &cbPublicKeyInfo))     // Size of PublicKeyInfo
{
     printf("The key has been exported. \n");
}
else
{
    free(pbNameEncoded);
    free(pbPublicKeyInfo);
    MyHandleError("The second call to CryptExportPublicKeyInfo failed. \n");
}
//-------------------------------------------------------------------
// Set the SubjectPublicKeyInfo member of the 
// CERT_REQUEST_INFO structure to point to the CERT_PUBLIC_KEY_INFO 
// structure created.

CertReqInfo.SubjectPublicKeyInfo = *pbPublicKeyInfo;

memset(&Parameters, 0, sizeof(Parameters));
SigAlg.pszObjId = szOID_OIWSEC_sha1RSASign;
SigAlg.Parameters = Parameters;

//-------------------------------------------------------------------
// Call CryptSignAndEncodeCertificate to get the size of the
// returned BLOB. The dwKeySpec argument should match the KeySpec
// (AT_SIGNATURE or AT_KEYEXCHANGE) used to create the private
// key. Here, AT_KEYEXCHANGE is assumed.

if(CryptSignAndEncodeCertificate(
          hCryptProv,                      // Crypto provider
          AT_KEYEXCHANGE,                  // Key spec
          MY_ENCODING_TYPE,                // Encoding type
          X509_CERT_REQUEST_TO_BE_SIGNED,  // Structure type
          &CertReqInfo,                    // Structure information
          &SigAlg,                         // Signature algorithm
          NULL,                            // Not used
          NULL,                            // pbSignedEncodedCertReq
          &cbEncodedCertReqSize))          // Size of certificate 
                                           // required
{
    printf("The size of the encoded certificate is set. \n");
}
else
{
    free(pbNameEncoded);
    free(pbPublicKeyInfo);
    MyHandleError("First call to CryptSignandEncode failed. \n");
}
//-------------------------------------------------------------------
// Allocate memory for the encoded certificate request.

if(pbSignedEncodedCertReq = (BYTE*)malloc(cbEncodedCertReqSize))
{
    printf("Memory has been allocated.\n");
}
else
{
    free(pbNameEncoded);
    free(pbPublicKeyInfo);
    MyHandleError("The malloc operation failed. \n");
}
//-------------------------------------------------------------------
// Call CryptSignAndEncodeCertificate to get the 
// returned BLOB.

if(CryptSignAndEncodeCertificate(
          hCryptProv,                     // Crypto provider
          AT_KEYEXCHANGE,                 // Key spec
          MY_ENCODING_TYPE,               // Encoding type
          X509_CERT_REQUEST_TO_BE_SIGNED, // Struct type
          &CertReqInfo,                   // Struct info        
          &SigAlg,                        // Signature algorithm
          NULL,                           // Not used
          pbSignedEncodedCertReq,         // Pointer
          &cbEncodedCertReqSize))         // Length of the message
{
     printf("The message is encoded and signed. \n");
}
else
{
    free(pbNameEncoded);
    free(pbPublicKeyInfo);
    free(pbSignedEncodedCertReq);
    MyHandleError("The second call to CryptSignAndEncode failed. \n");
}
//-------------------------------------------------------------------
// View the signed and encoded certificate request BLOB.

pSignedEncodedCertReqBlob = 
                        new char[(cbEncodedCertReqSize *2) +1];

//-------------------------------------------------------------------
// Call ByteToStr, one of the general purpose functions, to convert 
// the byte BLOB to ASCII hexadecimal format. 

ByteToStr(cbEncodedCertReqSize,
          pbSignedEncodedCertReq,
          pSignedEncodedCertReqBlob);

//-------------------------------------------------------------------
// Print the string.
printf("The string created is: \n");
printf("%s\n",pSignedEncodedCertReqBlob);

//-------------------------------------------------------------------
// Free memory.

free(pbNameEncoded);
free(pbPublicKeyInfo);
free(pbSignedEncodedCertReq);
CryptReleaseContext(hCryptProv,0);

printf("\nMemory freed. Program ran without error. \n");
} // End of main

//-------------------------------------------------------------------
//  This example uses the function MyHandleError, a simple error
//  handling function, to print an error message to  
//  the standard error (stderr) file and exit the program. 
//  For most applications, replace this function with one 
//  that does more extensive error reporting.

void MyHandleError(char *s)
{
    fprintf(stderr,"An error occurred in running the program. \n");
    fprintf(stderr,"%s\n",s);
    fprintf(stderr, "Error number %x.\n", GetLastError());
    fprintf(stderr, "Program terminating. \n");
    exit(1);
} // End of MyHandleError
#endif
