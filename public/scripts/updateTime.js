function updateTime() {
    document.getElementById('date_output').innerHTML = new Date().toLocaleString();
}

updateTime(); // Update when loading html

// Update the time every second
setInterval(updateTime, 1000);
