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

function validateJournalForm() {
   const ISSN = document.getElementById('ISSN').value;

   let isValid = true;

   const issnRegex = /^[0-9]{4}-[0-9]{3}[0-9Xx]$/;
   if (!issnRegex.test(ISSN)) {
      alert("ISSN格式错误");
      document.getElementById('ISSN').value = '';
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

// 期刊荐购
document.getElementById('journalForm').addEventListener('submit', async (event) => {
   event.preventDefault();

   console.log('期刊荐购 start');

   if (!validateJournalForm()) {
      return;
   }

   const journalTitle = document.getElementById('jou-title').value;
   const ISSN = document.getElementById('ISSN').value;
   const Press = document.getElementById('jou-press').value;
   const uID = localStorage.getItem('uID');
   const recommendTime = new Date().toISOString().split('T')[0];
   const reason = document.getElementById('jou-recommend-reason').value;

   try {
      console.log('期刊荐购 表单发送 开始');
      const res = await fetch('/api_zhao/journalrecommend', {
         method: 'POST',
         headers: {
            'Content-Type': 'application/json',
         },
         body: JSON.stringify({
            journalTitle: journalTitle,
            ISSN: ISSN,
            Press: Press,
            uID: uID,
            recommendTime: recommendTime,
            reason: reason,
         }),
      });

      console.log('期刊荐购 表单发送 结束');

      if (res.ok) {
         const result = await res.json();
         console.log('期刊荐购 成功', result);
         alert('期刊荐购 成功');
      } else {
         console.log('期刊荐购 失败');
         alert('期刊荐购 失败');
      } 
   } catch (error) {
      console.error('期刊荐购出错', error);
      alert('期刊荐购发生错误，请稍后重试');
   } finally {
      document.getElementById('jou-title').value = '';
      document.getElementById('ISSN').value = '';
      document.getElementById('jou-press').value = '';
      document.getElementById('jou-recommend-reason').value = '';
   }
});
