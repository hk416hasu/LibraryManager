document.getElementById("execute").addEventListener("click", async () => {
  try {
    const response = await fetch('/executeHello', { method: 'POST' });
    const result = await response.text();
    document.getElementById("result").innerHTML = result.replace(/\n/g, '<br>'); // 替换换行符为 <br>
  } catch (error) {
    console.error("发生错误：", error);
    document.getElementById("result").textContent = "发生错误，请查看控制台日志。";
  }
});
