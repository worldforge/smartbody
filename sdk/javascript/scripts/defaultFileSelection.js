function handleFileSelect(evt) {
	var files;
	if(evt.target.files){
		files = evt.target.files; // FileList object.
	}else{
		evt.stopPropagation();
		evt.preventDefault();
		files = evt.dataTransfer.files;
	}
	var name = files[0].name;
	var extension = name.substr(name.lastIndexOf('.')+1);
	//script file: display and run it
	if(extension == 'js'){
		var reader = new FileReader();  
		reader.onload = function(event) {
			document.getElementById('command').value = event.target.result;
			scene.run(event.target.result);
		}
		reader.readAsText(files[0],"UTF-8");
	}
}

function handleDragOver(evt) {
    evt.stopPropagation();
    evt.preventDefault();
    evt.dataTransfer.dropEffect = 'copy'; // Explicitly show this is a copy.
}
Module.noImageDecoding = true;
Module.noAudioDecoding = true;
var dropZone = document.getElementById('command');
if(dropZone){
	dropZone.addEventListener('dragover', handleDragOver, false);
	dropZone.addEventListener('drop', handleFileSelect, false);
}