package transport

import "testing"

func TestSeqWrapAround(t *testing.T) {
	// 环绕前后比较
	a := TCPSeq(0xFFFFFFF0)
	b := TCPSeq(0x00000010) // b 是 a 之后(逻辑上)

	if !a.LT(b) {
		t.Error("a < b should hold around wrap-around")
	}
	if !b.GT(a) {
		t.Error("b > a should hold around wrap-around")
	}
	if a.GE(b) {
		t.Error("a >= b should NOT hold")
	}
}

func TestSeqAddSub(t *testing.T) {
	a := TCPSeq(100)
	b := a.Add(50)
	if b != 150 {
		t.Errorf("Add: %d", b)
	}
	if b.Sub(a) != 50 {
		t.Errorf("Sub: %d", b.Sub(a))
	}

	// 环绕加法
	c := TCPSeq(0xFFFFFFFF)
	d := c.Add(2)
	if d != 1 {
		t.Errorf("wrap add: %d", d)
	}
}

func TestSeqBetween(t *testing.T) {
	// (lo, hi] 区间
	lo, hi := TCPSeq(100), TCPSeq(200)
	cases := []struct {
		s    TCPSeq
		want bool
	}{
		{100, false}, // 左开
		{101, true},
		{150, true},
		{200, true}, // 右闭
		{201, false},
		{99, false},
	}
	for _, c := range cases {
		if got := c.s.Between(lo, hi); got != c.want {
			t.Errorf("%d.Between(%d,%d): got %v want %v",
				c.s, lo, hi, got, c.want)
		}
	}
}
