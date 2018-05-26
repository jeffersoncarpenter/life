# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE= @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

CXX=g++.exe
CXXFLAGS=$(DEPFLAGS) -std=c++17 -O3 -IH:\glm-0.9.8.5 -IH:\glew\include -IH:\glfw-3.2.1\include -IH:\SOIL\src
LDFLAGS=-LH:\glew\lib -LH:\glfw-3.2.1\lib-mingw -LH:\SOIL\lib -lSOIL -lglew32.dll -lglfw3 -lglu32 -lopengl32 -lgdi32

objects=\
appState.o\
assets.o\
compile_assets.o\
life.o\
main.o\

all: $(objects)
	$(CXX) $(objects) $(LDFLAGS) -o life.exe

$(objects): %.o: %.cpp $(DEPDIR)/%.d
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(patsubst %,$(DEPDIR)/%.d,$(basename $(objects)))
