#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#define MAX_ARGS 10
#define REC_LENGTH 49	// size of a record in db
#define REC_WINDOW 10	// number of records to read at a time
#define LATITUDE_POS 28	// location of latitude coordinates in input record
#define OPEN 10000	// initial value of nearest neighbors
struct neighbor {
	char entry[REC_LENGTH];
	double dist;
};

/**
* This program finds the k-nearest neighbors
* Usage:	./nn <filelist> <num> <target latitude> <target longitude>
*			filelist: File with the filenames to the records
*			num: Number of nearest neighbors to find
*			target lat: Latitude coordinate for distance calculations
*			target long: Longitude coordinate for distance calculations
* The filelist and data are generated by hurricane_gen.c
* REC_WINDOW has been arbitrarily assigned; A larger value would allow more work for the threads
*/
int main(int argc, char* argv[]) {
	FILE   *flist,*fp;
	int    i=0,j=0, k=0, rec_count=0, done=0;
	char   sandbox[REC_LENGTH * REC_WINDOW], *rec_iter, dbname[64];
	struct neighbor *neighbors = NULL;
	float target_lat, target_long, tmp_lat=0, tmp_long=0;

	if(argc < 5) {
		fprintf(stderr, "Invalid set of arguments\n");
		exit(-1);
	}

	flist = fopen(argv[1], "r");
	if(!flist) {
		printf("error opening flist\n");
		exit(1);
	}

	k = atoi(argv[2]);
	target_lat = atof(argv[3]);
	target_long = atof(argv[4]);

	neighbors = malloc(k*sizeof(struct neighbor));

	if(neighbors == NULL) {
		fprintf(stderr, "no room for neighbors\n");
		exit(0);
	}

	for( j = 0 ; j < k ; j++ ) { //Initialize list of nearest neighbors to very large dist
		neighbors[j].dist = OPEN;
	}

	/**** main processing ****/  
	if(fscanf(flist, "%s\n", dbname) != 1) {
		fprintf(stderr, "error reading filelist\n");
		exit(0);
	}

	fp = fopen(dbname, "r");
	if(!fp) {
		printf("error opening flist\n");
		exit(1);
	}

	float *z;
	z  = (float *) malloc(REC_WINDOW * sizeof(float));

	while(!done) {
		//Read in REC_WINDOW number of records
		rec_count = fread(sandbox, REC_LENGTH, REC_WINDOW, fp);
		if( rec_count != REC_WINDOW ) {
			if(!ferror(flist)) {// an eof occured
				fclose(fp);

				if(feof(flist))
		  			done = 1;
				else {
	 				if(fscanf(flist, "%s\n", dbname) != 1) {
	    					fprintf(stderr, "error reading filelist\n");
	    					exit(0);
					}

	  				fp = fopen(dbname, "r");

	  				if(!fp) {
					    printf("error opening a db\n");
					    exit(1);
	  				}
				}
			} else {
				perror("Error");
				exit(0);
			}
		}

		/* Launch threads to  */
		#pragma omp parallel for shared(z, target_lat, target_long) private(i, rec_iter, tmp_lat, tmp_long)
		for( i = 0 ; i < rec_count ; i++ ) {
			rec_iter = sandbox+(i * REC_LENGTH + LATITUDE_POS - 1);
			sscanf(rec_iter, "%f %f", &tmp_lat, &tmp_long);
			z[i] = sqrt(( (tmp_lat-target_lat) * (tmp_lat-target_lat) )+( (tmp_long-target_long) * (tmp_long-target_long) ));
		} /* omp end parallel */
		#pragma omp barrier

		for( i = 0 ; i < rec_count ; i++ ) {
			float max_dist = -1;
			int max_idx = 0;
			// find a neighbor with greatest dist and take his spot if allowed!
			for( j = 0 ; j < k ; j++ ) {
				if( neighbors[j].dist > max_dist ) {
					max_dist = neighbors[j].dist;
					max_idx = j;
				}
			}
			// compare each record with max value to find the nearest neighbor
			if( z[i] < neighbors[max_idx].dist ) {
				sandbox[(i+1)*REC_LENGTH-1] = '\0';
			  	strcpy(neighbors[max_idx].entry, sandbox +i*REC_LENGTH);
			  	neighbors[max_idx].dist = z[i];
			}
		}
	}//End while loop

	fprintf(stderr, "The %d nearest neighbors are:\n", k);
	for( j = 0 ; j < k ; j++ ) {
		if( !(neighbors[j].dist == OPEN) )
			fprintf(stderr, "%s --> %f\n", neighbors[j].entry, neighbors[j].dist);
	}

	fclose(flist);
	return 0;
}

