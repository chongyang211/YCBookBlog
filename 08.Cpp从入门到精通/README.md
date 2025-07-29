# 专栏笔记总结大全



## 参考博客

黑马程序员：https://space.bilibili.com/37974444

黑马程序员匠心之作|C++教程从0到1入门编程,学习编程不再难 ：https://www.bilibili.com/video/av41559729/

- **书籍**：
    - 《C++ Primer》
    - 《Effective C++》
    - 《The C++ Programming Language》（Bjarne Stroustrup 著）
- **在线教程**：
    - [C++ 官方文档](https://isocpp.org/)
    - [Learn C++](https://www.learncpp.com/)
- **开发工具**：
    - 编译器：GCC、Clang、MSVC。
    - IDE：Visual Studio、Code::Blocks、CLion。


--------------------------------------------------------------------------------------------------



--------------------------------------------------------------------------------------------------


#### 11.2 修改职工函数实现





#### 12.3 测试修改职工

在main函数分支 4  选项中，调用修改职工接口

![1546502651922](assets/1546502651922.png)



测试1 - 修改不存在职工情况

![1546502759643](assets/1546502759643.png)

测试2 - 修改存在职工情况，例如将职工 "李四" 改为 "赵四"

![1546502830350](assets/1546502830350.png)

修改后再次查看所有职工信息，并确认修改成功

![1546502865443](assets/1546502865443.png)

再次确认文件中信息也同步更新

![1546502898653](assets/1546502898653.png)

至此，修改职工功能已实现！











## 12.查找职工

功能描述：提供两种查找职工方式，一种按照职工编号，一种按照职工姓名

#### 12.1 查找职工函数声明

在workerManager.h中添加成员函数  `void Find_Emp();`

```c++
	//查找职工
	void Find_Emp();
```



#### 12.2 查找职工函数实现

在workerManager.cpp中实现成员函数 ` void Find_Emp();`

```C++
//查找职工
void WorkerManager::Find_Emp()
{
	if (this->m_FileIsEmpty)
	{
		cout << "文件不存在或记录为空！" << endl;
	}
	else
	{
		cout << "请输入查找的方式：" << endl;
		cout << "1、按职工编号查找" << endl;
		cout << "2、按姓名查找" << endl;

		int select = 0;
		cin >> select;


		if (select == 1) //按职工号查找
		{
			int id;
			cout << "请输入查找的职工编号：" << endl;
			cin >> id;

			int ret = IsExist(id);
			if (ret != -1)
			{
				cout << "查找成功！该职工信息如下：" << endl;
				this->m_EmpArray[ret]->showInfo();
			}
			else
			{
				cout << "查找失败，查无此人" << endl;
			}
		}
		else if(select == 2) //按姓名查找
		{
			string name;
			cout << "请输入查找的姓名：" << endl;
			cin >> name;

			bool flag = false;  //查找到的标志
			for (int i = 0; i < m_EmpNum; i++)
			{
				if (m_EmpArray[i]->m_Name == name)
				{
					cout << "查找成功,职工编号为："
                           << m_EmpArray[i]->m_Id
                           << " 号的信息如下：" << endl;
					
					flag = true;

					this->m_EmpArray[i]->showInfo();
				}
			}
			if (flag == false)
			{
				//查无此人
				cout << "查找失败，查无此人" << endl;
			}
		}
		else
		{
			cout << "输入选项有误" << endl;
		}
	}


	system("pause");
	system("cls");
}
```





#### 12.3 测试查找职工

在main函数分支 5  选项中，调用查找职工接口

![1546504714318](assets/1546504714318.png)

测试1 - 按照职工编号查找 - 查找不存在职工

![1546504767229](assets/1546504767229.png)

测试2 - 按照职工编号查找 -  查找存在职工

![1546505046521](assets/1546505046521.png)

测试3 - 按照职工姓名查找 - 查找不存在职工

![1546505115610](assets/1546505115610.png)



测试4 - 按照职工姓名查找 - 查找存在职工（如果出现重名，也一并显示，在文件中可以添加重名职工）

例如 添加两个王五的职工，然后按照姓名查找王五

![1546507850441](assets/1546507850441.png)

![1546507760284](assets/1546507760284.png)

至此，查找职工功能实现完毕！







## 13.职工排序

功能描述：按照职工编号进行排序，排序的顺序由用户指定

#### 13.1 排序函数声明

在workerManager.h中添加成员函数  `void Sort_Emp();`

```C++
	//排序职工
	void Sort_Emp();
```



#### 13.2 排序函数实现

在workerManager.cpp中实现成员函数 ` void Sort_Emp();`

```C++
//排序职工
void WorkerManager::Sort_Emp()
{
	if (this->m_FileIsEmpty)
	{
		cout << "文件不存在或记录为空！" << endl;
		system("pause");
		system("cls");
	}
	else
	{
		cout << "请选择排序方式： " << endl;
		cout << "1、按职工号进行升序" << endl;
		cout << "2、按职工号进行降序" << endl;

		int select = 0;
		cin >> select;


		for (int i = 0; i < m_EmpNum; i++)
		{
			int minOrMax = i;
			for (int j = i + 1; j < m_EmpNum; j++)
			{
				if (select == 1) //升序
				{
					if (m_EmpArray[minOrMax]->m_Id > m_EmpArray[j]->m_Id)
					{
						minOrMax = j;
					}
				}
				else  //降序
				{
					if (m_EmpArray[minOrMax]->m_Id < m_EmpArray[j]->m_Id)
					{
						minOrMax = j;
					}
				}
			}

			if (i != minOrMax)
			{
				Worker * temp = m_EmpArray[i];
				m_EmpArray[i] = m_EmpArray[minOrMax];
				m_EmpArray[minOrMax] = temp;
			}

		}

		cout << "排序成功,排序后结果为：" << endl;
		this->save();
		this->Show_Emp();
	}

}
```





#### 13.3 测试排序功能

在main函数分支 6  选项中，调用排序职工接口

![1546510145181](assets/1546510145181.png)

测试：

首先我们添加一些职工，序号是无序的，例如：

![1546658169987](assets/1546658169987.png)



测试 - 升序排序

![1546658190479](assets/1546658190479.png)

文件同步更新

![1546658273581](assets/1546658273581.png)





测试 - 降序排序

![1546658288936](assets/1546658288936.png)

文件同步更新

![1546658313704](assets/1546658313704.png)

至此，职工按照编号排序的功能实现完毕！











## 14.清空文件

功能描述：将文件中记录数据清空



#### 14.1 清空函数声明

在workerManager.h中添加成员函数  `void Clean_File();`

```C++
	//清空文件
	void Clean_File();
```





#### 14.2 清空函数实现

在workerManager.cpp中实现员函数 ` void Clean_File();`

```C++
//清空文件
void WorkerManager::Clean_File()
{
	cout << "确认清空？" << endl;
	cout << "1、确认" << endl;
	cout << "2、返回" << endl;

	int select = 0;
	cin >> select;

	if (select == 1)
	{
		//打开模式 ios::trunc 如果存在删除文件并重新创建
		ofstream ofs(FILENAME, ios::trunc);
		ofs.close();

		if (this->m_EmpArray != NULL)
		{
            for (int i = 0; i < this->m_EmpNum; i++)
			{
				if (this->m_EmpArray[i] != NULL)
				{
					delete this->m_EmpArray[i];
				}
			}
			this->m_EmpNum = 0;
			delete[] this->m_EmpArray;
			this->m_EmpArray = NULL;
			this->m_FileIsEmpty = true;
		}
		cout << "清空成功！" << endl;
	}

	system("pause");
	system("cls");
}
```





#### 14.3 测试清空文件

在main函数分支 7  选项中，调用清空文件接口

![1546511085541](assets/1546511085541.png)

测试：确认清空文件

![1546510976745](assets/1546510976745.png)

再次查看文件中数据，记录已为空

![1546510994196](assets/1546510994196.png)

打开文件，里面数据已确保清空，该功能需要慎用！

![1546511018517](assets/1546511018517.png)



随着清空文件功能实现，本案例制作完毕  ^ _ ^






--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------

在 C++ 中，**`std::mutex`** 是标准库提供的一种互斥锁（Mutex），用于保护共享资源，避免多个线程同时访问导致的数据竞争问题。`std::mutex` 是 C++11 引入的，是多线程编程中最基本的同步工具之一。

---

### **1. `std::mutex` 的基本用法**
`std::mutex` 提供了以下主要方法：
- `lock()`：加锁，如果锁已被其他线程持有，则当前线程会阻塞。
- `unlock()`：解锁，释放锁。
- `try_lock()`：尝试加锁，如果锁已被其他线程持有，则立即返回 `false`，否则返回 `true`。

#### **1.1 手动加锁和解锁**
```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int sharedData = 0;

void increment() {
    for (int i = 0; i < 1000; ++i) {
        mtx.lock(); // 加锁
        ++sharedData;
        mtx.unlock(); // 解锁
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Shared Data: " << sharedData << std::endl; // 输出: Shared Data: 2000
    return 0;
}
```

#### **1.2 使用 `try_lock()`**
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        while (!mtx.try_lock()) { // 尝试加锁
            std::this_thread::yield(); // 让出 CPU
        }
        ++sharedData;
        mtx.unlock(); // 解锁
    }
}
```

---

### **2. 自动管理锁**
为了避免忘记解锁或异常情况下未解锁，C++ 提供了 `std::lock_guard` 和 `std::unique_lock` 来自动管理锁的生命周期。

#### **2.1 `std::lock_guard`**
- 在构造时加锁，在析构时解锁。
- 适用于简单的加锁场景。

示例：
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // 自动加锁和解锁
        ++sharedData;
    }
}
```

