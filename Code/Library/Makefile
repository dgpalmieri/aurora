
# Apple G4 uniprocessor version
#CC = gcc -g
CC = cc -O2
AR = ar -r

libaurora.a :: double_array_overview.h double_array_overview.c
	$(CC) $(CFLAGS) -c double_array_overview.c
	$(AR) $@ double_array_overview.o
	ranlib libaurora.a
	rm -f double_array_overview.o

libaurora.a :: energy_2_alpha.h energy_2_alpha.c
	$(CC) $(CFLAGS) -c energy_2_alpha.c
	$(AR) $@ energy_2_alpha.o
	ranlib libaurora.a
	rm -f energy_2_alpha.o

libaurora.a :: energy_2_volc.h energy_2_volc.c
	$(CC) $(CFLAGS) -c energy_2_volc.c
	$(AR) $@ energy_2_volc.o
	ranlib libaurora.a
	rm -f energy_2_volc.o

libaurora.a :: float_array_overview.h float_array_overview.c
	$(CC) $(CFLAGS) -c float_array_overview.c
	$(AR) $@ float_array_overview.o
	ranlib libaurora.a
	rm -f float_array_overview.o

libaurora.a :: min_max.h min_max.c
	$(CC) $(CFLAGS) -c min_max.c
	$(AR) $@ min_max.o
	ranlib libaurora.a
	rm -f min_max.o

libaurora.a :: pot_2_energy.h pot_2_energy.c
	$(CC) $(CFLAGS) -c pot_2_energy.c
	$(AR) $@ pot_2_energy.o
	ranlib libaurora.a
	rm -f pot_2_energy.o

libaurora.a :: pot_2_ppm_file.h pot_2_ppm_file.c
	$(CC) $(CFLAGS) -c pot_2_ppm_file.c
	$(AR) $@ pot_2_ppm_file.o
	ranlib libaurora.a
	rm -f pot_2_ppm_file.o

libaurora.a :: energy_2_ppm_file.h energy_2_ppm_file.c
	$(CC) $(CFLAGS) -c energy_2_ppm_file.c
	$(AR) $@ energy_2_ppm_file.o
	ranlib libaurora.a
	rm -f energy_2_ppm_file.o

libaurora.a :: read_alpha.h read_alpha.c
	$(CC) $(CFLAGS) -c read_alpha.c
	$(AR) $@ read_alpha.o
	ranlib libaurora.a
	rm -f read_alpha.o

libaurora.a :: read_potential.h read_potential.c
	$(CC) $(CFLAGS) -c read_potential.c
	$(AR) $@ read_potential.o
	ranlib libaurora.a
	rm -f read_potential.o

libaurora.a :: write_potential.h write_potential.c
	$(CC) $(CFLAGS) -c write_potential.c
	$(AR) $@ write_potential.o
	ranlib libaurora.a
	rm -f write_potential.o

clean:
	rm -f *.o libaurora.a
