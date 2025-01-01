function validateForm() {
   const username = document.getElementById('username').value;
   const password = document.getElementById('password').value;

   let isValid = true;

   // Username validation: 3 - 12 characters, alphanumeric
   const usernameRegex = /^[a-zA-Z0-9]{3,12}$/;
   const usernameError = document.getElementById('username-error');
   if (!usernameRegex.test(username)) {
      usernameError.textContent = '长度为3至12，仅包含字母和数字';
      isValid = false;
   } else {
      usernameError.textContent = '';
   }

   // Password validation: At least 6 characters
   const passwordRegex = /^.{6,}$/;
   const passwordError = document.getElementById('password-error');
   if (!passwordRegex.test(password)) {
      passwordError.textContent = '长度至少为6';
      isValid = false;
   } else {
      passwordError.textContent = '';
   }

   return isValid;
}

document.getElementById('login-form').addEventListener('submit', async (event) => {
   event.preventDefault(); // Prevent form submission

   if (!validateForm()) {
      alert("\u{1f605}");
      return;
   }

   const username = document.getElementById('username').value;
   const passwd = document.getElementById('password').value;
   const hashed_passwd = CryptoJS.SHA256(passwd).toString();
   console.log("your hashed_passwd is : " + hashed_passwd);

   try {
      const response = await fetch('/api/login', {
         method: 'POST',
         headers: {
            'Content-Type': 'application/json',
         },
         body: JSON.stringify({
            username: username,
            hashed_passwd: hashed_passwd, // 使用哈希后的密码
         }),
      });

      if (response.ok) {
         const result = await response.json();
         console.log('登录成功:', result);
         localStorage.setItem('username', username);
         console.log("your authority_num is: " + result.authority_num);
         localStorage.setItem('authority_num', result.authority_num);
         alert("登录成功: \u{1f60a}");
         window.location.reload();
      } else {
         console.error('登录失败');
         alert('登录失败，请检查用户名或密码\u{1f605}');
      }
   } catch (error) {
      console.error('请求出错:', error);
      alert('登录时发生错误，请稍后重试');
   } finally {
      // 清空密码框
      document.getElementById('password').value = '';
   }
});

document.getElementById('login-form').addEventListener('input', () => {
   validateForm(); // Validate in real-time as user inputs data
});

function welcomeLoginedUser() {
   document.getElementById('login-box').innerHTML = `
      <span>
         hi, <strong>${localStorage.username}</strong>!
      </span>
      <button style="margin-left: 3px;" onclick="logout()">登出</button>
   `;
}

function logout() {
   localStorage.removeItem('username');
   window.location.reload();
   window.location.href = '/';
}

// change the welcome message for logined user
if (localStorage.getItem('username')) {
   welcomeLoginedUser();
}

// When the user clicks anywhere outside of the modal, close it
var modal = document.getElementById('id01');
window.onclick = function(event) {
   if (event.target == modal) {
      modal.style.display = "none";
   }
}