#### **2.2 `std::unique_lock`**
- 比 `std::lock_guard` 更灵活，支持手动加锁和解锁。
- 适用于需要延迟加锁或条件变量的场景。

示例：
```cpp
void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::unique_lock<std::mutex> lock(mtx); // 自动加锁
        ++sharedData;
        lock.unlock(); // 手动解锁
    }
}
```

---

### **3. 死锁问题**
死锁是指多个线程互相等待对方释放锁，导致程序无法继续执行。使用 `std::mutex` 时需要注意避免死锁。

#### **3.1 死锁示例**
```cpp
std::mutex mtx1, mtx2;

void thread1() {
    mtx1.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mtx2.lock(); // 等待 mtx2
    mtx2.unlock();
    mtx1.unlock();
}

void thread2() {
    mtx2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mtx1.lock(); // 等待 mtx1
    mtx1.unlock();
    mtx2.unlock();
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);
    t1.join();
    t2.join();
    return 0;
}
```

#### **3.2 避免死锁**
- 按固定顺序加锁。
- 使用 `std::lock()` 同时锁定多个互斥锁。

示例：
```cpp
void thread1() {
    std::lock(mtx1, mtx2); // 同时锁定
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // 操作共享资源
}

void thread2() {
    std::lock(mtx1, mtx2); // 同时锁定
    std::lock_guard<std::mutex> lock1(mtx1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mtx2, std::adopt_lock);
    // 操作共享资源
}
```

