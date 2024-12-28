const nodemailer = require('nodemailer');

const transporter = nodemailer.createTransport({
   host: "smtp.qq.com",
   // service: "qq",
   secure: true,
   auth: {
      user: "487882183@qq.com",
      pass: "rnxczusjmmlsbhdf"
   }
});

const options = {
   from: "487882183@qq.com",
   to: "vbcgfh487882183@126.com",
   text: "Hello, World?", // will be covered by HTML
   html: "<h1>Hello😇</h1><p>This is a <strong>test email</strong> from Node.js.😊</p>",
}
transporter.sendMail(options, (err) => {
   if (err) {
      console.log(err);
   } else {
      console.log("Message sent: okay!");
   }
});
