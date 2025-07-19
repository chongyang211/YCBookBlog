# 专栏笔记总结大全



### **4. 使用 `Repeater`**


---

### **5. 使用 `ListView` 或 `GridView`**

`ListView` 和 `GridView` 是用于动态生成列表或网格布局的组件，通常与模型（`model`）结合使用。

#### **示例：动态生成列表**

```javascript
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    // 使用 ListView 动态生成列表
    ListView {
        width: 200
        height: 300
        model: ["Item 1", "Item 2", "Item 3"] // 数据模型
        delegate: Text { text: modelData } // 委托组件
    }
}
```

- **优点**：适合生成列表或网格布局。
- **缺点**：组件类型必须相同。

---

### **6. 使用 JavaScript 动态创建对象**

在 QML 中，可以直接使用 JavaScript 动态创建对象并添加到父对象中。

#### **示例：动态创建多个矩形**

```javascript
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Component.onCompleted: {
        // 使用 JavaScript 动态创建多个矩形
        for (var i = 0; i < 5; i++) {
            var rect = Qt.createQmlObject(`
                import QtQuick 2.15
                Rectangle {
                    width: 50
                    height: 50
                    color: "green"
                    x: ${i * 60}
                    y: 100
                }
            `, this, "dynamicRect" + i);
        }
    }
}
```

- **优点**：灵活，适合复杂场景。
- **缺点**：性能较差。

---

### **总结**

| 方法                  | 适用场景                           | 优点                     | 缺点                     |
|-----------------------|------------------------------------|--------------------------|--------------------------|
| `Qt.createQmlObject`  | 从字符串动态创建对象               | 灵活                     | 性能较差                 |
| `Component`           | 需要频繁创建对象的场景             | 性能较好                 | 需要提前定义模板         |
| `Loader`              | 动态加载和切换组件                 | 适合动态切换组件         | 只能加载一个组件         |
| `Repeater`            | 生成多个相同类型的组件             | 简单易用                 | 组件类型必须相同         |
| `ListView`/`GridView` | 生成列表或网格布局                 | 适合列表或网格布局       | 组件类型必须相同         |
| JavaScript            | 复杂场景下的动态创建               | 灵活                     | 性能较差                 |

根据具体需求选择合适的方法，可以高效地实现 QML 中的动态创建功能。


在 **QML** 中，`Qt.createQmlObject` 是一个用于动态创建 QML 对象的函数。它允许在运行时从字符串形式的 QML 代码创建对象，并将其添加到 QML 场景中。以下是关于 `Qt.createQmlObject` 的详细说明、用法和注意事项。

---

### **函数签名**

```javascript
object Qt.createQmlObject(string qml, object parent, string filepath)
```

- **`qml`**：字符串形式的 QML 代码。
- **`parent`**：新创建对象的父对象。
- **`filepath`**（可选）：用于调试的源文件路径，通常设置为创建 QML 代码的文件路径。

---

### **功能说明**

1. **动态创建对象**：
    - 通过字符串形式的 QML 代码动态创建对象。
    - 适用于需要在运行时动态生成 UI 组件或逻辑的场景。

2. **指定父对象**：
    - 新创建的对象需要指定一个父对象，用于管理其生命周期。
    - 父对象销毁时，新创建的对象也会自动销毁。

3. **调试支持**：
    - 通过 `filepath` 参数指定源文件路径，便于调试时定位问题。

---

### **使用示例**

#### **示例 1：动态创建一个矩形**

```javascript
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Component.onCompleted: {
        // QML 代码字符串
        var qmlCode = `
            import QtQuick 2.15
            Rectangle {
                width: 100
                height: 100
                color: "red"
            }
        `;

        // 动态创建对象并添加到窗口中
        var newObject = Qt.createQmlObject(qmlCode, this, "dynamicRect");
        console.log("动态创建的对象:", newObject);
    }
}
```

- **说明**：
    - 在窗口加载完成后，动态创建一个红色的矩形，并将其添加到窗口中。
    - `this` 是父对象，表示新创建的矩形的父对象是当前窗口。

---

#### **示例 2：动态创建一个按钮并绑定事件**

```javascript
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Component.onCompleted: {
        // QML 代码字符串
        var qmlCode = `
            import QtQuick.Controls 2.15
            Button {
                text: "点击我"
                onClicked: {
                    console.log("按钮被点击了！");
                }
            }
        `;

        // 动态创建对象并添加到窗口中
        var newObject = Qt.createQmlObject(qmlCode, this, "dynamicButton");
        newObject.x = 150; // 设置按钮位置
        newObject.y = 100;
    }
}
```

