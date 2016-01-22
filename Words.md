#Currently supported forth words.

# Word Set #

## Memory Locations ##
  * BOOT
  * DPL
  * SP0
  * RP0
  * '?KEY
  * 'EMIT
  * 'EXPECT
  * 'TAP
  * 'ECHO
  * 'PROMPT
  * BASE
  * tmp
  * SPAN
  * >IN
  * #TIBB
  * TIBB
  * CSP
  * 'EVAL
  * 'NUMBER
  * HLD
  * HANDLER
  * CONTEXT
  * CURRENT
  * CP
  * NP
  * LAST
  * STATE
  * SPP
  * RPP

## CONSTANTS ##
  * TRUE
  * FALSE
  * BL
  * BS
  * =IMMED
  * =WORDLIST

## BASE ##
  * +
  * IMMEDIATE
  * +!
  * HERE
  * ALLOT
  * ,
  * `[`,`]`

## PRIMITIVE ##
  * NOP
  * HALT
  * LIT
  * EXIT
  * @
  * !
  * DROP
  * OVER
  * SWAP
  * DUP
  * UM+
  * NAND
  * 0<
  * >R
  * R>
  * next
  * BRANCH
  * 0BRANCH
  * EXECUTE
  * PUTC
  * GETC

## COMPILING ##
  * COMPILE
  * STATE?
  * LITERAL
  * `[`
  * `]`

## FLOW CONTROL ##
  * IF
  * THEN
  * FOR
  * NEXT
  * BEGIN
  * AGAIN
  * UNTIL
  * AHEAD
  * REPEAT
  * AFT
  * ELSE
  * WHILE
  * @EXECUTE

## RUNTIME VERSIONS ##
  * @
  * !
  * DROP
  * OVER
  * SWAP
  * DUP

## TIB ##
  * #TIB
  * TIB
  * \

## STACK MANIPULATION ##
  * ROT
  * -ROT
  * NIP
  * TUCK
  * R@
  * 2>R
  * 2R>
  * 2R@
  * 2DROP
  * 2DUP
  * 2SWAP
  * 2OVER
  * 2ROT
  * -2ROT
  * 2NIP
  * 2TUCK

## BITWISE ##
  * NOT
  * AND
  * OR
  * NOR
  * XOR
  * XNOR
  * NEGATE

## MATH+ ##
  * -
  * 1+
  * 1-
  * 2+
  * 2-
  * D+
  * DNEGATE
  * D-
  * CELL+
  * CELL-
  * CELLS
  * ALIGNED
  * 2!
  * 2@
  * TX!
  * EMIT
  * COUNT
  * TYPE
  * ?RX
  * ?KEY
  * KEY
  * ?DUP
  * S>D
  * ABS
  * DABS
  * U<
  * U>
  * =
  * <
  * >
  * 0>
  * 0<>
  * 0=
  * <>
  * D0<
  * D0>
  * D0=
  * D=
  * D<
  * DU<
  * DMIN
  * DMAX
  * M+
  * M-
  * MIN
  * MAX
  * UMIN
  * UMAX
  * WITHIN
  * SP@
  * DEPTH
  * PICK
  * NUF?
  * UM/MOD
  * M/MOD
  * /MOD
  * MOD
  * /
  * UM*** `*`
  * M`*`
  * `*`/MOD
  * `*`/
  * 2`*`
  * 2`*`
  * 2/
  * MU/MOD
  * D2`*`
  * DU2/
  * D2/**

> ## PARSING ##
  * parse
  * PARSE
  * CHAR
  * PACK$
  * FILL
  * -TRAILING
  * WORD
  * TOKEN
  * NAME>
  * LINK>
  * SAME?
  * find
  * '
  * `[`'`]`
  * `[`COMPILE`]`
  * `[`CHAR`]`
  * (

## DICTIONARY ##
  * NONAME
  * OVERT
  * $,n
  * doVAR
  * CREATE
  * VARIABLE
  * :
  * ;
  * RECURSE
  * DOES
  * DOES>
  * CONSTANT
  * 2LITERAL
  * 2CONSTANT
  * 2VARIABLE

## BIT SHIFT ##
  * #BITS
  * LSHIFT
  * RSHIFT

## NUMBER OUTPUT ##
  * SPACE
  * SPACES
  * PAD
  * DECIMAL
  * HEX
  * BINARY
  * OCTAL
  * >CHAR
  * DIGIT
  * <#
  * HOLD
  * #
  * #S
  * SIGN
  * #>
  * S.R
  * D.R
  * U.R
  * .R
  * D.
  * U.
  * .
  * ?
  * DU.R
  * DU.

## STRINGS ##
  * do$
  * ."|
  * $,"
  * ."
  * .(
  * $"|
  * $"
  * s"
  * CR

## INPUT ##
  * ^H
  * TAP
  * KTAP
  * ACCEPT
  * EXPECT
  * QUERY
  * DIGIT?
  * /STRING
  * >NUMBER
  * NUMBER?
  * $INTERPRET
  * $COMPILE
  * eval
  * EVAL
  * OK
  * QUIT
  * .ID
  * WORDS
  * STATS
  * .S



