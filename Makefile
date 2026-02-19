# Works with GNU Make on Linux (Fedora and similar).

BIN=fype
# Use $(shell ...) for command substitution (GNU make idiom)
SRCS=$(shell find ./src -name '*.c')
OBJS=$(SRCS:.c=.o)
CC?=cc
DEBUG=-g3 -ggdb3
# Enable POSIX extensions (e.g. strdup) before any system headers are seen
CFLAGS+=-c -Wall -std=c99 -pedantic -D_POSIX_C_SOURCE=200809L $(DEBUG)
LDADD+=
HEADER?=docs/header.txt
# Detect the OS name for updating build.h
OSYSTEM=$(shell uname)
PREFIX=/usr/local

all: build $(OBJS) newline stats-tofile
	@$(CC) -lm -o $(BIN) $(OBJS) $(LDADD)
	@if test -z '$(DEBUG)'; then strip $(BIN); fi
	@awk '$$2 == "BUILDNR" { printf("===> Fype build number % 13s :% 6s%d\n", \
		"", "", $$3); exit(0); }' src/build.h
	@echo "===> Fype binary size			:     `du -hs $(BIN)`"
	@#echo "===> `./$(BIN) -v | sed 's/Build .*//'`"
	@./fype -v > ./docs/version.txt
	@./fype -h > ./docs/help.txt
	@./fype -s > ./docs/synopses.txt
	@echo

# Compile each .c to its .o using a GNU make pattern rule; $< is the source file
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	find ./ -name 1 -exec rm -f {} \;
	find ./ -name '*.o' -exec rm -f {} \;
	find ./ -name '*.bin' -exec rm -f {} \;
	find . -name '*.core' -exec rm -f {} \;
	if [ -f $(BIN) ]; then rm -f $(BIN); fi

# Increment BUILDNR and update the OS_ define in build.h before compiling
build: ctags
	@awk '{ \
		if ($$2 == "BUILDNR") print $$1,$$2,$$3+1; \
		else if ($$2 ~ /OS_/) printf("%s OS_%s\n", $$1, \
			toupper("$(OSYSTEM)")); \
		else print }' \
			src/build.h  >.tmp && mv -f .tmp src/build.h

printbuild:
	@awk '$$2 == "BUILDNR" { printf("%d\n", \
		$$3); exit(0); }' src/build.h

# Generate ctags for editor navigation
ctags:
	ctags `find . -name '*.c'`

style: astyle check
astyle:
	find ./src -name '*.[ch]' -exec sh -c 'astyle -s3 {}; rm -f {}.orig' \;

# Check for source lines exceeding 80 characters
check:
	for f in `find ./src -name '*.[ch]'`; do awk -v f=$$f \
		'{ if (length($$0) > 80) { \
		printf "Max line length reached @ %s:%d => %d\n", \
		f, NR, length($$0) } }' $$f; done

touch:
	find ./src -name '*.c' -exec touch {} \;

stats:
	@sh -c 'wc=`find ./src -name "*.[ch]" | xargs wc -l`; \
		echo "===> Num of C source files		: `echo \"$$wc\" | \
		grep -E \"\\.(c|h)$$\" | wc -l`"; \
		echo "===> Num of C source lines		: `echo \"$$wc\" | \
		tail -n 1 | sed s/total//`"'
	@sh -c 'wc=`find ./examples -name "*.fy" | xargs wc -l`; \
		echo "===> Num of Fype source examples	: `echo \"$$wc\" | \
		grep -E \"\\.fy$$\" | wc -l`"; \
		echo "===> Num of Fype source lines		: `echo \"$$wc\" | \
		tail -n 1 | sed s/total//`"'

# Use $(MAKE) for recursive invocations so the correct make binary is always used
stats-tofile:
	$(MAKE) stats | tee ./docs/stats.txt

testrun:
	cat ./test.fy > ./test.out
	./$(BIN) -V ./test.fy | tee -a ./test.out

tr: testrun
test: all testrun
t: test

run:
	./$(BIN) ./test.fy

core:
	gdb $(BIN) $(BIN).core

gdb:
	gdb --args $(BIN) .//test.fy

newline:
	@echo

examples: all
	echo > ./examples/all-examples.txt
	for i in ./examples/*.fy; do \
		echo "===> Running $$i"; \
		./$(BIN) $$i; \
		cat $$i >> ./examples/all-examples.txt; \
		echo >> ./examples/all-examples.txt; \
	done

replace:
	find ./src -name '*.[ch]' -exec sh -c 'sed -n "s/$(FROM)/$(INTO)/g; \
		w .tmp" {} && mv -f .tmp {}' \;

headers:
	@find ./src -name '*.[ch]' -exec sh -c 'export FILE={}; \
		$(MAKE) header' \;

header:
	@echo "===> Processing $(FILE)"
	@sed -n '/*:/d; w .tmp' $(FILE)
	@header=`sed 's/\(.*\)/ echo " \*: \1"/' $(HEADER)`; \
		echo '/*:*' > $(FILE); eval "$$header" >> $(FILE); \
		echo ' *:*/' >> $(FILE); cat .tmp >> $(FILE); rm -f .tmp

# Create a release tarball; use $(MAKE) to call printbuild correctly
tar: clean
	sh -c 'build=`$(MAKE) -s printbuild`;cd ../;\
		tar cvjf $(BIN)-$$build.tar.bz2 $(BIN)'

install: all
	cp ./$(BIN) $(PREFIX)/bin
	cp ./docs/pod/fype.1.gz $(PREFIX)/man/man1

deinstall:
	rm $(PREFIX)/bin/$(BIN)
	rm $(PREFIX)/man/man1/fype.1.gz

uninstall: deinstall

# Build man page from POD source
pod:
	@cd ./docs/pod; $(MAKE) clean all
	@cp ./docs/pod/fype.pod README.pod
