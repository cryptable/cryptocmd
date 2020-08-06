
browser.runtime.onMessage.addListener(
	(request, sender, sendResponse) => {
		console.log("DEBUG: Background message received " + request);
		if (request) {
			browser.runtime.sendNativeMessage("keymgmnt", request, function(resp) {
				console.log("Background Native Message received:");
				console.log(resp);
				sendResponse(resp);
			});
		}
		else {
			sendResponse("{error}");
		}
		return true;
	}
);