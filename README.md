# wordring_cpp

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