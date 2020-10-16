function handleError(error) {
	console.log(error);
}

function createCSRResponse(requestId) {
	return function(message) {
        if (message.response.request_id == requestId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    request_id: requestId,
                    request: 'create_csr',
                    response: message.response.response 
                }
            }, "*")            
        }
	}
}

function createCSRRequest(requestId, dName, rsaBitLength) {
	var handleResp = createCSRResponse(requestId)
	var sendMessage = browser.runtime.sendMessage({
        request_id: requestId,
		request:'create_csr',
		subject_name: dName,
        rsa_key_length: rsaBitLength
	});
	sendMessage.then(handleResp, handleError);
}

function importCertificateResponse(requestId) {
    return function(message) {
        if (message.response.request_id == requestId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    request_id: requestId,
                    request: 'import_certificate',
                    response: message.response.response 
                }
            }, "*")            
        }
    }
}

function importCertificateRequest(requestId, pemCertificate) {
    var handleResp = importCertificateResponse(requestId)
	var sendMessage = browser.runtime.sendMessage({
		request:'import_certificate',
		request_id: requestId,
		certificate: pemCertificate
	});
	sendMessage.then(handleResp, handleError);
}

function importPfxKeyResponse(requestId) {
    return function(message) {
        if (message.response.request_id == requestId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    request_id: requestId,
                    request: 'import_pfx_key',
                    response: message.response.response 
                }
            }, "*")            
        }
    }
}

function importPfxKeyRequest(requestId, p12, passwd) {
    var handleResp = importPfxKeyResponse(requestId)
	var sendMessage = browser.runtime.sendMessage({
		request:'import_pfx_key',
		request_id: requestId,
		pkcs12: p12,
        password: passwd
	});
	sendMessage.then(handleResp, handleError);
}

function exportPfxKeyResponse(requestId) {
    return function(message) {
        if (message.response.request_id == requestId) {
            window.postMessage({
                direction: "from-content-script",
                message: {
                    request_id: requestId,
                    request: 'export_pfx_key',
                    response: message.response.response 
                }
            }, "*")            
        }
    }
}

function exportPfxKeyRequest(requestId, issuerName, serialNr, passWord) {
    var handleResp = exportPfxKeyResponse(requestId)
    var sendMessage = browser.runtime.sendMessage({
        request:'export_pfx_key',
        request_id: requestId,
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
    requestId = request.request_id;
  	switch (request.request) {
  		case 'create_csr': {
            createCSRRequest(requestId, request.subject_name, request.rsa_key_length);
            break;            
        }
  		case 'import_certificate': {
            importCertificateRequest(requestId, request.certificate);
            break;
        }
        case 'import_pfx_key': {
            importPfxKeyRequest(requestId, request.pkcs12, request.password);
            break;
        }
        case 'export_pfx_key': {
            exportPfxKeyRequest(requestId, request.issuer, request.serial_number, request.password);
            break;
        }
  		break;
  	}
  }
});
