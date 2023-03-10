test:
	@echo "   TESTING   test.c.ib"
	@../ib test.c.ib
	@gcc test.c -o test
	@./test
	@rm test test.c
	
	@echo "   TESTING   test.java.ib"
	@../ib test.java.ib
	@javac test.java
	@java test
	@rm test.class test.java
	
	@echo "   TESTING   test.go.ib"
	@../ib test.go.ib
	@go build test.go
	@./test
	@rm test test.go
