HTML
====

wordring_cpp の HTML 実装は、　HTML5 パーサーと単純な HTML ノードが含まれます。
単純な HTML ノード実装は wordring\::html\::simple_node<> です。
HTML ノードを木コンテナに格納して HTML ドキュメントとして扱います。

現在、wrdring_cpp に木コンテナは tree しか在りません。
tree は HTML の編集に最適ではありません。
tree は終了タグを表現できないため、不完全な HTML をシンプルに保持出来ません。
HTML 編集用途のための木コンテナとして、 tag_tree を計画しています。

HTML5 パーサは DOM 実装についての前提を持ちません。
あなた独自のあるいは既存の DOM 実装に対して解析結果を格納することが出来ます。
そのために、 wordring\::html\::node_traits<> を特殊化してください。
node_traits<> は十分小さなクラスで、特殊化を書くのが容易です。
simple_node 用の実装である simple_node_traits を参考にしてください。
わずか 300 行程度です。

HTML ノードの検索用に、 CSS セレクタを含みます。
wordring\::css\::query_selector() を見てください。
simple_html.cpp 内にサンプルが在ります。

CSS セレクタは規格そのままに実装したもので、 HTML 以外にも適用できます。
セレクタ実装は、ノードそのものではなく node_traits<> に対して操作を行います。
