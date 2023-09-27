コンパイラが動作するマシンのことを「ホスト」、コンパイラが出力したコードが動作するマシンのことを「ターゲット」という。またホストとターゲットが異なるコンパイラのことをクロスコンパイラと呼ぶ。
現在実行中の命令のアドレスのことを「プログラムカウンタ」（PC）や「インストラクションポインタ」（IP）と呼ぶ。
関数からリターンしたときにRAXに入っている値が関数の返り値
オンラインコンパイラ　https://godbolt.org/z/RyNqgE
「再帰下降構文解析法」（recursive descent parsing）
bash -x test.sh　でデバッグできる
パーサが読み込むトークン列は、グローバル変数tokenで表現ぢた。パーサは、連結リストになっているtokenをたどっていくことで入力を読み進めていく。
tokenize関数では連結リストを構築している。連結リストを構築するときは、ダミーのhead要素を作ってそこに新しい要素を繋げていって、最後にhead->nextを返している
助長な要素のない構文木を抽象構文木と呼ぶ
これに対して文法に完全に一対一なもんを具象構文木よ呼ぶ
BNFでは、一つ一つの生成規則をA = α₁α₂⋯という形式で表す。
EBNFではA*,A?,A|B,(...)が追加される
加減算
expr = num ("+" num | "-" num)*
乗除算
expr = mul ("+" mul | "-" mul)*
mul  = num ("*" num | "/" num)*
かっこ
expr    = mul ("+" mul | "-" mul)*
mul     = primary ("*" primary | "/" primary)*
primary = num | "(" expr ")"
再帰下降構文解析法は規則からその規則から生成される文にマッチする構文木を求めるコードを機械的に書いていくことができる
トークンを1つだけ先読みする再帰下降パーサのことをLL(1)パーサと呼ぶ
スタックマシンは、スタックをデータ保存領域として持っているコンピュータのこと
x86-64は、「CISC」と呼ばれるスタイルのプロセッサ。CISCプロセッサの特徴は、機械語の演算がレジスタだけではなくメモリアドレスを取ることが可能であるということ、機械語命令の長さが可変長であること
RISCプロセッサの特徴は、演算は必ずレジスタ間でのみ行い、メモリに対する操作はレジスタへのロードとレジスタからのストアだけであること、機械語命令の長さがどの命令でも同じことであること(ARM、PowerPC、SPARC、MIPS、RISC-V)
x86-64はスタックマシンではなくレジスタマシンなので修正が必要
x86-64のpushやpopといった命令は、暗黙のうちにRSPをスタックポインタとして使って、その値を変更しつつ、RSPが指しているメモリにアクセスする命令
add rax,rdi
idivは暗黙のうちにRDXとRAXを取って、それを合わせたものを128ビット整数とみなして、それを引数のレジスタの64ビットの値で割り、商をRAXに、余りをRDXにセットする
cqo命令を使うと、RAXに入っている64ビットの値を128ビットに伸ばしてRDXとRAXにセットすることができる
expr    = mul ("+" mul | "-" mul)*
mul     = unary ("*" unary | "/" unary)*
unary   = ("+" | "-")? primary
primary = num | "(" expr ")"

expr       = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | "(" expr ")"
cmpの結果はフラグレジスタへ。
sete al
movzb rax, al
seteでRAXの下位8ビットalにcmoの結果保存。movzbで上位56ビットをゼロクリアしてalをraxに保存
リロケーション（Relocation）は、コンパイルされたオブジェクトファイルやライブラリから最終的な実行可能ファイルや共有ライブラリを生成する過程で、プログラム中のシンボルのアドレスを正確に割り当てる作業
make fooでfooというファイルを作ろうとする。.PHONYでダミーファイルを指定し、必ず実行させられる
SRCS=$(wildcard *.c)で.cのファイルをとってくる
OBJS=$(SRCS:.c=.o)で.cを.oに置換
$(OBJS): 9cc.hは全ての.oが9cc.hに依存しているということ
-static オプションは、Cプログラムをコンパイルする際にリンクされるライブラリをプログラムに静的に埋め込むためのフラグ
変数はメモリアドレスに名前をつけてあげたものにすぎない
関数呼び出しごとに確保されるメモリ領域のことを「関数フレーム」や「アクティベーションレコード」という
常に変化しうるRSPとは別に、現在の関数フレームの開始位置を常に指しているレジスタを「ベースレジスタ」、そこに入っている値のことを「ベースポインタ」と呼ぶ。x86-64ではRBPレジスタ
コンパイラが関数の先頭に出力する定型の命令のことを「プロローグ」、逆を「エピローグ」と呼ぶ
program    = stmt*
stmt       = expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
メモリから値をロードするときは、mov dst, [src]という構文を使う
program = stmt*
stmt    = expr ";"
        | "return" expr ";"
スタックに積まれたメモリアドレスを管理
printf("  pop rax\n");
printf("  mov rax, [rax]\n");
printf("  push rax\n");
if,while,for
program = stmt*
stmt    = expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
        | ...
fstack-protectorというオプションを渡すと、プロローグで「カナリー」（canary）といわれるポインタサイズのランダムな整数を関数フレームに出力して、エピローグでバッファーオーバーフローなどによってカナリーの値が変わっていないことを確認する
機械語レベルの仕様はある程度決まっており、その仕様をABI（Application Binary Interface）という。
「ブロック」は{ ... }の間に複数のステートメントを書くことをサポートするもの。「複文」（compound statement）とも呼ばれる
program = stmt*
stmt    = expr ";"
        | "{" stmt* "}"
        | ...
...
引数なしのfunction
primary = num
        | ident ("(" ")")?
        | "(" expr ")"