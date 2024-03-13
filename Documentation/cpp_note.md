# cpp note day day dream

参考网站：

1. https://cplusplus.com/reference

## 0. c++类

### 六大默认属性，三函数，三运算符

- 默认构造函数
- 析构函数
- 拷贝构造函数 A("");
- 赋值运算符  = 
- 取址运算符 &
- const取址运算符 const &

### 取址运算符重载

```c++
class Date
{
public :
    Date* operator&()
    {
        return this ;
    }
    const Date* operator&()const // const取址重载
    {
        return this ;
    }
private :
	int _year ; // 年
	int _month ; // 月 
	int _day ; // 日
};
```

### 构造函数与析构函数

#### What is ...

构造函数：以类名作为函数名，无返回类型，构造函数可有多个

> C++构造函数分为三种
>
> 1.无参构造函数
>
> 2.一般构造函数，普通传参
>
> 3.复制构造函数，直接传参数为同对象

如果将默认构造函数声明为private,那么在类外将无法使用直接声明的方式定义对象，这时可以在public中声明静态初始化方法，以后可以通过调用方法来初始化对象。

#### 析构函数什么时候被调用

**生命周期**：对象**生命周期结束**，会调用析构函数。

1. **delete**：调用delete，会删除指针类对象。
2. **包含关系**：对象Dog是对象Person的成员，Person的析构函数被调用时，对象Dog的析构函数也被调用。
3. **继承关系**：当Person是Student的父类，调用Student的析构函数，会调用Person的析构函数。



## 1. c++ 模板编程(泛编程)
### 1.1 自动推导
#### **auto**
c++11中，auto作为类型指示符，编译时推导变量的数据类型
**使用场景：**

* 右值可以是具体的数，也可以是表达式或函数
* 不能作为形参类型
* 不能声明数组，类的非静态成员变量。
* 代替冗长变量声明

#### delctype
c++11中，用于推导数据类型
语法
```c++
decltype(expression) var;

//示例：
void func(std::string line)
{
	std::cout << "func() is called at " << line << std::endl;
}
//...
void test_DeclType()
{
	//1.
	/*short n1 = 1;
	short& n2 = n1;
	decltype(n2) n3 = n1; // 引用类型需要初始化
	Print(n3);*/

	//2.
	decltype(func(std::to_string(__LINE__)))* p1 = func; // 不会调用
	p1;

	decltype(func)* p2 = func; // 声明为函数指针
	p2(std::to_string(__LINE__));

	//3.
	int a1 = 0;
	decltype((a1)) a2 = a1; // expression = (a1)时等价于引用
	decltype((func)) a3 = func;
	a3(std::to_string(__LINE__));
}
//...

// 其他使用方法
// auto func(int a,int b) -> int{return a + b;}
```

### 1.2 函数模板
示例
```cpp
// 函数模板示例
//1.
template<typename T>
void Print(T a, T b)
{
	std::cout << a << ' ' << b << std::endl;
}
//...
Print(1,2);      // 隐式调用
Print<int>(1,2); // 显示调用
//...
//2.
template<typename T,typename N>
void Print(T a, N b)
{
	std::cout << a << ' ' << b << std::endl;
}

// 函数模板的具体化（特化）
// 3.具体函数模板,使用优先级大于普通函数模板
template<> void Print(User& u1, User& u2)
{
	std::cout << u1.name << ' ' << u2.name << std::endl;
}
```
原理就是，调用函数的时候，发现函数模板，然后分析实参的数据类型，内部生成对应的函数实例，生成的函数名会在原有的名字上加各种修饰符，这也是函数重载的原理。
**使用场景：**
* 可以用于类函数，但是不能用于虚函数，不能用于析构函数（没有参数）
* 模板不能隐式转换，需要显示声明类型。
  例如，如果调用模板函数，自动分析数据类型，在实例1中，传入一个int和一个char会报错，char不会隐式转换成int类型。
* 支持函数重载
* 优先级，普通函数>具体函数模板>普通函数模板
  具体优先级还需要看参数匹配度
* 函数模板没定义就放在头文件中，有定义就在源文件中

### 1.3 类模板

示例

```c++
// 5. 类模板,可以指定默认类型实现缺省
template<class T,class N = std::string>
class Student
{
public:
    Student(T age,N name) : age(age), name(name){}
    void PrintStudentData()
    {
        printf("该学生年龄=%d", age);
        std::cout << "名字是" << name << std::endl;
    }
private:
    N name;
    T age;
};
```

* 不能在类外为函数指定默认类型

* template<class>和template<typename>本质上一样，但是后者可以用于嵌套，

  typename用于告诉编译器修饰的不是类，也不是函数，而是数据类型。

## 2. c++实现计时器

### std::chrono

c++11引入

包含了三种时钟类

* `system_clock`系统时钟，可以自己设置
* `steady_clock` 稳定时钟，不可修改时钟，刻度为1ns,单调的时钟，相当于秒表，可以用于记录程序耗时（时间差），不受系统时间影响。
* `high_resolution_clock` 高精度时钟，当前系统最高精度，高精度版`steady_clock`

这三种类相互独立，都不是模板类；但是成员类型和函数大多相同。

都提供了以下成员

* `rep` 周期
* `period` 单位比率
* `duration` 时间长度，时间段

* `now()` 获取当前时间 返回类型为`time_point` 时间点

#### 编写计时器

计时器类基本代码

```c++
class Timer
{
public:
    Timer()
    {
        Reset();
    }
    // 重置
    void Reset()
    {
        // 获取当前时间(不是系统的时间)
        m_Start = std::chrono::high_resolution_clock::now();
    }
    // 已经过去的时间（消逝）
		float Elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}
private:
    // 声明一个高精度的time_point(记录初始时间)
	std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};
```

**项目中计时器的使用**(调试时用于测试程序运行时间来进行代码优化optimize)

```c++
template<typename Fn>
class Timer
{
public:
	/*
	 *@ class Timer
	 *@ description steady_clock用于记录时刻 
	*/
	Timer(const char* name,Fn&& func)
		: m_Name(name),m_Stopped(false),m_Func(func)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		// microseconds微秒
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;

		//std::cout << m_Name << " Duration: " << duration << "ms" << std::endl;

		m_Func({ m_Name,duration });
	}
private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	Fn m_Func;
};

// []表示捕获（capture）上下文变量，[=]按值，[&]按引用，[]什么也不捕获
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name)
```

