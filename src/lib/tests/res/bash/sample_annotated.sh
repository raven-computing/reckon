#!/bin/bash
# Golden sample: Contains Bash language features and constructs

set -Eeuo pipefail; # +1 (command)
shopt -s extglob nullglob globstar lastpipe; # +1 (command)
set -o noclobber; # +1 (command)

alias ll='ls -alF'; # +1 (command)
unalias ll 2>/dev/null || true; # +2 (command, command)

readonly CONST_STR="const"; # +1 (declaration command)
declare -ir CONST_INT=7; # +1 (declaration command)
declare -x EXPORTED_VAR="exported"; # +1 (declaration command)
declare -i num=10; # +1 (declaration command)
declare -a arr=(zero one "two words"); # +1 (declaration command)
declare -A map=([k1]=v1 ["k 2"]="v 2"); # +1 (declaration command)
declare -n arr_ref=arr; # +1 (declaration command)

tmp_files=(); # +1 (variable assignment)

function cleanup() { # +1 (function definition)
  local rc=$? # +1 (declaration command)
  for f in "${tmp_files[@]:-}"; do # +1 (for statement)
    [[ -e "$f" ]] && echo "$f"; # +1 (command)
  done
  exit "$rc"; # +1 (command)
}
trap cleanup EXIT INT TERM; # +1 (command)

parse_opts() { # +1 (function definition)
  local opt verbose=0 name=""; # +1 (declaration command)
  local OPTIND=1; # +1 (declaration command)
  while getopts ":vn:" opt; do # +2 (while statement, command)
    case "$opt" in # +1 (case statement)
      v) ((verbose++)) ;; # +1 (case item)
      n) name="$OPTARG" ;; # +2 (case item, variable assignment)
      :) printf 'missing arg for -%s\n' "$OPTARG" >&2 ;; # +2 (case item, command)
      \?) printf 'invalid option: -%s\n' "$OPTARG" >&2 ;; # +2 (case item, command)
    esac
  done
  shift $((OPTIND - 1)) # +1 (command)
  : "$verbose" "$name" "$#" # +1 (command)
}

function f_keyword_style() { # +1 (function definition)
  local x="${1:-default}": # +1 (declaration command)
  [[ -n "$x" ]] && return 0 || return 1; # +2 (command, command)
}

f_posix_style() { # +1 (function definition)
  local -a local_arr=("a" "b"); # +1 (declaration command)
  printf '%s\n' "${local_arr[@]}" > /dev/null; # +1 (command)
}

parse_opts "$@"; # +1 (command)

set -- "arg1" "arg 2" "arg3"; # +1 (command)
printf '%s %s %s %s %s %s %s %s %s\n' \ # +1 (command)
  "$0" "$#" "$*" "$@" "$?" "$-" "$$" "${!#}" "${PIPESTATUS[*]:-}" >/dev/null;

: "${MISSING_VAR:=default}"; # +1 (command)
tmp="${EXPORTED_VAR:-fallback}"; # +1 (variable assignment)
len="${#tmp}"; # +1 (variable assignment)
slice="${tmp:1:3}"; # +1 (variable assignment)
repl="${tmp/por/XXX}"; # +1 (variable assignment)
trim_r="${tmp%%ed}"; # +1 (variable assignment)
keys="${!map[@]}"; # +1 (variable assignment)
arith=$((CONST_INT + num * 2)); # +1 (variable assignment)
let "num += 5"; # +1 (command)

if [[ -n "$tmp" && "$tmp" =~ ^[[:alpha:]]+$ ]]; then # +1 (if statement)
  : # +1 (command)
elif [ -f /some/file ]; then # +1 (elif clause)
  : # +1 (command)
else # +1 (else clause)
  : # +1 (command)
fi

(( num > 0 )) && : || : # +2 (command, command)

case "${1:-a1}" in # +1 (case statement)
  a*) : ;& # +2 (case item, command)
  *1) : ;;& # +2 (case item, command)
  *)  : ;; # +2 (case item, command)
esac

for v in "${arr[@]}"; do # +1 (for statement)
  : "$v"; # +1 (command)
done

for ((i = 0; i < 3; i++)); do # +1 (c style for statement)
  ((i == 1)) && continue # +1 (command)
  : # +1 (command)
