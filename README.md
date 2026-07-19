# rpi_ipcam

Raspberry Pi 4向けのネットワークカメライメージを、Yocto Projectとkasで構築するリポジトリ。  
[wikiのホームはこちら](https://github.com/k-srs/rpi_ipcam/wiki)

## 構成

- Board: Raspberry Pi 4
- Architecture: 64-bit
- Yocto release: `scarthgap`
- MACHINE: `raspberrypi4-64`
- kas config: [`kas/rpi4.yml`](kas/rpi4.yml)

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
