# ------------------------------------------------
# Copyright (C) 2010 by Ryan N. Lichtenwalter
# Email: rlichtenwalter@gmail.com
# 
# This file is part of LPmade.
# LPmade is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. LPmade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with LPmade. If not, see <http://www.gnu.org/licenses/>.
# ------------------------------------------------

CFLAGS = -Wall -lm -O3

all: hmeasure aupr auroc roc pr ef topnprec cost_curve

hmeasure: hmeasure.c point_vector.c
	gcc -o $@ $^ $(CFLAGS)

cost_curve: curve.c point_vector.c
	gcc -o $@ $^ $(CFLAGS)

aupr: aupr.c
	gcc -o $@ $^ $(CFLAGS)

auroc: auroc.c
	gcc -o $@ $^ $(CFLAGS)

roc: roc.c
	gcc -o $@ $^ $(CFLAGS)

pr: pr.c
	gcc -o $@ $^ $(CFLAGS)

ef: ef.c
	gcc -o $@ $^ $(CFLAGS)

topnprec: topnprec.c
	gcc -o $@ $^ $(CFLAGS)

.phony: all clean

clean:
	-rm -f main.o hmeasure aupr auroc roc pr ef topnprec cost_curve
