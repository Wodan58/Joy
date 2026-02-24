" Vim syntax file
" Language:	Joy
" Maintainer:	Ivan Tomac
" Last Change:	January 31, 2005

if exists("b:current_syntax")
  finish
endif

syntax case match

syntax match  joyError   oneline "]"
syntax region joyString  start=/"/ end=/"/
syntax region joyList    matchgroup=joyBracket start=/\[/ end=/\]/ contains=ALL
syntax match  joyComment /[#].*/
syntax region joyComment start=/(\*/ end=/\*)/
syntax match  joySymbol  /[^"#'()\.\;\[\]\{\} ]+/
syntax match  joySpecial /[.;]/
syntax match  joySpecial /==/
syntax match  joySpecial /LIBRA[ \t\n]/
syntax match  joySpecial /DEFINE[ \t\n]/
syntax match  joySpecial /HIDE[ \t\n]/
syntax match  joySpecial /IN[ \t\n]/
syntax match  joySpecial /END[ \t\n]/
syntax match  joySpecial /MODULE[ \t\n]/
syntax match  joySpecial /PRIVATE[ \t\n]/
syntax match  joySpecial /PUBLIC[ \t\n]/
syntax match  joySpecial /CONST[ \t\n]/
syntax match  joySpecial /INLINE[ \t\n]/
syntax match  joySpecial /%[^ ]*/

if !exists("did_joy_syntax_inits")

  command -nargs=+ HiLink hi def link <args>

  HiLink joyString  String
  HiLink joySymbol  Identifier
  HiLink joyList    Identifier
  HiLink joySpecial Keyword
  HiLink joyBracket Function
  HiLink joyComment Comment
  HiLink joyError   Error

  delcommand HiLink

endif

let b:current_syntax = "joy"
