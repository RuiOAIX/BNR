.PHONY: all clean help
#no PHONY targets: $(lib_archive), $(bin_folder)/%.d, $(bin_folder)/%.o, $(apps)
all:

## variables and functions #######################################
RM := rm -rf
CC := gcc
CFLAGS += -Wall -g
bin_folder := bin
src_folder := src
findsource = $(wildcard $(1)/*.c)
findobj = $(patsubst %.c,$(bin_folder)/%.o,$(1))
##################################################################

## all ###########################################################
all_sources :=  $(call findsource,$(src_folder))
all_objects := $(call findobj,$(all_sources))
all_folders_in_bin := $(sort $(dir $(all_objects)))
##################################################################

## make sure directory is existed#################################
makesuredir := $(shell mkdir -p $(all_folders_in_bin))
##################################################################

## automatic prerequisites (working for both lib and app) ########
autodependence := $(patsubst %.c,$(bin_folder)/%.d,$(all_sources))
ifneq "$(MAKECMDGOALS)" "clean"
	include $(autodependence)
endif
$(bin_folder)/$(src_folder)/%.d: $(src_folder)/%.c
	set -e; rm -f $@; \
		$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,$(bin_folder)/$(src_folder)/\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
##################################################################

## .o compiled from .c  (working for both lib and app) ###########
$(bin_folder)/$(src_folder)/%.o : $(src_folder)/%.c 
	$(COMPILE.c) $(OUTPUT_OPTION) $<
##################################################################

## x compiled from x.o and libs ##################################
run: $(all_objects)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -lm -o $@
##################################################################

## clean #########################################################
clean:
	@$(RM) $(bin_folder)
	@$(RM) run
##################################################################

## run #########################################################
r: run
#./run -i ../DATA/movielens/movielens_3c.txt -wD
	./run -i ../DATA/netflix/netflix_3c.txt -wD
#./run -i ../DATA/rym/rym_3c.txt -wD
val: run
	valgrind --tool=memcheck --leak-check=full -v --track-origins=yes ./run -i ../DATA/movielens/movielens_3c.txt -wD --rate-mass-degree 0.8 --rate-mass-score-third 0.8 -s 1000
#valgrind --tool=memcheck --leak-check=full -v --track-origins=yes ./run -i ../DATA/netflix/netflix_3c.txt -wD --rate-mass-degree 0.8 --rate-mass-score-third 0.8 -s 1000
gdb: run
	gdb --args ./run -i ../DATA/rym/rym_3c.txt -x
#gdb --args ./run -i ../DATA/movielens/movielens_3c.txt -S
##################################################################

all: run
