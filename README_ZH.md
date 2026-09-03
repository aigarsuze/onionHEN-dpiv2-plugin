<p align="center">
  <img src="assets/logo.png" alt="OnionHEN" height="128" width="128"/>
</p>

<p align="center">
  <b>OnionHEN DPI v2 插件</b><br/>
  OnionHEN 的浏览器远程软件包安装器
</p>

<p align="center">
  <b>简体中文</b> · <a href="README.md">English</a>
</p>

DPI v2 通过本地网络接收 PS4 与 PS5 `.pkg` 文件，将文件暂存到主机后提交给
PS5 系统安装器。它是带内嵌 descriptor 和 WebUI 的普通 OnionHEN 插件 ELF，
无需额外安装包或运行时资源。

浏览器仍是主要的安装操作界面。OnionHEN 动态 XML 页面只负责服务配置：启停、
API 端口、WebUI 端口和重启，不承载上传或安装流程。

## 功能

- 从电脑或移动设备浏览器拖放并批量上传
- 分块传输、断点暂存和已有暂存文件复用
- 安装前识别 PS4/PS5 软件包类型
- 可排序安装队列、单文件重试和 SSE 实时进度
- WebUI 与主机通知支持 14 种语言
- API 与 WebUI 端口可配置，绑定失败时自动回滚
- 由 OnionHEN 完成优雅启动、停止、重载、替换和删除

## 构建要求

- 支持外部插件的 OnionHEN 版本
- [OnionHEN Plugin SDK](https://github.com/OnionBuddies/onionHEN-plugin-sdk)
- [PS5 Payload SDK](https://github.com/ps5-payload-dev/sdk)
- CMake 3.20 或更高版本、Ninja、Git、Python 3.9 或更高版本
- 仅在重新构建 WebUI 时需要 Node.js 和 npm

## 构建

```sh
export PS5_PAYLOAD_SDK=/path/to/ps5-payload-sdk
cmake --preset ps5
cmake --build --preset ps5
```

产物是 `build-ps5/bin/dpiv2.elf`，构建结束后会自动校验内嵌 descriptor。

开发 SDK 时可直接使用本地检出：

```sh
cmake --preset ps5 \
  -DONIONHEN_PLUGIN_SDK_SOURCE=/path/to/onionHEN-plugin-sdk
cmake --build --preset ps5
```

重新构建内嵌网页：

```sh
cd webui
npm ci
npm run build
```

`webui/dist/index.html` 是单文件生产 bundle，编译插件时会直接嵌入 ELF。

## 安装

先上传为 `/data/OnionHEN/plugins/DPIV00001.installing`，上传完成后再原子重命名为
`/data/OnionHEN/plugins/DPIV00001.elf`。

OnionHEN 会自动发现并启动插件。进入 **★ OnionHEN 插件 → DPI v2**，可以启停
服务、修改端口或重启服务。

默认配置下，在同一局域网的其他设备上打开：

```text
http://<PS5-IP>:12800
```

TCP `9090` 提供 DPI 传输 API，TCP `12800` 提供 WebUI 和 SSE 状态流；两个端口
不能相同。配置保存在 `/data/OnionHEN/plugins/DPIV00001.ini`。

## 存储与日志

| 路径 | 用途 |
| --- | --- |
| `/data/OnionHEN/pkgs/` | 保留用于重试或复用的软件包暂存文件 |
| `/data/OnionHEN/plugins/DPIV00001.ini` | 启用状态与监听端口 |
| `/data/OnionHEN/DPIV00001.log` | 插件生命周期与动态 UI 错误 |
| `/data/OnionHEN/DPIV00001-server.log` | DPI 传输与安装日志 |

HTTP 与 SSE 协议详见 [docs/api.md](docs/api.md)。

## 项目结构

```text
.
├── i18n/                    主机通知翻译目录
├── include/                 插件、服务、设置和 UI 接口
├── source/                  生命周期、动态 UI、服务和本地化实现
├── third_party/pkgserver/   DPI 传输与安装服务
├── tools/                   通知翻译表生成器
├── webui/                   浏览器应用和内嵌 dist bundle
├── CMakeLists.txt           SDK 集成与 PS5 插件目标
└── CMakePresets.json        标准 PS5 配置和构建命令
```

## 安全说明

DPI 会监听主机的所有网络接口，并且不验证客户端身份。只应在可信局域网中使用，
不用时请关闭服务，不要将任一端口暴露到互联网。上传的软件包是不可信输入，最终
的软件包校验由 PS5 系统安装器完成。

提交 PR 前请阅读 [CONTRIBUTING.md](CONTRIBUTING.md)。社区行为受
[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) 约束。安全问题请按照
[SECURITY.md](SECURITY.md) 私下报告。第三方归属见
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

## 许可证

本项目采用 [GNU General Public License v3.0](LICENSE)。

OnionHEN 是非官方自制软件项目，与 Sony Interactive Entertainment 无关。请仅在
自己拥有的硬件上使用，风险自负，不提供任何担保。