- **说明**：
    - 动态创建一个按钮，并绑定点击事件。
    - 设置按钮的位置，并将其添加到窗口中。

---

### **注意事项**

1. **性能问题**：
    - 频繁使用 `Qt.createQmlObject` 可能会影响性能，尤其是在创建复杂对象时。
    - 对于需要频繁创建的对象，建议使用 `Component` 或 `Loader`。

2. **作用域问题**：
    - 动态创建的对象的作用域受限于其父对象。
    - 如果父对象被销毁，动态创建的对象也会被销毁。

3. **错误处理**：
    - 如果 QML 代码字符串有语法错误，`Qt.createQmlObject` 会抛出异常。
    - 建议使用 `try-catch` 捕获异常。

   ```javascript
   try {
       var newObject = Qt.createQmlObject(qmlCode, parent, "dynamicObject");
   } catch (e) {
       console.error("创建对象失败:", e);
   }
   ```

4. **调试支持**：
    - 通过 `filepath` 参数指定源文件路径，便于调试时定位问题。

---

### **替代方案**

1. **`Component` + `createObject`**：
    - 使用 `Component` 定义模板，然后通过 `createObject` 动态创建对象。
    - 性能优于 `Qt.createQmlObject`，适合需要频繁创建对象的场景。

   ```javascript
   Component {
       id: buttonComponent
       Button {
           text: "点击我"
           onClicked: console.log("按钮被点击了！");
       }
   }

   var newObject = buttonComponent.createObject(parent, { x: 150, y: 100 });
   ```

2. **`Loader`**：
    - 使用 `Loader` 动态加载 QML 组件。
    - 适合需要动态切换 UI 的场景。

   ```javascript
   Loader {
       id: dynamicLoader
       sourceComponent: buttonComponent
       x: 150
       y: 100
   }
   ```

---

### **总结**

`Qt.createQmlObject` 是 QML 中用于动态创建对象的函数，适用于需要在运行时从字符串形式的 QML 代码生成对象的场景。虽然它非常灵活，但在性能要求较高的场景中，建议使用 `Component` 或 `Loader` 作为替代方案。使用时需注意作用域、错误处理和性能问题。




在 Qt Quick/QML 中，`Keys` 附加属性用于处理键盘按键事件。通过 `Keys`，可以监听和处理用户按下、释放或长按键盘按键的操作。以下是 `Keys` 的详细使用方法：

---

### 1. **基本用法**
`Keys` 是一个附加属性，通常与 `Item` 或 `FocusScope` 一起使用。它提供了以下常用事件处理器：
- **`onPressed`**：当按键按下时触发。
- **`onReleased`**：当按键释放时触发。
- **`onShortcutOverride`**：当按键事件可能被系统快捷键覆盖时触发。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"
        focus: true // 必须设置焦点才能接收按键事件

        Keys.onPressed: {
            console.log("按键按下:", event.key);
            if (event.key === Qt.Key_Return) {
                console.log("回车键按下");
            }
        }

        Keys.onReleased: {
            console.log("按键释放:", event.key);
        }
    }
}
```

---

### 2. **常用按键常量**
Qt 提供了一系列按键常量，用于识别特定的按键。以下是一些常用的按键常量：
- **`Qt.Key_Return`**：回车键。
- **`Qt.Key_Enter`**：小键盘上的回车键。
- **`Qt.Key_Escape`**：Esc 键。
- **`Qt.Key_Space`**：空格键。
- **`Qt.Key_Backspace`**：退格键。
- **`Qt.Key_Delete`**：删除键。
- **`Qt.Key_Left`**：左箭头键。
- **`Qt.Key_Right`**：右箭头键。
- **`Qt.Key_Up`**：上箭头键。
- **`Qt.Key_Down`**：下箭头键。
- **`Qt.Key_A` 到 `Qt.Key_Z`**：字母键。
- **`Qt.Key_0` 到 `Qt.Key_9`**：数字键。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightgreen"
        focus: true

        Keys.onPressed: {
            if (event.key === Qt.Key_Left) {
                console.log("左箭头键按下");
            } else if (event.key === Qt.Key_Right) {
                console.log("右箭头键按下");
            } else if (event.key === Qt.Key_Space) {
                console.log("空格键按下");
            }
        }
    }
}
```

