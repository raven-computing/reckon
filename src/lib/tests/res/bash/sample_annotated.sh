#!/bin/bash
# Golden sample: Contains Bash language features and constructs

# =================================================================================================
# Shell options
# =================================================================================================
set -Eeuo pipefail; # +1 (command)
shopt -s extglob nullglob globstar lastpipe; # +1 (command)
set -o noclobber; # +1 (command)
set +o noclobber; # +1 (command)

# =================================================================================================
# Aliases
# =================================================================================================
alias ll='ls -alF'; # +1 (command)
unalias ll 2>/dev/null || true; # +1 (command)

# =================================================================================================
# Variable declarations
# =================================================================================================
plain="value"; # +1 (variable assignment)
readonly RO="immutable"; # +1 (declaration command)
declare -ir INT_RO=7; # +1 (declaration command)
declare -x EXPORTED="env"; # +1 (declaration command)
declare -l LOWER="TOLOWER"; # +1 (declaration command)
declare -u UPPER="toupper"; # +1 (declaration command)
declare -i num=10; # +1 (declaration command)
declare -a indexed=(a b "c d"); # +1 (declaration command)
declare -A assoc=([k1]=v1 ["k 2"]="v 2"); # +1 (declaration command)
declare -n nameref=indexed; # +1 (declaration command)

# =================================================================================================
# Parameter expansion
# =================================================================================================
: "${MISSING:=default_assign}"; # +1 (command)
: "${EXPORTED:-fallback}"; # +1 (command)
: "${EXPORTED:+alternate}"; # +1 (command)
: "${EXPORTED:?must be set}"; # +1 (command)
: "${#EXPORTED}"; # +1 (command)
: "${EXPORTED:1:3}"; # +1 (command)
: "${EXPORTED/por/XXX}"; # +1 (command)
: "${EXPORTED//o/O}"; # +1 (command)
: "${EXPORTED/#e/E}"; # +1 (command)
: "${EXPORTED/%d/D}"; # +1 (command)
: "${EXPORTED%%or*}"; # +1 (command)
: "${EXPORTED%d}"; # +1 (command)
: "${EXPORTED##*or}"; # +1 (command)
: "${EXPORTED#e}"; # +1 (command)
: "${EXPORTED^}"; # +1 (command)
: "${EXPORTED^^}"; # +1 (command)
: "${EXPORTED,}"; # +1 (command)
: "${EXPORTED,,}"; # +1 (command)
: "${EXPORTED@Q}"; # +1 (command)
: "${EXPORTED@a}"; # +1 (command)
: "${!assoc[@]}"; # +1 (command)
: "${!assoc[*]}"; # +1 (command)
: "${#indexed[@]}"; # +1 (command)
: "${indexed[*]}"; # +1 (command)
: "${indexed[@]:0:2}"; # +1 (command)

# =================================================================================================
# Special parameters
# =================================================================================================
set -- "arg1" "arg 2" "arg3"; # +1 (command)
: "$0" "$#" "$*" "$@" "$?" "$-" "$$" "$!" "${!#}"; # +1 (command)
: "${PIPESTATUS[*]:-}"; # +1 (command)
: "${BASH_VERSION}" "${BASH_SOURCE[0]}" "${FUNCNAME[0]:-main}" "${LINENO}"; # +1 (command)
: "$SECONDS" "$RANDOM" "$EPOCHSECONDS" "$BASHPID"; # +1 (command)

# =================================================================================================
# Arithmetic
# =================================================================================================
arith=$((INT_RO + num * 2 - 1)); # +1 (variable assignment)
(( num += 5 )); # +1 (compound statement)
let "num -= 2"; # +1 (command)
(( num++, num-- )); # +1 (compound statement)
(( num > 0 ? 1 : 0 )); # +1 (compound statement)
(( num <<= 1 )); # +1 (compound statement)
(( num >>= 1 )); # +1 (compound statement)
(( num &= 0xFF )); # +1 (compound statement)
(( num |= 1 )); # +1 (compound statement)
(( num ^= 3 )); # +1 (compound statement)
(( num %= 7 )); # +1 (compound statement)
(( num /= 2 )) || true; # +1 (compound statement)

