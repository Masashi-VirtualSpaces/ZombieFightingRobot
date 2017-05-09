CC = gcc
ODIR = obj
SDIR = src
INC = -Iinclude
LIBS = -lpthread -lwiringPi
CFLAGS = -Wall
BINS = lab4
     # lab4

_OBJ_DIAG = utils.o proximity.o web.o audio_player.o main.o
# _OBJ_MAIN = utils.o proximity.o web.o audio_player.o lab4.o

OBJS_DIAG = $(patsubst %,$(ODIR)/%,$(_OBJ_DIAG))
# OBJS_MAIN = $(patsubst %,$(ODIR)/%,$(_OBJ_MAIN))

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) $(LIBS)

lab4: $(OBJS_DIAG)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# zombie: $(OBJS_MAIN)
#	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
all: $(BINS)

clean:
	rm -Rf $(ODIR) $(BINS)