---

### 3. **处理组合键**
可以通过 `event.modifiers` 属性检测是否按下了修饰键（如 `Ctrl`、`Shift`、`Alt` 等），从而实现组合键的功能。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightcoral"
        focus: true

        Keys.onPressed: {
            if ((event.key === Qt.Key_S) && (event.modifiers & Qt.ControlModifier)) {
                console.log("Ctrl + S 按下");
            } else if ((event.key === Qt.Key_Q) && (event.modifiers & Qt.AltModifier)) {
                console.log("Alt + Q 按下");
            }
        }
    }
}
```

---

### 4. **阻止事件传播**
默认情况下，按键事件会向上传播到父组件。可以通过 `event.accepted = true` 阻止事件传播。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"
        focus: true

        Keys.onPressed: {
            if (event.key === Qt.Key_Space) {
                console.log("空格键按下，事件被阻止");
                event.accepted = true; // 阻止事件传播
            }
        }
    }
}
```

---

### 5. **处理快捷键覆盖**
`onShortcutOverride` 事件处理器可以用于处理可能被系统快捷键覆盖的按键事件。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightyellow"
        focus: true

        Keys.onShortcutOverride: {
            if (event.key === Qt.Key_Space) {
                console.log("空格键被覆盖");
                event.accepted = true; // 阻止系统快捷键
            }
        }
    }
}
```

---

### 6. **动态设置焦点**
通过 `focus` 属性可以动态设置组件是否接收按键事件。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        id: rect1
        width: 100
        height: 100
        color: "lightblue"
        focus: true

        Keys.onPressed: {
            console.log("矩形 1 接收到按键:", event.key);
        }
    }

    Rectangle {
        id: rect2
        width: 100
        height: 100
        color: "lightgreen"
        anchors.right: parent.right

        Keys.onPressed: {
            console.log("矩形 2 接收到按键:", event.key);
        }
    }

    Button {
        text: "切换焦点"
        anchors.bottom: parent.bottom
        onClicked: {
            rect1.focus = !rect1.focus;
            rect2.focus = !rect2.focus;
        }
    }
}
```

---

### 7. **注意事项**
- **焦点管理**：只有设置了 `focus: true` 的组件才能接收按键事件。
- **事件传播**：按键事件会向上传播到父组件，除非通过 `event.accepted = true` 阻止传播。
- **平台差异**：某些按键在不同平台上的行为可能有所不同，需要根据具体平台进行测试。

---

### 总结
`Keys` 是 Qt Quick/QML 中用于处理键盘按键事件的强大工具。通过它，可以监听按键按下、释放和组合键操作，同时控制事件传播和焦点管理。合理使用 `Keys` 可以实现丰富的键盘交互功能。


————————————————————————————————————————————————————————————————————————————————————————————————————————————

在 Qt Quick/QML 中，`MouseArea` 是一个用于处理鼠标（或触摸）事件的组件。它可以附加到任何可视元素（如 `Rectangle`、`Image` 等），用于监听鼠标点击、拖动、悬停等操作。以下是 `MouseArea` 的详细使用方法：

---

### 1. **基本用法**
`MouseArea` 提供了以下常用属性和信号处理器：

#### 属性：
- **`enabled`**：是否启用 `MouseArea`（默认为 `true`）。
- **`hoverEnabled`**：是否启用悬停事件（默认为 `false`）。
- **`acceptedButtons`**：指定接收哪些鼠标按钮事件（如 `Qt.LeftButton`、`Qt.RightButton`）。
- **`pressed`**：鼠标是否按下（`true` 表示按下，`false` 表示释放）。
- **`containsMouse`**：鼠标是否在 `MouseArea` 区域内。

#### 信号处理器：
- **`onClicked`**：鼠标点击时触发。
- **`onDoubleClicked`**：鼠标双击时触发。
- **`onPressed`**：鼠标按下时触发。
- **`onReleased`**：鼠标释放时触发。
- **`onEntered`**：鼠标进入 `MouseArea` 区域时触发。
- **`onExited`**：鼠标离开 `MouseArea` 区域时触发。
- **`onPositionChanged`**：鼠标在 `MouseArea` 区域内移动时触发。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"

        MouseArea {
            anchors.fill: parent
            onClicked: console.log("矩形被点击")
            onDoubleClicked: console.log("矩形被双击")
            onPressed: console.log("鼠标按下")
            onReleased: console.log("鼠标释放")
        }
    }
}
```

---

### 2. **处理鼠标点击**
通过 `onClicked` 信号处理器可以监听鼠标点击事件。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightgreen"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("点击位置:", mouse.x, mouse.y);
                parent.color = "lightcoral"; // 改变矩形颜色
            }
        }
    }
}
```

