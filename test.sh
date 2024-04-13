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

assert 0 "0;"
assert 42 "42;"
assert 3 "1+2;"
assert 2 "5-3;"
assert 15 "3*5;"
assert 3 "7/2;"
assert 17 "2+3*5;"
assert 25 "(2+3)*5;"
assert 2 "-3+5;"
assert 8 "+3+5;"
assert 1 "2==2;"
assert 0 "1==2;"
assert 0 "2!=2;"
assert 1 "1!=2;"
assert 1 "1<2;"
assert 0 "2<2;"
assert 1 "2<=2;"
assert 0 "3<=2;"
assert 1 "2>1;"
assert 0 "2>2;"
assert 1 "2>=1;"
assert 1 "2>=2;"
assert 0 "2>=3;"
assert 3 "abc=3;"
assert 2 "abc=3;abc=2;abc;"
assert 7 "abc=1+2*3;"
assert 16 "abc=1+2*3;bcd=(1+2)*3;abc+bcd;"
assert 7 "abc=1+2*3;return abc;"
assert 9 "abc=(1+2)*3;return abc;return 1;"
assert 1 "if(1) 1;else 2;"
assert 2 "if(0) 1;else 2;"
assert 0 "if(1) return 0;else return 1;"
assert 0 "if(1) if(1)return 0;else return 1;else return 2;"
assert 1 "if(1) if(0)return 0;else return 1;"
assert 2 "if(0) return 0; else if (0) return 1; else return 2;"
assert 5 "a=3;while(a<5)a=a+1;return a;"
assert 2 "{a=1;a=2;}return a;"
assert 35 "if(0){a=2;b=3;}else{a=5;b=7;}return a*b;"
assert 14 "a=3;b=3;c=2;while(a<5){a=a+1;b=b+1;c=c+1;}return a+b+c;"

echo OK