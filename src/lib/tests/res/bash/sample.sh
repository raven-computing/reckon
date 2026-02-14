#!/bin/bash
# Golden sample: Contains Bash language features and constructs

set -Eeuo pipefail;
shopt -s extglob nullglob globstar lastpipe;
set -o noclobber;

alias ll='ls -alF';
unalias ll 2>/dev/null || true;

readonly CONST_STR="const";
declare -ir CONST_INT=7;
declare -x EXPORTED_VAR="exported";
declare -i num=10;
declare -a arr=(zero one "two words");
declare -A map=([k1]=v1 ["k 2"]="v 2");
declare -n arr_ref=arr;

tmp_files=();

function cleanup() {
  local rc=$?
  for f in "${tmp_files[@]:-}"; do
    [[ -e "$f" ]] && echo "$f";
  done
  exit "$rc";
}
trap cleanup EXIT INT TERM;

parse_opts() {
  local opt verbose=0 name="";
  local OPTIND=1;
  while getopts ":vn:" opt; do
    case "$opt" in
      v) ((verbose++)) ;;
      n) name="$OPTARG" ;;
      :) printf 'missing arg for -%s\n' "$OPTARG" >&2 ;;
      \?) printf 'invalid option: -%s\n' "$OPTARG" >&2 ;;
    esac
  done
  shift $((OPTIND - 1))
  : "$verbose" "$name" "$#"
}

function f_keyword_style() {
  local x="${1:-default}":
  [[ -n "$x" ]] && return 0 || return 1;
}

f_posix_style() {
  local -a local_arr=("a" "b");
  printf '%s\n' "${local_arr[@]}" > /dev/null;
}

parse_opts "$@";

set -- "arg1" "arg 2" "arg3";
printf '%s %s %s %s %s %s %s %s %s\n' \
  "$0" "$#" "$*" "$@" "$?" "$-" "$$" "${!#}" "${PIPESTATUS[*]:-}" >/dev/null;

: "${MISSING_VAR:=default}";
tmp="${EXPORTED_VAR:-fallback}";
len="${#tmp}";
slice="${tmp:1:3}";
repl="${tmp/por/XXX}";
trim_r="${tmp%%ed}";
keys="${!map[@]}";
arith=$((CONST_INT + num * 2));
let "num += 5";

if [[ -n "$tmp" && "$tmp" =~ ^[[:alpha:]]+$ ]]; then
  :
elif [ -f /some/file ]; then
  :
else
  :
fi

(( num > 0 )) && : || :

case "${1:-a1}" in
  a*) : ;&
  *1) : ;;&
  *)  : ;;
esac

for v in "${arr[@]}"; do
  : "$v";
done

for ((i = 0; i < 3; i++)); do
  ((i == 1)) && continue
  :
done

while ((num > 0)); do
  ((num--))
  ((num == 2)) && continue
  ((num == 1)) && break
done

until [[ -e /definitely/not/here ]]; do
  break
done

PS3="pick> "
select choice in alpha beta quit; do
  : "$choice" "$REPLY"
  break
# Here-string causes tree-sitter to show a syntax error ?
done <<'SELECT_INPUT'
1
SELECT_INPUT

{ printf 'grouped\n'; :; } >/dev/null
( cd / && pwd >/dev/null )

time { sleep 0.01; } 2> /dev/null || true;

mapfile -t lines < <(printf 'l1\nl2\n')
while IFS= read -r line; do
  : "$line"
done < <(printf 'x\ny\n')

IFS=, read -r -a csv <<< "a,b,c"
read -r full_line <<< "hello world"

cat <<'HEREDOC' > /dev/null
literal $text `no expansion`
HEREDOC

cat <<-HEREDOC >/dev/null
  indented heredoc
HEREDOC

tmp1="$(mktemp)";
tmp2="$(mktemp)";
tmp_files+=("$tmp1" "$tmp2");

printf 'A\nB\n' > "$tmp1";
printf 'A\nC\n' > "$tmp2";
diff <(cat "$tmp1") <(cat "$tmp2") >/dev/null || true;

exec 3> "$tmp1";
printf 'fd write\n' >&3;
exec 3>&-;

# <> causes tree-sitter to show a syntax error ?
exec {fdw} > "$tmp2";
printf 'X\n' >& "$fdw";
exec {fdw} >&-;

exec {fdr} < "$tmp2";
read -r _ <&"$fdr" || true;
exec {fdr} <&-;

printf '%s\n' "${arr_ref[@]}" | grep -n . | while IFS=: read -r n val; do
  : "$n" "$val";
done

true && : || :
false || :

if [[ "foobar" == @(foo|bar)* ]]; then
  :
fi

for b in {1..2}; do
  : "$b"
done

code='dyn_var=42'
eval "$code"
: "${dyn_var:?must exist}"

command printf '%s\n' "via command" >/dev/null;
builtin echo "via builtin" >/dev/null;
type -a printf >/dev/null 2>&1 || true

source /dev/null;
. /dev/null;

coproc CP { tr '[:lower:]' '[:upper:]'; }
printf 'coproc\n' >&"${CP[1]}";
read -r cp_out <&"${CP[0]}";
wait "$COPROC_PID";

sleep 0.01 &
bg_pid=$!
wait "$bg_pid"

unset MISSING_VAR dyn_var
readonly FINAL_READONLY="done"
export FINAL_EXPORT="done"

: "$len" "$slice" "$repl" "$trim_r" "$keys" "$arith" "$cp_out" "${csv[*]}" "${lines[*]}"
