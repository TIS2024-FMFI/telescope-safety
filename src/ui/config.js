// Funkcia na potvrdenie odoslania formulára
function confirmSubmit(event) {
    event.preventDefault();
    const confirmed = confirm("Naozaj chcete uložiť tieto zmeny?");
    if (confirmed) {
        event.target.submit();
    }
}

function generateDateRange(fromDate, toDate) {
    let dateList = [];
    let currentDate = new Date(fromDate);
    let endDate = new Date(toDate);

    while (currentDate <= endDate) {
        let formattedDate = currentDate.toISOString().split("T")[0];
        dateList.push(formattedDate);
        currentDate.setDate(currentDate.getDate() + 1);
    }
    return dateList;
}


document.addEventListener('DOMContentLoaded', function () {
    document.getElementById("forbidden-positions").setAttribute("placeholder", 
        "# Zakázaná oblasť č. 1\n90 45\n40 38\n78 66"
    );

    const restartButton = document.getElementById('restart-button');
    if (restartButton) {
        restartButton.addEventListener('click', () => {
            const confirmed = confirm('Systém sa vypne. Chcete pokračovať?');
            if (confirmed) {
                alert('Systém bude vypnutý.');
            } else {
                alert('Vypnutie systému bolo zrušené.');
            }
        });
    }

    const saveZonesButton = document.getElementById('zones-settings');
    if (saveZonesButton) {
        saveZonesButton.addEventListener('click', confirmSubmit);
    }

    const saveAlarmButton = document.getElementById('alarm-settings');
    if (saveAlarmButton) {
        saveAlarmButton.addEventListener('click', confirmSubmit);
    }

    const downloadLogsButton = document.getElementById('download-logs');
    if (downloadLogsButton) {
        downloadLogsButton.addEventListener("click", async function (event) {
            event.preventDefault();
    
            const fromDate = document.getElementById("log-from").value;
            const toDate = document.getElementById("log-to").value;
    
            if (!fromDate || !toDate) {
                alert("Prosím, vyberte dátumový rozsah.");
                return;
            }
    
            try {
                const dateList = generateDateRange(fromDate, toDate);
                console.log(dateList);
    
                for (let date of dateList) {
                    const fileName = `Log_${date}.csv`;
    
                    const url = `/download?file=logs/${encodeURIComponent(fileName)}`;

                    console.log(url);
    
                    const response = await fetch(url);
    
                    if (!response.ok) {
                        console.warn(`Súbor ${fileName} nebol nájdený.`);
                        continue;
                    }
    
                    // Convert response to Blob (CSV file)
                    const blob = await response.blob();
                    const fileURL = window.URL.createObjectURL(blob);
    
                    // Create a temporary download link
                    const a = document.createElement("a");
                    a.href = fileURL;
                    a.download = fileName;
                    document.body.appendChild(a);
                    a.click();
                    document.body.removeChild(a);
                }
    
            } catch (error) {
                alert(`Chyba pri sťahovaní logov: ${error.message}`);
            }
        });
    }

    const downloadEventsButton = document.getElementById('download-events');
    if (downloadEventsButton) {
        downloadEventsButton.addEventListener("click", async function (event) {
            event.preventDefault();

            try {
                const configFileName = "logs/events/Log_configuration.csv";
                const colisionsFileName = "logs/events/Log_collisions.csv";

                const events = [configFileName, colisionsFileName];

                for (let fileName of events) {
                    // Construct the request URL
                    const url = `/download?file=${fileName}`;

                    console.log(url);
    
                    const response = await fetch(url);
    
                    if (!response.ok) {
                        console.warn(`Súbor ${fileName} nebol nájdený.`);
                        continue;
                    }
    
                    // Convert response to Blob (CSV file)
                    const blob = await response.blob();
                    const fileURL = window.URL.createObjectURL(blob);
    
                    // Create a temporary download link
                    const a = document.createElement("a");
                    a.href = fileURL;
                    a.download = fileName;
                    document.body.appendChild(a);
                    a.click();
                    document.body.removeChild(a);
                }
    
            } catch (error) {
                alert(`Chyba pri sťahovaní logov: ${error.message}`);
            }

        });
    }

});


