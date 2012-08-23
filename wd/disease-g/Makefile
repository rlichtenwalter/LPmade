# ------------------------------------------------
# Copyright (C) 2010 by Ryan N. Lichtenwalter
# Email: rlichtenwalter@gmail.com
# 
# This file is part of LPmade.
# LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
# ------------------------------------------------

include ../Makefile.options

SRC := disease-g.txt.gz
WEIGHTED_STREAM := 1
DIRECTED := 0
SRC2STREAM := ./script/src_to_stream.pl

PERIOD1 := 1
PERIOD2 := 2
PERIOD3 := 3
PERIOD4 := 4
PERIOD5 := 5
PERIOD6 := 6
PERIOD7 := 7
PERIOD8 := 8
PERIOD9 := 9
PERIOD10:= 10

FEATURE := $(addprefix PERIOD,$(shell echo {1..8}))
LABEL := PERIOD9
UNSUPERVISED := $(addprefix PERIOD,$(shell echo {1..9}))
TEST := PERIOD10
COMPLETE := $(addprefix PERIOD,$(shell echo {1..10}))
PERIODS := $(addprefix PERIOD,$(shell echo {1..10}))

empty :=
space := $(empty) $(empty)
define PERIOD_STREAM_TEMPLATE
./stream/$(1).txt.gz: $(addprefix ./src/,$(SRC))
	mkdir -p $$(@D);
	< $$< gunzip | $(SRC2STREAM) | grep "^\($(subst $(space),\|,$($(1)))\)," | gzip > $$@.tmp;
	mv $$@.tmp $$@;
endef
$(foreach period,$(PERIODS),$(eval $(call PERIOD_STREAM_TEMPLATE,$(period))))

include ../Makefile.common

./src/%.txt.gz:
	mkdir -p $(@D);
	wget -O - http://www.nd.edu/~rlichten/linkpred/$(@F) > $@.tmp;
	mv $@.tmp $@;
