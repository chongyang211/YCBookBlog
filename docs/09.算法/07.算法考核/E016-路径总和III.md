---
title: E016-路径总和III
date: 2025-06-07 10:00:00
permalink: /pages/algo200-e016/
categories: [book, 通用技术的提升, 数据结构算法, 算法考核, 树与二叉树]
tags: [算法, 二叉树, 前缀和, DFS, 回溯, LeetCode]
author: {name: 杨充, link: https://github.com/yangchong211}
---
# E016 路径总和 III

> LeetCode 437 · ⭐⭐ · 前缀和+DFS回溯

## 01. 题目

统计二叉树中路径和等于 targetSum 的路径数。路径不需要从根开始，也不需要到叶子结束，但必须向下。

## 02. 分析

前缀和 `currSum - targetSum` 在之前路径中出现过多少次，就有多少条以当前节点结尾的有效路径。

## 03. 代码

**Java**：
```java
public int pathSum(TreeNode root, int targetSum) {
    Map<Long, Integer> prefix = new HashMap<>();
    prefix.put(0L, 1);
    return dfs(root, 0, targetSum, prefix);
}
int dfs(TreeNode node, long sum, int target, Map<Long, Integer> prefix) {
    if (node == null) return 0;
    sum += node.val;
    int count = prefix.getOrDefault(sum - target, 0);
    prefix.merge(sum, 1, Integer::sum);
    count += dfs(node.left, sum, target, prefix);
    count += dfs(node.right, sum, target, prefix);
    prefix.merge(sum, -1, Integer::sum);
    return count;
}
```

**Python**：
```python
def pathSum(self, root, targetSum):
    prefix = {0: 1}
    def dfs(node, s):
        if not node: return 0
        s += node.val
        cnt = prefix.get(s - targetSum, 0)
        prefix[s] = prefix.get(s, 0) + 1
        cnt += dfs(node.left, s) + dfs(node.right, s)
        prefix[s] -= 1
        return cnt
    return dfs(root, 0)
```

**复杂度**：O(N)/O(N)。
