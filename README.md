# VirtualMath

## 什么是VirtualMath

VirtualMath是基于``C``语言实现的动态强类型解释形编程语言，使用``VirtualMathCore``解释器解释执行。

项目分为两部分：

1. ``VirtualMathCore``是``VirtualMath``核心，包括``parser``文本解析器和``vm``运行器。
2. ``VMHello``是使用``VirtualMathCore``的可视化编程工具，使用基于命令行模式。

## 项目编译

使用``cmake``生成项目``Makefile``文件后使用``make``和``make install``工具进行编译和安装。

``cmake``支持参数包括：

1. ``GC``，默认为``ON``状态表示编译GC模块，否则不编译``GC``模块
2. ``CMAKE_BUILD_TYPE``，指定编译类型，设置为``Debug``的时候可以编译``Debug``程序。

## 使用VirtualMath

``VirtualMath``支持如下命令行参数：

```
-n 不进入command line模式
--stdout xxx 指定标准输出的位置
--stderr xxx 指定标准错误的位置
--stdin xxx 指定标准输入的位置
```

注意以上设定的标准输入\输出\错误的位置只适用于解释器所运行的``VM``代码。

命令行参数末尾的单独参数被解释为``vm``文件名，将交由``VirtualMathCore``逐一解释执行。

## VirtualMath语法

### 字面量

支持字面量数字和字面量字符串

```
10
20.3
"Hello"
'World'
```

注意，字符串暂不支持转义字符串。

注意，解析器允许使用小数但``VirtualMathCore``仅支持整数。

#### 字面量后缀

``VirtualMath``支持使用字面量后缀。

如，``123i``等价于``i(123)``。关于回调将会在后面详细说明。

### 变量

``VritaulMath``变量分为普通变量和超级变量。

普通变量名(标识符)由下划线或字母开头，仅包含下划线、字母和数字的字符组成，且为非关键词。

```
_
_a
a_
_23
a23
a_23
```

以上均为合法变量名

```
23a
23_
if
```

以上为非法的变量名

超级变量由``$``开头后跟表达式，如：

```
$10
$(20 + 30)
```

因此，``VirtualMath``允许使用数字等其他类型的数据作为变量名。但其访问时必须使用``$``符号。

关于``$``的优先级：

```
$20+30 等价于 ($20) + 30
```

### 运算

``VirtualMath``支持如下运算：

#### 加法和减法

```
10 + 20
20 + 30
"Hello" + " World"
100 - 200
500 - 100
```

#### 乘法和除法

```
10 * 30
5 / 4
"Hello" * 3
```

#### 赋值运算

```
a = 10
b = 20
20 = 30
$(20) = 30
```

其中``20 = 30``是等价于``$(20) = 30``的，也许会引起一些争议，但别紧张你会习惯的。

同时，还有特殊类型的赋值语句：

```
a[2] = 10
a[2:3] = z
```

注意，该赋值并非``$(a[2]) = 10``，这种赋值方式需要对象尊需一定协议才可使用，具体在后面将会介绍。

同时，也支持如下赋值语句：

```
f(x, y) = x * y
```

注意，以上语句并不等价于``$(f(x, y)) = x * y``，他其实是一种简单地函数定义，等价于：

```
def f(x, y){
	return x * y
}
```

### 分支语句

#### 条件分支

```
do {

} if (xxx) {

} elif (xxx) as f{

} do{

} elif (xxx) {

} else {

} finally{

}
```

语句自上而下执行，遇到``do``语句会直接执行``do``语句内的代码并且继续执行。遇到``if``或者``elif``分支则会判断条件，然后赋值结果，再执行内部代码块，然后调准到``finally``分支。``finally``分支是条件分支离开前必然会执行的，即便条件分支内发生错误(``VirtualMathCore``或``HelloVM``的错误除外)。

#### 循环分支

```
do{
# 循环前执行一次
} while (xxx) as f{

} do {
# 每次循环后执行一次(break除外)
} else {

} finally{

}
```

``else``分支只有当``while``中的条件为``false``的时候才会执行。当循环体内使用了``break``语句跳出循环的时候``else``不会执行。

#### 遍历分支

```
do {

} for i in a{

} do {

} else {

} finally {

}
```

``for``循环的``do``分支和``while``循环类似。``else``分支只有当迭代结束的时候才会执行。当循环体内使用了``break``语句跳出循环的时候``else``不会执行。

#### do…循环

```
do while (xxx) as f{

} do {

} else {

} finally {

}
```

该循环并没有将条件判断后置，而是对首次条件判断的结果忽略并且设定为``True``。

#### 错误捕获分支

```
try {

} except {

} except as e{

} except xxx {

} except xxx as e {

} else {

} finally {

}
```

### with分支

```
with xxx {

} else {

} finally {

}

with xxx as f{

} else {

} finally {

}
```

### 更多特性

``VirtualMath``还有很多特性，可自行探索。