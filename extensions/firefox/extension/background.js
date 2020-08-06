
browser.runtime.onMessage.addListener(
	(request, sender, sendResponse) => {
		if (request) {
			sendNative = browser.runtime.sendNativeMessage("org.cryptable.pki.keymgmnt", request)

			sendNative.then(function(resp) {
				sendResponse({response: resp})
			},
			function(error) {
				sendResponse({error: err})
			});
		}
		else {
			sendResponse("{error:'empty request'}");
		}
		return true;
	}
);