# 专栏笔记总结大全



## 01.CPU实体卡读写库

- 2.1 如何去使用CPU卡：非接触式CPU卡一般都是通过NFC，想用手机去感应身份证有两个前提：一你手机支持NFC功能，二你得知道对操作身份证的指令。
- 2.2 读卡器交互流程是怎样的：通用的步骤是命令发送和数据解析，大概就是，寻卡——>APDU通信——>关闭
- 3.1 读CPU卡流程是怎样的：一般是，选择目录——>选择文件——>取随机数——>认证文件密码——>读数据
- 3.2 写CPU卡流程是怎样的：一般是，选择目录——>选择文件——>取随机数——>认证文件密码——>写数据
- 4.1 如何理解APDU概念：APDU指令数据是在智能卡领域中用于通信的一种数据格式。APDU 指令数据通常用于在读取或写入智能卡时与卡片进行通信。
- 4.2 APDU指令类型有哪些：分为两种类型：命令指令（Command APDU）和响应指令（Response APDU）。命令指令用于向智能卡发送指令，请求执行某种操作；响应指令则是智能卡对命令指令的响应，包含执行结果或返回的数据。
- 4.3 APDU指令结构是怎样的：命令指令包含 CLA（类别）、INS（指令代码）、P1（参数1）、P2（参数2）、LC（数据长度）、Data 等字段；响应指令包含响应状态字（SW1-SW2）和响应数据等字段。




## 06.韦根信号设计和原理

- 1.1 了解韦根协议是什么：用于门禁系统和访问控制系统的数据传输协议。
- 1.2 硬件开发有哪些：主要是读卡器设计（包括韦根协议输出，支持卡片类型等），控制器设计（包括门禁控制逻辑，与其他设备通信接口）
- 2.2 Wiegand开发准备事项有哪些：要掌握韦根传输协议的格式比如26位还是34位，韦根接线操作，数据处理和验证
- 2.3 韦根Wiegand数据格式：起始位（Start Bit）、数据位（Data Bits）和校验位（Parity Bit）
- 2.4 如何理解校验位验证：计算并验证前校验位（Even Parity）和后校验位（Odd Parity）。主要是确保数据的正确性。



## 11.距离传感器设计实践

- 距离传感器的大概工作原理：红外线传感器使用红外线发射器和接收器来测量和计算物体与设备之间的距离。超声波传感器则通过发射和接收超声波信号来实现距离测量。




### **1. 继电器的工作原理**
继电器是一种电磁开关，通过控制线圈的通电和断电来切换触点的状态：
- **线圈通电**：触点闭合，电路接通。
- **线圈断电**：触点断开，电路断开。



---

### **2. 硬件连接**
以下是继电器与门禁系统的基本连接方式：
1. **继电器线圈**：连接到控制信号（如单片机、PLC 或 QML 控制的 GPIO 引脚）。
2. **继电器触点**：连接到电磁锁或电机的电源电路。

---

### **3. 使用 QML 控制继电器**
如果使用 QML 控制继电器，可以通过以下步骤实现：

#### **3.1 使用 GPIO 控制继电器**
假设你使用的是支持 GPIO 的设备（如树莓派），可以通过 QML 控制 GPIO 引脚的高低电平，从而控制继电器。

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Extras 1.4

ApplicationWindow {
    visible: true
    width: 300
    height: 200

    // 模拟 GPIO 控制
    property bool relayState: false

    Button {
        text: relayState ? "关门" : "开门"
        anchors.centerIn: parent
        onClicked: {
            relayState = !relayState
            controlRelay(relayState)
        }
    }

    function controlRelay(state) {
        if (state) {
            console.log("开门：继电器通电，电磁锁断电")
            // 在这里调用 GPIO 高电平操作
        } else {
            console.log("关门：继电器断电，电磁锁通电")
            // 在这里调用 GPIO 低电平操作
        }
    }
}
```

#### **3.2 使用串口或网络控制继电器**
如果继电器控制器支持串口或网络通信，可以通过 QML 发送控制指令。例如：
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

ApplicationWindow {
    visible: true
    width: 300
    height: 200

    Button {
        text: "开门"
        anchors.centerIn: parent
        onClicked: {
            sendCommand("OPEN")
        }
    }

    Button {
        text: "关门"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            sendCommand("CLOSE")
        }
    }

    function sendCommand(command) {
        // 在这里实现串口或网络通信，发送控制指令
        console.log("发送指令：" + command)
    }
}
```

---

### **4. 安全注意事项**
1. **电源隔离**：确保继电器与控制系统之间的电源隔离，避免干扰或损坏设备。
2. **电流匹配**：选择适合电磁锁或电机工作电流的继电器。
3. **防反接保护**：在继电器线圈和触点电路中加入二极管等保护元件，防止反向电压损坏设备。
4. **紧急开关**：在门禁系统中加入紧急开关，以便在紧急情况下手动控制门的开关。

