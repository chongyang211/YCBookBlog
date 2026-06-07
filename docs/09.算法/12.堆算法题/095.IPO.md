---
title: F010-IPO
date: 2025-06-07 10:00:00
permalink: /pages/algo200-f010/
categories: [book, 通用技术的提升, 数据结构算法, 算法考核, 堆与优先队列]
tags: [算法, 堆, 贪心, LeetCode]
author: {name: 杨充, link: https://github.com/yangchong211}
---
# F010 IPO（最大资本）

> LeetCode 502 · ⭐⭐⭐ · 贪心+双堆

## 01. 题目

初始资本 w，最多选 k 个项目，每个项目有成本 c 和利润 p。资本足够时可选，利润加入资本。最大化最终资本。

## 02. 代码

**Java**：
```java
public int findMaximizedCapital(int k, int w, int[] p, int[] c) {
    int n=p.length; int[][] arr=new int[n][2];
    for(int i=0;i<n;i++) arr[i]=new int[]{c[i],p[i]};
    Arrays.sort(arr,(a,b)->a[0]-b[0]);
    PriorityQueue<Integer> pq=new PriorityQueue<>(Collections.reverseOrder());
    int i=0;
    while(k-->0){ while(i<n&&arr[i][0]<=w) pq.offer(arr[i++][1]); if(pq.isEmpty()) break; w+=pq.poll(); }
    return w;
}
```

**Python**：
```python
def findMaximizedCapital(self, k, w, profits, capital):
    import heapq
    arr=sorted(zip(capital,profits))
    pq=[]; i=0
    for _ in range(k):
        while i<len(arr) and arr[i][0]<=w: heapq.heappush(pq,-arr[i][1]); i+=1
        if not pq: break
        w+=-heapq.heappop(pq)
    return w
```

**复杂度**：O(NlogN)/O(N)。
