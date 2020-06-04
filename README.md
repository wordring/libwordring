wordring_cpp
====

C++標準ライブラリとBoostに収録されていないが良く使うものをライブラリ化している。

# 機能

## HTML
| 名前 | 説明 |
|----|----|
| simple_html | WHATWGのHTML5規格を実装するパーサと単純なノードの実装。DOMの実装は提供しないが、代わりにあなた独自のどのような実装も許容するよう設計してある。 |

## コンテナ
| 名前 | 説明 |
|----|----|
| basic_trie | 任意の整数型をラベルとして用いることが出来る汎用Trie |
| static_vector | 最大長固定の静的vector |
| tree | 木コンテナ |

## イテレータ
| 名前 | 説明 |
|----|----|
| basic_tree_iterator | プレ・オーダーあるいはレベル・オーダーで木を走査するイテレータ・アダプター |
| deserialize_iterator | バイト列から任意型の整数列へ変換するイテレータ・アダプター |
| serialize_iterator | 任意型の整数列からバイト列へ変換するイテレータ・アダプター |

## 文字列
| 名前 | 説明 |
|----|----|
| basic_atom_set | 文字列と整数値を関連付ける文字列アトム |


# 説明

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