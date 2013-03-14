# ------------------------------------------------
# Copyright (C) 2010 by Ryan N. Lichtenwalter
# Email: rlichtenwalter@gmail.com
# 
# This file is part of LPmade.
# LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
# ------------------------------------------------

CONFIG := BIN
ifeq ($(CONFIG),DEBUG) 
	CFLAGS := -Wall -Werror -g
	BINDIR := debug
endif
ifeq ($(CONFIG),BIN)
	CFLAGS := -Wall -Werror -O3 -D NDEBUG
	BINDIR := bin
endif
CC := g++
NETWORK_BINS := $(patsubst %.cpp,%,$(notdir $(wildcard ./mains/*.cpp)))

all: network predictors
	cp ./LinkPredictor/$(BINDIR)/* $(BINDIR);

network: $(addprefix $(BINDIR)/,$(NETWORK_BINS))
predictors: ./LinkPredictor/$(BINDIR)/predict
test: $(patsubst %,./test/%.TEST,$(NETWORK_BINS) $(PREDICTOR_BINS))

$(BINDIR)/%: ./mains/%.o WeightedNetwork.o Statistics.o
	@mkdir -p $(@D);
	$(CC) $(CFLAGS) $^ -o $@;

./LinkPredictor/$(BINDIR)/predict: ./LinkPredictor/mains/predict.o $(patsubst %.cpp,%.o,$(wildcard ./LinkPredictor/*.cpp)) WeightedNetwork.o
	@mkdir -p $(@D);
	$(CC) $(CFLAGS) $^ -o $@;

./Model/$(BINDIR)/%: ./Model/mains/%.o ./Model/%Model.o WeightedNetwork.o
	@mkdir -p $(@D);
	$(CC) $(CFLAGS) $^ -o $@;

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@;

./test/%.TEST: $(BINDIR)/%
	@if [ -f ./test/$*.RUN ] ; then \
		echo -n 'Testing $*...'; \
		./test/$*.RUN $(BINDIR); \
	else \
		echo 'Testing $*...TEST UNAVAILABLE'; \
	fi

.PHONY: clean all test network predictors

clean:
	-rm -rf *.o mains/*.o LinkPredictor/mains/*.o $(BINDIR) LinkPredictor/$(BINDIR) LinkPredictor/*.o ./test/*.OUTPUT
