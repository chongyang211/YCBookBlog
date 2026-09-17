// Package aggregate provides generic counting containers used by the pipeline.
package aggregate

// Counter increments per-key counts. K must be a comparable type.
type Counter[K comparable] struct {
	m map[K]uint64
}

// NewCounter returns an empty Counter with a reasonable initial capacity.
func NewCounter[K comparable]() *Counter[K] {
	return &Counter[K]{m: make(map[K]uint64, 1024)}
}

// Inc increments the count for k by one.
func (c *Counter[K]) Inc(k K) { c.m[k]++ }

// Add increments the count for k by n.
func (c *Counter[K]) Add(k K, n uint64) { c.m[k] += n }

// Total returns the sum of all counts.
func (c *Counter[K]) Total() uint64 {
	var sum uint64
	for _, v := range c.m {
		sum += v
	}
	return sum
}

// Len returns the number of distinct keys.
func (c *Counter[K]) Len() int { return len(c.m) }

// Range iterates over all key/value pairs until fn returns false.
func (c *Counter[K]) Range(fn func(k K, v uint64) bool) {
	for k, v := range c.m {
		if !fn(k, v) {
			return
		}
	}
}
