






**设计原理：**

**实现机制分析：**
```java
// Java中的软引用实现原理
public class SoftReferenceCache<K, V> {
    private final Map<K, SoftReference<V>> cache = new ConcurrentHashMap<>();
    
    public V get(K key) {
        SoftReference<V> ref = cache.get(key);
        if (ref != null) {
            V value = ref.get();  // 可能返回null，如果对象已被回收
            if (value == null) {
                cache.remove(key);  // 清理已失效的引用
            }
            return value;
        }
        return null;
    }
    
    public void put(K key, V value) {
        cache.put(key, new SoftReference<>(value));
    }
    
    // 垃圾回收器的决策逻辑（概念性描述）
    private boolean shouldReclaim(SoftReference<?> ref) {
        long availableMemory = Runtime.getRuntime().freeMemory();
        long totalMemory = Runtime.getRuntime().totalMemory();
        double memoryPressure = 1.0 - (double) availableMemory / totalMemory;
        
        // 内存压力越大，越倾向于回收软引用对象
        return memoryPressure > 0.8;  // 简化的决策逻辑
    }
}
```


### 2.3 弱引用（Weak Reference）

弱引用是一种"非拥有性"的引用类型，它不会阻止对象被垃圾回收。

**设计原理：**
弱引用的设计基于"观察而不拥有"的思想。它允许程序观察一个对象的存在，但不会因为这种观察而延长对象的生命周期。这种设计特别适合实现观察者模式、缓存系统和避免循环引用。

**典型应用场景：**
```java
// 观察者模式中的弱引用应用
public class WeakObserverPattern {
    public static class Subject {
        private final List<WeakReference<Observer>> observers = new ArrayList<>();
        
        public void addObserver(Observer observer) {
            observers.add(new WeakReference<>(observer));
        }
        
        public void notifyObservers(String message) {
            Iterator<WeakReference<Observer>> it = observers.iterator();
            while (it.hasNext()) {
                WeakReference<Observer> ref = it.next();
                Observer observer = ref.get();
                if (observer != null) {
                    observer.update(message);
                } else {
                    it.remove();  // 清理已失效的观察者
                }
            }
        }
    }
    
    public interface Observer {
        void update(String message);
    }
    
    // 使用示例
    public static void demonstrateWeakObserver() {
        Subject subject = new Subject();
        
        Observer observer = message -> System.out.println("Received: " + message);
        subject.addObserver(observer);
        
        subject.notifyObservers("Hello");  // 正常接收消息
        
        observer = null;  // 移除强引用
        System.gc();      // 触发垃圾回收
        
        subject.notifyObservers("World");  // 观察者可能已被回收
    }
}
```

**弱引用的内存语义：**
- **非拥有性**：不会延长对象的生命周期
- **即时性**：对象一旦不可达就可能被立即回收
- **不确定性**：无法保证对象在任何时刻都存在

### 2.4 虚引用（Phantom Reference）

虚引用是最弱的引用类型，主要用于对象回收的通知和资源清理。

**设计原理：**
虚引用的设计基于"回收通知"的需求。它不能用于访问对象，但可以用于检测对象何时被垃圾回收器回收。这种设计主要用于实现精确的资源管理和清理逻辑。

**实现原理：**
```java
// Java中的虚引用应用
public class PhantomReferenceCleanup {
    private static final ReferenceQueue<Object> queue = new ReferenceQueue<>();
    private static final Map<PhantomReference<Object>, Runnable> cleanupTasks = 
        new ConcurrentHashMap<>();
    
    static {
        // 启动清理线程
        Thread cleanupThread = new Thread(() -> {
            while (true) {
                try {
                    Reference<?> ref = queue.remove();  // 阻塞等待
                    Runnable cleanup = cleanupTasks.remove(ref);
                    if (cleanup != null) {
                        cleanup.run();  // 执行清理任务
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        });
        cleanupThread.setDaemon(true);
        cleanupThread.start();
    }
    
    public static void registerCleanup(Object obj, Runnable cleanup) {
        PhantomReference<Object> ref = new PhantomReference<>(obj, queue);
        cleanupTasks.put(ref, cleanup);
    }
    
    // 使用示例
    public static class ResourceHolder {
        private final long nativeHandle;
        
        public ResourceHolder() {
            this.nativeHandle = allocateNativeResource();
            
            // 注册清理任务
            registerCleanup(this, () -> {
                freeNativeResource(nativeHandle);
                System.out.println("Native resource freed: " + nativeHandle);
            });
        }
        
        private native long allocateNativeResource();
        private native void freeNativeResource(long handle);
    }
}
```

**虚引用的特殊性质：**
- **不可访问性**：无法通过虚引用访问对象
- **回收通知**：对象被回收时会收到通知
- **清理保证**：确保清理逻辑在对象回收后执行

## 3. 不同编程语言中的引用类型实现

### 3.1 Java中的引用类型体系

Java提供了最完整的引用类型体系，这得益于其成熟的垃圾回收机制。

**Java引用类型的层次结构：**
```java
// Java引用类型的完整实现
public class JavaReferenceSystem {
    
    // 1. 强引用 - 默认的引用类型
    public void strongReferenceDemo() {
        Object obj = new Object();  // 强引用
        List<String> list = new ArrayList<>();  // 强引用
        // 这些对象只有在引用被清除时才可能被回收
    }
    
    // 2. 软引用 - 内存敏感的引用
    public void softReferenceDemo() {
        Object obj = new Object();
        SoftReference<Object> softRef = new SoftReference<>(obj);
        obj = null;  // 清除强引用
        
        // 在内存不足时，softRef指向的对象可能被回收
        Object retrieved = softRef.get();  // 可能返回null
    }
    
    // 3. 弱引用 - 不阻止回收的引用
    public void weakReferenceDemo() {
        Object obj = new Object();
        WeakReference<Object> weakRef = new WeakReference<>(obj);
        obj = null;  // 清除强引用
        
        System.gc();  // 建议垃圾回收
        Object retrieved = weakRef.get();  // 很可能返回null
    }
    
    // 4. 虚引用 - 回收通知引用
    public void phantomReferenceDemo() {
        ReferenceQueue<Object> queue = new ReferenceQueue<>();
        Object obj = new Object();
        PhantomReference<Object> phantomRef = new PhantomReference<>(obj, queue);
        
        obj = null;  // 清除强引用
        System.gc();  // 触发垃圾回收
        
        // 检查回收通知
        Reference<?> ref = queue.poll();
        if (ref != null) {
            System.out.println("Object has been garbage collected");
        }
    }
}
```

**Java引用类型的内存管理集成：**
Java的引用类型与垃圾回收器紧密集成，不同的垃圾回收器对引用类型有不同的处理策略：

- **Serial GC**：按照引用强度顺序处理
- **Parallel GC**：并行处理不同强度的引用
- **G1 GC**：在不同的回收阶段处理不同的引用类型
- **ZGC/Shenandoah**：低延迟处理引用类型

### 3.2 C++中的智能指针体系

C++通过智能指针实现了类似的引用管理机制，但更注重确定性和性能。

