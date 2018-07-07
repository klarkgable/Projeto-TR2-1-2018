COMPILER = g++

# Comando para remover arquivos
RM = rm -f

# Flags para geração automática de dependências
DEP_FLAGS = -M -MT $@ -MT /$(*F).o -MP -MF $@

# Bibliotecas a serem linkadas
LIBS = -lm -lpthread
#LIBS += $(shell pkg-config --libs gtk+-3.0)

# compilacao
FLAGS = -std=c++11 -Wall -Wextra -Wno-unused-parameter -Werror=init-self
# debug
DFLAGS = -ggdb -O0 -DDEBUG
# release
RFLAGS = -O3 -mtune=native

# Uma lista de arquivos por extensão:
CPP_FILES = $(wildcard *.cpp)
INC_FILES = $(wildcard *.h)
FILE_NAMES = $(sort $(notdir $(CPP_FILES:.cpp=)) $(notdir $(INC_FILES:.h=)))
DEP_FILES = $(addprefix ,$(addsuffix .d,))
OBJ_FILES = $(addprefix ,$(notdir $(CPP_FILES:.cpp=.o)))

# Nome do executável
EXEC = aracne

##############################################################################################

.PRECIOUS: $(DEP_FILES)

release: FLAGS += $(RFLAGS)
release: $(EXEC)

debug: FLAGS += $(DFLAGS)
debug: $(EXEC)

clean:
	-$(RMDIR) 
	-$(RM) $(EXEC)

# Gera o executável
$(EXEC): $(OBJ_FILES)
	$(COMPILER) -o $@ $^ $(LIBS) $(FLAGS)

# Gera os arquivos objetos
/%.o: %.d
	$(COMPILER) $(INC_FLAGS) $(addprefix ,$(notdir $(<:.d=.cpp))) -c $(FLAGS) -o $@

# Gera os arquivos de dependencia
/%.d: /%.cpp
	$(COMPILER) $(INC_FLAGS) $< $(DEP_FLAGS) $(FLAGS)

folders:
	@mkdir -p


.PHONY: release debug clean folders help

# Regra pra debug
print-% : ; @echo $* = $($*)

help:
	@echo.
	@echo Available targets:
	@echo - release:  Builds the release version (default target)
	@echo - debug:    Builds the debug version
	@echo - help:     Shows this help
	@echo.

-include $(DEP_FILES)
