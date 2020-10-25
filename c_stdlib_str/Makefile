include ../c.mk

# ----------- Install --------------

install: $(patsubst %,~/bin/%,wchar uchar)

~/bin/%: bin-c/%
	install -DT $^ $@