**C++智能指针的设计哲学：**
```cpp
#include <memory>
#include <iostream>
#include <vector>

// C++智能指针体系
class CppReferenceSystem {
public:
    // 1. unique_ptr - 独占所有权
    void uniquePtrDemo() {
        std::unique_ptr<int> ptr = std::make_unique<int>(42);
        // ptr拥有对象的独占所有权
        
        std::unique_ptr<int> ptr2 = std::move(ptr);  // 所有权转移
        // 现在ptr为空，ptr2拥有对象
        
        // 对象在ptr2析构时自动释放
    }
    
    // 2. shared_ptr - 共享所有权（类似强引用）
    void sharedPtrDemo() {
        std::shared_ptr<int> ptr1 = std::make_shared<int>(42);
        std::shared_ptr<int> ptr2 = ptr1;  // 引用计数增加
        
        std::cout << "Reference count: " << ptr1.use_count() << std::endl;  // 输出2
        
        ptr1.reset();  // 引用计数减少
        // 对象在最后一个shared_ptr析构时释放
    }
    
    // 3. weak_ptr - 弱引用，不影响引用计数
    void weakPtrDemo() {
        std::shared_ptr<int> shared = std::make_shared<int>(42);
        std::weak_ptr<int> weak = shared;  // 不增加引用计数
        
        if (auto locked = weak.lock()) {  // 尝试获取shared_ptr
            std::cout << "Object still exists: " << *locked << std::endl;
        } else {
            std::cout << "Object has been destroyed" << std::endl;
        }
        
        shared.reset();  // 对象被销毁
        
        if (auto locked = weak.lock()) {
            // 这里不会执行，因为对象已被销毁
        } else {
            std::cout << "Object no longer exists" << std::endl;
        }
    }
    
    // 循环引用问题的解决
    struct Node {
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> parent;  // 使用weak_ptr避免循环引用
        int value;
        
        Node(int val) : value(val) {}
        ~Node() {
            std::cout << "Node " << value << " destroyed" << std::endl;
        }
    };
    
    void cyclicReferenceDemo() {
        auto node1 = std::make_shared<Node>(1);
        auto node2 = std::make_shared<Node>(2);
        
        node1->next = node2;
        node2->parent = node1;  // 使用weak_ptr，避免循环引用
        
        // 当node1和node2离开作用域时，都会被正确销毁
    }
};
```

**C++引用管理的特点：**
- **确定性析构**：对象的销毁时机是确定的
- **零开销抽象**：智能指针的开销接近原始指针
- **RAII原则**：资源获取即初始化，确保资源正确释放
- **类型安全**：编译时检查引用的正确性

### 3.3 JavaScript中的引用与垃圾回收

JavaScript采用了不同的方法，主要依赖垃圾回收器的智能化。

**JavaScript引用管理的演进：**
```javascript
// JavaScript中的引用管理
class JavaScriptReferenceSystem {
    
    // 1. 强引用 - 默认的引用类型
    strongReferenceDemo() {
        let obj = { name: "example" };  // 强引用
        let arr = [1, 2, 3];           // 强引用
        
        // 这些对象在引用存在时不会被回收
        obj = null;  // 清除引用，对象可能被回收
    }
    
    // 2. WeakMap - 弱键引用
    weakMapDemo() {
        const weakMap = new WeakMap();
        let key = { id: 1 };
        
        weakMap.set(key, "some value");
        
        // key对象可以被垃圾回收，即使它在WeakMap中
        key = null;  // WeakMap中的条目也会被自动清理
    }
    
    // 3. WeakSet - 弱值引用
    weakSetDemo() {
        const weakSet = new WeakSet();
        let obj = { name: "example" };
        
        weakSet.add(obj);
        
        // obj可以被垃圾回收，WeakSet不会阻止回收
        obj = null;  // WeakSet中的条目也会被自动清理
    }
    
    // 4. WeakRef - ES2021引入的弱引用
    weakRefDemo() {
        let obj = { name: "example" };
        const weakRef = new WeakRef(obj);
        
        obj = null;  // 清除强引用
        
        // 稍后检查对象是否还存在
        setTimeout(() => {
            const retrieved = weakRef.deref();
            if (retrieved) {
                console.log("Object still exists:", retrieved.name);
            } else {
                console.log("Object has been garbage collected");
            }
        }, 1000);
    }
    
    // 5. FinalizationRegistry - 清理回调
    finalizationDemo() {
        const registry = new FinalizationRegistry((heldValue) => {
            console.log("Object with held value", heldValue, "was garbage collected");
        });
        
        let obj = { name: "example" };
        registry.register(obj, "example-object");
        
        obj = null;  // 对象被回收时会触发回调
    }
    
    // 内存泄漏的避免
    avoidMemoryLeaks() {
        // 避免闭包中的意外引用
        function createHandler() {
            const largeData = new Array(1000000).fill("data");
            
            return function handler(event) {
                // 如果不需要largeData，应该避免在闭包中引用它
                console.log("Event handled");
            };
        }
        
        // 使用WeakMap存储私有数据
        const privateData = new WeakMap();
        
        class MyClass {
            constructor(data) {
                privateData.set(this, data);
            }
            
            getData() {
                return privateData.get(this);
            }
        }
        
        // 当MyClass实例被回收时，privateData中的条目也会被清理
    }
}
```

**JavaScript引用管理的特点：**
- **自动垃圾回收**：程序员无需手动管理内存
- **标记清除算法**：现代引擎使用先进的垃圾回收算法
- **弱引用支持**：ES2021引入了WeakRef和FinalizationRegistry
- **内存泄漏预防**：通过WeakMap/WeakSet避免常见的内存泄漏

### 3.4 Go语言中的引用管理

Go语言采用了简化的引用模型，主要依赖垃圾回收器。

