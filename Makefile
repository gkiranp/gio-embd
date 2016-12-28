default: all

STATIC = ar rcs
RELEASE = g++

SHAPELIB_VER 	= 1.3.0
SHAPELIB_SRC 	= ./shapelib/shapelib-$(SHAPELIB_VER)
SHAPELIB_BUILD 	= ./x86_release
SHAPELIB_STATIC = $(SHAPELIB_BUILD)/libshape.a
SHAPELIB_OBJ	= $(SHAPELIB_BUILD)/shpopen.o \
		  		  $(SHAPELIB_BUILD)/shptree.o \
		  		  $(SHAPELIB_BUILD)/dbfopen.o \
		  		  $(SHAPELIB_BUILD)/safileio.o

SRC = .
BUILD = ./x86_release
APP = $(BUILD)/tz.exe
OBJ = $(BUILD)/tz.o \
			$(BUILD)/parser_shp.o \
			$(BUILD)/utc2local.o

# build rules
all: $(SHAPELIB_STATIC) $(APP)

$(SHAPELIB_BUILD)/shpopen.o: $(SHAPELIB_SRC)/shpopen.c $(SHAPELIB_SRC)/shapefil.h
	$(CC) $(CFLAGS) -c -g $(SHAPELIB_SRC)/shpopen.c -o $(SHAPELIB_BUILD)/shpopen.o

$(SHAPELIB_BUILD)/shptree.o: $(SHAPELIB_SRC)/shptree.c $(SHAPELIB_SRC)/shapefil.h
	$(CC) $(CFLAGS) -c -g $(SHAPELIB_SRC)/shptree.c -o $(SHAPELIB_BUILD)/shptree.o

$(SHAPELIB_BUILD)/dbfopen.o: $(SHAPELIB_SRC)/dbfopen.c $(SHAPELIB_SRC)/shapefil.h
	$(CC) $(CFLAGS) -c -g $(SHAPELIB_SRC)/dbfopen.c -o $(SHAPELIB_BUILD)/dbfopen.o

$(SHAPELIB_BUILD)/safileio.o: $(SHAPELIB_SRC)/safileio.c $(SHAPELIB_SRC)/shapefil.h
	$(CC) $(CFLAGS) -c -g $(SHAPELIB_SRC)/safileio.c -o $(SHAPELIB_BUILD)/safileio.o

$(BUILD)/tz.o: $(SRC)/tz.cpp
	$(CXX) -c -g $(SRC)/tz.cpp -o $(BUILD)/tz.o 

$(BUILD)/parser_shp.o: $(SRC)/parser_shp.cpp
	$(CXX) -c -g $(SRC)/parser_shp.cpp -o $(BUILD)/parser_shp.o

$(BUILD)/utc2local.o: $(SRC)/utc2local.cpp
	$(CXX) -c -g $(SRC)/utc2local.cpp -o $(BUILD)/utc2local.o

$(SHAPELIB_STATIC): $(SHAPELIB_OBJ)
	$(STATIC) $(SHAPELIB_STATIC) $(SHAPELIB_OBJ)

$(APP): $(OBJ)
	$(RELEASE) $(OBJ) -L$(SHAPELIB_BUILD) -lshape -o $(APP)

clean:
	rm -f $(SHAPELIB_OBJ) $(SHAPELIB_STATIC) $(OBJ) $(APP)
