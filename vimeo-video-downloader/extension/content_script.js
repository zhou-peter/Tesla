
//iframe objects have
//videoWrapper - iframe or video, URL, inited(boolean)
//region {calls}
console.log("content script loaded");
var iframesCollection = new Array();
var lastHoverItem=null;
var autoPlayItem=null;
function searchIframes() {
	var iframes = document.getElementsByTagName("iframe");
	var videos=document.getElementsByTagName("video");
	if (iframes.length>0 || videos.length>0){
		console.log("iframes count " + iframes.length);
		console.log("videos count " +videos.length);
		for (var i=0;i<iframes.length;i++){
			var iframe=iframes[i];
			var cn=iframe.getAttribute('src') || "";
			//console.log('class name '+cn);
			if (cn.indexOf("player.vimeo.com")>=0){
				console.log(iframe);
				var colItem={ 
					videoWrapper: iframe,
					URL: "",
					isIframe: true,
					inited: false};
				iframesCollection.push(colItem);
				iframe.addEventListener("mouseover", 
					function(){lastHoverItem=this;}.bind(colItem));
					
				if (autoPlayItem!=null){
					colItem.URL=autoPlayItem.URL;
					autoPlayItem=null;
					lastHoverItem=colItem;
					initItem(colItem);
				}
			}
		}
		
		for (var i=0;i<videos.length;i++){
			var v=videos[i];
			var cn=v.getAttribute('src') || "";
			var colItem={ 
				videoWrapper: v,
				URL: "<no video url>",
				isIframe: false,
				inited: false};
			iframesCollection.push(colItem);			
		}
		console.log(iframesCollection);
		
		if (videos.length>0){
			setTimeout(checkVideos, 1000);
		}
	}else{
		setTimeout(searchIframes, 1000);
	}
}
setTimeout(searchIframes, 1000);

function checkVideos(){
	var shouldRescan=false;
	for (var i=0;i<iframesCollection.length;i++){
		var colItem=iframesCollection[i];
		//is not inited <video/> element
		if (colItem.inited==false && colItem.isIframe==false){
			var url=colItem.videoWrapper.getAttribute('src') || "";
			var re = /player\.vimeo\.com.+mp4/
			if (re.test(url)){
				colItem.URL=url;
				initItem(colItem);
			}else{
				shouldRescan=true;
			}
		}
	}
	if (shouldRescan){
		setTimeout(checkVideos, 1000);		
	}
}

var initItem=function(item){
	console.log(item.URL);
	item.inited=true;	
	var a = null;
	if (item.isIframe){
		a = document.createElement("a");
		a.setAttribute("style", "position:relative; z-index:100");
		a.setAttribute("href", "vimeo-downloader://"+item.URL);		
	}else{
		a = document.createElement("span");
		a.setAttribute("style", "color:blue; position:relative; z-index:100");
		a.setAttribute("href", item.URL);	
		a.addEventListener("click", function(event){
			chromeDownload(this);
			}.bind(a));
	}
	a.innerHTML="<h3>Download</h3>";
	var parent=item.videoWrapper.parentElement;
	parent.appendChild(a);
}

function chromeDownload(a){
	chrome.runtime.sendMessage({url:a.getAttribute("href")},null);
	//use alt+click free download manager 
	return false;
}

var onMessage=function(requestMessage,sender,sendResponse) {
	console.log(requestMessage);
	if (requestMessage.base64_found==true){
		if (lastHoverItem!=null){
			lastHoverItem.URL=requestMessage.data.url;			
		}else if (iframesCollection.length>0){
			lastHoverItem=iframesCollection[0];//todo check autoplay
			lastHoverItem.URL=requestMessage.data.url;
		}else{
			autoPlayItem={ 
					videoWrapper: null,
					URL: requestMessage.data.url,
					inited: false};
		}
		
		if (lastHoverItem!=null){
			if (lastHoverItem.inited==false){
				initItem(lastHoverItem);
			}
		}
	}
};

chrome.runtime.onMessage.addListener(onMessage);


