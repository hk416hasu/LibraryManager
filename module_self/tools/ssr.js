// Server-Side Rendering, but manually
const fs = require('fs');
const path = require('path');

// 文件路径
const projPath = '/home/ubuntu/web/'
const headPath = path.join(projPath, 'public', 'html', 'share', 'header.html');
const templatePath = path.join(projPath, 'public', 'html', 'template');
const outputPath = path.join(projPath, 'public');

// 读取共享模板
const sharedHeader = fs.readFileSync(headPath, 'utf-8');

// 确保输出目录存在
if (!fs.existsSync(outputPath)) {
  fs.mkdirSync(outputPath);
}

// 处理每个页面
fs.readdirSync(templatePath).forEach((file) => {
  if (file.endsWith('.html')) {
    // 读取页面内容
    const filePath = path.join(templatePath, file);
    let pageContent = fs.readFileSync(filePath, 'utf-8');

    // 替换占位符
    pageContent = pageContent.replace('<!-- header -->', sharedHeader);

    // 输出到新的目录
    const outputFilePath = path.join(outputPath, file);
    fs.writeFileSync(outputFilePath, pageContent);

    console.log(`Processed: ${file}`);
  }
});

console.log('All pages processed successfully!');

