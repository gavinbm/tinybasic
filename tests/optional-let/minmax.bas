LET minsofar = 0
LET maxsofar = 0
LET sum = 0

LET num = 0
PRINT "Enter number of inputs: "
INPUT num

i = 0
c = 0
WHILE i < num REPEAT
    INPUT c
    IF i == 0 THEN
        LET minsofar = c
        maxsofar = c
    ENDIF
    IF i != 0 THEN
        IF c < minsofar THEN
            LET minsofar = c
        ENDIF
        IF c > maxsofar THEN
            maxsofar = c
        ENDIF
    ENDIF
    sum = sum + c
    LET i = i + 1
ENDWHILE

PRINT "Min: "
PRINT minsofar
PRINT "Max: "
PRINT maxsofar
PRINT "Sum: "
PRINT sum
PRINT "Avg: "
PRINT sum / num
