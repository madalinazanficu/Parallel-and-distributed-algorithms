TARGETS=main
build: $(TARGETS)

main: Main.java MyThread.java
	javac $^

run_main:
	java Main

clean:
	rm -f *.class

.PHONY: build clean