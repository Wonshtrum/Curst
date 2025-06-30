SRC=main.c curst.h derive.h fmt.h do.h
all=a.out

a.out: $(SRC)
	gcc -ftrack-macro-expansion=0 main.c -o $@

.PHONY: debug
debug: $(SRC)
	gcc -E -P main.c | python3 -c "import sys; print(sys.stdin.read().encode('unicode_escape').replace(b'\\U',b'U').decode('unicode_escape'))" > out.c
	clang-format out.c > out2.c
	gcc out2.c

.PHONY: clean
clean:
	rm -f a.out out.c out2.c
