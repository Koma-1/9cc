#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc compile "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42
assert 3 "1+2"
assert 2 "5-3"
assert 15 "3*5"
assert 3 "7/2"
assert 17 "2+3*5"
assert 25 "(2+3)*5"
assert 2 "-3+5"
assert 8 "+3+5"
assert 1 "2==2"
assert 0 "1==2"
assert 0 "2!=2"
assert 1 "1!=2"
assert 1 "1<2"
assert 0 "2<2"
assert 1 "2<=2"
assert 0 "3<=2"
assert 1 "2>1"
assert 0 "2>2"
assert 1 "2>=1"
assert 1 "2>=2"
assert 0 "2>=3"

echo OK