**Go语言的引用管理策略：**
```go
package main

import (
    "fmt"
    "runtime"
    "sync"
    "unsafe"
    "weak"  // 假设的弱引用包
)

// Go语言中的引用管理
type GoReferenceSystem struct{}

// 1. 强引用 - 默认的引用类型
func (g *GoReferenceSystem) StrongReferenceDemo() {
    obj := &struct{ name string }{"example"}
    slice := make([]int, 1000)
    
    // 这些对象在引用存在时不会被回收
    _ = obj
    _ = slice
    
    // 当函数结束时，局部变量的引用被清除
}

// 2. 指针与引用的关系
func (g *GoReferenceSystem) PointerDemo() {
    value := 42
    ptr := &value  // ptr是指向value的指针（强引用）
    
    // 通过unsafe包可以进行更底层的操作
    uintptrValue := uintptr(unsafe.Pointer(ptr))
    
    // 注意：uintptr不会阻止垃圾回收
    runtime.GC()
    
    // 重新转换为指针（危险操作）
    newPtr := (*int)(unsafe.Pointer(uintptrValue))
    fmt.Println(*newPtr)  // 可能导致程序崩溃
}

// 3. 使用sync.Pool实现对象复用
func (g *GoReferenceSystem) ObjectPoolDemo() {
    pool := &sync.Pool{
        New: func() interface{} {
            return make([]byte, 1024)
        },
    }
    
    // 获取对象
    buffer := pool.Get().([]byte)
    
    // 使用对象
    copy(buffer, []byte("Hello, World!"))
    
    // 归还对象到池中
    pool.Put(buffer)
    
    // 池中的对象可能被垃圾回收器回收
    runtime.GC()
}

// 4. 弱引用的模拟实现
type WeakReference struct {
    ptr unsafe.Pointer
    id  uintptr
}

func NewWeakReference(obj interface{}) *WeakReference {
    ptr := unsafe.Pointer(&obj)
    return &WeakReference{
        ptr: ptr,
        id:  uintptr(ptr),
    }
}

func (w *WeakReference) Get() interface{} {
    // 这是一个简化的实现，实际的弱引用需要与GC集成
    if w.ptr != nil {
        return *(*interface{})(w.ptr)
    }
    return nil
}

// 5. 使用finalizer实现清理逻辑
func (g *GoReferenceSystem) FinalizerDemo() {
    type Resource struct {
        handle uintptr
    }
    
    resource := &Resource{handle: 12345}
    
    // 设置finalizer
    runtime.SetFinalizer(resource, func(r *Resource) {
        fmt.Printf("Resource %d is being finalized\n", r.handle)
        // 执行清理逻辑
    })
    
    // 清除引用
    resource = nil
    
    // 触发垃圾回收
    runtime.GC()
    runtime.GC()  // 可能需要多次GC才能触发finalizer
}

// 6. 内存管理的最佳实践
func (g *GoReferenceSystem) BestPractices() {
    // 避免循环引用
    type Node struct {
        value int
        next  *Node
        // 如果需要父节点引用，考虑使用弱引用或者不存储父节点指针
    }
    
    // 及时清理大对象的引用
    largeSlice := make([]byte, 10*1024*1024)  // 10MB
    // 使用完毕后
    largeSlice = nil
    
    // 使用context控制生命周期
    // ctx, cancel := context.WithTimeout(context.Background(), time.Second)
    // defer cancel()
    
    // 监控内存使用
    var m runtime.MemStats
    runtime.ReadMemStats(&m)
    fmt.Printf("Allocated memory: %d KB\n", m.Alloc/1024)
}
```

**Go语言引用管理的特点：**
- **简化的模型**：主要依赖垃圾回收器
- **指针语义**：明确的指针概念
- **Finalizer支持**：类似于Java的finalize机制
- **性能导向**：注重垃圾回收的性能优化

## 4. 引用类型的设计模式与最佳实践

### 4.1 缓存系统的引用策略

不同的引用类型在缓存系统中有不同的应用场景和设计模式。

**多层次缓存的引用策略：**
```java
// 基于引用类型的多层次缓存系统
public class MultiLevelCache<K, V> {
    // L1缓存：强引用，小容量，快速访问
    private final Map<K, V> l1Cache = new ConcurrentHashMap<>();
    private final int l1MaxSize = 100;
    
    // L2缓存：软引用，中等容量，内存敏感
    private final Map<K, SoftReference<V>> l2Cache = new ConcurrentHashMap<>();
    private final int l2MaxSize = 1000;
    
    // L3缓存：弱引用，大容量，最后的机会
    private final Map<K, WeakReference<V>> l3Cache = new ConcurrentHashMap<>();
    
    public V get(K key) {
        // 首先尝试L1缓存
        V value = l1Cache.get(key);
        if (value != null) {
            return value;
        }
        
        // 尝试L2缓存
        SoftReference<V> softRef = l2Cache.get(key);
        if (softRef != null) {
            value = softRef.get();
            if (value != null) {
                promoteToL1(key, value);  // 提升到L1缓存
                return value;
            } else {
                l2Cache.remove(key);  // 清理失效引用
            }
        }
        
        // 尝试L3缓存
        WeakReference<V> weakRef = l3Cache.get(key);
        if (weakRef != null) {
            value = weakRef.get();
            if (value != null) {
                promoteToL2(key, value);  // 提升到L2缓存
                return value;
            } else {
                l3Cache.remove(key);  // 清理失效引用
            }
        }
        
        return null;  // 缓存未命中
    }
    
    public void put(K key, V value) {
        // 新数据直接放入L1缓存
        if (l1Cache.size() >= l1MaxSize) {
            evictFromL1();  // L1缓存满时进行淘汰
        }
        l1Cache.put(key, value);
    }
    
    private void promoteToL1(K key, V value) {
        if (l1Cache.size() >= l1MaxSize) {
            evictFromL1();
        }
        l1Cache.put(key, value);
    }
    
    private void promoteToL2(K key, V value) {
        if (l2Cache.size() >= l2MaxSize) {
            evictFromL2();
        }
        l2Cache.put(key, new SoftReference<>(value));
    }
    
    private void evictFromL1() {
        // 将L1中的数据降级到L2
        Iterator<Map.Entry<K, V>> it = l1Cache.entrySet().iterator();
        if (it.hasNext()) {
            Map.Entry<K, V> entry = it.next();
            l2Cache.put(entry.getKey(), new SoftReference<>(entry.getValue()));
            it.remove();
        }
    }
    
    private void evictFromL2() {
        // 将L2中的数据降级到L3
        Iterator<Map.Entry<K, SoftReference<V>>> it = l2Cache.entrySet().iterator();
        if (it.hasNext()) {
            Map.Entry<K, SoftReference<V>> entry = it.next();
            V value = entry.getValue().get();
            if (value != null) {
                l3Cache.put(entry.getKey(), new WeakReference<>(value));
            }
            it.remove();
        }
    }
}
```

### 4.2 观察者模式的引用管理

观察者模式中的引用管理是一个经典的应用场景。

**基于弱引用的观察者模式：**
```java
// 防止内存泄漏的观察者模式
public class WeakObservable<T> {
    private final List<WeakReference<Observer<T>>> observers = 
        Collections.synchronizedList(new ArrayList<>());
    
    public interface Observer<T> {
        void onChanged(T data);
    }
    
    public void addObserver(Observer<T> observer) {
        observers.add(new WeakReference<>(observer));
    }
    
    public void removeObserver(Observer<T> observer) {
        observers.removeIf(ref -> {
            Observer<T> obs = ref.get();
            return obs == null || obs == observer;
        });
    }
    
    public void notifyObservers(T data) {
        Iterator<WeakReference<Observer<T>>> it = observers.iterator();
        while (it.hasNext()) {
            WeakReference<Observer<T>> ref = it.next();
            Observer<T> observer = ref.get();
            if (observer != null) {
                try {
                    observer.onChanged(data);
                } catch (Exception e) {
                    // 处理观察者异常
                    System.err.println("Observer error: " + e.getMessage());
                }
            } else {
                it.remove();  // 清理失效的观察者
            }
        }
    }
    
    public int getObserverCount() {
        // 清理失效引用并返回有效观察者数量
        observers.removeIf(ref -> ref.get() == null);
        return observers.size();
    }
}

// 使用示例
class ObserverExample {
    public static void demonstrate() {
        WeakObservable<String> observable = new WeakObservable<>();
        
        // 创建观察者
        WeakObservable.Observer<String> observer1 = data -> 
            System.out.println("Observer1: " + data);
        
        WeakObservable.Observer<String> observer2 = data -> 
            System.out.println("Observer2: " + data);
        
        observable.addObserver(observer1);
        observable.addObserver(observer2);
        
        observable.notifyObservers("Hello");  // 两个观察者都会收到通知
        
        observer1 = null;  // 移除强引用
        System.gc();       // 建议垃圾回收
        
        observable.notifyObservers("World");  // 只有observer2会收到通知
        
        System.out.println("Active observers: " + observable.getObserverCount());
    }
}
```

