#!/usr/bin/env python3
import os
import re
import sys

def extract_info(content):
    """从文件内容中提取标题、核心诗词和精神痛点"""
    lines = content.split('\n')
    title = ''
    core_poem = ''
    pain_point = ''
    
    # 提取标题（第一行以#开头）
    for line in lines:
        if line.startswith('# '):
            title = line.strip()
            break
    
    # 提取核心诗词
    for line in lines:
        if '核心诗词：' in line:
            core_poem = line.strip()
            # 移除标记，只保留诗词
            core_poem = core_poem.replace('核心诗词：', '').strip()
            break
    
    # 提取精神痛点
    for line in lines:
        if '精神痛点：' in line:
            pain_point = line.strip()
            pain_point = pain_point.replace('精神痛点：', '').strip()
            break
    
    return title, core_poem, pain_point

def generate_new_content(title, core_poem, pain_point, filename):
    """生成新的8幕结构内容"""
    # 从文件名提取章节号，例如2.2咬定青山.md -> 第二章
    # 简单起见，使用标题中的章节号
    chapter_num = ''
    if title.startswith('# '):
        # 假设格式为 "# 第二章 咬定青山：一个人的原则感"
        chapter_num = title.split(' ')[1]  # 取第二个部分
    
    # 如果核心诗词没有包含在>块中，添加格式
    if core_poem and not core_poem.startswith('>'):
        core_poem = f'> 核心诗词：{core_poem}'
    
    if pain_point and not pain_point.startswith('>'):
        pain_point = f'> 精神痛点：{pain_point}'
    
    # 提取诗词标题和作者（简单解析）
    poem_title = ''
    poem_author = ''
    poem_lines = []
    if core_poem:
        # 尝试解析诗词，例如 "于谦《石灰吟》「千锤万凿出深山...」"
        # 这里简化处理，直接使用原样
        pass
    
    # 生成新内容
    new_content = f"""{title}

{core_poem}
{pain_point}

---

## 第一幕｜故事引入：一个普通人的卡点（约 900 字）

【此处需要填写：一个具体人物的真实场景，展示其精神痛点。】

---

## 第二幕｜诗词出场：在故事断点处亮相（约 350 字）

> ### 诗词标题
>
> **诗人**
>
> 诗词原文...
>
> *【字词】注释...*

---

## 第三幕｜经典解读：字面之内的常规理解（约 700 字）

【此处需要填写：对诗词的经典解读，包括画面、情感、主旨三层。】

---

## 第四幕｜反主流解读：字面之外的另一种看法（约 950 字）

【此处需要填写：反主流解读，例如揭示诗人写诗时的真实处境和内心挣扎。】

---

## 第五幕｜诗人与我们：同一个困境的两次相遇（约 900 字）

【此处需要填写：诗人写诗时的真实处境与开篇人物的跨千年互文。】

---

## 第六幕｜现实映射：这首诗到底在说今天的谁（约 900 字）

【此处需要填写：将诗词映射到3-4类典型当代人群。】

---

## 第七幕｜故事回响：那个人，读完这首诗之后（约 750 字）

【此处需要填写：回到第一幕人物，描述他/她读完诗后的微小变化。】

---

## 第八幕｜自我改变：这首诗如何改写你（约 800 字）

### 认知层：从“改变环境”到“安顿内心”的思维转换

【此处需要填写：这首诗带来的核心思维转换。】

### 行动层：3 条可落地的日常践行清单

#### 践行一：1 分钟级——每晚睡前，默念一句诗（1 分钟）

- **具体操作**：关灯后，闭上眼睛，在心里慢慢默念诗中的关键句。
- **会发生什么**：坚持一周，你会发现自己入睡前“胡思乱想”的时间缩短了。
- **原理**：用一句确定的话，终结不确定的思维漩涡。

#### 践行二：7 天级——每天找一个“诗意时刻”（5–10 分钟）

- **具体操作**：在一天中，找出一个完全属于你的5–10分钟，做一件与诗相关的小事。
- **会发生什么**：七天后，你会开始期待这个“诗意时刻”。
- **原理**：在忙碌的生活中创造诗意的缝隙。

#### 践行三：21 天级——建立你的“诗意践行清单”（每周一次，15 分钟）

- **具体操作**：每周日晚，拿出纸笔，回顾本周的诗意践行，规划下周的践行。
- **会发生什么**：21天后，你会拥有一份属于自己的诗意践行地图。
- **原理**：把诗意的感悟变成可观察、可记录、可复制的行为数据。

### 全章总锚

> **诗中的关键句——不是世界变好了，是你在不够好的世界里，找到了自己的诗意。**

---

## 隐藏彩蛋｜诗词拾遗（约 200 字）

【此处需要填写：同主题小众诗短解读。】

---

## 本章节奏自查清单

- [ ] **故事钩**：第一幕的人物是否具体到能让读者“看见他的工位”？
- [ ] **诗词救场**：第二幕的诗词是否在“卡点”出场，而不是“轮到该出场了”？
- [ ] **基础解读**：第三幕是否让“完全没读过这首诗的人”也能站稳？
- [ ] **差异化**：第四幕的反主流解读是否有文本依据，而不是哗众取宠？
- [ ] **跨千年互文**：第五幕是否把“诗人此刻”和“我们此刻”并置在同一个画面里？
- [ ] **现实映射**：第六幕的 3–4 类人群是否覆盖了本章对应的精神痛点？
- [ ] **故事闭环**：第七幕是否回到了第一幕的具体人物，并形成了“细节回响”？
- [ ] **可执行**：第八幕的 3 条践行，是否一条 1 分钟级、一条 7 天级、一条 21 天级？

---

**下一章预告**：下一章主题预告。
"""
    return new_content

def process_volume(volume_dir):
    """处理指定卷目录下的所有章节文件"""
    for root, dirs, files in os.walk(volume_dir):
        for file in files:
            if file.endswith('.md') and file != 'README.md':
                filepath = os.path.join(root, file)
                print(f"处理文件: {filepath}")
                with open(filepath, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                title, core_poem, pain_point = extract_info(content)
                print(f"  标题: {title}")
                print(f"  核心诗词: {core_poem}")
                print(f"  精神痛点: {pain_point}")
                
                new_content = generate_new_content(title, core_poem, pain_point, file)
                
                # 备份原文件（可选）
                # backup_path = filepath + '.bak'
                # with open(backup_path, 'w', encoding='utf-8') as f:
                #     f.write(content)
                
                # 写入新内容
                with open(filepath, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                
                print(f"  已更新")

def main():
    base_dir = '/Users/yc/YCBookBlog/09.出版作品的书籍/06.品读唐诗宋词'
    
    # 处理所有卷（从第二卷开始，第一卷已处理）
    volumes = [
        '02.少年立志',
        '03.看见自我',
        '04.直面困境',
        '05.千里乡愁',
        '06.人生和解',
        '07.安顿内心',
        '08.山河远方',
        '09.超越世俗',
        '10.活在当下'
    ]
    
    for volume in volumes:
        volume_dir = os.path.join(base_dir, volume)
        if os.path.exists(volume_dir):
            print(f"\n处理卷: {volume}")
            process_volume(volume_dir)
        else:
            print(f"目录不存在: {volume_dir}")

if __name__ == '__main__':
    main()