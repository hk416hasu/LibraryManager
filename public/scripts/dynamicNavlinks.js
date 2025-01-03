// 获取导航栏的 ul 元素
const navLinks = document.getElementById("nav-links");

/*
0 ----- 用户管理员
1 ----- 默认普通用户
2 ----- VIP用户
3 ----- 教师
4 ----- 采编部
5 ----- 流通部
6 ----- 系统维护员
*/

// 定义导航项
const navItems = [
   { href: "/index.html", text: "首页", roles: ["all"] },
   { href: "/search.html", text: "馆藏查询", roles: ["1", "2", "3"] }, // 在这里借书/预约, 甚至是归还
   { href: "/recommend.html", text: "资源荐购", roles: ["1", "2", "3"] },
   { href: "/userinfo.html", text: "个人信息", roles: ["1", "2", "3"] },
   { href: "/borrowInfo.html", text: "借阅记录", roles: ["1", "2", "3"] },
   { href: "/userManage.html", text: "用户管理", roles: ["0"] },
   { href: "/buy.html", text: "订单管理", roles: ["4"] },
   { href: "/catalog.html", text: "书籍编目", roles: ["4"] },
   { href: "/destroy.html", text: "注销报损", roles: ["4"] },
   { href: "/flow.html", text: "流通状态", roles: ["5"] }, // 查看所有图书的流通信息
   { href: "/system.html", text: "系统维护", roles: ["6"] },
];

// 获取用户权限
const authorityNum = localStorage.getItem("authority_num") || null;

// 生成导航项
navItems.forEach(item => {
   if (item.roles.includes("all") || (authorityNum && item.roles.includes(authorityNum))) {
      const li = document.createElement("li");
      const a = document.createElement("a");
      a.href = item.href;
      a.textContent = item.text;
      li.appendChild(a);
      navLinks.appendChild(li);
   }
});