---

### **4. 递归互斥锁 `std::recursive_mutex`**
`std::recursive_mutex` 是一种特殊的互斥锁，允许同一个线程多次加锁。

#### **4.1 使用场景**
- 当某个函数可能递归调用自身，并且需要加锁时。

示例：
```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::recursive_mutex rmtx;

void recursiveFunction(int n) {
    std::lock_guard<std::recursive_mutex> lock(rmtx); // 递归加锁
    if (n > 0) {
        std::cout << "n = " << n << std::endl;
        recursiveFunction(n - 1);
    }
}

int main() {
    std::thread t(recursiveFunction, 3);
    t.join();
    return 0;
}
```

---

### **5. 超时互斥锁 `std::timed_mutex`**
`std::timed_mutex` 是一种支持超时加锁的互斥锁，提供了 `try_lock_for()` 和 `try_lock_until()` 方法。

#### **5.1 使用 `try_lock_for()`**
```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::timed_mutex tmtx;

void tryLockFunction() {
    if (tmtx.try_lock_for(std::chrono::milliseconds(100))) { // 尝试加锁，最多等待 100ms
        std::cout << "Lock acquired!" << std::endl;
        tmtx.unlock();
    } else {
        std::cout << "Failed to acquire lock!" << std::endl;
    }
}

int main() {
    std::thread t1(tryLockFunction);
    std::thread t2(tryLockFunction);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **6. 总结**
- `std::mutex` 是 C++ 中最基本的互斥锁，用于保护共享资源。
- 使用 `std::lock_guard` 或 `std::unique_lock` 可以自动管理锁的生命周期。
- 避免死锁的方法包括按固定顺序加锁和使用 `std::lock()`。
- `std::recursive_mutex` 允许同一个线程多次加锁。
- `std::timed_mutex` 支持超时加锁。

合理使用 `std::mutex` 可以确保多线程程序的正确性和性能。

--------------------------------------------------------------------------------------------------



### **3. 线程同步**
多线程访问共享资源时，需要使用同步机制（如互斥锁、条件变量）避免数据竞争。

#### **3.1 使用 `std::mutex` 保护共享资源**
```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int sharedData = 0;

void increment() {
    for (int i = 0; i < 1000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // 自动加锁和解锁
        ++sharedData;
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << "Shared Data: " << sharedData << std::endl; // 输出: Shared Data: 2000
    return 0;
}
```

#### **3.2 使用 `std::condition_variable` 实现线程间通信**
```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void waitForReady() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return ready; }); // 等待条件成立
    std::cout << "Ready!" << std::endl;
}

void setReady() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one(); // 通知等待的线程
}

int main() {
    std::thread t1(waitForReady);
    std::thread t2(setReady);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **4. 线程局部存储**
使用 `thread_local` 关键字声明线程局部变量，每个线程拥有独立的变量副本。

#### **4.1 使用 `thread_local`**
```cpp
#include <iostream>
#include <thread>

thread_local int threadLocalData = 0;

void threadFunction(int id) {
    threadLocalData = id;
    std::cout << "Thread " << id << " has data: " << threadLocalData << std::endl;
}

int main() {
    std::thread t1(threadFunction, 1);
    std::thread t2(threadFunction, 2);
    t1.join();
    t2.join();
    return 0;
}
```

---

### **5. 线程池**
C++ 标准库没有直接提供线程池，但可以使用第三方库（如 [ThreadPool](https://github.com/progschj/ThreadPool)）或手动实现。

#### **5.1 简单线程池实现**
```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return !tasks.empty() || stop; });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template <class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};

int main() {
    ThreadPool pool(4);

    for (int i = 0; i < 8; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i << " is running on thread " << std::this_thread::get_id() << std::endl;
        });
    }

    return 0;
}
```

---

### **6. 总结**
- 使用 `std::thread` 创建和管理线程。
- 使用 `join()` 或 `detach()` 控制线程的生命周期。
- 使用 `std::mutex` 和 `std::condition_variable` 实现线程同步。
- 使用 `thread_local` 声明线程局部变量。
- 线程池可以提高多线程任务的效率。

通过合理使用线程，可以编写出高效、并发的 C++ 程序。


--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------



--------------------------------------------------------------------------------------------------


---

### **2. 虚函数**
虚函数是实现动态多态的关键。通过在基类中声明虚函数，派生类可以重写该函数，从而实现多态。

#### **2.1 虚函数的声明**
```cpp
virtual 返回类型 函数名(参数列表);
```

#### **2.2 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    virtual void speak() { // 虚函数
        cout << "Animal speaks." << endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Dog barks." << endl;
    }
};

// 派生类
class Cat : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Cat meows." << endl;
    }
};

