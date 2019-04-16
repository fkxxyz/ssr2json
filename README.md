# ssr2json

## 简介
该项目包含一系列工具集合，订阅链接的内容、ssr链接都是用base64编码的，该工具集合包含处理订阅链接、ssr链接的功能，最终转换成shadowsocksr可用的json配置文件。

## 工具介绍
### ssr2json
c语言编译出的二进制程序，将 ssr:// 开头的链接地址转换为 json 数据格式并输出到标准输出。
### ssr2jsonsave
shell 脚本，用于批量处理，从标准输入的每一行的 ssr:// 链接，均转换为 json 数据格式，并保存到指定目录中，每一组单独文件夹名，每一个名称对应文件名。
### ssrsub
shell 脚本，用于批量处理，从标准输入的每一行的订阅链接，解析出 ssr:// 链接输出到标准输出。
### ssrsubsave
shell 脚本，用于批量处理，从标准输入的每一行的订阅链接，自动调用 ssrsub 和 ssr2jsonsave 保存至指定文件夹。
### ssrctl
shell 脚本，用于储存和管理订阅链接和节点，可以选择性的切换某个节点（需要python版的[shadowsocksr](https://github.com/shadowsocksr-backup/shadowsocksr)支持，并添加进systemd 服务设置）。

## 安装方法
克隆该仓库
```
git clone https://github.com/fkxxyz/ssr2json.git
```
进入仓库目录
```
cd ssr2json
ls
```
自动生成 configure 脚本
```
./autogen.sh
```
配置、编译、安装三连
```
./configure --prefix=/usr
make
make install
```