### 4.3 资源管理的引用策略

不同类型的资源需要不同的引用管理策略。

**基于引用类型的资源管理框架：**
```java
// 资源管理框架
public class ResourceManager {
    
    // 关键资源使用强引用
    private final Map<String, CriticalResource> criticalResources = 
        new ConcurrentHashMap<>();
    
    // 缓存资源使用软引用
    private final Map<String, SoftReference<CacheableResource>> cacheableResources = 
        new ConcurrentHashMap<>();
    
    // 临时资源使用弱引用
    private final Map<String, WeakReference<TemporaryResource>> temporaryResources = 
        new ConcurrentHashMap<>();
    
    // 需要清理的资源使用虚引用
    private final ReferenceQueue<CleanableResource> cleanupQueue = new ReferenceQueue<>();
    private final Map<PhantomReference<CleanableResource>, Runnable> cleanupTasks = 
        new ConcurrentHashMap<>();
    
    public interface Resource {
        String getId();
        void close();
    }
    
    public static class CriticalResource implements Resource {
        private final String id;
        private boolean closed = false;
        
        public CriticalResource(String id) {
            this.id = id;
        }
        
        @Override
        public String getId() { return id; }
        
        @Override
        public void close() {
            if (!closed) {
                System.out.println("Closing critical resource: " + id);
                closed = true;
            }
        }
    }
    
    public static class CacheableResource implements Resource {
        private final String id;
        private final byte[] data;
        
        public CacheableResource(String id, int size) {
            this.id = id;
            this.data = new byte[size];
        }
        
        @Override
        public String getId() { return id; }
        
        @Override
        public void close() {
            System.out.println("Closing cacheable resource: " + id);
        }
    }
    
    public static class TemporaryResource implements Resource {
        private final String id;
        
        public TemporaryResource(String id) {
            this.id = id;
        }
        
        @Override
        public String getId() { return id; }
        
        @Override
        public void close() {
            System.out.println("Closing temporary resource: " + id);
        }
    }
    
    public static class CleanableResource implements Resource {
        private final String id;
        private final long nativeHandle;
        
        public CleanableResource(String id) {
            this.id = id;
            this.nativeHandle = System.nanoTime();  // 模拟native资源
        }
        
        @Override
        public String getId() { return id; }
        
        @Override
        public void close() {
            System.out.println("Closing cleanable resource: " + id);
        }
        
        public long getNativeHandle() { return nativeHandle; }
    }
    
    // 资源获取方法
    public CriticalResource getCriticalResource(String id) {
        return criticalResources.computeIfAbsent(id, CriticalResource::new);
    }
    
    public CacheableResource getCacheableResource(String id, int size) {
        SoftReference<CacheableResource> ref = cacheableResources.get(id);
        if (ref != null) {
            CacheableResource resource = ref.get();
            if (resource != null) {
                return resource;
            }
        }
        
        CacheableResource resource = new CacheableResource(id, size);
        cacheableResources.put(id, new SoftReference<>(resource));
        return resource;
    }
    
    public TemporaryResource getTemporaryResource(String id) {
        WeakReference<TemporaryResource> ref = temporaryResources.get(id);
        if (ref != null) {
            TemporaryResource resource = ref.get();
            if (resource != null) {
                return resource;
            }
        }
        
        TemporaryResource resource = new TemporaryResource(id);
        temporaryResources.put(id, new WeakReference<>(resource));
        return resource;
    }
    
    public CleanableResource getCleanableResource(String id) {
        CleanableResource resource = new CleanableResource(id);
        
        // 注册清理任务
        PhantomReference<CleanableResource> phantomRef = 
            new PhantomReference<>(resource, cleanupQueue);
        
        cleanupTasks.put(phantomRef, () -> {
            System.out.println("Cleaning up native resource: " + resource.getNativeHandle());
            // 执行native资源清理
        });
        
        return resource;
    }
    
    // 清理线程
    public void startCleanupThread() {
        Thread cleanupThread = new Thread(() -> {
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    Reference<?> ref = cleanupQueue.remove();
                    Runnable cleanup = cleanupTasks.remove(ref);
                    if (cleanup != null) {
                        cleanup.run();
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        });
        cleanupThread.setDaemon(true);
        cleanupThread.start();
    }
    
    // 资源统计
    public void printResourceStats() {
        System.out.println("Critical resources: " + criticalResources.size());
        
        // 清理失效的软引用
        cacheableResources.entrySet().removeIf(entry -> entry.getValue().get() == null);
        System.out.println("Cacheable resources: " + cacheableResources.size());
        
        // 清理失效的弱引用
        temporaryResources.entrySet().removeIf(entry -> entry.getValue().get() == null);
        System.out.println("Temporary resources: " + temporaryResources.size());
        
        System.out.println("Cleanup tasks pending: " + cleanupTasks.size());
    }
}
```

## 5. 引用类型的性能影响与优化策略

### 5.1 性能特征分析

不同引用类型对程序性能有不同的影响，理解这些影响对于优化程序性能至关重要。

