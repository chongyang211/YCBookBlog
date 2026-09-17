package aggregate

import "container/heap"

// Item is one element in the TopK heap.
type Item[K any] struct {
	Key   K
	Count uint64
}

// minHeap is the internal type that satisfies heap.Interface.
type minHeap[K any] []Item[K]

func (h minHeap[K]) Len() int           { return len(h) }
func (h minHeap[K]) Less(i, j int) bool { return h[i].Count < h[j].Count }
func (h minHeap[K]) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *minHeap[K]) Push(x any)        { *h = append(*h, x.(Item[K])) }
func (h *minHeap[K]) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

// TopK keeps the K largest items by Count.
// 保留 Top K 大用最小堆：堆顶是当前 K 个里最小的，
// 新元素超过堆顶则替换并下沉。时间 O(n log K)。
type TopK[K any] struct {
	k int
	h *minHeap[K]
}

// NewTopK returns a TopK that keeps the k largest items.
func NewTopK[K any](k int) *TopK[K] {
	h := minHeap[K]{}
	return &TopK[K]{k: k, h: &h}
}

// Offer adds an item to the heap. Cheap when count < heap minimum.
func (t *TopK[K]) Offer(key K, count uint64) {
	if t.k <= 0 {
		return
	}
	if t.h.Len() < t.k {
		heap.Push(t.h, Item[K]{Key: key, Count: count})
		return
	}
	if count > (*t.h)[0].Count {
		(*t.h)[0] = Item[K]{Key: key, Count: count}
		heap.Fix(t.h, 0) // 替换堆顶后下沉，比 Pop+Push 快一倍
	}
}

// Result returns items sorted by Count desc.
func (t *TopK[K]) Result() []Item[K] {
	out := make([]Item[K], t.h.Len())
	copy(out, *t.h)
	// 简单冒泡降序——K 通常很小（≤ 100），不必上 sort
	for i := 0; i < len(out); i++ {
		for j := i + 1; j < len(out); j++ {
			if out[j].Count > out[i].Count {
				out[i], out[j] = out[j], out[i]
			}
		}
	}
	return out
}
