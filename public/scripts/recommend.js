function validateBookForm() {
   const ISBN = document.getElementById('ISBN').value;

   let isValid = true;

   const ISBN_Regex = /^(978|979)\d{10}$/;
   if (!ISBN_Regex.test(ISBN)) {
      alert("ISBN 应为13位数字");
      document.getElementById('ISBN').value = '';
      isValid = false;
   }

   return isValid;
}

// 书籍荐购
document.getElementById('bookForm').addEventListener('submit', async (event) => {
   event.preventDefault();

   console.log('书籍荐购 start');

   if (!validateBookForm()) {
      return;
   }

   const bookTitle = document.getElementById('book-title').value;
   const ISBN = document.getElementById('ISBN').value;
   const Press = document.getElementById('book-press').value;
   const author = document.getElementById('book-author').value;
   const uID = localStorage.getItem('uID');
   const recommendTime = new Date().toISOString().split('T')[0];
   const reason = document.getElementById('book-recommend-reason').value;

   try {
      console.log('书籍荐购 表单发送 开始');
      const res = await fetch('/api_zhao/bookrecommend', {
         method: 'POST',
         headers: {
            'Content-Type': 'application/json',
         },
         body: JSON.stringify({
            bookTitle: bookTitle,
            ISBN: ISBN,
            Press: Press,
            author: author,
            uID: uID,
            recommendTime: recommendTime,
            reason: reason,
         }),
      });

      console.log('书籍荐购 表单发送 结束');

      if (res.ok) {
         const result = await res.json();
         console.log('书籍荐购 成功', result);
         alert('书籍荐购 成功');
      } else {
         console.log('书籍荐购 失败');
         alert('书籍荐购 失败');
      } 
   } catch (error) {
      console.error('书籍荐购出错', error);
      alert('书籍荐购发生错误，请稍后重试');
   } finally {
      document.getElementById('book-title').value = '';
      document.getElementById('ISBN').value = '';
      document.getElementById('book-press').value = '';
      document.getElementById('book-author').value = '';
      document.getElementById('book-recommend-reason').value = '';
   }
});
