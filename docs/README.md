# ADR セットアップガイド

このプロジェクトでは、アーキテクチャ上の重要な判断を Architecture
Decision Record（ADR）として Markdown で管理します。ADR の作成・検索・状態変更には
[`adrs`](https://github.com/joshrotenberg/adrs)を使用します。

## 1. Rust と rustup のインストール

```bash
sudo apt update
sudo apt install rustup
rustup default stable
```

## 2. Cargo の PATH 設定

```bash
export PATH="$HOME/.cargo/bin:$PATH"
source "$HOME/.bashrc"
```

## 3. adrs のインストール

```bash
cargo install adrs --locked
```

インストール結果を確認します。

```bash
command -v adrs
adrs --version
```

通常、実行ファイルは次の場所に作成されます。

```text
$HOME/.cargo/bin/adrs
```

## 4. プロジェクトでのADR初期化

プロジェクトのルートディレクトリで実行します。

```bash
cd ~/rpi_ipcam
adrs init
```

初期化後は、ADRの保存先やテンプレートなど、生成された設定を確認してください。
このプロジェクトではMADR 4.0形式のMarkdownを推奨します。

最初のADRは、ADRを採用した判断そのものを記録します。

```bash
adrs new --format madr --variant minimal \
  "Use architecture decision records"
```

## 5. 日常的に使用するコマンド

新しいADRをMADR形式で作成します。

```bash
adrs new --format madr --variant minimal \
  "Select camera capture backend"
```

ADRの一覧を表示します。

```bash
adrs list
```

ADRを全文検索します。

```bash
adrs search camera
```

ADRリポジトリの構成やリンクを検査します。

```bash
adrs doctor
```

既存のADRを新しい判断で置き換える場合は、置き換えられるADR番号を指定します。

```bash
adrs new --format madr --variant minimal --supersedes 2 \
  "Replace camera capture backend"
```

目次を生成する場合は、ADRの保存先に合わせて出力先を指定します。

```bash
adrs generate toc > docs/decisions/README.md
```

## 6. 更新

Rustのstableツールチェーンを更新します。

```bash
rustup update stable
```

`adrs`を最新版へ更新します。

```bash
cargo install adrs --locked --force
```

更新後に動作を確認します。

```bash
rustc --version
adrs --version
adrs doctor
```

## 参考資料

- [adrs](https://github.com/joshrotenberg/adrs)
- [The rustup book](https://rust-lang.github.io/rustup/)
- [MADR（Markdown Architectural Decision Records）](https://adr.github.io/madr/)
