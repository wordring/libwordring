libwordring
====

C++ ライブラリ。 
Linux と Windows でテストされています。

# 主な機能

- WHATWG HTML5 仕様の HTML Parser。
- WHATWG Encoding 仕様の文字コード変換。
- CSS Selectors Level 4 仕様の CSS セレクタ。
- Tree コンテナ、および、Tree 走査用のイテレータ・アダプタ。
- 動的 TRIE 木。

# 説明

組み合わせられる部品の集まりであることを念頭にデザインしています。
過度の前提を持ちません。

文書は未整備です。
Doxygen によるリファレンスを参考にしてください。
**sample** あるいは **test** ディレクトリで実際の使用例を見てください。

プログラミング言語として C\++17 、ビルド・システムに CMake を用います。
テストしている環境は以下の通りです。

- Linux gcc9
- Windows10 clang-cl
- Windows10 cl

# 使い方

Windows 用、 Linux 用など、個別の環境用のパッケージは有りません。
ヘッダーファイルと libwordring ライブラリをあなたのプロジェクトへ追加してください。

## CMake であなたのプロジェクトを管理する場合

1. あなたのプロジェクトの Git サブモジュールとして libwordring を追加する。
2. あなたのプロジェクトの CMakeLists.txt で add_subdirectory("submodules/libwordring") する。
3. あなたのプロジェクトの CMakeLists.txt で include_directories("submodules/libwordring/include") する。
4. あなたのプロジェクトの CMakeLists.txt で target_link_libraries("libwordring") する。

のが容易です。
ライブラリのビルドは、あなたのプロジェクトのビルド時に、必要に応じて CMake が行います。

# ライブラリのビルド

Github からサブモジュールを含めてクローンし、 CMake でビルドするだけです。
CMake から要求ライブラリが見える状態が必要です。
要求ライブラリの用意が最も難しいと思います。

ビルド時間のほぼすべてが単体テストをビルドするために費やされます。
時間がかかりすぎる場合は、ルート・ディレクトリの CMakeLists.txt を編集して、
ライブラリ単体をビルドしてください

## 要求環境

- C++17
- 出来るだけ新しい CMake

## 要求ライブラリ

- 出来るだけ新しい Boost

## 要求サブモジュール

| 名前 | 説明 |
|----|----|
| https://github.com/publicsuffix/list | URLパーサでデータを使用 |
| https://github.com/whatwg/encoding | HTML5パーサでデータを使用 |
| https://github.com/dwyl/english-words | TRIE用単体テストでデータを使用 |
| https://github.com/hingston/japanese | TRIE用単体テストでデータを使用 |

## Ubuntu20.04 (WSL) のメモ

- sudo apt install build-essential cmake ninja-build libboost-all-dev

## Debian 10 buster のメモ

標準の C++ コンパイラが gcc-8 です。
このコンパイラは、 c\++17 への対応が弱いです。
もう少しで Debian 11 bullseye が出荷されると　gcc-10 になるので対応を行っていません。
具体的には、　CMakeLists.txt に -lstdc++fs を含めると gcc-8 でビルドできます。

## Visualstudio 2019 のメモ

Visualstudio 2019 から WSL 上の Linux コマンドを直接呼び出しビルドすることが出来ます。
しかし、 WSL 上で複数のディストリビューションを切り替えて使う場合は、注意が必要です。
Visualstudio の作業ディレクトリに CMake のキャッシュが残っているからです。
デフォルトのディストリビューションを切り替えた時は、メニューバーの「プロジェクト」から CMake キャッシュを削除してください。
CMake がパッケージを見つけられない理由の大部分は、別のディストリで使ったキャッシュが残っているからです。

Windows 用に CMake プロジェクトをビルドするのはフォルダを開いてボタンを押すだけですが、ライブラリの用意は非常に大変です。
そこで、 VCPKG の利用をお勧めします。

# 文書

| 名前 | 説明 |
|----|----|
| https://wordring.github.io/libwordring/ | 付属文書 |

# フォルダとファイル

| 名前 | 説明 |
|----|----|
| docs | 文書を格納するフォルダ |
| generator | ソース・コード生成プログラムを格納するフォルダ |
| include | ヘッダ・ファイルを格納するフォルダ |
| lib | ライブラリのソース・コードを格納するフォルダ |
| sample | 例を格納するフォルダ |
| submodules | githubから取り込む他のモジュールを参照するフォルダ |
| test | テスト・コードを格納するフォルダ |
| .editorconfig | （Visual studioを含む多くのエディタがサポートする）エディタの設定ファイル |
| .gitignore | gitに無視させるファイルとフォルダの設定ファイル |
| .gitmodules | サブモジュールの設定ファイル |
| CMakeLists.txt | CMakeプロジェクトの設定ファイル |
| CMakeSettings.json | Visual studio用のCMake環境設定ファイル |
| LICENSE | パブリック・ドメインであることを明示するライセンス・ファイル |
| README.md | このファイル |
