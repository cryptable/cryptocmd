Setup
=====

- https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_manifests#Manifest_location

MacOS X
-------

- ~/Library/Application Support/Mozilla/NativeMessagingHosts/\<name\>.json
- ~/Library/Application Support/Google/Chrome/NativeMessagingHosts/\<name\>.json


Content scripts should use runtime.sendMessage to communicate with the background script.  Web context scripts can use custom events to communicate with content scripts (with randomly generated event names, if needed, to prevent snooping from the guest page).