**引用类型的性能开销对比：**
```java
// 性能测试框架
public class ReferencePerformanceAnalysis {
    
    private static final int ITERATIONS = 1_000_000;
    private static final int OBJECT_COUNT = 10_000;
    
    public static class TestObject {
        private final int value;
        private final String data;
        
        public TestObject(int value) {
            this.value = value;
            this.data = "Data-" + value;
        }
        
        public int getValue() { return value; }
        public String getData() { return data; }
    }
    
    // 强引用性能测试
    public void testStrongReferencePerformance() {
        long startTime = System.nanoTime();
        
        List<TestObject> objects = new ArrayList<>(OBJECT_COUNT);
        for (int i = 0; i < OBJECT_COUNT; i++) {
            objects.add(new TestObject(i));
        }
        
        // 访问测试
        int sum = 0;
        for (int i = 0; i < ITERATIONS; i++) {
            TestObject obj = objects.get(i % OBJECT_COUNT);
            sum += obj.getValue();
        }
        
        long endTime = System.nanoTime();
        System.out.println("Strong reference time: " + (endTime - startTime) / 1_000_000 + "ms");
        System.out.println("Sum: " + sum);
    }
    
    // 软引用性能测试
    public void testSoftReferencePerformance() {
        long startTime = System.nanoTime();
        
        List<SoftReference<TestObject>> objects = new ArrayList<>(OBJECT_COUNT);
        for (int i = 0; i < OBJECT_COUNT; i++) {
            objects.add(new SoftReference<>(new TestObject(i)));
        }
        
        // 访问测试
        int sum = 0;
        int nullCount = 0;
        for (int i = 0; i < ITERATIONS; i++) {
            SoftReference<TestObject> ref = objects.get(i % OBJECT_COUNT);
            TestObject obj = ref.get();
            if (obj != null) {
                sum += obj.getValue();
            } else {
                nullCount++;
            }
        }
        
        long endTime = System.nanoTime();
        System.out.println("Soft reference time: " + (endTime - startTime) / 1_000_000 + "ms");
        System.out.println("Sum: " + sum + ", Null count: " + nullCount);
    }
    
    // 弱引用性能测试
    public void testWeakReferencePerformance() {
        long startTime = System.nanoTime();
        
        List<WeakReference<TestObject>> objects = new ArrayList<>(OBJECT_COUNT);
        List<TestObject> strongRefs = new ArrayList<>(OBJECT_COUNT);  // 保持强引用
        
        for (int i = 0; i < OBJECT_COUNT; i++) {
            TestObject obj = new TestObject(i);
            objects.add(new WeakReference<>(obj));
            strongRefs.add(obj);  // 防止被回收
        }
        
        // 访问测试
        int sum = 0;
        int nullCount = 0;
        for (int i = 0; i < ITERATIONS; i++) {
            WeakReference<TestObject> ref = objects.get(i % OBJECT_COUNT);
            TestObject obj = ref.get();
            if (obj != null) {
                sum += obj.getValue();
            } else {
                nullCount++;
            }
        }
        
        long endTime = System.nanoTime();
        System.out.println("Weak reference time: " + (endTime - startTime) / 1_000_000 + "ms");
        System.out.println("Sum: " + sum + ", Null count: " + nullCount);
    }
    
    // 内存使用分析
    public void analyzeMemoryUsage() {
        Runtime runtime = Runtime.getRuntime();
        
        // 测试强引用的内存使用
        runtime.gc();
        long beforeStrong = runtime.totalMemory() - runtime.freeMemory();
        
        List<TestObject> strongObjects = new ArrayList<>();
        for (int i = 0; i < OBJECT_COUNT; i++) {
            strongObjects.add(new TestObject(i));
        }
        
        long afterStrong = runtime.totalMemory() - runtime.freeMemory();
        System.out.println("Strong reference memory: " + (afterStrong - beforeStrong) / 1024 + " KB");
        
        // 测试软引用的内存使用
        strongObjects.clear();
        runtime.gc();
        long beforeSoft = runtime.totalMemory() - runtime.freeMemory();
        
        List<SoftReference<TestObject>> softObjects = new ArrayList<>();
        for (int i = 0; i < OBJECT_COUNT; i++) {
            softObjects.add(new SoftReference<>(new TestObject(i)));
        }
        
        long afterSoft = runtime.totalMemory() - runtime.freeMemory();
        System.out.println("Soft reference memory: " + (afterSoft - beforeSoft) / 1024 + " KB");
        
        // 测试弱引用的内存使用
        softObjects.clear();
        runtime.gc();
        long beforeWeak = runtime.totalMemory() - runtime.freeMemory();
        
        List<WeakReference<TestObject>> weakObjects = new ArrayList<>();
        for (int i = 0; i < OBJECT_COUNT; i++) {
            weakObjects.add(new WeakReference<>(new TestObject(i)));
        }
        
        long afterWeak = runtime.totalMemory() - runtime.freeMemory();
        System.out.println("Weak reference memory: " + (afterWeak - beforeWeak) / 1024 + " KB");
    }
}
```

### 5.2 优化策略与最佳实践

**引用类型选择的决策树：**
```java
// 引用类型选择指南
public class ReferenceSelectionGuide {
    
    public enum ReferenceType {
        STRONG,    // 强引用
        SOFT,      // 软引用
        WEAK,      // 弱引用
        PHANTOM    // 虚引用
    }
    
    public static class ReferenceDecision {
        private final ReferenceType recommendedType;
        private final String reason;
        private final List<String> considerations;
        
        public ReferenceDecision(ReferenceType type, String reason, List<String> considerations) {
            this.recommendedType = type;
            this.reason = reason;
            this.considerations = considerations;
        }
        
        // getters...
    }
    
    public static ReferenceDecision selectReferenceType(
            boolean isEssential,           // 是否是必需的对象
            boolean isMemorySensitive,     // 是否对内存敏感
            boolean canRecreate,           // 是否可以重新创建
            boolean needsCleanup,          // 是否需要清理逻辑
            boolean isObserver,            // 是否是观察者关系
            boolean isCyclicReference      // 是否存在循环引用
    ) {
        List<String> considerations = new ArrayList<>();
        
        // 必需对象使用强引用
        if (isEssential) {
            considerations.add("对象是必需的，不能被意外回收");
            return new ReferenceDecision(ReferenceType.STRONG, 
                "Essential objects require strong references", considerations);
        }
        
        // 需要清理逻辑的对象使用虚引用
        if (needsCleanup) {
            considerations.add("对象需要清理逻辑，使用虚引用监控回收");
            return new ReferenceDecision(ReferenceType.PHANTOM, 
                "Objects requiring cleanup should use phantom references", considerations);
        }
        
        // 观察者关系或循环引用使用弱引用
        if (isObserver || isCyclicReference) {
            considerations.add("避免内存泄漏，使用弱引用");
            if (isCyclicReference) {
                considerations.add("打破循环引用");
            }
            return new ReferenceDecision(ReferenceType.WEAK, 
                "Observer pattern or cyclic references should use weak references", considerations);
        }
        
        // 内存敏感且可重新创建的对象使用软引用
        if (isMemorySensitive && canRecreate) {
            considerations.add("内存敏感的缓存对象，可以在内存不足时回收");
            return new ReferenceDecision(ReferenceType.SOFT, 
                "Memory-sensitive cacheable objects should use soft references", considerations);
        }
        
        // 默认使用强引用
        considerations.add("默认选择，确保对象不被意外回收");
        return new ReferenceDecision(ReferenceType.STRONG, 
            "Default choice for regular objects", considerations);
    }
    
    // 使用示例
    public static void demonstrateSelection() {
        // 缓存对象
        ReferenceDecision cacheDecision = selectReferenceType(
            false, true, true, false, false, false);
        System.out.println("Cache object: " + cacheDecision.recommendedType);
        
        // 观察者对象
        ReferenceDecision observerDecision = selectReferenceType(
            false, false, false, false, true, false);
        System.out.println("Observer object: " + observerDecision.recommendedType);
        
        // 需要清理的资源
        ReferenceDecision resourceDecision = selectReferenceType(
            false, false, false, true, false, false);
        System.out.println("Resource object: " + resourceDecision.recommendedType);
    }
}
```

## 6. 引用类型的未来发展趋势

### 6.1 新兴的引用管理技术

**1. 自适应引用管理**
未来的引用管理系统可能会根据运行时的内存使用情况和访问模式，自动调整引用的强度。

**2. 机器学习驱动的垃圾回收**
通过机器学习算法预测对象的生命周期，优化引用类型的选择和垃圾回收策略。

**3. 硬件辅助的引用管理**
新的硬件特性可能会提供更高效的引用跟踪和垃圾回收支持。

### 6.2 编程语言的演进方向

**1. 更精细的引用控制**
未来的编程语言可能会提供更多种类的引用类型，以满足不同场景的需求。

**2. 编译时引用优化**
编译器可能会自动分析代码，选择最优的引用类型，减少程序员的负担。

**3. 跨语言的引用管理**
在多语言混合编程的环境中，统一的引用管理机制变得越来越重要。

