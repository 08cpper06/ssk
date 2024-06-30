# Lexical conventions
## Token
<b>*token*</b>:
&emsp;<b>*identifier*</b><br>
&emsp;<b>*keyword*</b><br>
&emsp;<b>*operator*</b><br>

## Identifier
<b>*identifier*</b>:<br>
&emsp;<b>*identifier-start*</b><br>
&emsp;<b>*identifier*</b> <b>*identifier-continue*</b>

<b>*identifier-start*</b>:<br>
&emsp;<b>*nondigit*</b><br>
&emsp;_<br>

<b>*identifier-continue*</b>:<br>
&emsp;<b>*digit*</b><br>
&emsp;<b>*nondigit*</b><br>
&emsp;_<br>

<b>*nondigit*</b>: one of<br>
&emsp;a b c d e f g h i j k l m<br>
&emsp;n o p q r s t u v w x y z<br>
&emsp;A B C D E F G H I J K L M<br>
&emsp;N O P Q R S T U V W X Y Z<br>

<b>*digit*</b>: one of<br>
&emsp;0 1 2 3 4 5 6 7 8 9<br>

## Number
<b>*number*</b>:<br>
&emsp;<b>*digit*</b><br>
&emsp;<b>*number*</b> <b>*digit*</b><br>

## Keyword
<b>*keyword*</b>: one of<br>
&emsp;float<br>
&emsp;int<br>
&emsp;return<br>
&emsp;mut<br>
&emsp;const<br>

## Operators
<b>*operator*</b>: one of<br>
&emsp;+ - * / = : ;<br>

# Expression
## Stmt
<b>*stmt*</b>:<br>
&emsp;return <b>*expression*</b> ;<br>
&emsp;<b>*variable-define*</b><br>

## Variable define
<b>*variable-define*</b>:<br>
&emsp;<b>*modifier*</b> <b>*identifier*</b> : <b>*variable-type*</b>;<br>
&emsp;<b>*modifier*</b> <b>*identifier*</b> : <b>*variable-type*</b> = <b>*expression*</b>;<br>

<b>*modifier*</b>:<br>
&emsp;const<br>
&emsp;mut<br>

<b>*variable-type*</b>:<br>
&emsp;int<br>
&emsp;float<br>

## Expression
<b>*expression*</b>:<br>
&emsp;<b>*expression*</b> <b>*assignment-expression*</b> <b>*expression*</b><br>

<b>*assignment-expression*</b>:<br>
&emsp;<b>*additive-expression*</b><br>
&emsp;<b>*additive-expression*</b> = <b>*additive-expression*</b><br>

<b>*additive-expression*</b>:<br>
&emsp;<b>*multiplicative-expression*</b><br>
&emsp;<b>*additive-expression*</b> + <b>*multiplicative-expression*</b><br>
&emsp;<b>*additive-expression*</b> - <b>*multiplicative-expression*</b><br>

<b>*multiplicative-expression*</b>:<br>
&emsp;<b>*value*</b><br>
&emsp;<b>*multiplicative-expression*</b> * <b>*value*</b><br>
&emsp;<b>*multiplicative-expression*</b> / <b>*value*</b><br>

<b>*literal*</b>:<br>
&emsp;<b>*number*</b><br>