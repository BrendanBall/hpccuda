all: bin

bin:  $(BIN)
	@cp $(BIN) ./

#link
$(BIN): $(UTILOBJS) $(NVOBJS) $(CPPOBJS) $(COBJS)
	$(NVCC) $(NVCLFLAGS) $^ -o $@ $(OMPLFLAGS) $(LFLAGS)

#Compile utils
$(UTILDIR)/$(TARGET)/%.o: $(UTILDIR)/%.cpp
	@mkdir -p $(UTILDIR)/$(TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

#Compile cpp
$(TARGET)/%.o: %.cpp
	@mkdir -p $(TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

#Compile c
$(TARGET)/%.o: %.c
	@mkdir -p $(TARGET)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<
	
#Compile cu
$(TARGET)/%.o: %.cu
	@mkdir -p $(TARGET)
	$(NVCC) $(NVCCFLAGS) $(INCLUDES) -o $@ -c $<

test: bin
	./$(BIN)

run: bin
	./$(BIN)

new:
	$(MAKE) clean
	$(MAKE)

superclean: distclean
	rm -rf *.e* *.o* ./.??* *.csv *.fft

distclean: clean
	rm -rf release debug

clean:
	rm -rf *.~ *~ *.o $(CPPOBJS) $(UTILOBJS) *.out $(BIN) $(BINNAME)