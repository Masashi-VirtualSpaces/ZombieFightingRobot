
CC = gcc
ODIR = obj
SDIR = src
DOCDIR = doc
INC = -Iinclude
LIBS = -lpthread -lwiringPi -lm
CFLAGS = -Wall
BINS = diagnostic
# zombie

_OBJ_DIAG = motors.o proximity.o diagnostic.o ir_transceiver.o audio_player.o utils.o web.o
# _OBJ_MAIN = motors.o zombie.o

OBJS_DIAG = $(patsubst %,$(ODIR)/%,$(_OBJ_DIAG))
# OBJS_MAIN = $(patsubst %,$(ODIR)/%,$(_OBJ_MAIN))

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) $(LIBS)

diagnostic: $(OBJS_DIAG)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# zombie: $(OBJS_MAIN)
# 	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: $(BINS)

doc: 
	doxygen
 
clean:
	rm -Rf $(ODIR) $(BINS) $(DOCDIR)