---

### 3. **处理鼠标悬停**
通过 `hoverEnabled` 属性和 `onEntered`、`onExited` 信号处理器可以监听鼠标悬停事件。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "lightgreen" // 鼠标进入时改变颜色
            onExited: parent.color = "lightblue"   // 鼠标离开时恢复颜色
        }
    }
}
```

---

### 4. **处理鼠标拖动**
通过 `onPositionChanged` 信号处理器可以监听鼠标拖动事件。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 100
        height: 100
        color: "lightcoral"

        MouseArea {
            anchors.fill: parent
            drag.target: parent // 允许拖动矩形
            onPositionChanged: {
                console.log("拖动位置:", parent.x, parent.y);
            }
        }
    }
}
```

---

### 5. **处理鼠标按钮**
通过 `acceptedButtons` 属性和 `onPressed`、`onReleased` 信号处理器可以区分鼠标左键、右键等。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressed: {
                if (mouse.button === Qt.LeftButton) {
                    console.log("左键按下");
                } else if (mouse.button === Qt.RightButton) {
                    console.log("右键按下");
                }
            }
        }
    }
}
```

---

### 6. **阻止事件传播**
默认情况下，鼠标事件会向上传播到父组件。可以通过 `mouse.accepted = false` 阻止事件传播。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("子区域点击");
                mouse.accepted = false; // 阻止事件传播
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("父区域点击");
    }
}
```

---

### 7. **动态启用/禁用**
通过 `enabled` 属性可以动态启用或禁用 `MouseArea`。

