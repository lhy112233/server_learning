1、为什么有PODStruct和PODUnion和Union三种不同的存储实现?
答: Union类型的存储实现是为了非POD类型而准备的。
    PODUnion则是为了较大的类型而准备,主要是因为Union能够复用存储,减少了花费的内存。
    PODStruct则是在存储sizeof(Value) + sizeof(Error) <= 2*sizeof(void*)小时使用的。

2、为什么有sizeof(Value) + sizeof(Error) <= 2*sizeof(void*)这个判断？
答：主要是因为对齐这一个因素的存在.在64位系统下,当malloc时(默认)，齐最少也会分配16字节的内存,刚刚好两个指针的大小。
 参考链接:[GCC](https://www.gnu.org/software/libc/manual/html_node/Aligned-Memory-Blocks.html)
          [MSVC](https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/malloc?view=msvc-170)