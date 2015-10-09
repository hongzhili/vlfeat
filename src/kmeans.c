/** @file   Clustering-KMeans.c
** @author Hongzhi LI
** @breif  kmeans interface
**/

#include <stdio.h>
#include <assert.h>

#include <vl/kmeans.h>
#include <vl/host.h>
#include <vl/kdtree.h>

void PrintUsage()
{
	printf("Usage:\n");
	printf("CommendLine SampleFileName ClusterNumber OutputFileName [MaxIterationNumber=100] [QuantizeFile QuantizeResult] \n");
}


void SerializeVLKmeans(VlKMeans * self, char* filename)
{
	FILE *fp;
	fp = fopen(filename,"wb");
	fwrite(&(self->distance), sizeof(self->distance),1,fp);
	fwrite(&(self->dataType), sizeof(self->dataType),1,fp);
	fwrite(&(self->numCenters), sizeof(self->numCenters),1,fp);
	fwrite(&(self->dimension), sizeof(self->dimension),1,fp);
	fwrite(self->centers, sizeof(float),self->numCenters*self->dimension,fp);

	
	fclose(fp);
}


int
	main (int argc VL_UNUSED, char *argv[] VL_UNUSED)
{
	int i ;
	int j;
	float * centers;
	char *buf;
	float *qdata;
	float * data;
	float * rp;
	char * tbuf;
	float *tdata;
	float *a;
	void * dist;
	vl_uint32* res;
	FILE * fp;
	FILE * qfp;
	FILE * parafp;

	double bestEnergy;
	int FeatNum;
	int FeatDim;
	int ClusterNum;
	int MaxIterNum;
	char * SampleFileName;
	char * OutputFileName;
	char * paraFileName;
	int round;

	
	if (argc<3)
	{
		PrintUsage();
		return 0;
	}

	
	SampleFileName = argv[1];
	ClusterNum = atoi(argv[2]);
	OutputFileName = argv[3];


	paraFileName = malloc(10240);

	sprintf(paraFileName,"%s.para",OutputFileName);

	parafp = fopen(paraFileName,"w");
	if (argc>=5)
	{
		MaxIterNum = atoi(argv[4]);
	}
	else 
	{
		MaxIterNum = 100;
	}

	if (argc>=6)
	{
		round = atoi(argv[5]);
	}
	else 
	{
		round = 1;
	}
	
	printf("Number of round: %d \n",round);

	fp = fopen(SampleFileName,"r");

	fscanf(fp,"%d %d\n",&FeatNum, &FeatDim);

	printf("Input File: %s \n Feature Number: %d, Feature Dimension: %d\n",SampleFileName, FeatNum, FeatDim);

	data=(float*) malloc(sizeof(float)*FeatNum*FeatDim);
	rp = data;


	for (i=0;i<FeatNum;i++)
	{
		for (j = 0;j<FeatDim;j++)
		{
			fscanf(fp,"%f",rp);
			rp++;
		}
	}



	fclose(fp);

	//vl_kmeans_set_algorithm	(kmeans, VlKMeansElkan);  // faster but need more memory space
	//vl_kmeans_set_algorithm	(kmeans, VlKMeansLloyd);  // stander kmeans algorithm, but maybe slow.



	VlRand rand;

	vl_size numData = 100000;
	vl_size maxComp = 100;
	vl_size maxrep = 1;
	vl_size ntrees = 1;

	vl_size dataIdx, d;

	//VlKMeansAlgorithm algorithm = VlKMeansANN ;
	VlKMeansAlgorithm algorithm = VlKMeansLloyd;
	//VlKMeansAlgorithm algorithm = VlKMeansElkan ;
	VlVectorComparisonType distance = VlDistanceL2;
	VlKMeans * kmeans = vl_kmeans_new(VL_TYPE_DOUBLE, distance);

	vl_rand_init(&rand);
	vl_rand_seed(&rand, 1000);

	vl_kmeans_set_verbosity(kmeans, 1);
	vl_kmeans_set_max_num_iterations(kmeans, MaxIterNum);
	vl_kmeans_set_max_num_comparisons(kmeans, maxComp);
	vl_kmeans_set_num_repetitions(kmeans, maxrep);
	vl_kmeans_set_num_trees(kmeans, ntrees);
	vl_kmeans_set_algorithm(kmeans, algorithm);

	vl_kmeans_cluster(kmeans, data, FeatDim, FeatNum, ClusterNum);
	



	//bestEnergy=-1;

	//printf("Clustering....\n");

	////kmeans plus plus selection

	//vl_kmeans_set_initialization(kmeans,	VlKMeansPlusPlus);

	//if (vl_kmeans_get_initialization(kmeans)==VlKMeansPlusPlus)
	//{
	//	printf("Initialization Algorithm: KMeans Plus Plus\n");
	//}
	//else
	//{
	//	printf("Initialization Algorithm: Random Selection\n");
	//}

	//for (i=0;i<round;i++)
	//{
	//	printf("Round %d\n",i);

	//	printf("Init Centers....\n");

	//	vl_kmeans_seed_centers_plus_plus (kmeans, data, FeatDim, FeatNum, ClusterNum);
	//	kmeans->energy = vl_kmeans_refine_centers (kmeans, data, FeatNum);

	//	if (bestEnergy > vl_kmeans_get_energy(kmeans) || bestEnergy <= 0)
	//	{
	//		printf("============Get A Better Solution! Energy = %lf=========\n",vl_kmeans_get_energy(kmeans));
	//		bestEnergy = vl_kmeans_get_energy(kmeans);
	//		bestKmeans = vl_kmeans_new_copy(kmeans);
	//		bestKmeans->energy = kmeans->energy;

	//	}
	//	vl_kmeans_reset(kmeans);
	//}



	//// random selectin

	//vl_kmeans_set_initialization(kmeans,	VlKMeansRandomSelection);

	//if (vl_kmeans_get_initialization(kmeans)==VlKMeansPlusPlus)
	//{
	//	printf("Initialization Algorithm: KMeans Plus Plus\n");
	//}
	//else
	//{
	//	printf("Initialization Algorithm: Random Selection\n");
	//}

	//for (i=0;i<round;i++)
	//{
	//	printf("Round %d\n",i);

	//	printf("Init Centers....\n");

	//	vl_kmeans_seed_centers_with_rand_data (kmeans, data, FeatDim, FeatNum, ClusterNum);
	//	kmeans->energy = vl_kmeans_refine_centers (kmeans, data, FeatNum);

	//	if (bestEnergy > vl_kmeans_get_energy(kmeans) || bestEnergy < 0)
	//	{
	//		printf("============Get A Better Solution! Energy = %lf=========\n",vl_kmeans_get_energy(kmeans));
	//		bestEnergy = vl_kmeans_get_energy(kmeans);
	//		bestKmeans = vl_kmeans_new_copy(kmeans);
	//		bestKmeans->energy = kmeans->energy;
	//	}
	//	vl_kmeans_reset(kmeans);
	//}


	//kmeans = vl_kmeans_new_copy(bestKmeans);
	//kmeans->energy = bestKmeans->energy;

	//printf("====================Clustering Finished!!!===================\n ====================Final Energy: %lf=================== \n",vl_kmeans_get_energy(kmeans));

	SerializeVLKmeans(kmeans,OutputFileName);

	fprintf(parafp,"%lf",vl_kmeans_get_energy(kmeans));
	fclose(parafp);

	centers = (float *) vl_kmeans_get_centers (kmeans);



	rp = data;
	for (i=0;i<FeatNum;i++)
        {
		qdata = (float *) malloc(sizeof(float)*162);	
                for (j = 0;j<FeatDim;j++)
                {
                        fscanf(fp,"%f",rp);
                        rp++;
                }
        }

	if (argc==7)
	{
		printf("Quantizing: %s.... Output File: %s\n",argv[5],argv[6]);
		fp = fopen(argv[5],"r");
		qfp = fopen(argv[6],"w");

		buf = (char *) malloc(sizeof(char)*10240);
		qdata = (float *) malloc(sizeof(float)*162);
		while (!feof(fp))
		{
			memset(buf,0x00,10240);
			fgets(buf,10240,fp);
			if (buf[0]!='#')
			{
				tbuf = buf;
				tdata = qdata;
				for(i=0;i<FeatDim;i++)
				{
					a = (float*) malloc(sizeof(float));
					sscanf(tbuf,"%f",a);
					while (*tbuf!='\0' && *tbuf !=' ')
					{
						tbuf++;
					}
					if (*tbuf == ' ')
					{
						tbuf++;
					}
//					if (i>8)
					{
						*tdata = *a;
						tdata++;
					}
					free(a);
				}
				res = (vl_uint32*) malloc(sizeof(vl_uint32));

				vl_kmeans_quantize(kmeans,res,dist,qdata,1);
				fprintf(qfp,"%u\n",*res);
				free(res);
			}
			else
			{
				fprintf(qfp,"%s\n",buf);
			}
		}
		fclose(fp);
		fclose(qfp);
	}


	return 0;
}
