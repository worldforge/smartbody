$(document).ready(function() {

	var client, destination, scope;

	if (!String.prototype.trim) {
		String.prototype.trim=function(){return this.replace(/^\s\s*/, '').replace(/\s\s*$/, '');};
	}
	
	$('#connect_form').submit(function() {
		var url = $("#connect_url").val();
		url = "ws://" + url + ":61614/stomp";
		scope = $("#scope").val();
		destination = "/topic/" + scope;

		client = Stomp.client(url);

		// this allows to display debug logs directly on the web page
		client.debug = function(str) {
			$("#debug").append(str + "\n");
		};
		// the client is notified when it is connected to the server.
		var onconnect = function(frame) {
			client.debug("connected to Stomp");
			$('#connect').fadeOut({ duration: 'fast' });
			$('#disconnect').fadeIn();
			$('#send_form_input').removeAttr('disabled');

			client.subscribe(destination, function(message) {
				// here the message arrives

				$("#messages").append("<p>" + message.body + "</p>\n");
			});
		};
		client.connect("guest", "guest", onconnect);

		return false;
	});

	$('#disconnect_form').submit(function() {
		client.disconnect(function() {
			$('#disconnect').fadeOut({ duration: 'fast' });
			$('#connect').fadeIn();
			$('#send_form_input').addAttr('disabled');
		});
		return false;
	});

	$('#send_form').submit(function() {
		var text = $('#send_form_input').val();
		if (text) {
			text = text.trim();
			var arr   = text.split(" ");
			var first = arr.shift();

			client.send(destination, {ELVISH_SCOPE: scope, MESSAGE_PREFIX:first}, text);
			$('#send_form_input').val("");
		}
		return false;
	});

});