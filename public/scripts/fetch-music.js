document.getElementById("music").addEventListener("click", async () => {
    fetch('/audio/glitch_city.mp3')
        .then(response => response.blob())
        .then(audioBlob => {
            // Create a URL for the Blob and set it as the audio source
            const audioURL = URL.createObjectURL(audioBlob);
            const audioPlayer = document.getElementById("audio-player");
            audioPlayer.src = audioURL;
            audioPlayer.play();
        })
        .catch(error => console.error('Error:', error));
});

//  document.getElementById("music").addEventListener("click", async () => {
//      try {
//          const response = await fetch('/audio/glitch_city.mp3');
//          const audioBlob = await response.blob();
//          const audioURL = URL.createObjectURL(audioBlob);
//          const audioPlayer = document.getElementById("audio-player");
//          audioPlayer.src = audioURL;
//          audioPlayer.play();
//      } catch (error) {
//          console.error('Error:', error);
//      }
//  });
