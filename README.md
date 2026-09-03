<p align="center">
  <img src="assets/logo.png" alt="OnionHEN" height="128" width="128"/>
</p>

<p align="center">
  <b>OnionHEN DPI v2 Plugin</b><br/>
  Browser-based remote package installer for OnionHEN
</p>

<p align="center">
  <a href="README_ZH.md">简体中文</a> · <b>English</b>
</p>

<p align="center">
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-GPLv3-blue.svg" alt="license"/></a>
  <img src="https://img.shields.io/badge/Platform-PlayStation%205-003791?style=flat&logo=playstation" alt="PlayStation 5"/>
  <img src="https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white" alt="C"/>
  <img src="https://img.shields.io/badge/Build-CMake-064F8C?style=flat&logo=cmake" alt="CMake"/>
</p>

DPI v2 receives PS4 and PS5 `.pkg` files over the local network, stages them
on the console, and submits them to the PS5 system installer. It is a normal
OnionHEN plugin ELF with an embedded descriptor and WebUI; no separate package
or runtime assets are required.

The browser remains the primary installer interface. The dynamic OnionHEN XML
page is deliberately limited to service configuration: start/stop, API port,
WebUI port, and restart.

## Features

- Drag-and-drop multi-file uploads from a desktop or mobile browser
- Chunked transfer, resumable staging, and staged-file reuse
- PS4/PS5 package detection before installation
- Sortable install queue, per-file retry, and live SSE progress
- Localized WebUI and console notifications for 14 languages
- Configurable API and WebUI ports with rollback if rebinding fails
- Graceful start, stop, reload, replacement, and removal through OnionHEN

## Requirements

- An OnionHEN build with external plugin support
- [OnionHEN Plugin SDK](https://github.com/OnionBuddies/onionHEN-plugin-sdk)
- [PS5 Payload SDK](https://github.com/ps5-payload-dev/sdk)
- CMake 3.20 or newer, Ninja, Git, and Python 3.9 or newer
- Node.js and npm only when rebuilding the WebUI

## Build

```sh
export PS5_PAYLOAD_SDK=/path/to/ps5-payload-sdk
cmake --preset ps5
cmake --build --preset ps5
```

The result is `build-ps5/bin/dpiv2.elf`. The build validates the embedded
descriptor automatically.

The SDK dependency is pinned to a tested commit. During SDK development, use a
local checkout:

```sh
cmake --preset ps5 \
  -DONIONHEN_PLUGIN_SDK_SOURCE=/path/to/onionHEN-plugin-sdk
cmake --build --preset ps5
```

To rebuild the embedded browser application:

```sh
cd webui
npm ci
npm run build
```

`webui/dist/index.html` is a single-file production bundle and is embedded in
the ELF at build time.

## Install

Upload the ELF atomically as
`/data/OnionHEN/plugins/DPIV00001.installing`, then rename it to
`/data/OnionHEN/plugins/DPIV00001.elf` after the upload completes.

OnionHEN discovers and starts it automatically. Open **★ OnionHEN Plugins →
DPI v2** to enable or disable the server, change its ports, or restart it.

With the default configuration, open this URL from another device on the same
network:

```text
http://<PS5-IP>:12800
```

TCP `9090` serves the DPI transfer API and TCP `12800` serves the WebUI and SSE
stream. The ports must be different. Changes are stored in
`/data/OnionHEN/plugins/DPIV00001.ini`.

## Storage and logs

| Path | Purpose |
| --- | --- |
| `/user/data/tmp/` | Staged package uploads retained for retry/reuse |
| `/data/OnionHEN/plugins/DPIV00001.ini` | Enabled state and listener ports |
| `/data/OnionHEN/DPIV00001.log` | Plugin lifecycle and dynamic UI errors |
| `/data/OnionHEN/DPIV00001-server.log` | DPI transfer and installer log |

See [docs/api.md](docs/api.md) for the HTTP and SSE protocol.

## Project structure

```text
.
├── i18n/                    console-notification locale catalogs
├── include/                 plugin, service, settings, and UI interfaces
├── source/                  lifecycle, dynamic UI, service, and localization
├── third_party/pkgserver/   vendored DPI transfer/install server
├── tools/                   notification catalog generator
├── webui/                   browser application and embedded dist bundle
├── CMakeLists.txt           SDK integration and PS5 plugin target
└── CMakePresets.json        standard PS5 configure/build commands
```

## Security

DPI listens on all console network interfaces and does not authenticate
clients. Use it only on a trusted local network, stop it when it is not needed,
and never expose either port to the internet. Uploaded packages are untrusted
input; final package validation is performed by the PS5 system installer.

Read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a pull request.
Participation is governed by [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md). Report
security-sensitive issues privately according to [SECURITY.md](SECURITY.md).
Third-party attribution is listed in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).

OnionHEN is an unofficial homebrew project and is not affiliated with Sony
Interactive Entertainment. Use it only on hardware you own and at your own
risk. No warranty is provided.