#### 示例：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 300

    Rectangle {
        width: 200
        height: 100
        color: "lightblue"

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: console.log("矩形被点击")
        }
    }

    Button {
        text: "切换启用状态"
        anchors.bottom: parent.bottom
        onClicked: mouseArea.enabled = !mouseArea.enabled
    }
}
```

---

### 8. **注意事项**
- **触摸事件**：在触摸屏设备上，`MouseArea` 也可以处理触摸事件。
- **性能优化**：如果 `MouseArea` 的区域较大或数量较多，可能会影响性能，建议合理使用。
- **事件冲突**：如果多个 `MouseArea` 重叠，事件可能会被最上层的 `MouseArea` 捕获。

---

### 总结
`MouseArea` 是 Qt Quick/QML 中用于处理鼠标和触摸事件的核心组件。通过它，可以监听点击、悬停、拖动等操作，并实现丰富的交互功能。合理使用 `MouseArea` 可以提升用户体验，特别是在需要处理用户输入的场景中。



————————————————————————————————————————————————————————————————————————————————————————————————————————————

## 书籍

- Qt 5编程入门


4.11小结212
第5章图形动画基础213
5.1颜色、渐变和调色板213
5.1.1颜色213
5.1.2渐变216
5.1.3系统调色板216
5.2图片、边界图片和动态图片217
5.2.1图片217
5.2.2边界图片219
5.2.3动态图片221
5.3缩放、旋转和平移变换221
5.3.1使用属性实现简单变换221
5.3.2使用Transform实现高级变换223
5.4状态224
5.4.1创建状态225
5.4.2默认状态和when属性226
5.5动画和过渡227
5.5.1使用属性动画227
5.5.2使用预定义的目标和属性229
5.5.3在状态改变时使用过渡230
5.5.4使用默认的行为动画231
5.5.5使用并行或顺序动画组232
5.5.6使用动画师动画233
5.5.7控制动画的执行234
5.5.8共享动画实例235
5.6精灵动画236
5.6.1精灵引擎介绍236
5.6.2AnimatedSprite240
5.7Flickable和Flipable241
5.7.1弹动效果241
5.7.2翻转效果243
5.8小结244
第6章图形效果245
6.1混合效果245
6.2颜色效果248
6.3渐变效果251
6.3.1锥形渐变251
6.3.2线性渐变252
6.3.3辐射渐变253
6.4变形效果254
6.5阴影效果255
6.5.1投影255
6.5.2内阴影256
6.6模糊效果257
6.6.1快速模糊257
6.6.2高斯模糊258
6.6.3递归模糊259
6.6.4遮罩模糊260
6.7动感模糊效果261
6.7.1方向模糊261
6.7.2径向模糊262
6.7.3缩放模糊263
6.8发光效果263
6.8.1发光263
6.8.2矩形发光264
6.9遮罩效果266
6.9.1不透明遮罩266
6.9.2阈值遮罩267
6.10小结268
第7章粒子系统269
7.1ParticleSystem269
7.2发射器271
7.3渲染器272
7.4TrailEmitter275
7.5粒子组276
7.6随机参数278
7.7影响器279
7.7.1Age279
7.7.2Attractor280
7.7.3Friction281
7.7.4Gravity281
7.7.5Turbulence282
7.7.6Wander282
7.7.7GroupGoal282
7.7.8SpriteGoal283
7.8小结284


第10章Qt图表346
10.1创建一个图表项目347
10.2坐标轴Axes348
10.2.1数值坐标轴和对数数值坐标轴349
10.2.2分类坐标轴350
10.2.3柱形图分类坐标轴351
10.2.4日期时间坐标轴352
10.2.5坐标轴的共有属性354
10.3图例Legend354
10.4ChartView356
10.4.1设置主题357
10.4.2启用动画358
10.5使用外部数据动态创建图表359
10.6常用图表类型362
10.6.1折线图、曲线图和散点图362
10.6.2面积图364
10.6.3柱形图、堆积柱形图、百分比堆积柱形图365
10.6.4饼状图366
10.6.5盒须图（箱形图）367
10.6.6蜡烛图（K线图）368
10.6.7极坐标图369
10.7小结371
第11章Qt Data Visualization数据可视化372
11.13D柱形图372
11.1.13D坐标轴375
11.1.2数据代理377
11.1.33D系列380
11.2自定义3D场景381
11.3设置主题382
11.4选择模式和切片视图385
11.53D散点图387
11.63D曲面图389
11.7小结391
第12章多媒体应用392
12.1多媒体模块介绍392
12.2播放音频393
12.2.1播放压缩音频393
12.2.2播放未压缩音频395
12.3播放视频396
12.3.1使用Video播放视频文件396
12.3.2对视频使用图形效果397
12.4媒体播放器399
12.4.1播放音视频399
12.4.2使用Windows平台附加功能400
12.5小结406
第13章QML与C++的集成407
13.1快速选择正确的集成方式408
13.2QML运行时的C++类411
13.2.1QQmlEngine411
13.2.2QQmlContext412
13.2.3QQmlComponent414
13.2.4QQmlExpression415
13.3在QML中使用C++特性416
13.3.1数据类型处理和拥有权416
13.3.2数据类型的转换416
13.3.3使用C++属性419
13.3.4使用函数和槽423
13.3.5使用信号425
13.4注册QML类型426
13.4.1注册可实例化对象类型426
13.4.2注册不可实例化对象类型427
13.4.3注册单例类型427
13.4.4类型的修订和版本428
13.5定义QML特定类型和属性429
13.5.1提供附加对象注解数据429
13.5.2属性修饰符类型432
13.5.3指定QML对象类型的默认属性434
13.5.4接收对象初始化通知435
13.6在C++中使用QML对象436
13.6.1使用C++加载QML对象436
13.6.2使用对象名字访问加载的QML对象438
13.6.3使用C++访问QML对象成员439
13.7小结442
第14章使用Qt Quick设计器443
14.1Qt Quick UI表单文件444
14.2常用操作448
14.2.1管理项目层次448
14.2.2QML类型库449
14.2.3指定项目属性449
14.2.4添加状态452
14.2.5在画布上操作QML类型452
14.3使用设计器编辑QML文档457
14.3.1创建项目457
14.3.2创建主视图458
14.3.3添加视图461
14.3.4为视图添加动画462
14.4小结464
第15章Qt移动开发入门465
15.1Qt Android开发配置465
15.1.1工具软件的下载465
15.1.2工具软件的安装466
15.1.3使用虚拟机运行Qt Android程序468
15.1.4使用手机运行Qt Android程序471
15.1.5发布项目472
15.2Qt iOS开发配置476
15.2.1使用模拟器运行Qt iOS程序477
15.2.2使用手机运行Qt iOS程序478
15.3创建演示程序481
15.3.1创建应用主窗口481
15.3.2拍照功能486
15.3.3多点触控488
15.3.4传感器489
15.3.5地图491
15.3.6蓝牙493