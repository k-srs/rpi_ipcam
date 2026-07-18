# 2. ネットワークカメラのメディアパイプラインに GStreamer を採用する

日付: 2026-07-18

## ステータス

承認済み

## コンテキスト

Raspberry Pi 4 を使用したネットワークカメラには、映像の取得、処理、
エンコード、ネットワーク配信を行うメディアパイプラインが必要である。
このパイプラインは Yocto ベースのターゲットイメージ上で動作し、同時に
開発用ホスト上でも試作と調査を行いやすいものでなければならない。

想定するパイプラインは次のとおりである。

```text
カメラ入力 -> 映像処理とエンコード -> RTP/RTSP または HTTP/MJPEG
```

利用できるカメラ機能やハードウェアアクセラレーション対応エレメントは、
開発用ホストと Raspberry Pi のターゲットイメージで異なる可能性がある。
そのため、エレメントを組み替え可能で、実行時の調査手段があり、Yocto での
利用実績がある仕組みが必要となる。

## 検討した選択肢

### GStreamer

GStreamer のエレメントを組み合わせて、カメラ入力、映像処理、エンコード、
ネットワーク配信を構成する。利用可能な場合は libcamera ベースの入力を使い、
初期検証では V4L2 をフォールバックとして使用する。配信方式は RTP/UDP、
RTSP、HTTP/MJPEG を個別に評価できる。

### 独自のメディアパイプライン

libcamera または V4L2 を直接使用し、エンコード、バッファリング、タイムスタンプ、
ネットワーク配信との連携をアプリケーション側で実装する。

細かな制御は可能になるが、プロジェクトが所有・保守するメディア処理および
通信処理のコードが大幅に増える。

### FFmpeg のライブラリおよびツール

FFmpeg を利用して映像取得、エンコード、ネットワーク配信を構成する。
FFmpeg でも必要なパイプラインを構築できるが、GStreamer のほうがカメラ入力や
組み込み向けストリーミングパイプラインをエレメント単位で交換・検証しやすい。

## 決定

カメラ入力、映像処理、エンコード、ネットワーク配信を行うメディアパイプラインの
基盤として GStreamer を採用する。

カメラ入力は、次の優先順位で選択する。

1. Raspberry Pi のターゲットイメージで利用可能かつ動作確認できた場合は、
   GStreamer の libcamera 連携を使用する。
2. libcamera エレメントが利用できない場合や初期検証では、`v4l2src` を使用する。

配信プロトコルは、この ADR では確定しない。RTP/UDP は構成が単純であるため
開発中の動作確認に適している。RTSP はメディアプレーヤーや監視ソフトウェアから
広く利用できるため、製品向けの有力候補とする。最終的な配信プロトコルと認証方式は
別途決定する。

RTSP を採用する場合は、GStreamer RTSP server ライブラリとプロジェクトで実装する
サーバーアプリケーションを使用する。`gstreamer1.0-rtsp-server` をイメージへ追加する
だけでは、常駐する RTSP サービスは起動しない。

## Yocto への組み込み

初期検証用のイメージには、次のパッケージを追加する。

```bitbake
IMAGE_INSTALL:append = " \
    gstreamer1.0 \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    v4l-utils \
"
```

RTSP を試作で使用する場合は、次のパッケージも追加する。

```bitbake
IMAGE_INSTALL:append = " \
    gstreamer1.0-rtsp-server \
"
```

必要なレシピは、主に次のレイヤーから取得する。

```text
poky/meta
meta-openembedded/meta-oe
meta-openembedded/meta-multimedia
meta-raspberrypi
```

初期検証では、パッケージの追加設定を `kas/rpi4.yml` の
`local_conf_header` に記述してよい。製品へ組み込む段階では、設定を独自の
`meta-raspi-ipcam` レイヤーと専用のイメージレシピへ移す。

プロジェクトで実装するメディア処理または RTSP server プロセスは、それぞれ
BitBake レシピでパッケージ化し、systemd サービスとして管理する。サービスでは、
ネットワークの準備完了、カメラ未接続時の動作、異常終了時の再起動、設定変更時の
再起動を考慮する。

`gstreamer1.0-plugins-ugly` と `gstreamer1.0-libav` は、標準ではイメージへ
追加しない。選択したプラグインセットで必要なコーデックを提供できない場合に限り、
ライセンスと配布条件を確認した上で追加する。

## 結果

### 利点

- パイプライン全体を再設計せずに、カメラ入力、エンコーダー、ペイローダー、
  ネットワーク配信方式を交換できる。
- `gst-inspect-1.0` とコマンドラインパイプラインを利用して、ターゲットイメージの
  機能とプラグインの有無を調査できる。
- GStreamer、主要なプラグインセット、RTSP server ライブラリには、採用した
  Yocto レイヤー内に既存のレシピがある。
- Raspberry Pi の実機が利用可能になる前に、ホスト上でパイプラインの試作を
  開始できる。
- アプリケーションはメディアのバッファリング、タイムスタンプ、通信処理ではなく、
  設定とライフサイクルの管理に集中できる。

### 欠点

- GStreamer のランタイムとプラグインを追加するため、ターゲットイメージが大きくなる。
- プラグインの有無とエレメント名を実際のターゲットイメージ上で確認する必要があり、
  ホストで動作したパイプラインが Raspberry Pi 上でもそのまま動くとは限らない。
- ハードウェアアクセラレーションと libcamera 連携は、ターゲット固有の検証が必要となる。
- RTSP を使用する場合、GStreamer パッケージに加えて専用のサーバープロセスまたは
  アプリケーションが必要となる。
- コーデックとプラグインの選択により、ライセンスと配布条件の確認作業が発生する。

## 検証方法

Raspberry Pi のターゲットイメージ上で、最初に次のコマンドを実行する。

```bash
gst-inspect-1.0
gst-inspect-1.0 v4l2src
gst-inspect-1.0 libcamera
gst-inspect-1.0 rtspclientsink
ls -l /dev/video*
```

libcamera エレメントが利用できない場合は、カメラ連携用レシピを追加・変更する前に、
V4L2 を使用して映像取得とパイプラインの検証を進める。

## 参考資料

- [ネットワークカメラの調査メモ](../research.md)
- [GStreamer ドキュメント](https://gstreamer.freedesktop.org/documentation/)
- [GStreamer RTSP server](https://gitlab.freedesktop.org/gstreamer/gstreamer/-/tree/main/subprojects/gst-rtsp-server)
- [meta-raspberrypi](https://git.yoctoproject.org/meta-raspberrypi)
