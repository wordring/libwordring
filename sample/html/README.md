HTML
====

HTML パーサーは、HTML文書の解析のみに責任を持ち、木やノードに特別な前提を持ちません。
とはいえ、 wordring_cpp にも小さな木実装があります。
クラス **simple_node** とクラス **tag_tree** を組み合わせて、クラス **simple_tree** と呼んでいます。
単純な用途には **simple_tree** をお勧めします。
特殊な用途には、あなたの DOM 実装をパーサーと組み合わせてお使いください。

# Basic usage

ソース文字列から	HTML 木を作るには、 wordring\::html\::**make_document**() を使います。

HTML 木の走査に、一般化された木走査用のイテレータアダプタが使えます。
とはいえクラス tag_tree 専用のイテレータアダプタのほうが使いやすいと思います。

wordring_cpp の HTML 実装は、　HTML5 パーサーと単純な HTML ノードが含まれます。
単純な HTML ノード実装は wordring\::html\::**simple_node**<> です。
HTML ノードを木コンテナに格納して HTML 文書として扱います。

# CSS Selector

HTML ノードの検索には CSS セレクタ「 **query_selector**() 」あるいは「 **query_selector_all**() 」を使います。

# std::regex

HTML 木を直接 **std::regex** で検索するには、「 tag_tree::**character_iterator** 」を使います。

# File

| File             | Description                  |
| ----             | ----                         |
| css_selector.cpp | HTML 文書の query_selector() |
| regex.cpp        | HTML 文書の正規表現検索       |
| simple_html.cpp  | HTML 文書の作成と操作         |
