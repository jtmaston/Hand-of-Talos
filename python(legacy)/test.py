"""NME IF
$ a 500000
$ b 0
ADD b b #5
PRT b
IF b LE #25 GOTO 4
PRT b
$ b 0
ADD b b #1
PRT b
IF b LE a GOTO 9
END
"""

a = 500000
b = 0

while b < 25:
    b += 5
    print(b)
b = 0

while b < a:
    b += 1
    print(b)