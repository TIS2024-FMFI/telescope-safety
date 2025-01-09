// Kontrola autentifikácie pre config.html
const isAuthenticated = sessionStorage.getItem('authenticated') === 'true';

if (!isAuthenticated && window.location.pathname.includes('config.html')) {
    window.location.href = `login.html?redirect=config.html`;
}

// Funkcia na potvrdenie odoslania formulára
function confirmSubmit(event) {
    event.preventDefault();
    const confirmed = confirm("Naozaj chcete uložiť tieto zmeny?");
    if (confirmed) {
        event.target.submit();
    }
}

// Funkcia na overenie hesla na login stránke
function checkPassword(event) {
    event.preventDefault();

    const passwordInput = document.getElementById('password').value;

    if (passwordInput === '123') {
        sessionStorage.setItem('authenticated', 'true'); 

        const queryParams = new URLSearchParams(window.location.search);
        const redirectUrl = queryParams.get('redirect') || 'index.html';

        window.location.href = redirectUrl; 
    } else {
        document.getElementById('error-message').innerText = 'Nesprávne heslo. Skúste znova.';
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

// Funkcia na vymazanie autentifikácie pri opustení stránky
window.addEventListener('beforeunload', () => {
    if (window.location.pathname.includes('config.html')) {
        sessionStorage.removeItem('authenticated'); 
    }
});
