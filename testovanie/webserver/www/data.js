const hostName = window.location.hostname
let ws = new WebSocket("ws://" + hostName + ":8080");
bind()

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function bind(){
    ws.onopen = function () {
        document.getElementById("position").innerHTML = `<p>Connected. Waiting for data</p>`;
    }

    ws.onerror = function(event) {
        ws.close();
    }

    ws.onmessage = function(event) {
        console.log("Received message from server");
        console.log(JSON.parse(event.data));
        let data = JSON.parse(event.data);
        let newContent = ""
        for (var key in data) {
            newContent += `<p>${capitalize(key)}: ${data[key]}</p>\n`
        }
        document.getElementById("position").innerHTML = newContent;


    }

    ws.onclose = function(event) {
        document.getElementById("position").innerHTML = `<p>Not Connected...</p>`;
    }
}



function retry(){
    if (ws.readyState !== WebSocket.OPEN) {
        console.log("Reconnecting...");
        try {
            ws = new WebSocket("ws://" + hostName + ":8080");
            console.log("Reconnected");
            bind()
        }
        catch (e) {}
    }
    setTimeout(retry, 5000);
}

retry();