int main() {
    Animal *animal1 = new Dog(); // 基类指针指向派生类对象
    Animal *animal2 = new Cat(); // 基类指针指向派生类对象

    animal1->speak(); // 调用 Dog 的 speak()
    animal2->speak(); // 调用 Cat 的 speak()

    delete animal1;
    delete animal2;
    return 0;
}
```

**输出**：
```
Dog barks.
Cat meows.
```

---

### **3. 纯虚函数与抽象类**
- **纯虚函数**：在基类中声明但没有实现的虚函数，派生类必须重写纯虚函数。
- **抽象类**：包含纯虚函数的类，不能实例化，只能作为基类。

#### **3.1 纯虚函数的声明**
```cpp
virtual 返回类型 函数名(参数列表) = 0;
```

#### **3.2 示例**
```cpp
#include <iostream>
using namespace std;

// 抽象类
class Animal {
public:
    virtual void speak() = 0; // 纯虚函数
};

// 派生类
class Dog : public Animal {
public:
    void speak() override {
        cout << "Dog barks." << endl;
    }
};

// 派生类
class Cat : public Animal {
public:
    void speak() override {
        cout << "Cat meows." << endl;
    }
};

int main() {
    Animal *animal1 = new Dog(); // 基类指针指向派生类对象
    Animal *animal2 = new Cat(); // 基类指针指向派生类对象

    animal1->speak(); // 调用 Dog 的 speak()
    animal2->speak(); // 调用 Cat 的 speak()

    delete animal1;
    delete animal2;
    return 0;
}
```

**输出**：
```
Dog barks.
Cat meows.
```



--------------------------------------------------------------------------------------------------

在 C++ 中，**虚函数** 是实现 **多态** 的关键机制。它允许派生类重写基类的函数，并通过基类的指针或引用调用派生类的实现。虚函数的核心在于 **动态绑定**，即在运行时根据对象的实际类型决定调用哪个函数。

以下是关于 C++ 虚函数的详细说明和示例：

---

### **1. 虚函数的基本概念**
- **虚函数**：在基类中使用 `virtual` 关键字声明的函数，派生类可以重写该函数。
- **动态绑定**：在运行时根据对象的实际类型调用相应的函数，而不是根据指针或引用的类型。
- **多态**：通过虚函数实现“一个接口，多种实现”的效果。

---

### **2. 虚函数的声明与使用**
#### **2.1 虚函数的声明**
在基类中使用 `virtual` 关键字声明虚函数：
```cpp
virtual 返回类型 函数名(参数列表);
```

#### **2.2 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Animal {
public:
    virtual void speak() { // 虚函数
        cout << "Animal speaks." << endl;
    }
};

// 派生类
class Dog : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Dog barks." << endl;
    }
};

// 派生类
class Cat : public Animal {
public:
    void speak() override { // 重写基类的虚函数
        cout << "Cat meows." << endl;
    }
};

int main() {
    Animal *animal1 = new Dog(); // 基类指针指向派生类对象
    Animal *animal2 = new Cat(); // 基类指针指向派生类对象

    animal1->speak(); // 调用 Dog 的 speak()
    animal2->speak(); // 调用 Cat 的 speak()

    delete animal1;
    delete animal2;
    return 0;
}
```

**输出**：
```
Dog barks.
Cat meows.
```

---

### **3. 虚函数的工作原理**
- 当基类中的函数被声明为虚函数时，编译器会为该类生成一个 **虚函数表（vtable）**。
- 虚函数表是一个存储虚函数地址的数组，每个对象在运行时通过虚函数表查找要调用的函数。
- 派生类重写虚函数时，会更新虚函数表中的函数地址。

---

### **4. 纯虚函数与抽象类**
- **纯虚函数**：在基类中声明但没有实现的虚函数，派生类必须重写纯虚函数。
- **抽象类**：包含纯虚函数的类，不能实例化，只能作为基类。

#### **4.1 纯虚函数的声明**
```cpp
virtual 返回类型 函数名(参数列表) = 0;
```

#### **4.2 示例**
```cpp
#include <iostream>
using namespace std;

// 抽象类
class Shape {
public:
    virtual void draw() = 0; // 纯虚函数
};

// 派生类
class Circle : public Shape {
public:
    void draw() override {
        cout << "Drawing a circle." << endl;
    }
};

// 派生类
class Square : public Shape {
public:
    void draw() override {
        cout << "Drawing a square." << endl;
    }
};

int main() {
    Shape *shape1 = new Circle(); // 基类指针指向派生类对象
    Shape *shape2 = new Square(); // 基类指针指向派生类对象

    shape1->draw(); // 调用 Circle 的 draw()
    shape2->draw(); // 调用 Square 的 draw()

    delete shape1;
    delete shape2;
    return 0;
}
```

**输出**：
```
Drawing a circle.
Drawing a square.
```

---

### **5. 虚析构函数**
当基类的指针指向派生类对象时，如果基类的析构函数不是虚函数，则只会调用基类的析构函数，导致派生类的资源泄漏。因此，基类的析构函数应声明为虚函数。

#### **5.1 示例**
```cpp
#include <iostream>
using namespace std;

// 基类
class Base {
public:
    virtual ~Base() { // 虚析构函数
        cout << "Base destructor." << endl;
    }
};

// 派生类
class Derived : public Base {
public:
    ~Derived() {
        cout << "Derived destructor." << endl;
    }
};

int main() {
    Base *ptr = new Derived(); // 基类指针指向派生类对象
    delete ptr; // 调用派生类的析构函数
    return 0;
}
```