使用构造函数和析构函数控制计时器声明周期

```c++
{
	PROFILE_SCOPE("Name");//就可以调用
}
```

## 3. c++ STL

STL统称为：**"Standard TemplateLibrary 标准模板库"**

STL六大组件

* **容器：**就是熟知的存储各种数据结构的容器；
* **算法：**见名思义，就是各种常用的算法（比如：冒泡，排序算法等）；
* **迭代器：**这个可以看作成是一个指针，扮演了容器与算法之间的胶合剂的作用；
* **仿函数：**行为类似熟知的函数，它的作用可作为算法的某种策略；
* **适配器：**一种用来当作修饰（容器）或（仿函数）或（迭代器）接口的东西；
* **空间配置器（分配器）：**负责空间的配置与管理的作用；

常用STL快查

### 3.1 std::vector

* 以下是常用案例

```cpp
//初始化
vector<T> v(capacity,initial);
//在空容器的基础上分配空间
v.reserve(20); //v.capacity =20
//用数组来初始化
int arr[]={1,2,3};
vector<int> v(arr,arr+2);
vector<int> v1(v.begin(),v.begin()+2);
//判空
v.empty();

//返回元素个数
v.size();

//删除  不会减小容量 size变
vector<T>:: iterator it; //迭代器删除

v.erase(pos);    //it，如果是it，it会变成野指针
v.erase(begin(),end());
//stl是通过把vector中要删除位置后面的所有数据移动到要删除的位置，然后删除最后一个位置的数据来实现的

v.remove(ElemType); //  不会改变size
v.remove(v.begin(),v.end(),value);
//删除容器中所有和指定元素值相等的元素，并返回指向最后一个元素下一个位置的迭代器。值得一提的是，调用该函数不会改变容器的大小和容量。

v.clear();			//清空

//交换两个容器的全部数据
v1.swap(v2);
//单个元素交换
swap(v[i],v[j]);
    
//排序
sort(v.begin(),v.end());
sort(v.rbegin(),v.rend());

//插入
v.insert(v.begin()+n,value);

//将一个vector接到另一个vector后面
std::vector<int> src,dest;
dest.insert(dest.end(), src.begin(), src.end());

//查找
#include<algorithm>
vector<int> v;
v.find(v.begin(),v.end(),value);

//获取最大值
int a = *max_element(v.begin(),v.end());
```

* 更近一步

  * **全排列**

    `next_permuteation()`代码如下

    ` bool next_permutation(iterator start,iterator end)`

  ```c++
  #include<algorithm>
  do{
  
  }while(next_permutation(arr,arr+n)); //vector,string,[]
  //前提是vector中的数据是有序的
  ```

  * 二分查找

    ```c++
    lower_bound(v.begin(),v.end(),value);//返回不小于value的第一个数
    //也要求先排序才能用不然结果不对
    ```

  * 累加

    `std::accumulate(v.begin(),v.end(),startValue);`

### 3.2 std::string

 replace函数包含于头文件#include<string>中。 泛型算法replace把队列中与给定值相等的所有值替换为另一个值，整个队列都被扫描，即此算法的各个版本都在线性时间内执行———其复杂度为O(n)。即replace的执行要遍历由区间[frist，last)限定的整个队列，以把old_value替换成new_value.

原文链接：https://blog.csdn.net/zwj1452267376/article/details/46821527

```cpp
std::replace(mostLikelyCandidate.begin(), mostLikelyCandidate.end(), '\\', '/');
```



* string 函数

```c++
//增
string s;
string s1;
s.insert(5,s1) //在第六个元素出插入s1 ,但这只能插入字符串
   s.insert(5,1,c);//这样就可以插入字符

//删
s.erase(6);    //从第七个元素开始，默认全删
s.erase(6,3);   //从第七个元素开始，删三个
    
//子串
s.substr(6,6);  //从第七个元素，截取六个
s.substr(6);//默认到结尾
s.find('value'); //找到返回下标，没找到返回-1
s.find(s1,5);   //从第六个元素开始寻找子字符串，返回index
                //只要每次更新StartIndex,就可以达到连续寻找
                //如果没找到，就会返回最大值2的32次方
s.rfind(s1,5)   //找到第六个元素还没找到就返回最大值
    
//常用函数
s.length();     //区别vector 返回长度
reverse(s.begin(),s.end()); //逆转
```

**字符串转换**

`#include <string>`

`stoi` string to int

`stol` string to long 

`stoul` string to unsigned long

其他类推

### 3.3 std::unordered_map

### 3.4 std::set

### 3.5 std::priority_queue

`priority_queue`

默认对数据按大顶堆排序。

```
priority_queue<int,vector<int>,greater<int>()> headp;
```



## 4. c++ 关键字及符号

* **inline**

  在C++中，`inline`关键字用于建议编译器将函数的调用替换为函数体中的语句，这种优化方法称为内联函数（inline function）。内联函数可以减少函数调用的开销，提高程序的执行效率。当编译器将函数替换为函数体中的语句时，这个过程称为内联展开（inline expansion）。

  1. `inline`关键字只是对编译器的建议，编译器可以选择忽略该建议。编译器会根据函数的大小、调用次数等因素来决定是否进行内联展开。
  2. 内联函数通常适用于简单、执行速度快、调用频繁的函数。对于大型函数或调用次数较少的函数，内联展开可能会增加代码的大小和运行时间。
  3. 内联函数通常在头文件中声明和定义，这样可以在多个源文件中使用。如果在源文件中定义内联函数，则需要使用`inline`关键字。

  **总而言之**：内联函数: 适用于较简单、常用的小函数。需要判断函数自身所占空间资源是否小于函数调用的资源开销，小于则适用，反之会造成空间浪费。***内联是以代码膨胀复制为代价*** 。

  `inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }`

* **... (ellipsis)**,省略号，被称作可变参数模板(variadic template)

  函数传参，可以接受任意数量和类型的参数

  ```c++
  // 参数不能为空
  void foo(...) {  
      // 函数体  
  }
  
  // c++11引入，支持初始化列表的拓展
  int arr[] = {1, 2, 3, ...};
  ```

