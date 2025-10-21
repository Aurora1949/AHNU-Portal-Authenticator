# AHNU Portal Authenticator

**AHNU Portal Authenticator** 是一个为 **安徽师范大学校园网（AHNU）** 设计的轻量级网络认证工具，
实现了登录与在线状态维持。支持 **开机自启**、**后台运行**、**实时在线检测**， 让校园网登录更加稳定、无感、安全。
本项目受到此[项目]('https://github.com/SweetCaviar/AHNU-Network-Automatic-Csharp')的启发应运而生。

---

## 功能特性

- **使用 Qt 编写**
  具有直观、简洁的使用界面。

- **Portal 协议认证**
  自动完成校园网账号认证流程，支持账户与服务商选择。

- **开机自启**
  自动注册到系统启动项，无需手动登录即可联网。

- **后台运行**
  无窗口静默运行，不打扰使用体验，可在系统托盘中查看状态。

- **实时在线监测**
  定时检测网络连接状态，掉线后提示，保证网络稳定在线。

- **多服务商支持**
  支持多个运营商（电信、移动、联通）认证。

---

## 编译方法

1. **环境准备**

| 名称    | 推荐版本                  | 说明                |
|-------|-----------------------|-------------------|
| Qt    | ≥ 6.5                 | 推荐安装 Qt 6.6 或更新版本 |
| CMake | ≥ 4.0                 | 用于跨平台构建           |
| 编译器   | MSVC 2022 / MinGW 11+ | 均可使用              |

2. **编译**

```bash
git clone https://github.com/Aurora1949/AHNU-Portal-Authenticator.git
cd AHNU-Portal-Authenticator

mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```