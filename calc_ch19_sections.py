#!/usr/bin/env python3
import re

def count_chinese_chars(text):
    # 统计中文字符（Unicode范围），排除标点、空格、换行
    # 中文字符范围：\u4e00-\u9fff
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', text)
    return len(chinese_chars)

def main():
    with open('/Users/yc/YCBookBlog/ch19_body_split.md', 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # 寻找划分点
    # 当下入口：从开头到“然后，回忆像潮水一样漫上来”之前
    # 回忆主体：从“然后，回忆像潮水一样漫上来”到“他烧了第一张欠条。剩下的，还有二十六张。”之后？
    # 当下出口：从回忆结束回到当下场景开始
    # 根据内容特征划分
    text = ''.join(lines)
    paragraphs = text.split('\n\n')
    print(f"总段落数: {len(paragraphs)}")
    
    # 寻找关键词
    entry_end = -1
    recall_start = -1
    recall_end = -1
    for i, para in enumerate(paragraphs):
        if '回忆像潮水一样漫上来' in para:
            recall_start = i
            entry_end = i - 1
            break
    
    # 寻找回忆结束，当下出口开始：可能从“他烧了第一张欠条”之后开始
    for i in range(recall_start, len(paragraphs)):
        if '他烧了第一张欠条' in paragraphs[i]:
            # 可能这一句是回忆的结尾，当下出口从下一段开始
            recall_end = i
            break
    
    if recall_end == -1:
        # 如果没有找到，则假设最后几段是当下出口
        recall_end = len(paragraphs) - 5  # 估计最后5段是出口
    
    print(f"当下入口段落: 0-{entry_end}")
    print(f"回忆主体段落: {recall_start}-{recall_end}")
    print(f"当下出口段落: {recall_end+1}-{len(paragraphs)-1}")
    
    # 计算各部分字数
    entry_text = '\n\n'.join(paragraphs[0:entry_end+1])
    recall_text = '\n\n'.join(paragraphs[recall_start:recall_end+1])
    exit_text = '\n\n'.join(paragraphs[recall_end+1:])
    
    entry_chars = count_chinese_chars(entry_text)
    recall_chars = count_chinese_chars(recall_text)
    exit_chars = count_chinese_chars(exit_text)
    total_chars = entry_chars + recall_chars + exit_chars
    
    print(f"当下入口字数: {entry_chars}")
    print(f"回忆主体字数: {recall_chars}")
    print(f"当下出口字数: {exit_chars}")
    print(f"总字数: {total_chars}")
    print(f"比例: 入口 {entry_chars/total_chars*100:.1f}%, 主体 {recall_chars/total_chars*100:.1f}%, 出口 {exit_chars/total_chars*100:.1f}%")
    
    # 检查是否符合T3比例要求
    entry_ratio = entry_chars / total_chars
    recall_ratio = recall_chars / total_chars
    exit_ratio = exit_chars / total_chars
    
    print(f"\nT3比例要求: 入口8-12% ({0.08:.1%}-{0.12:.1%}), 主体70-76% ({0.70:.1%}-{0.76:.1%}), 出口12-18% ({0.12:.1%}-{0.18:.1%})")
    print(f"实际比例: 入口{entry_ratio:.1%}, 主体{recall_ratio:.1%}, 出口{exit_ratio:.1%}")
    
    if 0.08 <= entry_ratio <= 0.12 and 0.70 <= recall_ratio <= 0.76 and 0.12 <= exit_ratio <= 0.18:
        print("✅ 符合T3比例要求")
    else:
        print("❌ 不符合T3比例要求")
    
    return entry_chars, recall_chars, exit_chars, total_chars

if __name__ == '__main__':
    entry, recall, exit, total = main()