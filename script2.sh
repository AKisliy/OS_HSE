#!/bin/bash
user=alexeikiselev
if grep $user /etc/passwd
then
echo "User $user exists"
else
echo "User $user doesn't exist"
fi
