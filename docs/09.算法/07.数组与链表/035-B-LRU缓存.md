---
title: B015-LRU缓存
date: 2025-06-07 10:00:00
permalink: /pages/algo200-b015/
categories:
  - book
  - 通用技术的提升
  - 数据结构算法
  - 算法考核
  - 链表
tags:
  - 算法
  - 链表
  - 哈希表
  - LeetCode
author: 
  name: 杨充
  link: https://github.com/yangchong211
---
# B015 LRU 缓存

> LeetCode 146 · ⭐⭐ · 哈希+双向链表
>
> 面试最常考的"设计题"。HashMap 保证 O(1) 查找，双向链表保证 O(1) 插入/删除/移头。

---

## 01. 题目描述

设计 LRU（最近最少使用）缓存。支持 `get(key)` 和 `put(key, value)`，均 O(1)。

---

## 02. 代码

**Java**：

```java
class LRUCache {
    class Node { int key, val; Node prev, next; Node(int k, int v) { key=k; val=v; } }
    Map<Integer, Node> map = new HashMap<>();
    Node head, tail;
    int cap;

    public LRUCache(int capacity) {
        cap = capacity;
        head = new Node(0, 0); tail = new Node(0, 0);
        head.next = tail; tail.prev = head;
    }
    public int get(int key) {
        if (!map.containsKey(key)) return -1;
        Node node = map.get(key);
        remove(node); addHead(node);
        return node.val;
    }
    public void put(int key, int value) {
        if (map.containsKey(key)) { remove(map.get(key)); }
        else if (map.size() == cap) { map.remove(tail.prev.key); remove(tail.prev); }
        Node node = new Node(key, value);
        map.put(key, node); addHead(node);
    }
    void remove(Node node) { node.prev.next = node.next; node.next.prev = node.prev; }
    void addHead(Node node) { node.next = head.next; node.prev = head; head.next.prev = node; head.next = node; }
}
```

**Python**：

```python
class Node:
    def __init__(self, k=0, v=0): self.key, self.val = k, v

class LRUCache:
    def __init__(self, cap):
        self.cap = cap
        self.map = {}
        self.head = Node(); self.tail = Node()
        self.head.next = self.tail; self.tail.prev = self.head

    def get(self, key):
        if key not in self.map: return -1
        node = self.map[key]
        self._remove(node); self._add_head(node)
        return node.val

    def put(self, key, value):
        if key in self.map: self._remove(self.map[key])
        elif len(self.map) == self.cap:
            del self.map[self.tail.prev.key]
            self._remove(self.tail.prev)
        node = Node(key, value)
        self.map[key] = node; self._add_head(node)

    def _remove(self, node): node.prev.next = node.next; node.next.prev = node.prev
    def _add_head(self, node): node.next = self.head.next; node.prev = self.head; self.head.next.prev = node; self.head.next = node
```

**复杂度**：get/put 均 O(1)。
