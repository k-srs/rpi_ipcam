# rpi_ipcam

Raspberry Pi 4向けのネットワークカメライメージを、Yocto Projectとkasで構築するリポジトリ。

## 構成

- Board: Raspberry Pi 4
- Architecture: 64-bit
- Yocto release: `scarthgap`
- MACHINE: `raspberrypi4-64`
- kas config: [`kas/rpi4.yml`](kas/rpi4.yml)

## 開発環境

Yoctoのビルドには、Linux環境、ネットワーク接続、十分なメモリとディスク容量が必要。空きディスク容量は140 GB以上を推奨する。

Ubuntuでは、次のパッケージをインストールする。

```bash
sudo apt update
sudo apt install \
  build-essential clang-format clang-tidy clang-tools chrpath cpio curl \
  debianutils diffstat file gawk gcc git iputils-ping libacl1 libcrypt-dev \
  locales ninja-build pipx python3 python3-git python3-jinja2 python3-pexpect \
  python3-pip python3-subunit socat texinfo unzip wget xz-utils zstd
pipx ensurepath
pipx install meson
pipx install kas
```

`pipx ensurepath` の実行後は、ターミナルを開き直す。

MesonとNinjaは、`src/backend/` 以下をホスト環境で直接ビルドするときに使用する。kasによるYoctoビルドだけを行う場合は不要。

フロントエンドで使用するBunのバージョンを、`src/frontend/rpi-viewer/package.json` の `packageManager` から取得してインストールする。リポジトリルートで次のコマンドを実行する。

```bash
rpi_bun_version="$(python3 -c 'import json; print(json.load(open("src/frontend/rpi-viewer/package.json"))["packageManager"].removeprefix("bun@"))')"
curl -fsSL https://bun.com/install | bash -s "bun-v${rpi_bun_version}"
```

インストール後はターミナルを開き直し、バージョンを確認する。

```bash
bun --version
```

## バックエンド

バックエンドの各プロセスは、独立したMesonプロジェクトとして `src/backend/` 以下に配置する。ここでは `http-server` を例に説明する。

### セットアップ

初回のみビルドディレクトリを作成する。clang-tidyが参照する `out/compile_commands.json` も生成される。

```bash
cd src/backend/http-server
meson setup out
```

Mesonの設定を更新したときは、ビルドディレクトリを再構成する。

```bash
meson setup --reconfigure out
```

### ビルド

```bash
meson compile -C out
```

### テスト

テストにはCppUTestを使用する。初回の `meson setup` または `meson setup --reconfigure` で、`subprojects/cpputest.wrap` に固定されたソースを取得する。ホスト環境へのCppUTestのインストールは不要。

テストをビルドして実行する。

```bash
meson test -C out --print-errorlogs
```

### 静的解析

clang-tidyは、コンパイル可能なソースコードを前提とする。先にビルドを実行し、コンパイルエラーを解消する。

特定のソースファイルを検査する。

```bash
clang-tidy -p out http_server.cpp
```

`out/compile_commands.json` に登録されたすべてのソースファイルを検査する。

```bash
run-clang-tidy -p out
```

解析ルールは、リポジトリルートの [`.clang-tidy`](.clang-tidy) で管理する。

### コードフォーマット

フォーマットが必要かを、ファイルを変更せずに検査する。

```bash
clang-format --dry-run --Werror http_server.cpp
```

ファイルをフォーマットする。

```bash
clang-format -i http_server.cpp
```

フォーマットルールは、リポジトリルートの [`.clang-format`](.clang-format) で管理する。clang-formatは、対象ファイルから親ディレクトリをたどり、最初に見つかった設定ファイルを使用する。

新しいプロジェクトでLLVMスタイルの設定ファイルを生成するコマンドは次のとおり。

```bash
clang-format --style=llvm --dump-config > .clang-format
```

このコマンドは既存の `.clang-format` を上書きする。このリポジトリでは実行せず、登録済みの設定を使用する。

## フロントエンド

フロントエンドは `src/frontend/rpi-viewer/` に配置する。パッケージマネージャーにはBunを使用し、バージョンは `package.json` の `packageManager` で管理する。

### セットアップ

リポジトリの取得後やCIでは、`bun.lock` に記録されたバージョンをインストールする。

```bash
cd src/frontend/rpi-viewer
bun ci
```

`bun ci` は、`package.json` と `bun.lock` が一致しないとエラーになる。依存パッケージの追加や更新には `bun add` または `bun install` を使用し、変更された `package.json` と `bun.lock` を一緒にコミットする。

### 開発サーバー

```bash
bun run dev
```

### ビルドとテスト

```bash
bun run build
bun run test:unit
```

`package.json` と `bun.lock` は `src/frontend/rpi-viewer/` 直下で管理する。`node_modules/` はGitへコミットしない。

## Yoctoイメージのビルド

Yocto関連のリポジトリを取得する。

```bash
kas checkout kas/rpi4.yml
```

レイヤー構成を確認する。

```bash
kas shell kas/rpi4.yml -c 'bitbake-layers show-layers'
```

イメージをビルドする。

```bash
kas build kas/rpi4.yml
```

生成物は次のディレクトリに配置される。

```text
build/tmp/deploy/images/raspberrypi4-64/
```

## SDカードへの書き込み

`.wic.bz2` イメージをSDカードへ書き込む。

```bash
bzcat build/tmp/deploy/images/raspberrypi4-64/core-image-base-raspberrypi4-64.rootfs.wic.bz2 | sudo dd of=<デバイス名> bs=4M status=progress conv=fsync
```

デバイス名は、事前に `lsblk` で確認する。指定を誤ると、ホストPCのディスクを上書きする可能性がある。
