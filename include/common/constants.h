#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
namespace TrayIconMsg {
inline QString const Online = "AHNU上号器正在后台运行\n目前状态：在线";
inline QString const Offline = "AHNU上号器正在后台运行\n目前状态：离线";
inline QString const AutoHideToBackground =
    "你已成功上线，AHNU上号器正在后台运行";
} // namespace TrayIconMsg

namespace StatusBarMsg {
inline QString const WaitForProvider = "正在登录到%1...";
inline QString const AlreadyOnline = "你已经连接上互联网";
inline QString const NetworkError = "认证服务器连接错误：%1";
inline QString const MsgFromProvider = "认证服务器：%1";
inline QString const AlreadyOffline = "已断开连接";
} // namespace StatusBarMsg

namespace Config {
inline QString const RegKey =
    "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
inline QString const UserInfoFileName = "userinfo.json";
} // namespace Config

namespace Network {
inline QString const BaseUrl = "http://100.64.4.10:801/eportal/portal/login";
inline QString const TestUrl = "http://www.baidu.com";
inline QString const LogoutUrl = "http://100.64.4.10:801/eportal/portal/logout";
} // namespace Network
} // namespace Constants

#endif // CONSTANTS_H
