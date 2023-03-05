CC = gcc
CFLAGS = -Wall -g -Werror

## showFDtables: build the showFDtables executable
show_tables: show_tables.c
	$(CC) $(CFLAGS) -o $@ $< -lm

## clean: remove the showFDtables executable and all its output files
.PHONY: clean
clean:
	rm -f showFDtables compositeTable.txt compositeTable.bin

## help: display this help message
.PHONY: help
help: makefile
	@echo "Available targets:"
	@sed -n 's/^##//p' $<