

// Funkcia na potvrdenie odoslania formulára
function confirmSubmit(event) {
    event.preventDefault();
    const confirmed = confirm("Naozaj chcete uložiť tieto zmeny?");
    if (confirmed) {
        event.target.submit();
    }
}


document.addEventListener('DOMContentLoaded', () => {
    const shutdownButton = document.getElementById('shutdown-button');
    if (shutdownButton) {
        shutdownButton.addEventListener('click', () => {
            const confirmed = confirm('Systém sa vypne. Chcete pokračovať?');
            if (confirmed) {
                alert('Systém bude vypnutý.');
            } else {
                alert('Vypnutie systému bolo zrušené.');
            }
        });
    }

    const downloadLogsButton = document.getElementById('download-logs');
    if (downloadLogsButton) {
        downloadLogsButton.addEventListener('click', () => {
            const logFrom = document.getElementById('log-from').value;
            const logTo = document.getElementById('log-to').value;
            alert(`Sťahovanie logov od ${logFrom} do ${logTo}.`);
        });
    }

    const downloadEventsButton = document.getElementById('download-events');
    if (downloadEventsButton) {
        downloadEventsButton.addEventListener('click', () => {
            alert('Sťahovanie udalostí.');
        });
    }
});
