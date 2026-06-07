---
title: N004-打家劫舍III
date: 2025-06-07 10:00:00
permalink: /pages/algo200-n004/
categories: [book, 通用技术的提升, 数据结构算法, 算法考核, 动态规划]
tags: [算法, DP, 树形DP, LeetCode]
author: {name: 杨充, link: https://github.com/yangchong211}
---
# N004 打家劫舍 III（树形DP）

> LeetCode 337 · ⭐⭐ · 树形DP

## 01. 题目

二叉树形式排列房屋，不能偷直接相连的两家（父子关系）。

## 02. 题目分析

对每个节点返回 `[不偷, 偷]` 两种状态：
- 不偷：`max(L[0],L[1]) + max(R[0],R[1])`
- 偷：`node.val + L[0] + R[0]`

## 03. 代码

**Java**：
```java
public int rob(TreeNode root) {
    int[] res = dfs(root); return Math.max(res[0], res[1]);
}
int[] dfs(TreeNode node) { // [不偷, 偷]
    if (node == null) return new int[]{0, 0};
    int[] L = dfs(node.left), R = dfs(node.right);
    int notRob = Math.max(L[0], L[1]) + Math.max(R[0], R[1]);
    int rob = node.val + L[0] + R[0];
    return new int[]{notRob, rob};
}
```

**Python**：
```python
def rob(self, root):
    def dfs(node):
        if not node: return (0, 0)
        L, R = dfs(node.left), dfs(node.right)
        return (max(L)+max(R), node.val+L[0]+R[0])
    return max(dfs(root))
```

**C++**：
```cpp
int rob(TreeNode* root) {
    auto [notRob, rob] = dfs(root);
    return max(notRob, rob);
}
pair<int,int> dfs(TreeNode* node) {
    if (!node) return {0,0};
    auto [l0,l1] = dfs(node->left), [r0,r1] = dfs(node->right);
    return {max(l0,l1)+max(r0,r1), node->val+l0+r0};
}
```

**复杂度**：O(N)/O(H)。

**三题对比**：线性 → 环形（两次线性）→ 树形（自底向上返回双状态）。同一条 DP 思路的递进。

**相关**：[N002 打家劫舍](N002-打家劫舍.md)、[N003 打家劫舍II](N003-打家劫舍II.md)
