const express = require('express');
const router = express.Router();
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

router.post('/send-test', (req, res) => {

   const email = req.body.email; // local variable
   console.log(`Received email: ${email}`);

   const options = {
      from: "487882183@qq.com",
      to: `${email}`,
      text: `Hello ${email} from nodejs`,
   }

   transporter.sendMail(options, (err) => {
      if (err) {
         console.log(err);
         res.status(400).send('not Okay');
      } else {
         console.log("Message sent: okay!");
         res.status(200).send('Okay');
      }
   });

});

module.exports = router;
