
function handleError(error) {
	console.log(error);
}

function signatureCSRResponse(message) {

}

function signatureCSRRequest(keyId, dName) {
	var sendMessage = browser.runtime.sendMessage({
		request:'signature_csr',
		key_id: keyId,
		subjectName: dName
	});
	sending.then(signatureCSRResponse, handleError);
}

function importSignatureCertificateResponse() {

}

function importSignatureCertificate(keyId, pemCertificate) {
	var sendMessage = browser.runtime.sendMessage({
		request:'import_signature_certificate',
		key_id: keyId,
		certificate: pemCertificate
	});
	sending.then(importSignatureCertificateResponse, handleError);
}

function importEncryptionKeyResponse() {

}

function importEncryptionKey(keyId, p12) {
	var sendMessage = browser.runtime.sendMessage({
		request:'import_encryption_key',
		key_id: keyId,
		pkcs12: p12
	});
	sending.then(importEncryptionKeyResponse, handleError);
}
