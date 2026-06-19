const fs = require('fs');
const path = require('path');
const base = '/Users/yc/YCBookBlog/book/09.出版作品的书籍/03.毛选选集解读';
const dirs = ['02.哲学思维和基础','03.战略制定和规划','04.策略执行和实践','05.组织建设的艺术','06.个人修养与成长'];

let delCount = 0;
for (const dir of dirs) {
  const dp = path.join(base, dir);
  const files = fs.readdirSync(dp).filter(f => f.endsWith('.md'));
  
  // Group by number prefix
  const groups = {};
  for (const f of files) {
    const num = f.match(/^([\d.]+)/)?.[1] || f;
    if (!groups[num]) groups[num] = [];
    groups[num].push(f);
  }
  
  // For each group, keep the file with 8 Chinese chars, delete others
  for (const [num, group] of Object.entries(groups)) {
    if (group.length < 2) continue;
    const candidates = group.map(f => {
      const cn = f.replace(/\.md$/,'').replace(/^[\d.]+/,'')
        .replace(/[^\u4e00-\u9fff]/g,'');
      return { file: f, cnLen: cn.length };
    });
    
    // Keep the 8-char version
    const keep = candidates.find(c => c.cnLen === 8);
    const del = candidates.filter(c => c !== keep && c.cnLen !== 8);
    
    for (const d of del) {
      const fp = path.join(dp, d.file);
      try {
        fs.unlinkSync(fp);
        console.log(`DEL: ${dir}/${d.file} (${d.cnLen}字)`);
        delCount++;
      } catch(e) { console.error(`FAIL: ${dir}/${d.file}`, e.message); }
    }
  }
}
console.log(`\nDeleted ${delCount} old files`);
