function updateTime() {
    document.getElementById('date_output').innerHTML = new Date().toLocaleString();
}

updateTime(); // Update when loading html

// Update the time every second
setInterval(updateTime, 1000);

var count = 0;
document.getElementById('fetchDate').addEventListener('click', () => {
    count++;
    if (count < 3) {
        document.getElementById('main_text').innerHTML="HELLO WORLD \u{1F973}";
    } else {
        document.getElementById('main_text').innerHTML="don't try again \u{1F92C}";
    }
});
