
## 软件环境
* Windows 10 1809 64bit
* VS2017 Enterprise
* MFC

## 实现功能
1. 重绘ComboBox
2. 添加删除按钮
3. 自适应下拉List的宽度
4. 记录最后一次ComboBox的窗口文本

## 实现原理
### ComboBox属性设置 
* 重载ComboBox **CompareItem/DrawItem**
* Owner Draw 设置为 **Fixed**
* Has Strings 设置为 **True**
* Type 设置为 **DropDown/DropList**
### 技术要点
* **PreSubclassWindow**: 设置ListBox的**GWLP_WNDPROC**
 *获取ListBox上的鼠标信息*
 * **OnCbnDropdown**: 自适度下拉宽度
 * **OnPaint**: ComboBox自绘
 * **DrawItem**:  绘制下拉选项，删除图标

## 效果
### DropDown
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190124144130320.png)

### DropList
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190124144139149.png)

### Disable Window
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190124144216852.png)

