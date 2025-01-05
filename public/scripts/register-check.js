function validateRegisterForm() {
   const username = document.getElementById('register-username').value;
   const password = document.getElementById('register-password').value;
   const againPassword = document.getElementById('register-again-password').value;
   const email = document.getElementById('register-email').value;
   const tel = document.getElementById('register-tel').value;

   let isValid = true;

   // Username validation: 3 - 12 characters, alphanumeric
   const usernameRegex = /^[a-zA-Z0-9]{3,12}$/;
   const usernameError = document.getElementById('register-username-error');
   if (!usernameRegex.test(username)) {
      usernameError.textContent = '长度为3至12，仅包含字母和数字';
      isValid = false;
   } else {
      usernameError.textContent = '';
   }

   // Password validation: At least 6 characters
   const passwordRegex = /^.{6,}$/;
   const passwordError = document.getElementById('register-password-error');
   if (!passwordRegex.test(password)) {
      passwordError.textContent = '长度至少为6';
      isValid = false;
   } else {
      passwordError.textContent = '';
   }

   const againPasswordError = document.getElementById('register-again-password-error');
   if (password === againPassword) {
      againPasswordError.textContent = '';
   } else {
      againPasswordError.textContent = '两次密码不一样';
      isValid = false;
   }

   const emailRegex = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
   const emailError = document.getElementById('register-email-error');
   if (!emailRegex.test(email)) {
      emailError.textContent = 'email格式错误';
      isValid = false;
   } else {
      emailError.textContent = '';
   }

   const telRegex = /^1\d{10}$/;
   const telError = document.getElementById('register-tel-error');
   if (!telRegex.test(tel)) {
      telError.textContent = '手机号格式错误';
      isValid = false;
   } else {
      telError.textContent = '';
   }

   const sfz = document.getElementById('register-sfz').value;
   const sfzRegex = /^\d{18}$/;
   const sfzError = document.getElementById('register-sfz-error');
   if (!sfzRegex.test(sfz)) {
      console.log(sfz);
      sfzError.textContent = '身份证格式错误';
      isValid = false;
   } else {
      sfzError.textContent = '';
   }

   return isValid;
}

document.getElementById('register-form').addEventListener('input', () => {
   validateRegisterForm();
});

document.getElementById('register-form').addEventListener('submit', async (event) => {
   event.preventDefault(); // Prevent form submission

   if (!validateRegisterForm()) {
      alert("\u{1f605}");
      document.getElementById('register-password').value = '';
      document.getElementById('register-again-password').value = '';
      return;
   }

   const username = document.getElementById('register-username').value;
   const passwd = document.getElementById('register-password').value;
   const email = document.getElementById('register-email').value;
   const tel = document.getElementById('register-tel').value;
   const sfz = document.getElementById('register-sfz').value;
   const POF = document.getElementById('register-POF').value;
   const gender = 1;

   const hashed_passwd = CryptoJS.SHA256(passwd).toString();
   console.log("your hashed_passwd is : " + hashed_passwd);

   try {
      const response = await fetch('/api/register', {
         method: 'POST',
         headers: {
            'Content-Type': 'application/json',
         },
         body: JSON.stringify({
            uCode: hashed_passwd, // 使用哈希后的密码
            userName: username,
            POF: POF,
            shenFenZheng: sfz,
            gender: gender,
            phoneNumber: tel,
            email: email,
         }),
      });

      if (response.ok) {
         const result = await response.json();
         console.log('注册成功:', result);
         alert("注册成功: \u{1f60a}");
         window.location.reload();
      } else {
         const result = await response.json();
         console.error('注册失败:', result);
         alert('注册失败\u{1f623}');
      }
   } catch (error) {
      console.error('请求出错:', error);
      alert('注册时发生错误，请稍后重试');
   } finally {
      // 清空密码框
      document.getElementById('register-password').value = '';
      document.getElementById('register-again-password').value = '';
      document.getElementById('register-sfz').value = '';
   }
});