---

### **5. 示例应用场景**
- **办公楼门禁**：通过刷卡或人脸识别触发继电器，控制电磁锁的开关。
- **停车场道闸**：通过车牌识别触发继电器，控制电机的正反转，实现道闸的升降。
- **智能家居**：通过手机 App 或语音助手触发继电器，控制门的开关。

---

### **总结**
- 继电器是门禁系统中控制门开关的关键组件。
- 通过 QML 控制 GPIO、串口或网络通信，可以实现对继电器的控制。
- 注意硬件连接的安全性和可靠性，确保系统稳定运行。

如果你有具体的硬件平台或需求，可以提供更多信息，我可以为你提供更详细的实现方案。


            在您的代码中，继电器控制是通过向 `/proc/driver/ext2_gpio` 文件写入特定格式的数据来实现的。具体来说，写入的数据格式为 `0x<io><value>\n`，其中 `<io>` 是 GPIO 引脚编号，`<value>` 是要设置的电平值（1 表示高电平，0 表示低电平）。以下是详细的工作原理和流程：

---

### **1. 写入文件的作用**
当您调用 `writeGpio(6, 1)` 或 `writeGpio(6, 0)` 时，代码会向 `/proc/driver/ext2_gpio` 文件写入数据。这个文件通常是一个虚拟文件，由内核驱动程序提供，用于与硬件 GPIO 引脚进行交互。

- **写入的数据格式**：
    - `0x61\n`：表示将 GPIO 6 设置为高电平（`1`）。
    - `0x60\n`：表示将 GPIO 6 设置为低电平（`0`）。

- **文件的作用**：
    - 内核驱动程序会监听对该文件的写入操作，并根据写入的数据解析出 GPIO 引脚编号和电平值。
    - 驱动程序随后会通过硬件接口（如寄存器操作）将指定的 GPIO 引脚设置为高电平或低电平。

---

### **2. 发送电平信号的流程**
以下是写入文件后，系统如何发送电平信号的详细流程：

1. **文件写入**：
    - 代码通过 `FileOutputStream` 向 `/proc/driver/ext2_gpio` 文件写入数据（如 `0x61\n` 或 `0x60\n`）。

2. **内核驱动程序处理**：
    - 内核驱动程序会捕获对文件的写入操作，并解析数据。
    - 解析出 GPIO 引脚编号（如 `6`）和电平值（如 `1` 或 `0`）。

3. **硬件操作**：
    - 驱动程序通过操作 GPIO 控制器（如寄存器）将指定的 GPIO 引脚设置为高电平或低电平。
    - 例如：
        - 设置 GPIO 6 为高电平：将 GPIO 控制器的相应寄存器位设置为 `1`。
        - 设置 GPIO 6 为低电平：将 GPIO 控制器的相应寄存器位设置为 `0`。

4. **继电器响应**：
    - GPIO 引脚的电平变化会直接控制继电器的状态：
        - 高电平（`1`）：继电器线圈通电，触点闭合或断开，触发开门操作。
        - 低电平（`0`）：继电器线圈断电，触点恢复默认状态，触发关门操作。

---

### **3. 代码的详细解释**
以下是代码的逐行解释：

```java
public static boolean writeGpio(int io, int value) {
    // 检查文件是否存在
    boolean fileExists = AppFileUtils.isFileExists("/proc/driver/ext2_gpio");
    if(!fileExists) return false;

    // 构造写入的数据格式：0x<io><value>\n
    String str = "0x" + io + value + "\n";

    // 获取文件对象
    File file = new File("/proc/driver/ext2_gpio");

    // 设置文件权限（可执行、可读、可写）
    file.setExecutable(true);
    file.setReadable(true);
    file.setWritable(true);

    try {
        // 打开文件输出流并写入数据
        FileOutputStream fos = new FileOutputStream(file);
        fos.write(str.getBytes());
        fos.close();
    } catch (IOException var5) {
        var5.printStackTrace();
        return false;
    }

    return true;
}
```

---

### **4. 继电器的工作原理**
继电器是一种电磁开关，通过控制线圈的通电和断电来切换触点的状态：
- **高电平（`1`）**：
    - 继电器线圈通电，产生磁场，吸引触点闭合或断开。
    - 在门禁系统中，通常用于开门操作。
- **低电平（`0`）**：
    - 继电器线圈断电，磁场消失，触点恢复默认状态。
    - 在门禁系统中，通常用于关门操作。

---

### **5. 总结**
- 写入 `/proc/driver/ext2_gpio` 文件后，内核驱动程序会解析数据并操作 GPIO 引脚。
- GPIO 引脚的电平变化直接控制继电器的状态，从而实现开门或关门操作。
- 代码的核心是通过文件操作与硬件交互，是一种常见的嵌入式系统开发方式。

如果您有更多问题或需要进一步优化代码，请随时告诉我！


