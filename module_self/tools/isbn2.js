/**
 * npm install crypto-js node-fetch@2
 */
const fetch = require('node-fetch');
const CryptoJS = require("crypto-js");

// 云市场分配的密钥Id
const secretId = "AKIDeo2rtKdgwFawn07l4lm9q60sd10E4omPpXh9";
// 云市场分配的密钥Key
const secretKey = "4cgZ6GSaC639gGX3d2Y94g7V405Wuy3fKZnZi5un";
const source = "market";

// 获取命令行参数中的 ISBN
const isbn = process.argv[2];
if (!isbn) {
    console.error("Please provide an ISBN number as a command-line argument.");
    process.exit(1);
}

// 签名
const datetime = (new Date()).toGMTString();
const signStr = "x-date: " + datetime + "\n" + "x-source: " + source;
const sign = CryptoJS.enc.Base64.stringify(CryptoJS.HmacSHA1(signStr, secretKey));
const auth = `hmac id=\"${secretId}\", algorithm=\"hmac-sha1\", headers=\"x-date x-source\", signature=\"${sign}\"`;

// 请求头
const headers = {
    "X-Source": source,
    "X-Date": datetime,
    "Authorization": auth,
    "Content-Type": "application/x-www-form-urlencoded"
};

// URL 和 Body 参数
const url = "https://service-ed62t9xo-1305308687.gz.apigw.tencentcs.com/release/isbn/query";
const bodyParams = new URLSearchParams({
    "isbn": isbn
});

// 使用 fetch 发送请求
(async () => {
    try {
        const response = await fetch(url, {
            method: "POST",
            headers: headers,
            body: bodyParams
        });

        if (!response.ok) {
            console.error(`HTTP Error: ${response.status} ${response.statusText}`);
            return;
        }

        const responseBody = await response.text();
        console.log(responseBody);
    } catch (error) {
        console.error("Error:", error);
    }
})();