**输出**：
```
Derived destructor.
Base destructor.
```

---

### **6. 虚函数的限制**
- 虚函数不能是静态函数（`static`）。
- 构造函数不能是虚函数。
- 虚函数可以是内联函数（`inline`），但一旦被重写，内联特性将失效。

---

### **7. 总结**
- **虚函数** 是实现多态的关键，允许派生类重写基类的函数。
- **动态绑定** 在运行时根据对象的实际类型调用相应的函数。
- **纯虚函数** 和 **抽象类** 用于定义接口，派生类必须实现纯虚函数。
- **虚析构函数** 确保派生类的析构函数被正确调用，避免资源泄漏。

通过掌握虚函数的使用，你可以设计出更灵活、更可扩展的面向对象程序！


--------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------------------


第 1章 预备知识 1
1．1　C++简介　1
1．2　C++简史　2
1．2．1　C语言　2
1．2．2　C语言编程原理　2
1．2．3　面向对象编程　3
1．2．4　C++和泛型编程　3
1．2．5　C++的起源　4
1．3　可移植性和标准　5
1．3．1　C++的发展　5
1．3．2　本书遵循的C++标准　6
1．4　程序创建的技巧　6
1．4．1　创建源代码文件　6
1．4．2　编译和链接　7
1．5　总结　10
第　2章 开始学习C++　11
2．1　进入C++　11
2．1．1　main( )函数　12
2．1．2　C++注释　14
2．1．3　C++预处理器和iostream文件　14
2．1．4　头文件名　15
2．1．5　名称空间　15
2．1．6　使用cout进行C++输出　16
2．1．7　C++源代码的格式化　17
2．2　C++语句　18
2．2．1　声明语句和变量　19
2．2．2　赋值语句　20
2．2．3　cout的新花样　20
2．3　其他C++语句　21
2．3．1　使用cin　21
2．3．2　使用cout进行拼接　21
2．3．3　类简介　22
2．4　函数　23
2．4．1　使用有返回值的函数　23
2．4．2　函数变体　25
2．4．3　用户定义的函数　26
2．4．4　用户定义的有返回值的函数　28
2．4．5　在多函数程序中使用using编译指令　29
2．5　总结　30
2．6　复习题　30
2．7　编程练习　30
第3章　处理数据　32
3．1　简单变量　32
3．1．1　变量名　33
3．1．2　整型　34
3．1．3　整型short、int、long和long long　34
3．1．4　无符号类型　37
3．1．5　选择整型类型　38
3．1．6　整型字面值　39
3．1．7　C++如何确定常量的类型　40
3．1．8　char类型：字符和小整数　41
3．1．9　bool类型　46
3．2　const限定符　46
3．3　浮点数　47
3．3．1　书写浮点数　47
3．3．2　浮点类型　48
3．3．3　浮点常量　49
3．3．4　浮点数的优缺点　49
3．4　C++算术运算符　50
3．4．1　运算符优先级和结合性　51
3．4．2　除法分支　51
3．4．3　求模运算符　52
3．4．4　类型转换　53
3．4．5　C++11中的auto声明　56
3．5　总结　57
3．6　复习题　57
3．7　编程练习　58
第4章　复合类型　59
4．1　数组　59
4．1．1　程序说明　61
4．1．2　数组的初始化规则　61
4．1．3　C++11数组初始化方法　62
4．2　字符串　62
4．2．1　拼接字符串常量　63
4．2．2　在数组中使用字符串　63
4．2．3　字符串输入　64
4．2．4　每次读取一行字符串输入　65
4．2．5　混合输入字符串和数字　67
4．3　string类简介　68
4．3．1　C++11字符串初始化　69
4．3．2　赋值、拼接和附加　69
4．3．3　string类的其他操作　70
4．3．4　string类I/O　71
4．3．5　其他形式的字符串字面值　72
4．4　结构简介　73
4．4．1　在程序中使用结构　74
4．4．2　C++11结构初始化　75
4．4．3　结构可以将string类作为成员吗　75
4．4．4　其他结构属性　76
4．4．5　结构数组　77
4．4．6　结构中的位字段　78
4．5　共用体　78
4．6　枚举　79
4．6．1　设置枚举量的值　80
4．6．2　枚举的取值范围　80
4．7　指针和自由存储空间　80
4．7．1　声明和初始化指针　82
4．7．2　指针的危险　84
4．7．3　指针和数字　84
4．7．4　使用new来分配内存　84
4．7．5　使用delete释放内存　86
4．7．6　使用new来创建动态数组　86
4．8　指针、数组和指针算术　88
4．8．1　程序说明　89
4．8．2　指针小结　90
4．8．3　指针和字符串　92
4．8．4　使用new创建动态结构　94
4．8．5　自动存储、静态存储和动态存储　96
4．9　类型组合　97
4．10　数组的替代品　99
4．10．1　模板类vector　99
4．10．2　模板类array（C++11）　99
4．10．3　比较数组、vector对象和array对象　99
4．11　总结　101
4．12　复习题　101
4．13　编程练习　102
第5章　循环和关系表达式　104
5．1　for循环　104
5．1．1　for循环的组成部分　105
5．1．2　回到for循环　109
5．1．3　修改步长　110
5．1．4　使用for循环访问字符串　110
5．1．5　递增运算符（++）和递减运算符（ ）　111
5．1．6　副作用和顺序点　112
5．1．7　前缀格式和后缀格式　112
5．1．8　递增/递减运算符和指针　112
5．1．9　组合赋值运算符　113
5．1．10　复合语句（语句块）　113
5．1．11　其他语法技巧——逗号运算符　115
5．1．12　关系表达式　116
5．1．13　赋值、比较和可能犯的错误　117
5．1．14　C-风格字符串的比较　118
5．1．15　比较string类字符串　120
5．2　while循环　120
5．2．1　for与while　121
5．2．2　等待一段时间：编写延时循环　123
5．3　do while循环　124
5．4　基于范围的for循环（C++11）　125
5．5　循环和文本输入　125
5．5．1　使用原始的cin进行输入　126
5．5．2　使用cin．get(char)进行补救　126
5．5．3　使用哪一个cin．get( )　127
5．5．4　文件尾条件　128
5．5．5　另一个cin．get( )版本　129
5．6　嵌套循环和二维数组　131
5．6．1　初始化二维数组　132
5．6．2　使用二维数组　133
5．7　总结　134
5．8　复习题　134
5．9　编程练习　135
第6章　分支语句和逻辑运算符　137
6．1　if语句　137
6．1．1　if else语句　138
6．1．2　格式化if else语句　139
6．1．3　if else if else结构　140
6．2　逻辑表达式　141
6．2．1　逻辑OR运算符：||　141
6．2．2　逻辑AND运算符：&&　142
6．2．3　用&&来设置取值范围　144
6．2．4　逻辑NOT运算符：!　145
6．2．5　逻辑运算符细节　146
6．2．6　其他表示方式　146
6．3　字符函数库cctype　147
6．4　：运算符　148
6．5　switch语句　149
6．5．1　将枚举量用作标签　151
6．5．2　switch和if else　152
6．6　break和continue语句　153
6．7　读取数字的循环　154
6．8　简单文件输入/输出　157
6．8．1　文本I/O和文本文件　157
6．8．2　写入到文本文件中　158
6．8．3　读取文本文件　160
6．9　总结　163
6．10　复习题　163
6．11　编程练习　165
第7章　函数——C++的编程模块　167
7．1　复习函数的基本知识　167
7．1．1　定义函数　168
7．1．2　函数原型和函数调用　169
7．2　函数参数和按值传递　171
7．2．1　多个参数　172
7．2．2　另一个接受两个参数的函数　174
7．3　函数和数组　175
7．3．1　函数如何使用指针来处理数组　176
7．3．2　将数组作为参数意味着什么　176
7．3．3　更多数组函数示例　178
7．3．4　使用数组区间的函数　181
7．3．5　指针和const　182
7．4　函数和二维数组　185
7．5　函数和C-风格字符串　186
7．5．1　将C-风格字符串作为参数的函数　186
7．5．2　返回C-风格字符串的函数　187
7．6　函数和结构　188
7．6．1　传递和返回结构　188
7．6．2　另一个处理结构的函数示例　189
7．6．3　传递结构的地址　192
7．7　函数和string对象　194
7．8　函数与array对象　195
7．9　递归　196
7．9．1　包含一个递归调用的递归　196
7．9．2　包含多个递归调用的递归　197
7．10　函数指针　198
7．10．1　函数指针的基础知识　199
7．10．2　函数指针示例　200
7．10．3　深入探讨函数指针　201
7．10．4　使用typedef进行简化　204
7．11　总结　204
7．12　复习题　205
7．13　编程练习　206
第8章　函数探幽　208
8．1　C++内联函数　208
8．2　引用变量　210
8．2．1　创建引用变量　210
8．2．2　将引用用作函数参数　212
8．2．3　引用的属性和特别之处　214
8．2．4　将引用用于结构　216
8．2．5　将引用用于类对象　220
8．2．6　对象、继承和引用　222
8．2．7　何时使用引用参数　224
8．3　默认参数　225
8．4　函数重载　227
8．4．1　重载示例　228
8．4．2　何时使用函数重载　230
8．5　函数模板　230
8．5．1　重载的模板　232
8．5．2　模板的局限性　233
8．5．3　显式具体化　234
8．5．4　实例化和具体化　236
8．5．5　编译器选择使用哪个函数版本　237
8．5．6　模板函数的发展　241
8．6　总结　243
8．7　复习题　243
8．8　编程练习　244
第9章　内存模型和名称空间　246
9．1　单独编译　246
9．2　存储持续性、作用域和链接性　250
9．2．1　作用域和链接　250
9．2．2　自动存储持续性　250
9．2．3　静态持续变量　253
9．2．4　静态持续性、外部链接性　254
9．2．5　静态持续性、内部链接性　257
9．2．6　静态存储持续性、无链接性　258
9．2．7　说明符和限定符　260
9．2．8　函数和链接性　261
9．2．9　语言链接性　262
9．2．10　存储方案和动态分配　262
9．3　名称空间　266
9．3．1　传统的C++名称空间　266
9．3．2　新的名称空间特性　267
9．3．3　名称空间示例　271
9．3．4　名称空间及其前途　274
9．4　总结　274
9．5　复习题　275
9．6　编程练习　276
第　10章 对象和类　278
10．1　过程性编程和面向对象编程　278
10．2　抽象和类　279
10．2．1　类型是什么　279
10．2．2　C++中的类　280
10．2．3　实现类成员函数　283
10．2．4　使用类　285
10．2．5　修改实现　286
10．2．6　小结　287
10．3　类的构造函数和析构函数　288
10．3．1　声明和定义构造函数　288
10．3．2　使用构造函数　289
10．3．3　默认构造函数　290
10．3．4　析构函数　290
10．3．5　改进Stock类　291
10．3．6　构造函数和析构函数小结　296
10．4　this指针　296
10．5　对象数组　300
10．6　类作用域　302
10．6．1　作用域为类的常量　302
10．6．2　作用域内枚举（C++11）　303
10．7　抽象数据类型　304
10．8　总结　307
10．9　复习题　307
10．10　编程练习　308
第　11章 使用类　310
11．1　运算符重载　310
11．2　计算时间：一个运算符重载示例　311
11．2．1　添加加法运算符　313
11．2．2　重载限制　315
11．2．3　其他重载运算符　316
11．3　友元　318
11．3．1　创建友元　319
11．3．2　常用的友元：重载<<运算符　320
11．4　重载运算符：作为成员函数还是非成员函数　324
11．5　再谈重载：一个矢量类　324
11．5．1　使用状态成员　329
11．5．2　为Vector类重载算术运算符　331
11．5．3　对实现的说明　332
11．5．4　使用Vector类来模拟随机漫步　332
11．6　类的自动转换和强制类型转换　334
11．6．1　转换函数　338
11．6．2　转换函数和友元函数　341
11．7　总结　343
11．8　复习题　344
11．9　编程练习　344
第　12章 类和动态内存分配　346
12．1　动态内存和类　346
12．1．1　复习示例和静态类成员　346
12．1．2　特殊成员函数　352
12．1．3　回到Stringbad：复制构造函数的哪里出了问题　354
12．1．4　Stringbad的其他问题：赋值运算符　356
12．2　改进后的新String类　357
12．2．1　修订后的默认构造函数　357
12．2．2　比较成员函数　358
12．2．3　使用中括号表示法访问字符　359
12．2．4　静态类成员函数　360
12．2．5　进一步重载赋值运算符　360
12．3　在构造函数中使用new时应注意的事项　364
12．3．1　应该和不应该　365
12．3．2　包含类成员的类的逐成员复制　365
12．4　有关返回对象的说明　366
12．4．1　返回指向const对象的引用　366
12．4．2　返回指向非const对象的引用　366
12．4．3　返回对象　367
12．4．4　返回const对象　367
12．5　使用指向对象的指针　368
12．5．1　再谈new和delete　369
12．5．2　指针和对象小结　370
12．5．3　再谈定位new运算符　371
12．6　复习各种技术　374
12．6．1　重载<<运算符　374
12．6．2　转换函数　375
12．6．3　其构造函数使用new的类　375
12．7　队列模拟　375
12．7．1　队列类　376
12．7．2　Customer类　382
12．7．3　ATM模拟　384
12．8　总结　387
12．9　复习题　388
12．10　编程练习　389
第　13章 类继承　392
13．1　一个简单的基类　393
13．1．1　派生一个类　394
13．1．2　构造函数：访问权限的考虑　395
13．1．3　使用派生类　397
13．1．4　派生类和基类之间的特殊关系　398
13．2　继承：is-a关系　400
13．3　多态公有继承　400
13．4　静态联编和动态联编　409
13．4．1　指针和引用类型的兼容性　409
13．4．2　虚成员函数和动态联编　411
13．4．3　有关虚函数注意事项　412
13．5　访问控制：protected　414
13．6　抽象基类　415
13．6．1　应用ABC概念　416
13．6．2　ABC理念　421
13．7　继承和动态内存分配　421
13．7．1　第 一种情况：派生类不使用new　421
13．7．2　第二种情况：派生类使用new　422
13．7．3　使用动态内存分配和友元的继承示例　423
13．8　类设计回顾　427
13．8．1　编译器生成的成员函数　427
13．8．2　其他的类方法　428
13．8．3　公有继承的考虑因素　429
13．8．4　类函数小结　432
13．9　总结　433
13．10　复习题　433
13．11　编程练习　434
第　14章 C++中的代码重用　436
14．1　包含对象成员的类　436
14．1．1　valarray类简介　437
14．1．2　Student类的设计　437
14．1．3　Student类示例　438
14．2　私有继承　443
14．2．1　Student类示例（新版本）　443
14．2．2　使用包含还是私有继承　447
14．2．3　保护继承　448
14．2．4　使用using重新定义访问权限　448
14．3　多重继承　449
14．3．1　有多少Worker　452
14．3．2　哪个方法　455
14．3．3　MI小结　461
14．4　类模板　462
14．4．1　定义类模板　462
14．4．2　使用模板类　464
14．4．3　深入探讨模板类　465
14．4．4　数组模板示例和非类型参数　469
14．4．5　模板多功能性　470
14．4．6　模板的具体化　473
14．4．7　成员模板　474
14．4．8　将模板用作参数　476
14．4．9　模板类和友元　477
14．4．10　模板别名（C++11）　482
14．5　总结　482
14．6　复习题　483
14．7　编程练习　484
第　15章 友元、异常和其他　488
15．1　友元　488
15．1．1　友元类　488
15．1．2　友元成员函数　492
15．1．3　其他友元关系　494
15．1．4　共同的友元　495
15．2　嵌套类　495
15．2．1　嵌套类和访问权限　496
15．2．2　模板中的嵌套　497
15．3　异常　499
15．3．1　调用abort()　500
15．3．2　返回错误码　500
15．3．3　异常机制　501
15．3．4　将对象用作异常类型　504
15．3．5　异常规范和C++11　506
15．3．6　栈解退　506
15．3．7　其他异常特性　510
15．3．8　exception类　511
15．3．9　异常、类和继承　514
15．3．10　异常何时会迷失方向　517
15．3．11　有关异常的注意事项　519
15．4　RTTI　520
15．4．1　RTTI的用途　520
15．4．2　RTTI的工作原理　521
15．5　类型转换运算符　526
15．6　总结　528
15．7　复习题　528
15．8　编程练习　529
第　16章 string类和标准模板库　530
16．1　string类　530
16．1．1　构造字符串　530
16．1．2　string类输入　533
16．1．3　使用字符串　535
16．1．4　string还提供了哪些功能　538
16．1．5　字符串种类　539
16．2　智能指针模板类　539
16．2．1　使用智能指针　540
16．2．2　有关智能指针的注意事项　542
16．2．3　unique_ptr为何优于auto_ptr　543
16．2．4　选择智能指针　545
16．3　标准模板库　545
16．3．1　模板类vector　546
16．3．2　可对矢量执行的操作　547
16．3．3　对矢量可执行的其他操作　550
16．3．4　基于范围的for循环（C++11）　553
16．4　泛型编程　553
16．4．1　为何使用迭代器　553
16．4．2　迭代器类型　556
16．4．3　迭代器层次结构　557
16．4．4　概念、改进和模型　558
16．4．5　容器种类　562
16．4．6　关联容器　568
16．4．7　无序关联容器（C++11）　572
16．5　函数对象　572
16．5．1　函数符概念　573
16．5．2　预定义的函数符　575
16．5．3　自适应函数符和函数适配器　576
16．6　算法　577
16．6．1　算法组　578
16．6．2　算法的通用特征　578
16．6．3　STL和string类　579
16．6．4　函数和容器方法　579
16．6．5　使用STL　581
16．7　其他库　583
16．7．1　vector、valarray和array　583
16．7．2　模板initializer_list（C++11）　586
16．7．3　使用initializer_list　587
16．8　总结　588
16．9　复习题　589
16．10　编程练习　590
第　17章 输入、输出和文件　592
17．1　C++输入和输出概述　592
17．1．1　流和缓冲区　593
17．1．2　流、缓冲区和iostream文件　594
17．1．3　重定向　596
17．2　使用cout进行输出　597
17．2．1　重载的<<运算符　597
17．2．2　其他ostream方法　598
17．2．3　刷新输出缓冲区　600
17．2．4　用cout进行格式化　601
17．3　使用cin进行输入　611
17．3．1　cin>>如何检查输入　612
17．3．2　流状态　613
17．3．3　其他istream类方法　616
17．3．4　其他istream方法　620
17．4　文件输入和输出　622
17．4．1　简单的文件I/O　623
17．4．2　流状态检查和is_open()　625
17．4．3　打开多个文件　625
17．4．4　命令行处理技术　626
17．4．5　文件模式　627
17．4．6　随机存取　633
17．5　内核格式化　638
17．6　总结　640
17．7　复习题　641
17．8　编程练习　642
第　18章 探讨C++新标准　644
18．1　复习前面介绍过的C++11功能　644
18．1．1　新类型　644
18．1．2　统一的初始化　644
18．1．3　声明　645
18．1．4　智能指针　646
18．1．5　异常规范方面的修改　647
18．1．6　作用域内枚举　647
18．1．7　对类的修改　647
18．1．8　模板和STL方面的修改　648
18．1．9　右值引用　649
18．2　移动语义和右值引用　650
18．2．1　为何需要移动语义　650
18．2．2　一个移动示例　651
18．2．3　移动构造函数解析　654
18．2．4　赋值　655
18．2．5　强制移动　655
18．3　新的类功能　658
18．3．1　特殊的成员函数　658
18．3．2　默认的方法和禁用的方法　659
18．3．3　委托构造函数　660
18．3．4　继承构造函数　660
18．3．5　管理虚方法：override和final　661
18．4　Lambda函数　662
18．4．1　比较函数指针、函数符和Lambda函数　662
18．4．2　为何使用lambda　664
18．5　包装器　666
18．5．1　包装器function及模板的低效性　666
18．5．2　修复问题　667
18．5．3　其他方式　669
18．6　可变参数模板　669
18．6．1　模板和函数参数包　669
18．6．2　展开参数包　670
18．6．3　在可变参数模板函数中使用递归　670
18．7　C++11新增的其他功能　672
18．7．1　并行编程　672
18．7．2　新增的库　672
18．7．3　低级编程　673
18．7．4　杂项　673
18．8　语言变化　674
18．8．1　Boost项目　674
18．8．2　TR1　674
18．8．3　使用Boost　674
18．9　接下来的任务　675
18．10　总结　675
18．11　复习题　676
18．12　编程练习　678