## 7. 总结：引用类型设计的核心价值

引用类型的设计体现了现代编程语言在内存管理方面的深刻思考。它们不仅仅是技术实现的细节，更是编程哲学的体现：

**1. 语义的精确表达**
不同的引用类型允许程序员精确地表达对象之间的关系和依赖程度。

**2. 性能与安全的平衡**
通过提供多种引用选择，程序员可以在性能和内存安全之间找到最佳平衡点。

**3. 复杂性的分层管理**
引用类型将内存管理的复杂性分解为不同的层次，使得程序员可以根据需要选择合适的抽象级别。

**4. 系统的可扩展性**
良好的引用类型设计为系统的扩展和优化提供了基础。

理解引用类型的设计思想，不仅有助于编写更高效、更安全的程序，更重要的是能够培养正确的内存管理思维。在未来的软件开发中，随着系统复杂性的增加和性能要求的提高，引用类型的重要性将会越来越突出。

引用类型的设计灵魂在于其对对象关系的精确建模和对内存资源的智能管理，这种设计哲学值得每一个程序员深入理解和掌握。






- 01.四种引用介绍
    - 1.1 引用说明
    - 1.2 为何需要4种引用
    - 1.3 引用类型有哪些
- 02.一般使用场景
    - 2.1 强引用场景
    - 2.2 软引用场景
    - 2.3 弱引用场景
    - 2.4 虚引用场景
    - 2.5 四种引用比较
- 03.引用原理说明
    - 3.0 引用回收的流程
    - 3.1 看看Reference的源代码
    - 3.2 看看ReferenceQueue的enqueue函数
    - 3.3 看看ReferenceQueue的enqueueLocked(Reference)函数
    - 3.4 接着看看ReferenceQueue.isEnqueued()代码
    - 3.5 那么enqueueLocked(Reference)函数中的Cleaner是做什么的
    - 3.6 软引用SoftReference源码
    - 3.7 弱引用WeakReference源码
    - 3.8 虚引用PhantomReference源码
    - 3.9 弱引用回收的原理分析
- 04.引用回收机制
    - 4.1 待完善



### 01.四种引用介绍
#### 1.1 引用说明
- java.lang.ref包中提供了几个类：
    - SoftReference类、WeakReference类和PhantomReference类，它们分别代表软引用、弱引用和虚引用。
    - ReferenceQueue类表示引用队列，它可以和这三种引用类联合使用，以便跟踪Java虚拟机回收所引用的对象的活动。


#### 1.2 为何需要4种引用



#### 1.3 引用类型有哪些
- 引用类型有哪些种
    - 强引用：默认的引用方式，不会被垃圾回收，JVM宁愿抛出OutOfMemory错误也不会回收这种对象。
    - 软引用（SoftReference）：如果一个对象只被软引用指向，只有内存空间不足够时，垃圾回收器才会回收它；
    - 弱引用（WeakReference）：如果一个对象只被弱引用指向，当JVM进行垃圾回收时，无论内存是否充足，都会回收该对象。
    - 虚引用（PhantomReference）：虚引用和前面的软引用、弱引用不同，它并不影响对象的生命周期。如果一个对象与虚引用关联，则跟没有引用与之关联一样，在任何时候都可能被垃圾回收器回收。虚引用通常和ReferenceQueue配合使用。


### 02.一般使用场景
#### 2.1 强引用场景
- 关于强引用引用的场景，直接new出来的对象
    - String str = new String("yc");
- 强引用介绍
    - 强引用是使用最普遍的引用。如果一个对象具有强引用，那垃圾回收器绝不会回收它。当内存空间不足，Java虚拟机宁愿抛出OutOfMemoryError错误，使程序异常终止，也不会靠随意回收具有强引用的对象来解决内存不足的问题。
    - 通过引用，可以对堆中的对象进行操作。在某个函数中，当创建了一个对象，该对象被分配在堆中，通过这个对象的引用才能对这个对象进行操作。
- 强引用的特点
    - 强引用可以直接访问目标对象。强引用可能导致内存泄露。注意相互引用情况。
- 如何回收强引用
    - 如果想中断强引用和某个对象之间的关联，可以显示地将引用赋值为null，这样一来的话，JVM在合适的时间就会回收该对象。
    - 看看Vector类的清理方法：在清除数据的时候，将数组中的每个元素都置为null，中断强引用与对象之间的关系，让GC的时候能够回收这些对象的内存。
    ```
    protected Object[] elementData;
    
    public synchronized void removeAllElements() {
        modCount++;
        // Let gc do its work
        for (int i = 0; i < elementCount; i++)
            elementData[i] = null;
    
        elementCount = 0;
    }
    ```
- 思考：将引用设置成null后在什么时候jvm回收对象呢？



#### 2.2 软引用场景
- 关于SoftReference软引用
    - SoftReference：软引用–>当虚拟机内存不足时，将会回收它指向的对象；需要获取对象时，可以调用get方法。
    - 可以通过java.lang.ref.SoftReference使用软引用。一个持有软引用的对象，不会被JVM很快回收，JVM会根据当前堆的使用情况来判断何时回收。当堆的使用率临近阈值时，才会回收软引用的对象。
- 软引用应用场景
    - 例如从网络上获取图片，然后将获取的图片显示的同时，通过软引用缓存起来。当下次再去网络上获取图片时，首先会检查要获取的图片缓存中是否存在，若存在，直接取出来，不需要再去网络上获取。
- 软引用的简单使用用法如下
    ``` java
    MyObject aRef = new  MyObject();
    SoftReference aSoftRef = new SoftReference(aRef);
    MyObject anotherRef = (MyObject)aSoftRef.get();
    ```
- 软引用的特点
    - 如果一个对象只具有软引用，那么如果内存空间足够，垃圾回收器就不会回收它；如果内存空间不足了，就会回收这些对象的内存。只要垃圾回收器没有回收它，该对象就可以被程序使用。
    - 软引用可用来实现内存敏感的高速缓存。软引用可以和一个引用队列（ReferenceQueue）联合使用，如果软引用所引用的对象被垃圾回收，Java虚拟机就会把这个软引用加入到与之关联的引用队列中。
- 如何回收软引用
    - 那么当这个SoftReference所软引用的aMyOhject被垃圾收集器回收的同时，ref所强引用的SoftReference对象被列入ReferenceQueue。也就是说，ReferenceQueue中保存的对象是Reference对象，而且是已经失去了它所软引用的对象的Reference对象。另外从ReferenceQueue这个名字也可以看出，它是一个队列，当我们调用它的poll()方法的时候，如果这个队列中不是空队列，那么将返回队列前面的那个Reference对象。
    - 在任何时候，我们都可以调用ReferenceQueue的poll()方法来检查是否有它所关心的非强可及对象被回收。如果队列为空，将返回一个null,否则该方法返回队列中前面的一个Reference对象。利用这个方法，我们可以检查哪个SoftReference所软引用的对象已经被回收。于是我们可以把这些失去所软引用的对象的SoftReference对象清除掉。
    - 常用的方式为
    ```
    SoftReference ref = null;
    while ((ref = (EmployeeRef) q.poll()) != null) {
        // 清除ref
    }
    ```