* **`[[fallthrough]]`**

  c++17

  switch中不加break,按理说会顺序执行下一个case直到break,这个关键字的作用是没有break时，不让编译器抛出warning
  
* **`static_cast`**

  参考：[C++ 强制转换运算符 | 菜鸟教程 (runoob.com)](https://www.runoob.com/cplusplus/cpp-casting-operators.html#:~:text=C%2B%2B 强制转换运算符 1 const_cast (expr)%3A const_cast 运算符用于修改类型的 const,运算符把某种指针改为其他类型的指针。 ... 4 static_cast (expr)%3A static_cast 运算符执行非动态转换，没有运行时类检查来保证转换的安全性。 )

  四个强转符号之一。

  可用于普通数据类型的强转，基类与派生类指针(引用)的相互转换，关联性数据类型转换(如枚举和整型)

  缺点

  - **不能用于转换不相关的指针或引用类型**。例如，不能将 `void*` 转换为其他类型的指针，或反之。在这种情况下，应使用 `reinterpret_cast`。
  - **不能用于移除或添加 `const` 限定符**。在这种情况下，应使用 `const_cast`。

  优点

  - 相对于 C 风格的强制转换（如 `(int)3.14`），`static_cast` 更加明确和可读。
  - 与 C 风格的强制转换相比，`static_cast` 只能执行明确允许的转换，这有助于避免一些错误。

  因为 `static_cast` 不执行运行时类型检查。如果对转换不确定，考虑使用其他类型的转换，如 `dynamic_cast`
  
  **`reinterpret_cast`**既可以指针转指针，也可以指针整数互转

## 5. c++宏

### 基本概念

c++宏是预处理器(preprocessor)的一部分，预编译阶段被替换。

程序生成exe文件的流程：预编译->编译（编译成汇编代码）->汇编（生成机器语言）->链接（生成exe）

**编译之前就已经完成文本替换，被替换的字符叫做替换文本。**

用来定义常量，函数等。

```c++
// 1. 普通宏,可以是常量，函数，类型
#define PI 3.1415926

// 2. 带参函数
#define SQUARE(x) (x * x)

// 3. 条件编译，只定义，通过if else控制开关
#define DEBUG
```

### 项目中的示例

```c++
// []表示捕获（capture）上下文变量，[=]按值，[&]按引用，[]什么也不捕获; this指针只能在成员函数中使用,不能在静态函数中使用
#define AS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
```

## 6. c++线程

### 6.1 相关概念

**进程**：是指⼀个内存中运⾏的应⽤程序，每个进程都有⼀个独⽴的内存空间，⼀个应⽤程序可以同时运⾏多个进程；进程也是程序的⼀次执⾏过程，是系统运⾏程序的基本单位；系统运⾏⼀个程序即是 ⼀个进程从创建、运⾏到消亡的过程。
**线程**：线程是进程中的⼀个执⾏单元，负责当前进程中程序的执⾏，⼀个进程中⾄少有⼀个线程。⼀个进程中是可以有多个线程的，这个应⽤程序也可以称之为多线程程序。
**简⽽⾔之：⼀个程序运⾏后⾄少有⼀个进程，⼀个进程中可以包含多个线程**

并发指的是同一时间段内多个线程的发生，并行则是同一时刻多个线程的发生。

c中可以使用`pthread`

c++11中可以使用`std::thread`

**在C++中实现线程安全主要涉及两个方面：避免数据竞争和同步访问。**

### 6.2 如何实现线程安全

* mutex(互斥锁)

  ```c++
  #include <mutex>  //c++11
  std::mutex mtx;  
  // 线程函数  
  void thread_func() {  
      mtx.lock();  
      // 访问共享数据或资源  
      mtx.unlock();  
  }
  ```

* **Condition Variables**(条件变量)

  ```c++
  #include <condition_variable>  
  std::condition_variable cv;  
  std::mutex mtx;  
  // 线程函数  
  void thread_func() {  
      std::unique_lock<std::mutex> lock(mtx);  
      cv.wait(lock, []{return !queue.empty();}); // 等待队列不为空  
      // 处理队列中的数据  
  }
  ```

* Atomic(原子操作): 不可中断的操作，多线程中最小的不可并行的操作。

  ```c++
  #include <atomic>  
  
  // 变量是原子的，意思是每次操作这个变量，包括读取其值、更改其值，这次操作中执行的唯一一条指令是原子的
  std::atomic<int> counter(0);  
  // 线程函数  
  void thread_func() {  
      counter++; // 原子操作，自增计数器  
  }
  ```


## 7. c++ malloc, free, new, delete的区别

后两个是c++的操作符。

* new可以自动计算所需分配内存大小，malloc需要手动计算

* new返回对象指针，malloc返回 void*(所以需要类型转换)

  `Task *p = (Task*)malloc(sizeof(*p)); // sizeof里也可以是Task`

* new分配失败抛出异常，malloc失败返回NULL

* new在 free store(自由存储区)上分配内存，malloc则是在堆（主要），也有在文件映射。

* delete 需要对象类型指针，free则是void*。delete void * 会报错。

## 9.c++中值类型和引用类型

数据一般在两种内存中

* **栈内存**，空间小，读写速度快，先进后出，后进先出
* **堆内存**，空间大，读写慢，没有手动删除会存储到程序结束

## 10.c++类与结构体的区别

参考：[值类型和引用类型由来及详解(C++/C#混讲) - ShawBlack - 博客园 (cnblogs.com)](https://www.cnblogs.com/ShawBlack/p/16997772.html)

先说c与c++的struct的不同

* 前者成员函数，静态成员，继承都不行，默认权限public且不能修改，不能直接初始化数据成员；

  后者则是改进了以上五个。

然后是c++中class和struct的**不同**

* **在代码使用上**，唯一的不同是 成员默认访问权限不同，class是private, struct 是public

* 类可以继承，结构体也可以(c中不行)  (此条删去)

* **更深入地**，struct是值类型，class引用类型

  在C#中值类型有：整型:Int; 长整型:long; 浮点型:float; 字符型:char; 布尔型:bool 枚举:enum 结构:struct，余下皆是引用类型。

  c++默认都是值类型，引用类型则是需要通过new在堆上申请空间并返回存储对象地址。

  栈上的数据程序员不需要手动管理，但是堆上的数据则需要手动delete.
  
  、

## 11.c++函数指针+lambda函数

### 11.1 raw function pointer(c)

示例代码

```c++
void DoWork1(int a)
{
    std::cout << "Working...Value is:" << a << std::endl;
}
// - 传递函数指针作为参数
void ForEach(std::vector<int> values, void(*Print)(int))
{
    for (auto v : values)
        Print(v);
}

// ...main()
// 直接声明函数指针，取别名简化，使用auto自动推导创建
typedef void(*DoWorkFunc)(int); // 对函数指针取别名
DoWorkFunc function0 = DoWork1;
auto function1 = DoWork1; // 此时没有调用函数，而是传递函数指针
void(*function2)(int) = DoWork1; //声明变量funciton2为函数指针
function1(8);

// 函数指针传参
std::cout << "Start Print Values" << std::endl;
std::vector<int> v = { 0,3,4,1 };
ForEach(v, Print);
//...

```

### 11.2 lambda函数

示例

```c++
ForEach(v, [](int a) {std::cout << a << std::endl; });
```

## 12.c++中库的使用（静态和动态链接）

静态库是**直接被放到**exe可执行文件中的。

而动态库则是在程序运行时（runtime）动态**链接**调用的（LoadLibrary API import dll, get api,and call api)，一般需要dll和exe同目录。

通常来说静态链接会更快（可以优化的空间大），一般也会使用静态链接。

### 静态链接

静态库包含两个重要目录

* include: 包含了所有头文件
* lib：包含了函数的实现

### 动态链接

* 有的lib文件中可能会有dll文件，还有的有dll.lib文件，这个是用于指向dll的指针，就不用自己去检索函数的位置，这两个文件需要同时编译。

* 一般来说编译后会报错，因为dll需要和exe文件在同一目录，所以需要手动复制移动dll。

* 如果是静态链接，哪怕只有一个exe也可以运行。

**如果同时采用lib和dll链接，就不需要 _declspec(dllimport)来导入，因为所有的函数和变量已经在lib中了（IAT,Import Address Table)，只是如果采用了_declspec(dllimport)导入，就可以特定想要导入的函数和API，导入更快而已。**

### 如何制作自己的库

## 13.c++ namespace 

### 13.1 为什么不用using namespace std;

因为std::访问在复杂项目中容易让人辨认出函数或类型属于哪个库。



## 14.c++预编译头文件

大型项目需要使用

每一次#include<vector>都会把该文件及相关头文件合并到一起重新编译。

于是将编写预编译头文件，将这些不需要频繁（或不更改）更改的大型头文件包含起来。

这样就只用编译一次。

## 15. c++ 智能指针

### 为什么引入智能指针

浅拷贝 (shallow copy) 只是对指针的拷贝, 拷贝够两个指针指向同一个内存空间. 深拷贝 (deep copy) 不但对指针进行拷贝, 而且对指针指向的内容进行拷贝. 经过深拷贝后的指针是指向两个不同地址的指针.

编译器默认拷贝是浅拷贝，只会对值进行复制，用的是相同地址，对于指针而言，如果浅拷贝就会出现两个指针指向同一区域

如果指针发生拷贝，对同一区域的析构就会发生两次。两个指向同一区域的指针之一析构，另一个指针就会悬挂，可能造成程序崩溃。

于是智能指针出现，解决该问题的方式不同，导致智能指针的多样性。

解决方法主要是以下几种

1. 实现可深拷贝的指针
2. 对象管理唯一化(或者说删除权限唯一化)，防止多个指针指向同一区域
3. 引用计数，只有最后一个指针需要析构才真正析构。

### 具体的指针

**前言**

在c++98中，有auto_ptr，通过权限转让的方式管理资源，一个资源被一个指针管理。因为容易引起指针悬空所以弃用。

主要介绍

* c++11引入的三种智能指针。

* DirectX11 的Comptr
* 自定义智能指针

#### **1. unique_ptr**

一旦离开unique_ptr作用域，自动销毁。RAII(资源获取即使初始化)思想

该指针是move only的

只能通过std::move转交，不能通过 = 来浅拷贝

#### **2. shared_ptr**

通过引用计数的方式

智能指针shared_ptr的三种初始化方法

```cpp
//初始化方式1
std::shared_ptr<int> sp1(new int(123));

//初始化方式2
std::shared_ptr<int> sp2;
sp2.reset(new int(123));

//初始化方式3
std::shared_ptr<int> sp3;
sp3 = std::make_shared<int>(123);

// 不能够直接 = new()
```

**shared_ptr原理**

一般来想直接使用深拷贝，让每个指针对象new一片新的区域不就好了，但是这样显然会导致相同的数据复制了多份空间，不是明智之选。

这里则在shared_ptr这个类中，加入一个int *_pcount;

后续浅拷贝得到的智能指针，这个指针指向的值会加一

当智能指针不再使用或者析构时,pcount区域的值会减一，**只有所有的管理者都析构完，才会进行唯一一次delete**



之所以使用int*,显然不能让每一个对象都开辟一个int区域，而是多个ptr指向同一个int区域。

这时候可能会想到既然公用pcount区域的话，为什么不用static 类型。

如果使用static,在`shared_ptr<int>`和`shared_ptr<long>`同时出现,就会产生不同类型却共用同一个公有的static变量。

使用int* 使得每种shared_ptr都有单独的空间存储计数。

**shared_ptr问题**

* 有关`shared_ptr`的线程安全问题

因为自增自减不是原子操作，需要加锁

* 有关`shared_ptr`循环引用的问题

  ```cpp
  void test_shared_ptr2()
  {
      std::shared_ptr<ListNode> n1 (new ListNode);
      std::shared_ptr<ListNode> n2 (new ListNode);
  
      n1->_next = n2;
      n2->_prev = n1;
  }
  ```

  连接操作导致的循环，无法析构

  如果n1想要析构，就要释放指针的内容，但是指针指向n2,那么意味着要想析构n1，就要先释放_next；同理释放n2的过程中，n2要释放指针，但是指针_pre要先释放。

  这样相互等待对方释放，就会造成死循环。

* `shared_ptr`获取该类的智能指针

  ```c++
  class Logger : public std:: enable_shared_from_this<Logger>
  {
  	public:
  		typedef std::shared_ptr<Logger> ptr;
      //....
  };
  ```

  这里用基类智能指针管理整个类的生存周期

  同时继承enable_shared_from_this<Logger>

  成员函数内`shared_from_this()`获取类的智能指针

#### **3. weak_ptr**

std::weak_ptr 要与 std::shared_ptr 一起使用。 一个 std::weak_ptr 对象看做是 std::shared_ptr 对象管理的资源的观察者，它**不影响共享资源的生命周期**：

1. 如果需要使用 weak_ptr 正在观察的资源，可以将 weak_ptr 提升为 shared_ptr。
2. 当 shared_ptr 管理的资源被释放时，weak_ptr 会自动变成 nullptr

只要 weak_ptr 对象还存在，控制块就会保留，weak_ptr 可以通过控制块观察到对象是否存活。



**weak_ptr具体案例**

用于解决`shared_ptr`循环问题

weak和shared 一同管理

实际weak不参与管理，不增加资源计数

```cpp
struct ListNode
{
    std::weak_ptr<ListNode> _prev;
    std::weak_ptr<ListNode> _next;

    ~ListNode()
    {
        cout << "~ListNode()" << endl;
    }
};

void test_shared_ptr2()
{
    std::shared_ptr<ListNode> n1 (new ListNode);
    std::shared_ptr<ListNode> n2 (new ListNode);

    n1->_next = n2;
    n2->_prev = n1;

    cout << n1.use_count() << endl;
    cout << n2.use_count() << endl;
}
```

这时，当要析构n1时，要释放掉n1->_next,不再向n2寻求，而是n1的weak_ptr自带的指针(构造获得数据)

注意这里的问题不在于 像前面一样两个指针指向同一区域，

可以看到一开始初始化了两个区域，本质上n1,n2计数各自只有1(weak不参与),

问题在于**连接后相互等待析构彼此**

所以干脆将初始化的两个区域分别保存在weak里，然后析构

而不是相互企图析构另一个指针的区域



### 自定义智能指针

参考自Cherno的Hazel引擎

因为`shared_ptr`自增具有线程安全问题，并且存在循环引用问题，以下自定义智能指针解决这些问题。

```cpp
// Ref.h
#pragma once
#include <stdint.h>
#include <atomic>
#include "Memory.h"
namespace Aspect {
    // 计数类
	class RefCounted
	{
	public:
		void IncAspectRefCount() const
		{
			++m_AspectRefCount;
		}
		void DecAspectRefCount() const
		{
			--m_AspectRefCount;
		}

		uint32_t GetAspectRefCount() const { return m_AspectRefCount.load(); }
	private:
        // 原子操作
		mutable std::atomic<uint32_t> m_AspectRefCount = 0;
	};

	namespace AspectRefUtils {
		void AddToLiveAspectReferences(void* instance);
		void RemoveFromLiveAspectReferences(void* instance);
		bool IsLive(void* instance);
	}

	template<typename T>
	class AspectRef
	{
	public:
		AspectRef()
			: m_Instance(nullptr)
		{
		}

		AspectRef(std::nullptr_t n)
			: m_Instance(nullptr)
		{
		}

		AspectRef(T* instance)
			: m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not AspectRefCounted!");

			IncAspectRef();
		}

		template<typename T2>
		AspectRef(const AspectRef<T2>& other)
		{
			m_Instance = (T*)other.m_Instance;
			IncAspectRef();
		}

		template<typename T2>
		AspectRef(AspectRef<T2>&& other)
		{
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		static AspectRef<T> CopyWithoutIncrement(const AspectRef<T>& other)
		{
			AspectRef<T> result = nullptr;
			result->m_Instance = other.m_Instance;
			return result;
		}

		~AspectRef()
		{
			DecAspectRef();
		}

		AspectRef(const AspectRef<T>& other)
			: m_Instance(other.m_Instance)
		{
			IncAspectRef();
		}

		AspectRef& operator=(std::nullptr_t)
		{
			DecAspectRef();
			m_Instance = nullptr;
			return *this;
		}

		AspectRef& operator=(const AspectRef<T>& other)
		{
			other.IncAspectRef();
			DecAspectRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		AspectRef& operator=(const AspectRef<T2>& other)
		{
			other.IncAspectRef();
			DecAspectRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		AspectRef& operator=(AspectRef<T2>&& other)
		{
			DecAspectRef();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* Raw() { return  m_Instance; }
		const T* Raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecAspectRef();
			m_Instance = instance;
		}

		template<typename T2>
		AspectRef<T2> As() const
		{
			return AspectRef<T2>(*this);
		}

		template<typename... Args>
		static AspectRef<T> Create(Args&&... args)
		{
#if AS_TRACK_MEMORY
			return AspectRef<T>(new(typeid(T).name()) T(std::forward<Args>(args)...));
#else
			return AspectRef<T>(new T(std::forward<Args>(args)...));
#endif
		}

		bool operator==(const AspectRef<T>& other) const
		{
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const AspectRef<T>& other) const
		{
			return !(*this == other);
		}

		bool EqualsObject(const AspectRef<T>& other)
		{
			if (!m_Instance || !other.m_Instance)
				return false;

			return *m_Instance == *other.m_Instance;
		}
	private:
		void IncAspectRef() const
		{
			if (m_Instance)
			{
				m_Instance->IncAspectRefCount();
				AspectRefUtils::AddToLiveAspectReferences((void*)m_Instance);
			}
		}

		void DecAspectRef() const
		{
			if (m_Instance)
			{
				m_Instance->DecAspectRefCount();
				if (m_Instance->GetAspectRefCount() == 0)
				{
					delete m_Instance;
					AspectRefUtils::RemoveFromLiveAspectReferences((void*)m_Instance);
					m_Instance = nullptr;
				}
			}
		}

		template<class T2>
		friend class AspectRef;
		mutable T* m_Instance;
	};

	template<typename T>
	class WeakAspectRef
	{
	public:
		WeakAspectRef() = default;

		WeakAspectRef(AspectRef<T> AspectRef)
		{
			m_Instance = AspectRef.Raw();
		}

		WeakAspectRef(T* instance)
		{
			m_Instance = instance;
		}

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		bool IsValid() const { return m_Instance ? AspectRefUtils::IsLive(m_Instance) : false; }
		operator bool() const { return IsValid(); }
	private:
		T* m_Instance = nullptr;
	};

}
```



## 16. 设计模式

### 单例模式



## 17. 输入输出

[cplusplus.com/reference/iolibrary/](https://cplusplus.com/reference/iolibrary/)

### 17. 输出

```cpp
cout<<setiosflags(ios::scientific)<<setprecision(8);
cout<<resetiosflags(ios::scientific);
cin>>setw(n)>>string;
cout<<setfill('')<<setw(n)<<string;
cout<<setbase('进制')；
```



包含了istringstream,ostringstream和stringstream三种类

`rdbuf()`用于返回指向当前缓冲区的指针,下例中将流缓冲区内容写入文件

```cpp
#include <fstream>
#include <iostream>
// wrong revison
int main()
{
    std::ofstream log("oops.log");
    std::cout.rdbuf(log.rdbuf());
    std::cout << "Woo Oops!\n"; //输出到文件log
    return 0;
}
```

函数

```cpp
streambuf* rdbuf() const; //get
streambuf* rdbuf (streambuf* sb); //set
```



`fstream`

和`sstream`类似，但是前者是针对文件的，后者是字符串流数据

## 18. c++ 位运算

&，|，^, ~

分别为且，或，异或，非

* ^可用于奇数变偶数（-1），偶数变奇数（+1）

  `int b = a^1;`  这个式子可以形成0，1； 2，3；4，5的pair

**左右移运算符**

`1<<2`指的是左移两位，右移同理



### 异或的性质

不同为1，相同为0

1.  a ^ 0 = a
2.  a ^ a = 0

* 大小写转换可以通过异或32实现

## 19. Const

* `const`修饰普通类型

```c++
const int a = 7; //只允许定义时初始化
int b = a;  //限定后只能传给其他变量，而不能改变  a=1;是错误的
```

* 指针

  ```c++
  //1.
  const int *p = 2;//指的是*p的内容不能变，始终为2
  int * const p = 2; 指的是p的指向不能变，p= &b;是错误的;但是 *p=3;是正确的，即地址不能变
  //2.
  //对于非指针类型， int * p; p= &a;要加地址符来被指针类型指向
  //3.
  const int* const p;//两个都不变
  //4.
  //通过以上方法在传输参数时可以根据需要防止数值的更改，--->c#的 out(方法内改变) in(默认不变) ref(可改变)
  ```

* `memset`

  ```c++
  void *memset(void *str, int c, size_t n) 
  //初始化数组
  ```


## 20. 哈希

Unordered 系列容器

参考：[(88条消息) 【C++】哈希——unordered系列容器|哈希冲突|闭散列|开散列_c++哈希冲突_平凡的人1的博客-CSDN博客](https://blog.csdn.net/weixin_60478154/article/details/129252674?spm=1001.2014.3001.5502)

**在C++98中，STL提供了底层为`红黑树结构`的一系列关联式容器**

**在C++11中，STL又提供了4个unordered系列的关联式容器，`这四个容器与红黑树结构的关联式容器使用方式基本类似`，只是其底层结构不同 :unordered系列的关联式容器之所以效率比较高，是因为其底层使用了`哈希结构`**

计算哈希函数直接得到地址

![image-20230405162212873](../../../../Website/Blog/source/_posts/408/数据结构/图/image-20230405162212873.png)

如果哈希函数设计的不够合理就会引发哈希冲突。

哈希函数设计原则：

> **哈希函数的定义域必须包括需要存储的全部关键码，而如果散列表允许有m个地址时，其值域必须在0到m-1之间**
> **哈希函数计算出来的地址能均匀分布在整个空间中**
> **哈希函数应该比较简单**

### 解决哈希冲突

解决哈希冲突两种常见的方法是：闭散列和开散列

#### 1.闭散列——开放定址法

- **线性探测**：发生冲突寻找下一个空位置，问题是会导致发生冲突的数据堆积在一起

- **二次探测**

  ![image-20230405163334623](../../../../Website/Blog/source/_posts/408/数据结构/图/image-20230405163334623.png)

## 21. 红黑树(ref...)



## 22. c++反射

参考：[跟我学c++高级篇——反射的基本实现方式_c++反射-CSDN博客](https://blog.csdn.net/fpcc/article/details/130788656)

一般来说，基本静态反射可以通过宏和模板实现。

**要想实现反射，首先知道类的信息，这些信息包括大小，名称，变量数量、地址，函数的地址等等**



## 23.c++中库的使用（静态和动态链接）

静态库是**直接被放到**exe可执行文件中的。

而动态库则是在程序运行时（runtime）动态**链接**调用的（LoadLibrary API import dll, get api,and call api)，一般需要dll和exe同目录。

通常来说静态链接会更快（可以优化的空间大），一般也会使用静态链接。

### 静态链接

静态库包含两个重要目录

* include: 包含了所有头文件
* lib：包含了函数的实现

### 动态链接

* 有的lib文件中可能会有dll文件，还有的有dll.lib文件，这个是用于指向dll的指针，就不用自己去检索函数的位置，这两个文件需要同时编译。

* 一般来说编译后会报错，因为dll需要和exe文件在同一目录，所以需要手动复制移动dll。

* 如果是静态链接，哪怕只有一个exe也可以运行。

**如果同时采用lib和dll链接，就不需要 _declspec(dllimport)来导入，因为所有的函数和变量已经在lib中了（IAT,Import Address Table)，只是如果采用了_declspec(dllimport)导入，就可以特定想要导入的函数和API，导入更快而已。**

### 如何制作自己的库

## 算法题技巧

* 当结果过大，有时采用取模

  ```c++
  typedef long long ll;
  static const ll mod = 1e10 + 7; //只有long long这样写
  //long 会报错
  ll sum = 10000;
  ll ans = sum % mod;
  ```

* 堆排序

  * 数组

  ```cpp
  //选择排序————堆排序（不稳定，空间效率为O(1)，时间效率为O(nlogn)）
  #include <stdio.h>
  
  //交换
  void swap(int &a, int &b) {
      int temp = a;
      a = b;
      b = temp;
  }
  
  //将以k为根结点的子树调整为大根堆
  void MaxHeadAdjust(int A[], int k, int len) {
      A[0] = A[k];                                //将子树根结点暂存在A[0]
      for (int i = 2 * k; i <= len; i *= 2) {     //沿值较大的子节点向下筛选
          if (i < len && A[i] < A[i + 1])         //如果它的左孩子的值小于右孩子的值
              i++;
          if (A[0] >= A[i])                       //如果根结点的值比左右孩子的最大值还要大或相等
              break;
          else {
              A[k] = A[i];                        //交换根结点与左右子孩子中最大值的结点
              k = i;                              //修改k的值，以便继续向下筛选
          }
      }
      A[k] = A[0];                                  //被筛选结点的值放入最终位置
  }
  
  //将以k为根结点的子树调整为小根堆
  void MinHeadAdjust(int A[], int k, int len) {
      A[0] = A[k];                                //将子树根结点暂存在A[0]
      for (int i = 2 * k; i <= len; i *= 2) {     //沿值较大的子节点向下筛选
          if (i < len && A[i] > A[i + 1])         //如果它的左孩子的值大于右孩子的值
              i++;
          if (A[0] <= A[i])                       //如果根结点的值比左右孩子的最小值还要小或相等
              break;
          else {
              A[k] = A[i];                        //交换根结点与左右子孩子中最小值的结点
              k = i;                              //修改k的值，以便继续向下筛选
          }
      }
      A[k] = A[0];                                  //被筛选结点的值放入最终位置
  }
  
  //建立大根堆
  void BuildMaxHeap(int A[], int len) {
      for (int i = len / 2; i > 0; --i) {         //从i=[len/2]~1，反复调整堆
          MaxHeadAdjust(A, i, len);
      }
  }
  
  //建立小根堆
  void BuildMinHeap(int A[], int len) {
      for (int i = len / 2; i > 0; --i) {         //从i=[len/2]~1，反复调整堆
          MinHeadAdjust(A, i, len);
      }
  }
  
  //大根堆排序
  void MaxHeapSort(int A[], int len) {
      BuildMaxHeap(A, len);                       //初始建立大根堆
      for (int i = len; i > 1; --i) {             //len-1趟交换和建堆过程
          swap(A[i], A[1]);                 //将对顶元素和堆底元素交换
          MaxHeadAdjust(A, 1, i - 1);      //调整，将剩余的i-1个元素继续整理为大根堆
      }
  }
  
  //小根堆排序
  void MinHeapSort(int A[], int len) {
      BuildMinHeap(A, len);                       //初始建立小根堆
      for (int i = len; i > 1; --i) {             //len-1趟交换和建堆过程
          swap(A[i], A[1]);                 //将对顶元素和堆底元素交换
          MinHeadAdjust(A, 1, i - 1);      //调整，将剩余的i-1个元素继续整理为小根堆
      }
  }
  
  int main() {
      //0号位置为辅助空间，不存放有效元素！
      int MaxArr[] = {-1, 53, 17, 78, 9, 45, 65, 87, 32};
      int MinArr[] = {-1, 53, 17, 78, 9, 45, 65, 87, 32};
      int len_max = sizeof(MaxArr) / sizeof(int) - 1;
      int len_min = sizeof(MinArr) / sizeof(int) - 1;
  
      //大根堆排序
      MaxHeapSort(MaxArr, len_max);
      //将排序好的结果输出
      printf("大根堆排序结果为：");
      for (int i = 1; i <= len_max; ++i) {
          printf("%d ", MaxArr[i]);
      }
      
      printf("\n");
      
      //小根堆排序
      MinHeapSort(MinArr, len_min);
      //将排序好的结果输出
      printf("小根堆排序结果为：");
      for (int i = 1; i <= len_min; ++i) {
          printf("%d ", MinArr[i]);
      }
      return 0;
  
  }
  ```

  * 单链表

    ```c
    void HeapInit(Heap* php, HPDataType* a, int n)
    {
    	php->_a = (HPDataType*)malloc(sizeof(HPDataType)*n);
    	// ...
    	memcpy(php->_a, a, sizeof(HPDataType)*n);
    	php->_size = n;
    	php->_capacity = n;
    
    	// 构建堆
    	for (int i = (n - 1 - 1) / 2; i >= 0; --i)
    	{
    		AdjustDown(php->_a, php->_size, i);
    	}
    }
    ```

    这里只需采用一个简单的for循环来进行遍历即可，需要注意的是，在数组中，`第一个非叶子结点子树根的下标为(n - 1 - 1) / 2`，`二叉树根结点的下标是0，所以当结点下标小于0的时候就可以停止遍历了`。

    该建堆方式是从倒数第二层的结点（叶子结点的上一层）开始，从右向左，从上到下的向下调整。

    同样的，假设该队为满二叉树，堆高为h。

    则建堆的时间复杂度t(n) = 1 × (h - 1）+ 2 ×(h - 2) + 4 × (h - 3) + …+ 2h-2 × 1 = 20 × (h - 1) + 21 × (h - 2) +22 × (h - 3) + …+ 2^h - 2^ × 1

    可以看到最终的式子是一个差比数列，因此使用错位相减法，得到的时间复杂度为：t(n) = 2h - h - 1 = n - log(n + 1).

    即时间复杂度为T（n） = n.

    这里再简单解释一下：按照最坏情况计算，第一层一个元素，每个元素需要向下调整（h-1）次，第二层两个元素，每个元素需要调整（h-2)次，第三层4个元素，每个元素需要调整（h-3）次，而倒数第二层共2^（h-2）个元素，每个元素需要调整1次，将所有的次数加起来，最后计算出时间复杂度为O（N）。

## 杂项

参考：[关于 C++ 中的 extern "C" - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/123269132)

[语言链接 - cppreference.com](https://zh.cppreference.com/w/cpp/language/language_linkage)

* 宏

1. 一个#号表示用**带引号的字符串**完全替代，两个#会检查前后逗号空格一并删除，并将第二个参数直接连接到第一个参数后例如XX(front,back)  front ## back  最后XX(name,1)结果为name1

   可变参数的宏__ VA_ARGS __ 参数中用...表示替换若干个参数，c99出现，只用于gcc ，由于可变参数为0个时，宏替换会多一个逗号，这时需要加上##来去掉逗号。

* typeid().name()返回类型名

* namespace

  ```cpp
  	/**
  	* 无命名的命名空间，和静态变量具有一样的声明周期，直至程序结束销毁
  	* 并且和一般命名空间一样，都只能通过当前源文件访问这个空间
  	* 好处是等同于声明静态变量
  	* ---------------PAY ATTENTION!!!----------------------
  	* 1.同一文件可以有两个无命名，不同文件的两个无命名成员名字可以一样，因为在不同作用空间；
  		但是相同文件不同无命名不能有重名成员！！！
  	* 2.无命名成员不能和全局变量同名，会产生二义性
  	* 3.可以嵌套（和普通一样）
  	* ---------------PAY ATTENTION!!!----------------------*/
  ```

* const & 用于返回值时控制指针权限

  const &用于**自定义类型** 临时复制或者构造减少时间

* 代码区：存储代码
  全局数据区：**static** 数据, 全局变量, const常量
  堆区：由程序员自己new出来的动态数据， 需要手动释放。若忘记释放，会造成内存泄漏，则程序结束时会由操作系统回收。
  栈区：函数内部变量，由IDE自动分配，结束时自动释放。

* 为什么用std

  因为std::访问在复杂项目中容易让人辨认出函数或类型属于哪个库。

* 预编译头文件

  大型项目需要使用

  每一次#include<vector>都会把该文件及相关头文件合并到一起重新编译。

  于是将编写预编译头文件，将这些不需要频繁（或不更改）更改的大型头文件包含起来。

  这样就只用编译一次。

* extern "C"

  c++作为面向对象的语言，为应对函数重载，对全局函数处理方式和c不同。

  但它只是一种声明，**没有定义**，与之相对的static只能在本模块中使用。

  >\1) 将语言说明 *字符串字面量* 应用到在 *声明序列* 中声明的所有函数类型，具有外部链接的函数名，和具有外部链接的变量。
  >
  >\2) 将语言说明 *字符串字面量* 应用到单一声明或定义。

  **"C"，可以与使用 C 语言编写的函数进行链接，使得在 C++ 中定义能从 C 单元调用的函数成为可能。。**

  由于c++支持重载，函数编译后符号库的函数名会被加上特定后缀之类的修饰。

  为了实现c++和c的混合编译。


## 6. c++特性

* **std::move和std::forward**

  参考: [[C++特性\]对std::move和std::forward的理解 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/469607144)

  左值: 内存中有存储单元，左值引用用&

  右值：没有存储单元，寄存器立即数或者中间值，右值引用用&&

  ```c++
  int x = 10; // 左值
  int y = 20; // 左值
  int z = x + y; // z是左值，但是x+y却是右值
  ```

  比方说push_back(x)是将x复制一份，增加拷贝开销+重新申请空间，如果使用右值引用就能避免。

  ```c++
  v.push_back(std::move(x)); // x无论是左值还是右值都能转换成右值节省开支
  ```

  **c++折叠规则**

  - T& & -> T& （对左值引用的左值引用是左值引用）

  - T& && -> T& （对左值引用的右值引用是左值引用）

  - T&& & ->T& （对右值引用的左值引用是左值引用）

  - **T&& && ->T&& （对右值引用的右值引用是右值引用）**

    ```cpp
    // 1. std::forward是完美转发，左值转发成左值，右值转发成右值
    // 2. 没有使用std::forward会发生隐式转换，右值会变成左值
    // 3. 以下是一个lambda表达式，右值传递参数
    
    [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
    ```



* `std::bind`

  绑定器，用于将函数（可调用对象）存储在`std::function`（stl内部的仿函数）中

  ```c++
  // Application.cpp
  // 1. 将Application::Function 绑定到std::function中
  // 2. std::placeholders::_1 是一个占位符
  //    _1用于替代回调中第一个参数，简单来说就是动态传入的第一个参数的位置。_2则以此类推
  #define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1) 
  ```

  其它示例

  ```c++
  void output(int x, int y)
  {
      std::cout << x << " " << y << std::endl;
  }
  // main()
  std::bind(output, 1, 2)(); // 输出 : 1 2
  std::bind(output, std::placeholders::_1, 2)(1); // 输出 : 1 2
  std::bind(output, 2, std::placeholders::_1)(1); // 输出 : 2 1
  ```

  

* `std::optional`

  c++17引入

  * std::optional用来包装可以为空的类型（主要），例如工厂模式返回不同类型，如果为空的话就需要一个类型判断一次，使用std::optional可以解决该问题。
    std::optional或者为空，或者包含一个有效值
  * 使用operator *，operator->，value()或者value_or()来访问有效值
  * std::optional可以隐式转换为布尔类型，因此我们可以很方便地检查有效值是否存在

* `std::filesystem`

  参考：[C++17 C17 中的新增功能std::filesystem-CSDN博客](https://blog.csdn.net/shaderdx/article/details/108235666)

  C++17的新功能，用于跨平台文件系统

  常用类

  * path 类：说白了该类只是对字符串（路径）进行一些处理，这也是文件系统的基石。

  * directory_entry 类：功如其名，文件入口，这个类才真正接触文件。 

  * directory_iterator 类：获取文件系统目录中文件的迭代器容器，其元素为 directory_entry对象（可用于遍历目录）

  * file_status 类：用于获取和修改文件（或目录）的属性（需要了解C++11的强枚举类型（即枚举类））

  常用方法

  * std::filesystem::is_directory()检查是否为目录；

  * std::filesystem::current_path()返回当前路径；

  * std::filesystem::exists()文件目录是否存在；

  * std::filesystem::create_directories()创建目录，可以连续创建多级目录；

  

  ```cpp
  // 示例
  std::filesystem::path p1 = ""
  std::filesystem::path p2 = "a/b";
  p2 /= p1;  // 错误，左边是std::filesystem::path，右边就只能是std::string类型，不然结果路径会出错。
  ```

  
