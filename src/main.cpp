#include "rebound.h"
#include <stdio.h>
#include <stdlib.h>

void heartbeat(struct reb_simulation* r){
	printf("%f\n",r->t);
}

int main(int argc, char* argv[]) {
	struct reb_simulation* r = reb_create_simulation();
	r->dt = 0.1;
	r->heartbeat = heartbeat;
	r->exact_finish_time = 1; // Finish exactly at tmax in reb_integrate(). Default is already 1.

	struct reb_particle p1 = {0};
	p1.m = 1.;
	reb_add(r, p1);

	struct reb_particle p2 = {0};
	p2.x = 1;
	p2.vy = 1;
	reb_add(r, p2);

	reb_integrate(r,100.);
}
