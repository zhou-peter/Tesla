//region {calls}
console.log("background script loaded");
var callback = function(details) {
	//console.log("base64_init details: ");
	//console.log(details);	
	//details.url - contains base64_init link
	var data={base64_found:true, data:details};
	console.log(data);
	chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
	  chrome.tabs.sendMessage(tabs[0].id, data, function(response) {
		//console.log(response);
	  });
	});

};


chrome.webRequest.onBeforeRequest.addListener(
  callback,
  {urls: ["*://*/*base64_init=1"]},
  []
);

chrome.runtime.onMessage.addListener(
 function(requestMessage,sender,sendResponse) {
	 console.log(requestMessage);
	 chrome.downloads.download(requestMessage);
 }
); 

//end-region
