wordring_cpp
====

C\++標準ライブラリとBoostに収録されていないが良く使うものをライブラリ化している。
C\++17がベースライン。

文字コードのテーブルやHTMLのタグ・テーブルのためにビルドが必要です。
普段 Visualstudio を使っているため、コマンドラインのビルドシステムには詳しくありませんが、
Linux上で端末からビルドできることとテストが実行できることを常に確認しています。

# 機能

組み合わせて使うために STL 風のデザインとしている。
たとえば TRIE 木はある種の木なので、（木と同様に）木操作用のイテレータ・アダプタを組み合わせて使用する。
HTML5 パーサーは、（このライブラリは単純なノード実装しか提供しないが）どのような DOM 実装とも組み合わせて使える。

実装済みの代表的な機能は以下の通り。

- WHATWG の HTML5 仕様を実装する HTML5 Parser。
- WHATWG 仕様を実装する文字コード変換。
- 木コンテナ、および、木走査用のイテレータ・アダプタ。
- イテレータ・ベースのアクセスを提供する動的TRIE木。
- その他周辺あるいは細かな機能。

# 説明

プログラミング言語として C\++17 、ビルド・システムに CMake を用いる。
テストしている環境は以下の通り。

- Linux gcc7
- Windows10 clang-cl
- Windows10 cl

## 文書
| 名前 | 説明 |
|----|----|
| https://wordring.github.io/wordring_cpp/ | 付属文書 |

## フォルダとファイル
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

## 要求環境
- C++17
- CMake3.12以降

## 要求ライブラリ
- Boost1.67以降
- ICU
- SQLite3

## 要求サブモジュール
| 名前 | 説明 |
|----|----|
| https://github.com/publicsuffix/list | URLパーサでデータを使用 |
| https://github.com/whatwg/encoding | HTML5パーサでデータを使用 |
| https://github.com/dwyl/english-words | TRIE用単体テストでデータを使用 |
| https://github.com/hingston/japanese | TRIE用単体テストでデータを使用 |