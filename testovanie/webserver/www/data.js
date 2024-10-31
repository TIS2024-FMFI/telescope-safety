const hostName = window.location.hostname
let ws = new WebSocket("ws://" + hostName + ":8080");
console.log(hostName)
bind()

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function bind(){
    ws.onopen = function () {
        console.log("Connection open");
        document.getElementById("position").innerHTML = `<p>Connected. Waiting for data</p>`;
    }

    ws.onerror = function(event) {
        console.error(event);
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
    if (ws.readyState !== WebSocket.OPEN && ws.readyState !== WebSocket.CONNECTING) {
        console.log("Reconnecting...");
        try {
            ws = new WebSocket("ws://" + hostName + ":8080");
            bind()
            console.log("Reconnected");
        }
        catch (e) {}
    }
    setTimeout(retry, 5000);
}

retry();

