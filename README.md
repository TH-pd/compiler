# yacc, lexを用いてアセンブラを出力するプログラムを作成する

# 作成した言語の定義
<プログラム> ::= <変数宣言部> <文集合>  
<変数宣言部> ::= <宣言文> <変数宣言部> | <宣言文>  
<宣言文> ::= define <識別子>; | array <識別子> [<数>]; | array <識別子> [<数>][<数>];  
<文集合> ::= <文> <文集合>| <文>  
<文> ::= <代入文> | <ループ文> | <条件分岐文>  
<代入文> ::= <変数> = <算術式> ;  
<算術式> ::= <算術式> <加減演算子> <項> | <項>  
<項> ::= <項> <乗除演算子> <因子> | <因子>  
<因子> ::= <変数> | (<算術式>)  
<加減演算子> ::= + | -  
<乗除演算子> ::= * | / | %  
<変数> ::= <識別子> | <数> | <識別子>[<数>] | <識別子>[<数>][<数>]  
| <識別子>[<識別子>] | <識別子>[<識別子>][<数>] | <識別子>[<数>][<識別子>]  
| <識別子>[<識別子>][<識別子>]
<ループ文> ::= while (<条件式>) { <文集合> }  
<条件分岐文> ::= if (<条件式>) { <文集合> } | if (<条件式>) { <文集合> } else { <文集合> }  
<条件式> ::= <算術式> <比較演算子> <算術式>  
<比較演算子> ::= == | ’<’ | ’>’ |’>=’ |’<=’  
<識別子> ::= <英字> <英数字列> | <英字>  
<英数字列> ::= <英数字> <英数字列>| <英数字>  
<英数字> ::= <英字> | <数字>  
<数> ::= <数字> <数> | <数字>  
<英字> ::= a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|<数字> ::= 0|1|2|3|4|5|6|7|8|9

# 受理されるプログラムの例
1. 1.p 1 から10 までの数の和  
2. 2.p 5 の階乗  
3. 3_1.p FizzBuzz 問題(剰余演算子無し)  
4. 3_2.p FizzBuzz 問題(剰余演算子有り)  
5. 4.p エラトステネスのふるいを用いて1 から1000 までの素数を調べる  
6. 5.p 二つの2x2 行列の積の計算  

# コード生成の概要
まず始めに入力に対してlex 及びyacc を用いて抽象構文木を作成し、抽象構文木の作成が終わったら
codegen() 関数を呼び出してコードを作成する．
## 変数の取り扱い
今回作成する言語では変数はint 型のみであり，define 及びarray で宣言される．したがって，変数表
をグローバル変数で宣言し，連結リストで実装を行う．変数が持つ要素は変数名，オフセット，サイズ及
び2 次元配列の場合は長さを格納する．また，プログラムの最後でデータセグメントに変数を格納するた
めのメモリを確保する．
## レジスタの割り当て
本プログラムでは$t0 レジスタを$ra レジスタの格納場所として使用し，$v0, $v1 レジスタ及び，$t1–$t3 レジスタを計算の際に使用する．

## 算術式のコード生成
算術式のコード生成は再帰呼び出し関数で行っている．算術式のノードを引数とし，子が変数であれ
ばその値をスタックに格納し，算術式であれば自身を再度呼び出す．自身を呼び出す際には現在のスタッ
クポインタの位置とその4 バイト後ろにそれぞれ演算に用いる値を格納するために，現在のスタックポイ
ンタからどれだけ後ろのメモリを使用しているかを示す値を渡す．また，計算の結果は0($sp) に格納さ
れる．