- 实际应用案例
    - 正常是用来处理图片这种占用内存大的情况
- 这样使用软引用好处
    - 通过软引用的get()方法，取得drawable对象实例的强引用，发现对象被未回收。在GC在内存充足的情况下，不会回收软引用对象。此时view的背景显示
    - 实际情况中,我们会获取很多图片，然后可能给很多个view展示, 这种情况下很容易内存吃紧导致oom,内存吃紧，系统开始会GC。这次GC后，drawables.get()不再返回Drawable对象，而是返回null，这时屏幕上背景图不显示，说明在系统内存紧张的情况下，软引用被回收。
    - 使用软引用以后，在OutOfMemory异常发生之前，这些缓存的图片资源的内存空间可以被释放掉的，从而避免内存达到上限，避免Crash发生。
- 注意避免软引用获取对象为null
    - 在垃圾回收器对这个Java对象回收前，SoftReference类所提供的get方法会返回Java对象的强引用，一旦垃圾线程回收该Java对象之后，get方法将返回null。所以在获取软引用对象的代码中，一定要判断是否为null，以免出现NullPointerException异常导致应用崩溃。


#### 2.3 弱引用场景
- 关于WeakReference弱引用
- WeakReference
    - 弱引用–>随时可能会被垃圾回收器回收，不一定要等到虚拟机内存不足时才强制回收。要获取对象时，同样可以调用get方法。
- 特点
    - 如果一个对象只具有弱引用，那么在垃圾回收器线程扫描的过程中，一旦发现了只具有弱引用的对象，不管当前内存空间足够与否，都会回收它的内存。不过，由于垃圾回收器是一个优先级很低的线程，因此不一定会很快发现那些只具有弱引用的对象。
    - 弱引用也可以和一个引用队列（ReferenceQueue）联合使用，如果弱引用所引用的对象被垃圾回收，Java虚拟机就会把这个弱引用加入到与之关联的引用队列中。
- 防止内存泄漏，要保证内存被虚拟机回收
    - 为什么handler会造成内存泄漏？这种情况就是由于android的特殊机制造成的：当一个android主线程被创建的时候，同时会有一个Looper对象被创建，而这个Looper对象会实现一个MessageQueue(消息队列)，当我们创建一个handler对象时，而handler的作用就是放入和取出消息从这个消息队列中，每当我们通过handler将一个msg放入消息队列时，这个msg就会持有一个handler对象的引用。因此当Activity被结束后，这个msg在被取出来之前，这msg会继续存活，但是这个msg持有handler的引用，而handler在Activity中创建，会持有Activity的引用，因而当Activity结束后，Activity对象并不能够被gc回收，因而出现内存泄漏。
- 根本原因
    - Activity在被结束之后，MessageQueue并不会随之被结束，如果这个消息队列中存在msg，则导致持有handler的引用，但是又由于Activity被结束了，msg无法被处理，从而导致永久持有handler对象，handler永久持有Activity对象，于是发生内存泄漏。但是为什么为static类型就会解决这个问题呢？因为在java中所有非静态的对象都会持有当前类的强引用，而静态对象则只会持有当前类的弱引用。声明为静态后，handler将会持有一个Activity的弱引用，而弱引用会很容易被gc回收，这样就能解决Activity结束后，gc却无法回收的情况。当然解决源头还是在Activity退出的时候，移除Handler内部消息队列的数据。
- 解决办法，采用弱引用管理handler，代码如下所示
    ``` java
    private MyHandler handler = new MyHandler(this);
    private static class MyHandler extends Handler{
        WeakReference<FirstActivity> weakReference;
        MyHandler(FirstActivity activity) {
            weakReference = new WeakReference<>(activity);
        }
    
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what){
            }
        }
    }
    ```



#### 2.4 虚引用场景
- 关于PhantomReference类虚引用
    - 虚引用是所有引用类型中最弱的一个。一个持有虚引用的对象，和没有引用几乎是一样的，随时都可能被垃圾回收器回收。当试图通过虚引用的get()方法取得强引用时，总是会失败。并且，虚引用必须和引用队列一起使用，它的作用在于跟踪垃圾回收过程。 当垃圾回收器准备回收一个对象时，如果发现它还有虚引用，就会在垃圾回收后，销毁这个对象，奖这个虚引用加入引用队列。
- Android实际开发中没有用到过
    - 貌似开发中没有接触过虚引用


#### 2.5 四种引用比较
- 弱引用和软引用区别
    - 只具有弱引用的对象拥有更短暂的生命周期，可能随时被回收。而只具有软引用的对象只有当内存不够的时候才被回收，在内存足够的时候，通常不被回收。
- 使用软引用或者弱引用防止内存泄漏
    - 在Android应用的开发中，为了防止内存溢出，在处理一些占用内存大而且声明周期较长的对象时候，可以尽量应用软引用和弱引用技术。
    - 软引用，弱引用都非常适合来保存那些可有可无的缓存数据。如果这样做，当系统内存不足时，这些缓存数据会被回收，不会导致内存溢出。而当内存资源充足时，这些缓存数据又可以存在相当长的时间。
- 到底什么时候使用软引用，什么时候使用弱引用呢？
    - 个人认为，如果只是想避免OutOfMemory异常的发生，则可以使用软引用。如果对于应用的性能更在意，想尽快回收一些占用内存比较大的对象，则可以使用弱引用。
    - 还有就是可以根据对象是否经常使用来判断。如果该对象可能会经常使用的，就尽量用软引用。如果该对象不被使用的可能性更大些，就可以用弱引用。
    - 另外，和弱引用功能类似的是WeakHashMap。WeakHashMap对于一个给定的键，其映射的存在并不阻止垃圾回收器对该键的回收，回收以后，其条目从映射中有效地移除。WeakHashMap使用ReferenceQueue实现的这种机制。



### 03.引用原理说明
#### 3.0 引用回收的流程
- 带完善


#### 3.1 看看Reference的源代码
- 源码说明：
    - 看到Reference除了带有对象引用referent的构造函数，还有一个带有ReferenceQueue参数的构造函数。那么这个ReferenceQueue用来做什么呢？
    - 需要我们从enqueue这个函数来开始分析。当系统要回收Reference持有的对象引用referent的时候，Reference的enqueue函数会被调用，而在这个函数中调用了ReferenceQueue的enqueue函数。
    - 那么我们来看看ReferenceQueue的enqueue函数做了什么？
- **看看这段源代码**
    ```java
    public abstract class Reference<T> {
    
        private static boolean disableIntrinsic = false;
        private static boolean slowPathEnabled = false;
        volatile T referent;         /* Treated specially by GC */
        final ReferenceQueue<? super T> queue;
        Reference queueNext;
        Reference<?> pendingNext;
    
        //返回此引用对象的引用。如果这个引用对象有由程序或垃圾收集器清除，然后此方法返回
        public T get() {
            return getReferent();
        }
    
        private final native T getReferent();
    
        //清除此引用对象。调用此方法不会将对象加入队列
        public void clear() {
            this.referent = null;
        }
    
        //是否引用对象已进入队列，由程序或垃圾收集器。
        //如果该引用对象在创建队列时没有注册，则该方法将始终返回
        public boolean isEnqueued() {
            return queue != null && queue.isEnqueued(this);
        }
    
        //添加引用对象到其注册的队列，如果他的方法是通过java代码调用
        public boolean enqueue() {
           return queue != null && queue.enqueue(this);
        }
    
        Reference(T referent) {
            this(referent, null);
        }
    
        Reference(T referent, ReferenceQueue<? super T> queue) {
            this.referent = referent;
            this.queue = queue;
        }
    }
    ```



