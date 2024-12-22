//(async function async_print() {
//(async function () {
(async () => {
  await setTimeout(() => {
    document.getElementById("async_demo").innerHTML = "1";
  }, 1000);
  await setTimeout(() => {
    document.getElementById("async_demo").innerHTML = "2";
  }, 2000);
  await setTimeout(() => {
    document.getElementById("async_demo").innerHTML = "3";
  }, 3000);
})();
