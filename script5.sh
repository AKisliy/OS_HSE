#!/bin/bash
function sum {
echo $(( $1 + $2 ))
}

function multiplication {
echo $(( $1 * $2 ))
}

function sumAndMultiply {
local temp=$( sum $1 $2)
echo $(multiplication $temp $3)
}

echo "Input 3 numbers"
read -p "a = " a
read -p "b = " b
read -p "c = " c
result=$( sumAndMultiply $a $b $c)
echo "(a+b)*c = $result"