# =================================================================================================
# Arrays
# =================================================================================================
indexed+=(e f); # +1 (variable assignment)
indexed[5]="sparse"; # +1 (variable assignment)
unset 'indexed[5]'; # +1 (unset command)
assoc+=([k3]=v3); # +1 (variable assignment)
unset 'assoc[k3]'; # +1 (unset command)
copy=("${indexed[@]}"); # +1 (variable assignment)

# =================================================================================================
# Functions
# =================================================================================================
function fn_keyword() { # +1 (function definition)
  local x="${1:-default}"; # +1 (declaration command)
  local -i li=0; # +1 (declaration command)
  local -a la=(); # +1 (declaration command)
  local -A lA=(); # +1 (declaration command)
  local -n ln=$2 2>/dev/null || true; # +1 (declaration command)
  [[ -n "$x" ]] && return 0 || return 1; # +1 (command)
}

fn_posix() { # +1 (function definition)
  printf '%s\n' "${@}" > /dev/null; # +1 (command)
}

fn_recursive() { # +1 (function definition)
  (( $1 <= 1 )) && { echo 1; return; } # +2 (compound statement, compound statement)
  local prev; # +1 (declaration command)
  prev=$(fn_recursive $(($1 - 1))); # +1 (variable assignment)
  echo $(($1 * prev)); # +1 (command)
}

# =================================================================================================
# Trap
# =================================================================================================
trap 'echo cleanup' EXIT; # +1 (command)
trap '' PIPE; # +1 (command)
trap - INT TERM; # +1 (command)
trap 'echo caught' SIGUSR1; # +1 (command)

# =================================================================================================
# Conditionals: if / elif / else
# =================================================================================================
if [[ -n "$plain" && "$plain" =~ ^[[:alpha:]]+$ ]]; then # +1 (if statement)
  : # +1 (command)
elif [[ -z "$plain" || "$plain" == "other" ]]; then # +1 (elif clause)
  : # +1 (command)
elif [ -f /dev/null ]; then # +1 (elif clause)
  : # +1 (command)
elif [ -d /tmp ] && [ -r /tmp ]; then # +1 (elif clause)
  : # +1 (command)
elif test -e /dev/null; then # +2 (elif clause, command)
  : # +1 (command)
else # +1 (else clause)
  : # +1 (command)
fi

# =================================================================================================
# Conditionals: [[ ]] operators
# =================================================================================================
[[ "abc" == a* ]] && :; # +1 (command)
[[ "abc" != z* ]] && :; # +1 (command)
[[ "abc" < "def" ]] && :; # +1 (command)
[[ "abc" > "aaa" ]] && :; # +1 (command)
[[ "abc" =~ ^[a-z]+$ ]] && :; # +1 (command)
[[ -v plain ]] && :; # +1 (command)
[[ -z "" ]] && :; # +1 (command)
[[ -n "x" ]] && :; # +1 (command)
[[ -f /dev/null ]] && :; # +1 (command)
[[ -d /tmp ]] && :; # +1 (command)
[[ -r /dev/null ]] && :; # +1 (command)
[[ -w /dev/null ]] && :; # +1 (command)
[[ -x /bin/sh ]] && :; # +1 (command)
[[ -s /dev/null ]] || :; # +1 (command)
[[ -L /dev/stdin ]] || :; # +1 (command)
[[ -p /dev/stdin ]] || :; # +1 (command)
[[ -S /dev/null ]] || :; # +1 (command)
[[ -b /dev/null ]] || :; # +1 (command)
[[ -c /dev/null ]] && :; # +1 (command)
[[ -t 0 ]] || :; # +1 (command)
[[ -e /dev/null ]] && :; # +1 (command)
[[ /dev/null -nt /dev/null ]] || :; # +1 (command)
[[ /dev/null -ot /dev/null ]] || :; # +1 (command)
[[ /dev/null -ef /dev/null ]] && :; # +1 (command)

