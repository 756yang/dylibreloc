# dylibreloc

动态库函数运行时重定位(dynamic library function runtime relocation)，用以辅助
选择合适的函数版本。

通常，为避免全局变量的二次取值(在其他obj文件以指针变量存放全局变量地址，通过
此指针变量获取外部变量地址後可安全访问超出2GB地址范围的数据)，可使用小代码
模型，gcc编译参数是`-mcmodel=small`
