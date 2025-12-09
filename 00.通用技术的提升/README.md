





            ### **案例：订单处理流程**
我们将设计一个订单处理的案例，展示**不使用状态机**和**使用状态机**的区别。订单状态包括：**待支付**、**已支付**、**已发货**、**已收货**、**已完成**、**已取消**。状态转换规则如下：
1. **待支付 -> 已支付**（支付）
2. **已支付 -> 已发货**（发货）
3. **已发货 -> 已收货**（确认收货）
4. **已收货 -> 已完成**（完成订单）
5. **待支付 -> 已取消**（取消订单）
6. **已支付 -> 已取消**（取消订单，需退款）

---

### **1. 不使用状态机**
#### **代码实现**
```java
public class OrderWithoutStateMachine {
    private String state = "PENDING_PAYMENT"; // 初始状态：待支付

    public void processOrder(String action) {
        switch (state) {
            case "PENDING_PAYMENT":
                if (action.equals("PAY")) {
                    System.out.println("Order paid.");
                    state = "PAID";
                } else if (action.equals("CANCEL")) {
                    System.out.println("Order canceled.");
                    state = "CANCELED";
                } else {
                    System.out.println("Invalid action for PENDING_PAYMENT state.");
                }
                break;
            case "PAID":
                if (action.equals("SHIP")) {
                    System.out.println("Order shipped.");
                    state = "SHIPPED";
                } else if (action.equals("CANCEL")) {
                    System.out.println("Order canceled. Refund initiated.");
                    state = "CANCELED";
                } else {
                    System.out.println("Invalid action for PAID state.");
                }
                break;
            case "SHIPPED":
                if (action.equals("CONFIRM_RECEIPT")) {
                    System.out.println("Order received.");
                    state = "RECEIVED";
                } else {
                    System.out.println("Invalid action for SHIPPED state.");
                }
                break;
            case "RECEIVED":
                if (action.equals("COMPLETE")) {
                    System.out.println("Order completed.");
                    state = "COMPLETED";
                } else {
                    System.out.println("Invalid action for RECEIVED state.");
                }
                break;
            default:
                System.out.println("No further actions allowed for current state.");
        }
    }

    public static void main(String[] args) {
        OrderWithoutStateMachine order = new OrderWithoutStateMachine();
        order.processOrder("PAY");             // Order paid.
        order.processOrder("SHIP");            // Order shipped.
        order.processOrder("CONFIRM_RECEIPT"); // Order received.
        order.processOrder("COMPLETE");        // Order completed.
    }
}
```

#### **运行结果**
```
Order paid.
Order shipped.
Order received.
Order completed.
```

#### **问题分析**
1. **逻辑复杂**：
    - 状态转换逻辑集中在 `switch-case` 中，难以维护。
2. **扩展性差**：
    - 增加新状态或动作需要修改现有代码，容易引入错误。
3. **可读性差**：
    - 状态和动作的耦合度高，代码难以理解。

---

### **2. 使用状态机**
#### **代码实现**
```java
// 定义状态接口
interface OrderState {
    void process(OrderContext context, String action);
}

// 具体状态：待支付
class PendingPaymentState implements OrderState {
    @Override
    public void process(OrderContext context, String action) {
        if (action.equals("PAY")) {
            System.out.println("Order paid.");
            context.setState(new PaidState());
        } else if (action.equals("CANCEL")) {
            System.out.println("Order canceled.");
            context.setState(new CanceledState());
        } else {
            System.out.println("Invalid action for PENDING_PAYMENT state.");
        }
    }
}

// 具体状态：已支付
class PaidState implements OrderState {
    @Override
    public void process(OrderContext context, String action) {
        if (action.equals("SHIP")) {
            System.out.println("Order shipped.");
            context.setState(new ShippedState());
        } else if (action.equals("CANCEL")) {
            System.out.println("Order canceled. Refund initiated.");
            context.setState(new CanceledState());
        } else {
            System.out.println("Invalid action for PAID state.");
        }
    }
}

// 具体状态：已发货
class ShippedState implements OrderState {
    @Override
    public void process(OrderContext context, String action) {
        if (action.equals("CONFIRM_RECEIPT")) {
            System.out.println("Order received.");
            context.setState(new ReceivedState());
        } else {
            System.out.println("Invalid action for SHIPPED state.");
        }
    }
}

// 具体状态：已收货
class ReceivedState implements OrderState {
    @Override
    public void process(OrderContext context, String action) {
        if (action.equals("COMPLETE")) {
            System.out.println("Order completed.");
            context.setState(new CompletedState());
        } else {
            System.out.println("Invalid action for RECEIVED state.");
        }
    }
}

// 具体状态：已完成
class CompletedState implements OrderState {
    @Override
    public void process(OrderContext context, String action) {
        System.out.println("Order is completed. No further actions allowed.");
    }
}

// 具体状态：已取消
class CanceledState implements OrderState {
    @Override
    public void process(OrderContext context, String action) {
        System.out.println("Order is canceled. No further actions allowed.");
    }
}

// 上下文类
class OrderContext {
    private OrderState currentState;

    public OrderContext(OrderState initialState) {
        this.currentState = initialState;
    }

    public void setState(OrderState newState) {
        this.currentState = newState;
    }

    public void processOrder(String action) {
        currentState.process(this, action);
    }
}

// 测试类
public class OrderWithStateMachine {
    public static void main(String[] args) {
        OrderContext order = new OrderContext(new PendingPaymentState());
        order.processOrder("PAY");             // Order paid.
        order.processOrder("SHIP");            // Order shipped.
        order.processOrder("CONFIRM_RECEIPT"); // Order received.
        order.processOrder("COMPLETE");        // Order completed.
    }
}
```

#### **运行结果**
```
Order paid.
Order shipped.
Order received.
Order completed.
```

#### **优势分析**
1. **扩展性强**：
    - 增加新状态或动作只需添加新的状态类，无需修改现有代码。
   ```java
   class ReturnedState implements OrderState {
       @Override
       public void process(OrderContext context, String action) {
           System.out.println("Order returned.");
           context.setState(new CanceledState());
       }
   }
   ```
2. **可读性高**：
    - 状态转换逻辑封装在各自的状态类中，代码结构清晰。
3. **易于维护**：
    - 每个状态独立，修改一个状态不会影响其他状态。
4. **符合开闭原则**：
    - 对扩展开放，对修改关闭。

---

### **3. 对比总结**

---

### **4. 状态机的优势**
1. **清晰的结构**：
    - 状态和转换逻辑分离，代码更易理解。
2. **易于扩展**：
    - 增加新状态只需添加新类，符合开闭原则。
3. **降低耦合**：
    - 状态之间相互独立，修改一个状态不会影响其他状态。
4. **提高可维护性**：
    - 逻辑集中，便于调试和测试。

---

### **总结**
通过对比可以看出，状态机在复杂场景中具有明显优势。虽然初始实现可能稍复杂，但其扩展性、可读性和维护性显著优于传统的 `switch-case` 实现方式。对于状态数量多或可能变化的系统（如订单处理流程），状态机是更好的选择。