function myinput_test() {
    var x = document.getElementById('input_demo').value.trim(); // remove spaces
                                        // .value return a string
    if (x.match(/^[-+]?\d+(\.\d+)?$/)) { // test() is different from match()
        alert("Yes \uD83C\uDF89");
    } else {
        alert("No \uD83D\uDE02");
    }
}
