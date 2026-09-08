JAVAC    = javac
ENCODING = -encoding UTF-8
SRC      = StudentScore.java

all: StudentScore.class

StudentScore.class: $(SRC)
	$(JAVAC) $(ENCODING) $(SRC)

run: all
	java StudentScore

clean:
	rm -f *.class

.PHONY: all run clean
