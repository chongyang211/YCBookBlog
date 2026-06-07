---
title: O003-只出现一次的数字III
date: 2025-06-07 10:00:00
permalink: /pages/algo200-o003/
categories: [book, 通用技术的提升, 数据结构算法, 算法考核, 位运算]
tags: [算法, 位运算, 异或, LeetCode]
author: {name: 杨充, link: https://github.com/yangchong211}
---
# O003 只出现一次的数字 III

> LeetCode 260 · ⭐⭐ · 异或+分组

## 01. 题目

`[1,2,1,3,2,5]` → `[3,5]`。两个出现一次，其余两次。

## 02. 分析

全体异或 = a⊕b。取 `mask = xor & -xor`（最低位1）。根据 mask 将数组分为两组，每组各含一个目标数字。

## 03. 代码

```java
public int[] singleNumber(int[] nums) {
    int xor=0; for(int n:nums) xor^=n;
    int mask=xor&-xor, a=0;
    for(int n:nums) if((n&mask)==0) a^=n;
    return new int[]{a, xor^a};
}
```

**Python**：
```python
def singleNumber(self, nums):
    xor = reduce(xor, nums)
    mask = xor & -xor
    a = reduce(xor, (n for n in nums if n & mask))
    return [a, xor ^ a]
```

**复杂度**：O(N)/O(1)。

**相关**：[O001 出现一次](O001-只出现一次的数字.md)
