
function generateId(length) {
   var result           = '';
   var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
   var charactersLength = characters.length;
   for ( var i = 0; i < length; i++ ) {
      result += characters.charAt(Math.floor(Math.random() * charactersLength));
   }
   return result;
}

function handleError(error) {
	console.log(error);
}

function createCSRResponse(keyId) {
	return function(message) {
        if (message.response.key_id == keyId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    response: message.response.response 
                }
            }, "*")            
        }
	}
}

function createCSRRequest(keyId, dName, rsaBitLength) {
	var handleResp = createCSRResponse(keyId)
	var sendMessage = browser.runtime.sendMessage({
        key_id: keyId,
		request:'create_csr',
		subject_name: dName,
        rsa_key_length: rsaBitLength
	});
	sendMessage.then(handleResp, handleError);
}

function importCertificateResponse(keyId) {
    return function(message) {
        if (message.response.key_id == keyId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    response: message.response.response 
                }
            }, "*")            
        }
    }
}

function importCertificateRequest(keyId, pemCertificate) {
    var handleResp = importCertificateResponse(keyId)
	var sendMessage = browser.runtime.sendMessage({
		request:'import_certificate',
		key_id: keyId,
		certificate: pemCertificate
	});
	sendMessage.then(handleResp, handleError);
}

function importPfxKeyResponse() {
    return function(message) {
        if (message.response.key_id == keyId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    response: message.response.response 
                }
            }, "*")            
        }
    }
}

function importPfxKeyRequest(keyId, p12, passwd) {
    var handleResp = importPfxKeyResponse(keyId)
	var sendMessage = browser.runtime.sendMessage({
		request:'import_encryption_key',
		key_id: keyId,
		pkcs12: p12,
        password: passwd
	});
	sendMessage.then(handleResp, handleError);
}

function exportPfxKeyResponse() {
    return function(message) {
        if (message.response.key_id == keyId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    response: message.response.response 
                }
            }, "*")            
        }
    }
}

function exportPfxKeyRequest(keyId, issuerName, serialNr, passWord) {
    var handleResp = importPfxKeyResponse(keyId)
    var sendMessage = browser.runtime.sendMessage({
        request:'export_pfx_key',
        key_id: keyId,
        issuer: issuerName,
        serial_number: serialNr,
        password: passWord
    });
    sendMessage.then(handleResp, handleError);
}

window.addEventListener("message", function(event) {
  if (event.source == window &&
      event.data &&
      event.data.direction == "from-page-script") {
  	request = event.data.message
    keyid = generateId(10);
  	switch (request.request) {
  		case 'create_csr': {
            createCSRRequest(keyid, request.subject_name, request.rsa_key_length);
            break;            
        }
  		case 'import_certificate': {
            importCertificateRequest(keyid, request.certificate);
            break;
        }
        case 'import_pfx_key': {
            importPKCS12Request(keyid, request.pkcs12, request.password);
            break;
        }
        case 'export_pfx_key': {
            exportPKCS12Request(keyid, request.issuer, request.serial_number, request.password);
            break;
        }
  		break;
  	}
  }
});