#### 3.2 看看ReferenceQueue的enqueue函数
- **源码说明**
    - 可以看到首先获取同步锁，然后调用了enqueueLocked(Reference)函数
- **看看这段代码**
    - 可以看到这里用了synchronized同步锁，然后调用了对象的 notifyAll()方法（唤醒所有 wait 线程），notifyAll会将该对象等待池内的所有线程移动到锁池中，等待锁竞争。
    ``` java
    boolean enqueue(Reference<? extends T> reference) {
        synchronized (lock) {
            //将给定的引用加入这个队列
            if (enqueueLocked(reference)) {
                //然后
                lock.notifyAll();
                return true;
            }
            return false;
        }
    }
    ```


#### 3.3 看看ReferenceQueue的enqueueLocked(Reference)函数
- **源码说明**
    - 通过 enqueueLocked函数可以看到ReferenceQueue维护了一个队列（链表结构），而enqueue这一系列函数就是将reference添加到这个队列（链表）中
- **看看这段代码**
    ``` java
    private boolean enqueueLocked(Reference<? extends T> r) {
        // Verify the reference has not already been enqueued.
        if (r.queueNext != null) {
            return false;
        }
    
        if (r instanceof Cleaner) {
            // If this reference is a Cleaner, then simply invoke the clean method instead
            // of enqueueing it in the queue. Cleaners are associated with dummy queues that
            // are never polled and objects are never enqueued on them.
            Cleaner cl = (sun.misc.Cleaner) r;
            cl.clean();
    
            // Update queueNext to indicate that the reference has been
            // enqueued, but is now removed from the queue.
            r.queueNext = sQueueNextUnenqueued;
            return true;
        }
    
        if (tail == null) {
            head = r;
        } else {
            tail.queueNext = r;
        }
        tail = r;
        tail.queueNext = r;
        return true;
    }
    ```


#### 3.4 接着看看ReferenceQueue.isEnqueued()代码
- **让我们回到Reference源码中**
    - 可以看到除了enqueue这个函数还有一个isEnqueued函数，同样这个函数调用了ReferenceQueue的同名函数，源码如下：
    ``` java
    boolean isEnqueued(Reference<? extends T> reference) {
        synchronized (lock) {
            return reference.queueNext != null && reference.queueNext != sQueueNextUnenqueued;
        }
    }
    ```
- **源码分析说明**
    - 可以看到先获取同步锁，然后判断该reference是否在队列（链表）中。由于enqueue和isEnqueue函数都要申请同步锁，所以这是线程安全的。
    - 这里要注意“reference.queueNext != sQueueNextUnenqueued”用于判断该Reference是否是一个Cleaner类，在上面ReferenceQueue的enqueueLocked函数中我们可以看到如果一个Reference是一个Cleaner，则调用它的clean方法，同时并不加入链表，并且将其queueNext设置为sQueueNextUnequeued，这是一个空的虚引用


#### 3.5 那么enqueueLocked(Reference)函数中的Cleaner是做什么的
- 在stackoverflow网站中找到这个解释
    * sun.misc.Cleaner是JDK内部提供的用来释放非堆内存资源的API。JVM只会帮我们自动释放堆内存资源，但是它提供了回调机制，通过这个类能方便的释放系统的其他资源。
    * 可以看到Cleaner是用于释放非堆内存的，所以做特殊处理。
    * 通过enqueue和isEnqueue两个函数的分析，ReferenceQueue队列维护了那些被回收对象referent的Reference的引用，这样通过isEnqueue就可以判断对象referent是否已经被回收，用于一些情况的处理。


#### 3.6 软引用SoftReference源码
- **源码如下所示**
    ```java
    public class SoftReference<T> extends Reference<T> { 
        static private long clock; 
        private long timestamp; 
        public SoftReference(T referent) { 
            super(referent); 
            this.timestamp = clock; 
        } 
        public SoftReference(T referent, ReferenceQueue<? super T> q) { 
            super(referent, q); 
            this.timestamp = clock; 
        } 
        public T get() { 
            T o = super.get(); 
            if (o != null && this.timestamp != clock) 
                this.timestamp = clock; 
            return o; 
        } 
    } 
    ```
- **关于这段源码分析**
    - 可以看到SoftReference有一个类变量clock和一个变量timestamp，这两个参数对于SoftReference至关重要。
        * clock：记录了上一次GC的时间。这个变量由GC（garbage collector）来改变。
        * timestamp：记录对象被访问（get函数）时最近一次GC的时间。
    - 那么这两个参数有什么用？
        * 我们知道软引用是当内存不足时可以回收的。但是这只是大致情况，实际上软应用的回收有一个条件：
        * clock - timestamp <= free_heap * ms_per_mb
        * free_heap是JVM Heap的空闲大小，单位是MB
        * ms_per_mb单位是毫秒，是每MB空闲允许保留软引用的时间。Sun JVM可以通过参数-XX:SoftRefLRUPolicyMSPerMB进行设置
    - 举个栗子：
        * 目前有3MB的空闲，ms_per_mb为1000，这时如果clock和timestamp分别为5000和2000，那么
        * 5000 - 2000 <= 3 * 1000
        * 条件成立，则该次GC不对该软引用进行回收。
        * 所以每次GC时，通过上面的条件去判断软应用是否可以回收并进行回收，即我们通常说的内存不足时被回收。



#### 3.7 弱引用WeakReference源码
- **源码分析说明**
    - 没有其他代码，GC时被回收掉。
- **源码如下所示**
    ```java
    public class WeakReference<T> extends Reference<T> { 
        public WeakReference(T referent) { 
            super(referent); 
        } 
        public WeakReference(T referent, ReferenceQueue<? super T> q) { 
            super(referent, q); 
        } 
    } 
    ```


#### 3.8 虚引用PhantomReference源码
- **源码分析说明**
    - 可以看到get函数返回null，正如前面说得虚引用无法获取对象引用。（注意网上有些文章说虚引用不持有对象的引用，这是有误的，通过构造函数可以看到虚引用是持有对象引用的，但是无法获取该引用
    - 同时可以看到虚引用只有一个构造函数，所以必须传入ReferenceQueue对象。
    - 前面提到虚引用的作用是判断对象是否被回收，这个功能正是通过ReferenceQueue实现的。
    - 这里注意：不仅仅是虚引用可以判断回收，弱引用和软引用同样实现了带有ReferenceQueue的构造函数，如果创建时传入了一个ReferenceQueue对象，同样也可以判断。
- **源码如下所示**
    ```java
    public class PhantomReference<T> extends Reference<T> { 
        public T get() { 
            return null; 
        } 
        public PhantomReference(T referent, ReferenceQueue<? super T> q) { 
            super(referent, q); 
        } 
    }
    ```












