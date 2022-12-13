TARGETS=main
build: $(TARGETS)

main: Tema2.java MyThread.java MyTask.java
	javac $^

run_main:
	java Tema2

clean:
	rm -f *.class

.PHONY: build clean