# =================================================================================================
# Conditionals: arithmetic (( ))
# =================================================================================================
(( num > 0 )) && : || :; # +1 (compound statement)
(( num == 10 || num == 5 )) || :; # +1 (compound statement)

# =================================================================================================
# Case statement (all terminators)
# =================================================================================================
case "${1:-x}" in # +1 (case statement)
  a*)   : ;& # +2 (case item, command)
  b*)   : ;;& # +2 (case item, command)
  c|d)  : ;; # +2 (case item, command)
  *)    : ;; # +2 (case item, command)
esac

# =================================================================================================
# Loops: for
# =================================================================================================
for v in "${indexed[@]}"; do # +1 (for statement)
  : "$v"; # +1 (command)
done

# =================================================================================================
# Loops: C-style for
# =================================================================================================
for ((i = 0; i < 3; i++)); do # +1 (c style for statement)
  ((i == 1)) && continue; # +1 (compound statement)
  : # +1 (command)
done

# =================================================================================================
# Loops: while
# =================================================================================================
n=3; # +1 (variable assignment)
while ((n > 0)); do # +1 (while statement)
  ((n--)); # +1 (compound statement)
  ((n == 1)) && break; # +1 (compound statement)
done

# =================================================================================================
# Loops: until
# =================================================================================================
until false; do # +1 (while statement)
  break; # +1 (command)
done

# =================================================================================================
# Loops: infinite
# =================================================================================================
while :; do # +1 (while statement)
  break; # +1 (command)
done

# =================================================================================================
# Select
# =================================================================================================
PS3="pick> "; # +1 (variable assignment)
select choice in alpha beta quit; do # +1 (for statement)
  : "$choice" "$REPLY"; # +1 (command)
  break; # +1 (command)
done < <(echo 1) # +1 (command)

# =================================================================================================
# Grouping: brace group
# =================================================================================================
{ printf 'grouped\n'; :; } >/dev/null # +1 (compound statement)

# =================================================================================================
# Grouping: subshell
# =================================================================================================
( cd / && pwd >/dev/null ) # +1 (subshell)

# =================================================================================================
# Pipelines
# =================================================================================================
echo "abc" | tr a-z A-Z | cat >/dev/null; # +1 (pipeline)
echo "abc" |& cat >/dev/null; # +1 (pipeline)
! false; # +1 (command)

# =================================================================================================
# Lists: && || ; &
# =================================================================================================
true && true || false; # +1 (command)
true; false || true; # +1 (command)
sleep 0 & # +1 (command)
wait $!; # +1 (command)

# =================================================================================================
# Time
# =================================================================================================
time { sleep 0; } 2>/dev/null; # +1 (command)

# =================================================================================================
# Getopts
# =================================================================================================
parse_getopts() { # +1 (function definition)
  local opt OPTIND=1; # +1 (declaration command)
  while getopts ":vn:h" opt; do # +1 (while statement)
    case "$opt" in # +1 (case statement)
      v) : ;; # +2 (case item, command)
      n) : "$OPTARG" ;; # +2 (case item, command)
      h) : ;; # +2 (case item, command)
      :) printf 'missing arg -%s\n' "$OPTARG" >&2 ;; # +2 (case item, command)
      \?) printf 'bad opt -%s\n' "$OPTARG" >&2 ;; # +2 (case item, command)
    esac
  done
  shift $((OPTIND - 1)); # +1 (command)
}
parse_getopts -v -n foo; # +1 (command)

# =================================================================================================
# Read / mapfile
# =================================================================================================
mapfile -t lines < <(printf 'l1\nl2\n'); # +1 (command)
readarray -t lines2 < <(printf 'a\nb\n'); # +1 (command)
while IFS= read -r line; do # +1 (while statement)
  : "$line"; # +1 (command)
done < <(printf 'x\ny\n') # +1 (command)
IFS=, read -r -a csv <<< "a,b,c"; # +1 (command)
read -r single <<< "word"; # +1 (command)
read -r -d '' block <<< "multi" || true; # +1 (command)
read -r -n 1 -t 0.01 char </dev/null || true; # +1 (command)

