# Backend

バックエンドは複数プロセスで構成します。各プロセスを独立した Meson プロジェクトとして、`backend/` の直下に配置します。

```text
backend/
├── README.md
├── rpi-ipcam-server/
│   ├── meson.build
│   └── ...
└── <process-name>/
    ├── meson.build
    └── ...
```

各プロジェクトを分離することで、プロセスごとにビルド、テスト、Yocto レシピおよび systemd サービスを管理できるようにします。

## 前提ツール

Meson、Ninja および C++ コンパイラが必要です。

```bash
sudo apt install build-essential ninja-build pipx
pipx install meson
```

既に Meson をインストールしている場合は、再インストールする必要はありません。

## 新規プロジェクトの作成

リポジトリのルートから `src/backend/` へ移動します。

```bash
cd src/backend
```

プロセス名のディレクトリを作成し、その中で `meson init` を実行します。ディレクトリ名と実行ファイル名には、小文字のケバブケースを使用します。例えば `config-manager` プロセスを作る場合は、次のように実行します。

```bash
mkdir config-manager
cd config-manager
meson init --language cpp --name config-manager --type executable
```

プロジェクト作成後、少なくとも次のファイルが生成されます。

```text
config-manager/
├── meson.build
└── config_manager.cpp
```

## ビルド

各プロジェクトのルートで、初回のみビルドディレクトリを構成します。

```bash
meson setup out
```

コンパイルします。

```bash
meson compile -C out
```

実行ファイルは `out/` の直下に生成されます。

```bash
./out/config-manager
```

Meson の更新などによって既存のビルド情報を再構成する必要がある場合は、次を実行します。

```bash
meson setup --reconfigure out
```

## テスト

`meson init` が生成したプロジェクトには基本テストが登録されています。

```bash
meson test -C out --print-errorlogs
```

## 日常的な開発

初回の `meson setup out` が完了した後は、通常は次のコマンドを使用します。

```bash
meson compile -C out
meson test -C out --print-errorlogs
```

`out/` はビルド生成物なので Git にはコミットしません。ソースコード、`meson.build` およびテストコードをコミットしてください。

## プロセス追加時の方針

- 1つのプロセスにつき、1つのディレクトリと実行ファイルを作成します。
- プロセス同士を直接リンクせず、必要に応じて IPC を介して連携させます。
- 複数プロセスで利用するコードは、共通ライブラリとして切り出します。
- Yocto へ組み込む際は、プロセスごとにパッケージ化と systemd サービスの登録を検討します。
