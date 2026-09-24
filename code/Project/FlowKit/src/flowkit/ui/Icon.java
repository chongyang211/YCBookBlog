package flowkit.ui;

/**
 * 享元 —— 内部状态（像素数据）不可变、可共享；
 * 外部状态（坐标、标签）每次渲染时当参数传。
 * 铁律：享元对象必须不可变，否则一个节点改色、全屏同款一起变色。
 */
public class Icon {
    private final byte[] pixels;        // 内部状态：不可变，可共享

    public Icon(byte[] pixels) { this.pixels = pixels; }

    public void render(int x, int y, String label) {   // 外部状态：每次传
        // 模拟前端画布绘制
        System.out.printf("[图标] %-8s @(%d,%d) %s%n", label, x, y, pixels.length + "B");
    }
}