# =================================================================================================
# Here-document: literal (no expansion)
# =================================================================================================
cat <<'HEREDOC_LITERAL' > /dev/null # +1 (command)
literal $text `no expansion` ${none}
HEREDOC_LITERAL

# =================================================================================================
# Here-document: expanding
# =================================================================================================
cat <<HEREDOC_EXPAND >/dev/null # +1 (command)
expanded ${EXPORTED} and $plain
HEREDOC_EXPAND

# =================================================================================================
# Here-document: indented (tab-stripped)
# =================================================================================================
cat <<-HEREDOC_INDENT >/dev/null # +1 (command)
	indented heredoc
HEREDOC_INDENT

# =================================================================================================
# Here-string
# =================================================================================================
cat <<< "here-string" >/dev/null; # +1 (command)

# =================================================================================================
# Command substitution
# =================================================================================================
cs1="$(echo cmd_sub)"; # +1 (variable assignment)
cs2="$(echo nested "$(echo deep)")"; # +1 (variable assignment)
cs3="`echo backtick`"; # +1 (variable assignment)

# =================================================================================================
# Process substitution
# =================================================================================================
diff <(echo a) <(echo b) >/dev/null || true; # +1 (command)
echo "proc_sub_out" > >(cat >/dev/null); # +1 (command)

# =================================================================================================
# File descriptors / exec / redirection
# =================================================================================================
t="$(mktemp)"; # +1 (variable assignment)

echo "line" > "$t"; # +1 (command)
echo "append" >> "$t"; # +1 (command)
cat < "$t" >/dev/null; # +1 (command)
cat < "$t" 2>/dev/null 1>/dev/null; # +1 (command)
echo "stderr" 2>&1 >/dev/null; # +1 (command)
echo "all" &>/dev/null; # +1 (command)
echo "all2" >&/dev/null 2>/dev/null || true; # +1 (command)

exec 3> "$t"; # +1 (command)
printf 'fd3\n' >&3; # +1 (command)
exec 3>&-; # +1 (command)

exec 4< "$t"; # +1 (command)
read -r _ <&4 || true; # +1 (command)
exec 4<&-; # +1 (command)

exec {fd_w}> "$t"; # +1 (command)
printf 'dyn_fd\n' >&"$fd_w"; # +1 (command)
exec {fd_w}>&-; # +1 (command)

exec {fd_r}< "$t"; # +1 (command)
read -r _ <&"$fd_r" || true; # +1 (command)
exec {fd_r}<&-; # +1 (command)

rm -f "$t"; # +1 (command)

# =================================================================================================
# Brace expansion
# =================================================================================================
: {1..5}; # +1 (command)
: {1..10..2}; # +1 (command)
: {a..z}; # +1 (command)
: {A,B,C}; # +1 (command)
: pre{x,y}post; # +1 (command)

# =================================================================================================
# Tilde expansion
# =================================================================================================
: ~ ~+ ~-; # +1 (command)

# =================================================================================================
# Globbing / pathname expansion
# =================================================================================================
: /dev/nul?; # +1 (command)
: /dev/n*; # +1 (command)
: /dev/null; # +1 (command)
: /tmp/[a-z]* 2>/dev/null || true; # +1 (command)

# =================================================================================================
# Extglob patterns
# =================================================================================================
[[ "foobar" == @(foo|baz)* ]] && :; # +1 (command)
[[ "foobar" == ?(foo)bar ]] && :; # +1 (command)
[[ "foobar" == *(foo)bar ]] || :; # +1 (command)
[[ "foobar" == +(foo)bar ]] || :; # +1 (command)
[[ "foobar" != !(foo)* ]] && :; # +1 (command)

# =================================================================================================
# Quoting
# =================================================================================================
: 'single quoted'; # +1 (command)
: "double quoted with $plain"; # +1 (command)
: $'ansi\tescaped\n'; # +1 (command)
: $"locale string"; # +1 (command)
: "escaped \$ and \" and \\"; # +1 (command)
: "$(echo "nested quotes")"; # +1 (command)

