#
# Organisation des sources.
#

# Packages du projet (à compléter si besoin est).
PACKAGES = client remoteUI

# Un niveau de package est accessible.
SRC  = $(wildcard */*.c)
# Pour ajouter un second niveau :		
# SRC += $(wildcard */*/*.c)

OBJ = $(SRC:.c=.o)

# Point d'entrée du programme.
MAIN = telco.c

# Gestion automatique des dépendances.
DEP = $(MAIN:.c=.d)

# Exécutable à générer.
EXEC = ../$(PROG_TELCO)

# Inclusion depuis le niveau du package.
CCFLAGS += -I.

#
# Règles du Makefile.
#

# Compilation.
all:
	for p in $(PACKAGES); do (cd $$p; $(MAKE) $@); done
	@$(MAKE) CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" $(EXEC)

$(EXEC): $(OBJ) $(MAIN)
	$(CC) $(CCFLAGS) $(OBJ) $(MAIN) -MF $(DEP) -o $(EXEC) $(LDFLAGS)

# Nettoyage.
.PHONY: clean

clean:
	@for p in $(PACKAGES); do (cd $$p; $(MAKE) $@); done
	@rm -f $(DEP)

-include $(DEP)