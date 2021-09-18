OPEN "file.c" AS fout
READ 64 FROM fout INTO file
WRITE "hello" INTO fout
CLOSE fout