# =================================================================================================
# Eval
# =================================================================================================
eval 'dyn_var=42'; # +1 (command)
: "${dyn_var:?must exist}"; # +1 (command)

# =================================================================================================
# Command / builtin / type / enable / hash
# =================================================================================================
command printf '%s\n' "x" >/dev/null; # +1 (command)
command -v printf >/dev/null; # +1 (command)
builtin echo "x" >/dev/null; # +1 (command)
type -a printf >/dev/null 2>&1 || true; # +1 (command)
type -t printf >/dev/null 2>&1 || true; # +1 (command)
enable -n test 2>/dev/null || true; # +1 (command)
enable test 2>/dev/null || true; # +1 (command)
hash -r 2>/dev/null || true; # +1 (command)

# =================================================================================================
# Source / dot
# =================================================================================================
source /dev/null; # +1 (command)
. /dev/null; # +1 (command)

# =================================================================================================
# Coproc
# =================================================================================================
coproc CP { tr '[:lower:]' '[:upper:]'; } # +1 (command)
printf 'hi\n' >&"${CP[1]}"; # +1 (command)
read -r cp_out <&"${CP[0]}"; # +1 (command)
wait "$CP_PID" 2>/dev/null || true; # +1 (command)

# =================================================================================================
# Background / job control
# =================================================================================================
sleep 0 & # +1 (command)
bg_pid=$!; # +1 (variable assignment)
wait "$bg_pid"; # +1 (command)
jobs >/dev/null 2>&1 || true; # +1 (command)
disown 2>/dev/null || true; # +1 (command)

# =================================================================================================
# String / array operations
# =================================================================================================
printf -v formatted '%05d' 42; # +1 (command)
printf '%s\n' "${indexed[@]}" >/dev/null; # +1 (command)
printf '%q\n' "special chars: &|;" >/dev/null; # +1 (command)

# =================================================================================================
# Conditional command: && || chains with pipeline
# =================================================================================================
echo ok | grep -q ok && echo matched >/dev/null || echo no >/dev/null; # +1 (pipeline)

# =================================================================================================
# Arithmetic for loop with comma operator
# =================================================================================================
for ((x = 0, y = 10; x < 3; x++, y--)); do # +1 (c style for statement)
  : "$x" "$y"; # +1 (command)
done

# =================================================================================================
# Nested structures
# =================================================================================================
for a in 1 2; do # +1 (for statement)
  for b in x y; do # +1 (for statement)
    if [[ "$a" == 1 ]]; then # +1 (if statement)
      while true; do # +1 (while statement)
        case "$b" in # +1 (case statement)
          x) break ;; # +2 (case item, command)
          *) break ;; # +2 (case item, command)
        esac
      done
    fi
  done
done

# =================================================================================================
# Unset / readonly / export / declare post-hoc
# =================================================================================================
unset dyn_var; # +1 (unset command)
unset -v plain; # +1 (unset command)
unset -f fn_posix 2>/dev/null || true; # +1 (unset command)
readonly FINAL_RO="sealed"; # +1 (declaration command)
export FINAL_EX="exported"; # +1 (declaration command)
declare -g GLOBAL_VAR="global"; # +1 (declaration command)

# =================================================================================================
# Misc builtins
# =================================================================================================
: "noop"; # +1 (command)
true; # +1 (command)
false || true; # +1 (command)
echo "test" >/dev/null; # +1 (command)
printf '%s\n' "test" >/dev/null; # +1 (command)
pwd >/dev/null; # +1 (command)
cd /tmp && cd - >/dev/null; # +1 (command)
pushd /tmp >/dev/null 2>&1; # +1 (command)
popd >/dev/null 2>&1; # +1 (command)
dirs >/dev/null 2>&1; # +1 (command)
umask 0022; # +1 (command)
ulimit -n >/dev/null 2>&1 || true; # +1 (command)
caller 0 2>/dev/null || true; # +1 (command)
getopts "a" opt "" 2>/dev/null || true; # +1 (command)
shift 0; # +1 (command)
wait; # +1 (command)

# =================================================================================================
# String-valued exit
# =================================================================================================
exit 0; # +1 (command)
