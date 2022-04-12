LET a = 1

PRINT "What's your name?"
GET name AS STRING

# The input loop of the shell, will go until we change a
# current commands:
# exit      - closes the shell
# add       - adds two numbers
# sub       - subtracts two numbers
# whoami    - tell the user the name they entered
# whoareyou - tell the user our name

LABEL loop
    WHILE a > 0 REPEAT
        READ 64 FROM "stdin" INTO buffer
        IF buffer == "exit" THEN
            a = 0
        ENDIF
        IF buffer == "add" THEN
            PRINT "Enter two numbers"
            GET x AS INT
            GET y AS INT
            PRINT "Sum:"
            PRINT y + x
        ENDIF
        IF buffer == "sub" THEN
            PRINT "Enter two numbers"
            GET x AS INT
            GET y AS INT
            PRINT "Difference:"
            PRINT x - y
        ENDIF
        IF buffer == "whoareyou" THEN
            PRINT "My name is Tiny :D"
        ENDIF
        IF buffer == "whoami" THEN
            PRINT name
        ENDIF
    ENDWHILE