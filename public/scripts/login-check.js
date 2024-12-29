function validateForm() {
   const username = document.getElementById('username').value;
   const password = document.getElementById('password').value;

   let isValid = true;

   // Username validation: At least 3 characters, alphanumeric
   const usernameRegex = /^[a-zA-Z0-9]{3,}$/;
   const usernameError = document.getElementById('username-error');
   if (!usernameRegex.test(username)) {
      usernameError.textContent = '用户名必须至少包含3个字符，只能包含字母和数字。';
      isValid = false;
   } else {
      usernameError.textContent = '';
   }

   // Password validation: At least 6 characters
   const passwordRegex = /^.{6,50}$/;
   const passwordError = document.getElementById('password-error');
   if (!passwordRegex.test(password)) {
      passwordError.textContent = '密码必须至少包含6个字符。';
      isValid = false;
   } else {
      passwordError.textContent = '';
   }

   return isValid;
}

document.getElementById('login-form').addEventListener('submit', (event) => {
   if (!validateForm()) {
      event.preventDefault(); // Prevent form submission if validation fails
      alert("\u{1f605}");
   } else {
      const passwd = document.getElementById('password').value;
      const hashedPassed = CryptoJS.SHA256(passwd).toString();
      console.log(hashedPassed);
      document.getElementById('password').value = '';
      alert("\u{1f60a}");
   }
});

document.getElementById('login-form').addEventListener('input', () => {
   validateForm(); // Validate in real-time as user inputs data
});
