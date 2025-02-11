const hostName = window.location.hostname
const websecketLink = "ws://" + hostName + ":81";
let ws = new WebSocket(websecketLink);
console.log(hostName)
bind()


function bind() {
    ws.onopen = function () {
        const alert = document.getElementsByClassName("alert")[0];
        alert.style.display = "block";
        alert.className = "alert alert--success";

        const alertText = document.getElementsByClassName("alert__text")[0];
        alertText.innerText = `Connected. Waiting for data`;
    }

    ws.onerror = function (event) {
        const alert = document.getElementsByClassName("alert")[0];
        alert.style.display = "block";
        alert.className = "alert alert--error";

        const alertText = document.getElementsByClassName("alert__text")[0];
        alertText.innerText = `Unable to connect to the server. Please try again later.`;
        ws.close();
    }

    ws.onmessage = function (event) {
        const data = JSON.parse(event.data);
        const alert = document.getElementsByClassName("alert")[0];
        alert.style.display = "none";

        document.getElementById("azimut").innerText = `${data.azimuth}°`;
        document.getElementById("elevacia").innerText = `${data.elevation}°`;
        now = new Date();
        document.getElementById("update").innerText = now.toLocaleString();


        if (data.error){
            const alert = document.getElementsByClassName("alert")[0];
            alert.style.display = "block";
            alert.className = "alert alert--error";

            const alertText = document.getElementsByClassName("alert__text")[0];
            alertText.innerText = `Error: ${data.error}`;
        }

        if (data.warning){
            const alert = document.getElementsByClassName("alert")[0];
            alert.style.display = "block";
            alert.className = "alert alert--warning";

            const alertText = document.getElementsByClassName("alert__text")[0];
            alertText.innerText = `Warning: ${data.warning}`;
        }

    }

    ws.onclose = function (event) {
        const alert = document.getElementsByClassName("alert")[0];
        alert.style.display = "block";
        alert.className = "alert alert--error";

        const alertText = document.getElementsByClassName("alert__text")[0];
        alertText.innerText = `Not Connected...`;
    }
}

function retry() {
    if (ws.readyState !== WebSocket.OPEN && ws.readyState !== WebSocket.CONNECTING) {
        const alert = document.getElementsByClassName("alert")[0];
        alert.style.display = "block";
        alert.className = "alert alert--warning";

        const alertText = document.getElementsByClassName("alert__text")[0];
        alertText.innerText = `Attempting to connect to the server...`;

        console.log("Reconnecting...");
        try {
            ws = new WebSocket(websecketLink);
            bind()
            console.log("Reconnected");
        }
        catch (e) { }
    }
    setTimeout(retry, 10000);   // 10 seconds
}

retry();
