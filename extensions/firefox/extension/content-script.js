
function handleError(error) {
	console.log(error);
}

function signatureCSRResponse(key_id) {
	return function(message) {
		window.postMessage({
            direction: "from-content-script",
            message: { 
                key_id: key_id, 
                response: message.response.response 
            }
        }, "*")
	}
}

function signatureCSRRequest(keyId, dName) {
	var handleResp = signatureCSRResponse(keyId)
	var sendMessage = browser.runtime.sendMessage({
		request:'signature_csr',
		key_id: keyId,
		subjectName: dName
	});
	sendMessage.then(handleResp, handleError);
}

function importSignatureCertificateResponse() {

}

function importSignatureCertificate(keyId, pemCertificate) {
	var sendMessage = browser.runtime.sendMessage({
		request:'import_signature_certificate',
		key_id: keyId,
		certificate: pemCertificate
	});
	sendMessage.then(importSignatureCertificateResponse, handleError);
}

function importEncryptionKeyResponse() {

}

function importEncryptionKey(keyId, p12) {
	var sendMessage = browser.runtime.sendMessage({
		request:'import_encryption_key',
		key_id: keyId,
		pkcs12: p12
	});
	sendMessage.then(importEncryptionKeyResponse, handleError);
}

window.addEventListener("message", function(event) {
  if (event.source == window &&
      event.data &&
      event.data.direction == "from-page-script") {
  	request = event.data.message
  	switch (request.request) {
  		case 'signature_csr': signatureCSRRequest(request.key_id, request.subjectName)
  		break;
  	}
  }
});