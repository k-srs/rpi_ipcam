# research

Raspberry Pi 4 向けネットワークカメラを Yocto で作るための調査メモ。

## GStreamer を使う方針

ネットワークカメラ用途では、まず GStreamer で映像取得、エンコード、配信を組むのが扱いやすい。

想定する構成:

```text
camera source -> encode -> RTP/RTSP or HTTP/MJPEG
```

候補:

- カメラ入力: `libcamera` 系、または V4L2 経由
- 映像処理: GStreamer plugins
- 配信: RTSP server、または RTP/UDP

## 必要になりそうな Yocto layer

GStreamer 関連は主に以下を使う。

```text
poky/meta
meta-openembedded/meta-oe
meta-openembedded/meta-multimedia
meta-raspberrypi
```

## イメージへ追加するパッケージ候補

初期検証では、イメージに以下を追加する。

```bitbake
IMAGE_INSTALL:append = " \
    gstreamer1.0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    v4l-utils \
"
```

RTSP server を使う場合は、追加で以下を検討する。

```bitbake
IMAGE_INSTALL:append = " \
    gstreamer1.0-rtsp-server \
"
```

`gstreamer1.0-plugins-ugly` や `gstreamer1.0-libav` は、コーデックやライセンス条件を確認してから追加する。

## どこに書くか

初期検証だけなら `kas/rpi4.yml` の `local_conf_header` に `IMAGE_INSTALL:append` を書いてよい。

ただし、製品イメージとして整理する段階では、独自 layer と image recipe に移す。

想定:

```text
meta-raspi-ipcam/
  conf/
    layer.conf
  recipes-images/
    raspi-ipcam-image.bb
```

image recipe の例:

```bitbake
require recipes-core/images/core-image-base.bb

IMAGE_INSTALL:append = " \
    gstreamer1.0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-rtsp-server \
    v4l-utils \
"
```

この場合、kas の `target` は `core-image-base` から `raspi-ipcam-image` に変更する。

## 自作アプリを layer に組み込む方法

RTSP server などの自作アプリは、独自 layer に recipe として追加し、image recipe からインストール対象に含める。

想定する構成:

```text
meta-raspi-ipcam/
  conf/
    layer.conf
  recipes-apps/
    raspi-ipcam/
      raspi-ipcam.bb
      files/
        raspi-ipcam.service
```

`raspi-ipcam.bb` の例:

```bitbake
SUMMARY = "Raspberry Pi IP camera application"
LICENSE = "CLOSED"

SRC_URI = " \
    git://example.com/raspi-ipcam.git;protocol=https;branch=main \
    file://raspi-ipcam.service \
"
SRCREV = "<固定したコミットハッシュ>"

S = "${WORKDIR}/git"

inherit cmake systemd

SYSTEMD_SERVICE:${PN} = "raspi-ipcam.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/raspi-ipcam.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${systemd_system_unitdir}/raspi-ipcam.service"
```

リポジトリ外から取得せず、layer 内にソースを置いて初期検証する場合は、以下のように `files/` 配下へ置ける。

```text
meta-raspi-ipcam/
  recipes-apps/
    raspi-ipcam/
      raspi-ipcam.bb
      files/
        main.c
        meson.build
        raspi-ipcam.service
```

この場合の recipe 例:

```bitbake
SUMMARY = "Raspberry Pi IP camera application"
LICENSE = "CLOSED"

SRC_URI = " \
    file://main.c \
    file://meson.build \
    file://raspi-ipcam.service \
"

S = "${WORKDIR}"

inherit meson systemd

SYSTEMD_SERVICE:${PN} = "raspi-ipcam.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/raspi-ipcam.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${systemd_system_unitdir}/raspi-ipcam.service"
```

`raspi-ipcam.service` の例:

```ini
[Unit]
Description=Raspberry Pi IP camera service
After=network-online.target
Wants=network-online.target

[Service]
ExecStart=/usr/bin/raspi-ipcam
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
```

image recipe 側で自作アプリを追加する。

```bitbake
IMAGE_INSTALL:append = " \
    raspi-ipcam \
"
```

既存の `core-image-base` に一時的に入れるだけなら、`kas/rpi4.yml` の `local_conf_header` に追加してもよい。

```bitbake
IMAGE_INSTALL:append = " raspi-ipcam"
```

layer を kas 管理に含める場合は、`kas/rpi4.yml` の `repos` に独自 layer を追加する。ローカル layer として管理する例:

```yaml
  meta-raspi-ipcam:
    path: ../meta-raspi-ipcam
```

追加後は以下で認識されていることを確認する。

```bash
kas shell kas/rpi4.yml -c 'bitbake-layers show-layers'
kas shell kas/rpi4.yml -c 'bitbake-layers show-recipes raspi-ipcam'
```

## 動作確認の観点

ビルド後、Raspberry Pi 4 上で以下を確認する。

```bash
gst-inspect-1.0
gst-inspect-1.0 v4l2src
gst-inspect-1.0 libcamera
gst-inspect-1.0 rtspclientsink
ls -l /dev/video*
```

`libcamera` plugin が使えない場合は、まず V4L2 経由で確認する。

## 配信方式の候補

### RTP/UDP

構成が単純で初期確認しやすい。クライアント側の受信方法を固定できる開発中の確認に向く。

### RTSP

VLC や監視ソフトから扱いやすい。ネットワークカメラとしては最終候補になりやすい。

Yocto では `gstreamer1.0-rtsp-server` を入れた上で、RTSP server アプリを用意する。  
単なる plugin 追加だけでは、常駐する RTSP サービスは起動しない。

## systemd service 化

最終的には GStreamer pipeline または RTSP server アプリを systemd service として起動する。

検討項目:

- 起動順序: network-online 後に開始するか
- カメラ未接続時の再試行
- プロセス異常終了時の restart
- 設定ファイル変更時の service restart

## 参考リンク

- Yocto Releases: https://wiki.yoctoproject.org/wiki/Releases
- meta-raspberrypi: https://git.yoctoproject.org/meta-raspberrypi
- GStreamer documentation: https://gstreamer.freedesktop.org/documentation/
- gst-rtsp-server: https://gitlab.freedesktop.org/gstreamer/gstreamer/-/tree/main/subprojects/gst-rtsp-server
