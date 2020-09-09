
browser.runtime.onMessage.addListener(
	(request, sender, sendResponse) => {
		if (request) {
			console.log(request)
			sendNative = browser.runtime.sendNativeMessage("org.cryptable.pki.keymgmnt", request)

			sendNative.then(function(resp) {
				sendResponse({response: resp})
			},
			function(err) {
				sendResponse({error: err})
			});
		}
		else {
			sendResponse("{error:'empty request'}");
		}
		return true;
	}
);