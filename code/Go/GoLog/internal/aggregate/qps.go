package aggregate

import (
	"sort"
	"time"
)

// QPSBuckets aggregates request counts per time bucket.
type QPSBuckets struct {
	bucketSec int64
	m         map[int64]uint64 // unix-ts of bucket start -> count
}

// NewQPSBuckets returns a bucket aggregator with the given bucket size in
// seconds (clamped to >= 1).
func NewQPSBuckets(bucketSec int64) *QPSBuckets {
	if bucketSec <= 0 {
		bucketSec = 1
	}
	return &QPSBuckets{bucketSec: bucketSec, m: make(map[int64]uint64, 4096)}
}

// Add records one request at time t.
func (q *QPSBuckets) Add(t time.Time) {
	bucket := t.Unix() / q.bucketSec * q.bucketSec
	q.m[bucket]++
}

// Sorted returns (timestamp, count) tuples in ascending time order.
func (q *QPSBuckets) Sorted() (ts []time.Time, counts []uint64) {
	keys := make([]int64, 0, len(q.m))
	for k := range q.m {
		keys = append(keys, k)
	}
	sort.Slice(keys, func(i, j int) bool { return keys[i] < keys[j] })
	ts = make([]time.Time, len(keys))
	counts = make([]uint64, len(keys))
	for i, k := range keys {
		ts[i] = time.Unix(k, 0)
		counts[i] = q.m[k]
	}
	return
}

// Peak returns the busiest bucket and its request count.
func (q *QPSBuckets) Peak() (peakTs time.Time, peakCount uint64) {
	for k, v := range q.m {
		if v > peakCount {
			peakCount = v
			peakTs = time.Unix(k, 0)
		}
	}
	return
}
