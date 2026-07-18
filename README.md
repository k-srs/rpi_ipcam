# rpi_ipcam

Raspberry Pi 4 を使ったネットワークカメラを、Yocto Project + kas で構築するためのリポジトリです。

## 前提
- Board: Raspberry Pi 4
- Architecture: 64-bit
- Yocto release: `scarthgap`
- MACHINE: `raspberrypi4-64`
- kas config: [kas/rpi4.yml](/kas/rpi4.yml)

## ビルド環境
- Yocto のビルドには Linux 環境、十分なディスク容量、メモリ、ネットワーク接続が必要。  
目安は空きディスク 140GB 以上。
- 以下のコマンドを実行し、必要なパッケージをインストールする。
```bash
sudo apt update
sudo apt install build-essential chrpath cpio debianutils diffstat file gawk gcc git iputils-ping libacl1 libcrypt-dev locales python3 python3-git python3-jinja2 python3-pexpect python3-pip python3-subunit socat texinfo unzip wget xz-utils zstd
sudo apt install pipx
pipx ensurepath
pipx install kas
```

## フロントエンドのセットアップ

フロントエンドは `src/frontend/rpi-viewer/` に配置しています。
パッケージマネージャーには Bun を使用します。必要なバージョンは `package.json` の `packageManager` を参照してください。

### 依存パッケージのインストール

リポジトリを取得した直後や CI では、`bun.lock` に記録されたバージョンを変更せずにインストールします。

```bash
cd src/frontend/rpi-viewer
bun ci
```

`bun ci` は `package.json` と `bun.lock` が一致しない場合にエラーとなります。依存パッケージを追加・更新するときだけ `bun add` または `bun install` を使用し、更新された `package.json` と `bun.lock` を一緒にコミットしてください。

### 開発サーバーの起動

```bash
cd src/frontend/rpi-viewer
bun run dev
```

### ビルドとテスト

```bash
bun run build
bun run test:unit
```

フロントエンドの `package.json` と `bun.lock` は、Vue プロジェクトのルートである `src/frontend/rpi-viewer/` 直下で管理します。`node_modules/` は生成物のため Git にはコミットしません。

## ビルド方法
Yocto 関連リポジトリを取得

```bash
kas checkout kas/rpi4.yml
```

レイヤー構成を確認
```bash
kas shell kas/rpi4.yml -c 'bitbake-layers show-layers'
```

最小イメージをビルド
```bash
kas build kas/rpi4.yml
```

生成物は通常ここに出力される
```text
build/tmp/deploy/images/raspberrypi4-64/
```

## デバイスへの書き込み
`.wic.bz2` を SD カードへ書き込む。
```bash
bzcat build/tmp/deploy/images/raspberrypi4-64/core-image-base-raspberrypi4-64.rootfs.wic.bz2 | sudo dd of=<実際のデバイス名(lsblkコマンドで確認)> bs=4M status=progress conv=fsync
```

`<実際のデバイス名>` を間違えるとホスト PC のディスクを破壊する可能性があるため、必ず `lsblk` で確認してから実行します。
