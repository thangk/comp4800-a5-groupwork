# $< refers to the first dependency
# $@ refers to the target
# $^ refers to all dependencies
# $* refers to the stem of the target
# @ is used to suppress the command from being printed to the console

.PHONY: all greet dirs run clean

entry = main					# the entry point of the project
assignment = == Assignment 5 == 		# the name of the assignment
authors = == by Kap Thang & Shubham Mehta ==			# the author of the assignment
compiler = gcc					# compiler

# folder destinations
src = src
include = include
kt = kt
kth = kth
build = build
bin = bin
headers_only = include/kth

# external libraries
preqlibs = $(shell pkg-config --libs --cflags gtk4 libavcodec libavformat libswscale libavutil)

# misc
tab = ">   "

# Find all .c files in src directory, replace src/%.c with build/%.o
C_FILES = $(wildcard $(src)/*.c)
O_FILES = $(C_FILES:$(src)/%.c=$(build)/%.o)

all: greet dirs $(bin)/$(entry) run

debug: debugMode dirs $(bin)/$(entry) debugging

debugMode: 
	@echo ""
	@echo ">  $(assignment)"
	@echo ">  $(authors)"
	@echo ">"
	@echo ">  Debugging mode... let's get started!"
	@echo "> "

greet:
	@echo ""
	@echo ">  $(assignment)"
	@echo ">  $(authors)"
	@echo ">"
	@echo ">  Now... let's get started!"
	@echo "> "

dirs:
	@mkdir -p $(bin) $(build)

$(bin)/$(entry): $(O_FILES)
	@echo $(tab) Linking $(entry) and the other files
	@$(compiler) -o $@ $^ $(preqlibs)

# Rule to compile all .c files to .o files
$(build)/%.o: $(src)/%.c 
	@echo $(tab) Compiling $*.c
	@$(compiler) -I$(include) -I$(headers_only) -c $< -o $@ $(preqlibs)

run: $(bin)/$(entry)
	@echo $(tab) Finally, running $(assignment)
	@echo ""
	@./$< $(ARGS)

debugging: $(bin)/$(entry)
	@echo $(tab) Debugging $(assignment)
	@echo ""
	@gdb ./$< $(ARGS)

clean:
	@echo Cleaning up some mess
	@rm -rf $(build)/* $(bin)/* $(bin) $(build) $(app) *.o *.txt
	@echo "Done!"

mc:
	@echo "Merging C files from $(src) into merged-sources.txt..."
	@> merged-sources.txt # Clear the file first
	@for file in $(src)/*.c; do \
		echo "/*------- From file: $$file -------*/" >> merged-sources.txt; \
		cat "$$file" >> merged-sources.txt; \
		echo "" >> merged-sources.txt; \
		echo "-------------------- END OF FILE --------------------" >> merged-sources.txt; \
		echo "" >> merged-sources.txt; \
	done

mh:
	@echo "Merging header files from $(include)/$(kt) and $(include)/$(kth) into merged-headers.txt..."
	@> merged-headers.txt # Clear the file first
	@for dir in $(include)/$(kt) $(include)/$(kth); do \
        for file in $$dir/*.h; do \
            echo "/*------- From file: $$file -------*/" >> merged-headers.txt; \
            cat "$$file" >> merged-headers.txt; \
            echo "" >> merged-headers.txt; \
            echo "-------------------- END OF FILE --------------------" >> merged-headers.txt; \
            echo "" >> merged-headers.txt; \
            echo "" >> merged-headers.txt; \
        done; \
    done