done

while ((num > 0)); do # +1 (while statement)
  ((num--))
  ((num == 2)) && continue # +1 (command)
  ((num == 1)) && break # +1 (command)
done

until [[ -e /definitely/not/here ]]; do # +1 (while statement)
  break # +1 (command)
done

PS3="pick> " # +1 (variable assignment)
select choice in alpha beta quit; do # +1 (for statement)
  : "$choice" "$REPLY" # +1 (command)
  break # +1 (command)
# Here-string causes tree-sitter to show a syntax error ?
done <<'SELECT_INPUT'
1
SELECT_INPUT

{ printf 'grouped\n'; :; } >/dev/null # +2 (command, command)
( cd / && pwd >/dev/null ) # +2 (command, command)

time { sleep 0.01; } 2> /dev/null || true; # +3 (command, command, command)

mapfile -t lines < <(printf 'l1\nl2\n') # +2 (command, command)
while IFS= read -r line; do # +3 (while statement, command, variable assignment)
  : "$line" # +1 (command)
done < <(printf 'x\ny\n') # +1 (command)

IFS=, read -r -a csv <<< "a,b,c" # +2 (command, variable assignment)
read -r full_line <<< "hello world" # +1 (command)

cat <<'HEREDOC' > /dev/null # +1 (command)
literal $text `no expansion`
HEREDOC

cat <<-HEREDOC >/dev/null # +1 (command)
  indented heredoc
HEREDOC

tmp1="$(mktemp)"; # +2 (variable assignment, command)
tmp2="$(mktemp)"; # +2 (variable assignment, command)
tmp_files+=("$tmp1" "$tmp2"); # +1 (variable assignment)

printf 'A\nB\n' > "$tmp1"; # +1 (command)
printf 'A\nC\n' > "$tmp2"; # +1 (command)
diff <(cat "$tmp1") <(cat "$tmp2") >/dev/null || true; # +4 (command, command, command, command)

exec 3> "$tmp1"; # +1 (command)
printf 'fd write\n' >&3; # +1 (command)
exec 3>&-; # +1 (command)

# <> causes tree-sitter to show a syntax error ?
exec {fdw} > "$tmp2"; # +1 (command)
printf 'X\n' >& "$fdw"; # +1 (command)
exec {fdw} >&-; # +1 (command)

exec {fdr} < "$tmp2"; # +1 (command)
read -r _ <&"$fdr" || true; # +2 (command, command)
exec {fdr} <&-; # +1 (command)

printf '%s\n' "${arr_ref[@]}" | grep -n . | while IFS=: read -r n val; do # +5 (command, command, while statement, command, variable assignment)
  : "$n" "$val"; # +1 (command)
done

true && : || : # +3 (command, command, command)
false || : # +2 (command, command)

if [[ "foobar" == @(foo|bar)* ]]; then # +1 (if statement)
  : # +1 (command)
fi

for b in {1..2}; do # +1 (for statement)
  : "$b" # +1 (command)
done

code='dyn_var=42' # +1 (variable assignment)
eval "$code" # +1 (command)
: "${dyn_var:?must exist}" # +1 (command)

command printf '%s\n' "via command" >/dev/null; # +1 (command)
builtin echo "via builtin" >/dev/null; # +1 (command)
type -a printf >/dev/null 2>&1 || true # +2 (command, command)

source /dev/null; # +1 (command)
. /dev/null; # +1 (command)

coproc CP { tr '[:lower:]' '[:upper:]'; } # +2 (command, command)
printf 'coproc\n' >&"${CP[1]}"; # +1 (command)
read -r cp_out <&"${CP[0]}"; # +1 (command)
wait "$COPROC_PID"; # +1 (command)

sleep 0.01 & # +1 (command)
bg_pid=$! # +1 (variable assignment)
wait "$bg_pid" # +1 (command)

unset MISSING_VAR dyn_var # +1 (unset command)
readonly FINAL_READONLY="done" # +1 (declaration command)
export FINAL_EXPORT="done" # +1 (declaration command)

: "$len" "$slice" "$repl" "$trim_r" "$keys" "$arith" "$cp_out" "${csv[*]}" "${lines[*]}" # +1 (command)
