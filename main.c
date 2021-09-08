#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


void get_two_dimension(char* line, double** data, char *filename);
void print_two_dimension(double** data, int row, int col);
int get_row(char *filename);
int get_col(char *filename);
void random(double** data,int row);
void str_to_int(char* strdata);
void evaluate_algorithm(double** data,int n_folds,int num_neighbors);
void normalize_dataset(double** data);
double euclidean_distance(double* row1,double* row2);
double rmse_metric(double** actual,double** predicted);
double get_neighbor_predict_regression(double** train,double* test_row,int num_neighbors);

double **data;
double **train;
double **test;
double **actual, **predicted;
double *scores;
char *strdata,*strdata_uniq;
int row,col,fold_size,num_folds,n_neighbors;
double mean_accuracy;
double** neighbors;

int main()
{
	char filename[] = "abalone.data";
    char line[1024];
    row = get_row(filename);
    col = get_col(filename);
    data = (double **)malloc(row * sizeof(int *));
	for (int i = 0; i < row; ++i){
		data[i] = (double *)malloc(col * sizeof(double));
	}//��̬�����ά����
	strdata = (char *)malloc(row * sizeof(char));
	strdata_uniq = (char *)malloc(row/3 * sizeof(char));
	get_two_dimension(line, data, filename);
	printf("�������� = %d\n", row);
	printf("�������� = %d\n", col);
//��ʾ��1������
    printf("ԭʼ��һ������Ϊ\n");
	print_two_dimension(data,1 , col);

//�ѷ����ַ���ת��Ϊ����
    str_to_int(strdata);
    printf("�Ա�����ַ�������Ӧindex\n");
    for(int i=0;i<3;++i){
        printf("index%d=%c\n",i,strdata_uniq[i]);
            }
//��������˳��
    random(data,row);
    printf("���Һ��һ������Ϊ\n");
    print_two_dimension(data,1 , col);

    normalize_dataset(data);
    printf("��һ�����һ������Ϊ\n");
    print_two_dimension(data,1 , col);

    neighbors = (double **)malloc(n_neighbors * sizeof(int *));
	for (int i = 0; i < n_neighbors; ++i){
		neighbors[i] = (double *)malloc(col*sizeof(double));
	}


//ѵ��ģ�Ͳ�����׼ȷ��
    evaluate_algorithm(data,5,5);
    printf("mean_rmse=%f\n",mean_accuracy);


    free(strdata);
    free(strdata_uniq);
    strdata=NULL;
    strdata_uniq=NULL;

    free(scores);
    scores=NULL;
    for(int i=0;i<(num_folds-1)*fold_size;i++){
        free(train[i]);
        train[i]=NULL;
    }
    free(train);
    train=NULL;
    for(int i=0;i<fold_size;i++){
        free(test[i]);
        test[i]=NULL;
        free(predicted[i]);
        free(actual[i]);
        predicted[i]=NULL;
        actual[i]=NULL;
    }
    free(test);
    test=NULL;
    free(predicted);
    predicted=NULL;
    free(actual);
    actual=NULL;
    for(int i=0;i<row;i++){
        free(data[i]);
        data[i]=NULL;
    }
    free(data);
    data=NULL;
    for (int i = 0; i < n_neighbors;++i){
        free(neighbors[i]);
		neighbors[i] = NULL;
	}
	free(neighbors);
	neighbors=NULL;
}

void get_two_dimension(char* line, double** data, char *filename)
{
	FILE* stream = fopen(filename, "r");
	int i = 0;
	while (fgets(line, 1024, stream))//���ж�ȡ
    {
    	int j = 0;
    	char *tok;
        char* tmp = strdup(line);
        for (tok = strtok(line, ","); tok && *tok; j++, tok = strtok(NULL, ",\n")){
                if(j==0){
                    strdata[i]=*tok;//�洢�����ַ�
                }
            data[i][j] = atof(tok);//ת���ɸ�����
		}//�ַ�����ֲ���
        i++;
        free(tmp);
    }
    fclose(stream);//�ļ��򿪺�Ҫ���йرղ���
}
void str_to_int(char* strdata)
{
    int k=0;
    int j;
    for(int i=0;i<row;++i){
        for(j=i+1;j<row && strdata[i]!=strdata[j];++j);
        if(!(j-row))//û���ظ�Ԫ��
            strdata_uniq[k++]=strdata[i];
    }
    printf("�Ա�����ܹ���%d��\n",k);
    for(int i=0;i<row;++i){
            for(j=0;j<k;++j){
                if(strdata[i]==strdata_uniq[j]){
                    data[i][0]=j;
                }
            }
    }
}

void print_two_dimension(double** data, int row, int col)
{
	int i, j;
	for(i=0; i<row; i++){
		for(j=0; j<col; j++){
			printf("%f\t", data[i][j]);
		}
		printf("\n");
	}
}

int get_row(char *filename)
{
	char line[1024];
	int i = 0;
	FILE* stream = fopen(filename, "r");
	while(fgets(line, 1024, stream)){
		i++;
	}
	fclose(stream);
	return i;
}

int get_col(char *filename)
{
	char line[1024];
	int i = 0;
	FILE* stream = fopen(filename, "r");
	fgets(line, 1024, stream);
	char* token = strtok(line, ",");
	while(token){
		token = strtok(NULL, ",");
		i++;
	}
	fclose(stream);
	return i;
}

//��������
void random(double** data,int row)
{
    int index, tmp, i;

   // srand(time(NULL));
    for (i = 0; i <row; i++)
    {
        index = rand() % (row - i) + i;
        if (index != i)
        {
        tmp = data[i];

        data[i] = data[index];

        data[index] = tmp;
        }
    }
}

double rmse_metric(double** actual,double** predicted)
{
    double sum_error=0.0;
    double prediction_error=0.0;
    for(int i=0;i<fold_size;i++)
    {
        prediction_error=predicted[i][0]-actual[i][0];
        sum_error+= pow(prediction_error,2);
    }
    double mean_error=sum_error/fold_size;
    double rmse=sqrt(mean_error);
    return rmse;
}


void evaluate_algorithm(double** data,int n_folds,int num_neighbors)
{
    fold_size=(int)(floor(row/n_folds));
    num_folds=n_folds;
    n_neighbors=num_neighbors;
    train = (double **)malloc((n_folds-1) *fold_size* sizeof(int *));
	for (int i = 0; i < (n_folds-1) *fold_size; ++i){
		train[i] = (double *)malloc(col * sizeof(double));
	}
	test = (double **)malloc(fold_size * sizeof(int *));
	for (int i = 0; i < fold_size; ++i){
		test[i] = (double *)malloc(col * sizeof(double));
	}
	actual = (double **)malloc(fold_size * sizeof(int *));
	for (int i = 0; i < fold_size; ++i){
		actual[i] = (double *)malloc( sizeof(double));
	}
    predicted = (double **)malloc(fold_size * sizeof(int *));
    for (int i = 0; i < fold_size; ++i){
        predicted[i] = (double *)malloc( sizeof(double));
    }
     scores = (double *)malloc(n_folds * sizeof(double));
     for(int i=0;i<n_folds;++i){
        scores[i]=0.0;
     }
    mean_accuracy=0.0;
	for(int k=0;k<n_folds;++k){
        for(int i=0;i<fold_size;++i){
            test[i]=data[k*fold_size+i];
            actual[i][0]=test[i][col-1];
        }
        for(int i=0;i<n_folds*fold_size;++i){
            if(i<k*fold_size){
                train[i]=data[i];
            }
            if(i>=(k+1)*fold_size){
                train[i-fold_size]=data[i];
            }
        }
        for(int i=0;i<fold_size;++i){
            predicted[i][0]=get_neighbor_predict_regression(train,test[i],num_neighbors);
        }

        scores[k]=rmse_metric(actual,predicted);
        printf("fold_k=%d,score=%3f\n",k,scores[k]);
      //  printf("�������30������actual=%f   predict=%f\n",actual[40][0],predicted[40][0]);
        mean_accuracy+=scores[k];
	}
	mean_accuracy=mean_accuracy/(double)(n_folds);

}

void normalize_dataset(double** data)
{
    double minmax[2][col-1];
    for(int j=0;j<col-1;++j){
        minmax[0][j]=data[0][j];
        minmax[1][j]=data[0][j];
    }
    for(int i=1;i<row;++i){
            for(int j=0;j<col-1;++j){
                if(minmax[1][j]<data[i][j]){
                    minmax[1][j]=data[i][j];
                }
                if(minmax[0][j]>data[i][j]){
                    minmax[0][j]=data[i][j];
                }
            }
    }
     for(int i=0;i<row;++i){
            for(int j=0;j<col-1;++j){
                data[i][j]=(data[i][j]-minmax[0][j])/(minmax[1][j]-minmax[0][j]);
            }
    }
}

double euclidean_distance(double* row1,double* row2)
{
    double distance=0.0;
    for(int i=0;i<col-1;++i){
        distance+=pow((row1[i]-row2[i]),2);
    }
    return sqrt(distance);
}

double get_neighbor_predict_regression(double** train,double* test_row,int num_neighbors)
{
    double* dist=(double *)malloc((num_folds-1) *fold_size * sizeof(double));
    double* dist_copy=(double *)malloc((num_folds-1) *fold_size * sizeof(double));
//    double** neighbors;
//    neighbors = (double **)malloc(num_neighbors * sizeof(int *));
//	for (int i = 0; i < num_neighbors; ++i){
//		neighbors[i] = (double *)malloc(col*sizeof(double));
//	}
    for (int i = 0; i < (num_folds-1) *fold_size; ++i){
        dist[i]=euclidean_distance(train[i],test_row);
    }
    for(int i=0;i<(num_folds-1) *fold_size;++i){
        dist_copy[i]=dist[i];
    }

//    printf("\n����ǰǰʮ��Ϊ,");
//    for(int i=0; i<10; i++){
//        printf("%f,", dist[i]);
//    }
    for(int i=0; i<(num_folds-1) *fold_size-1; i++){
        int isSorted = 1;  //����ʣ�µ�Ԫ���Ѿ��������
        for(int j=0; j<(num_folds-1) *fold_size-1-i; j++){
            if(dist[j] > dist[j+1]){
                double temp = dist[j];
                dist[j] = dist[j+1];
                dist[j+1] = temp;
                isSorted = 0;  //һ����Ҫ��������Ԫ�أ���˵��ʣ�µ�Ԫ��û�������
            }
        }
        if(isSorted) break; //���û�з���������˵��ʣ�µ�Ԫ���Ѿ��������
    }
//    printf("\n�����ǰʮ��Ϊ,");
//    for(int i=0; i<10; i++){
//        printf("%f,", dist[i]);
//    }
//     printf("\n������ʮ��Ϊ");
//    for(int i=0; i<10; i++){
//        printf("%f ", dist[(num_folds-1) *fold_size-1-i]);
//    }
//    printf("\n");
    for(int i=0;i<num_neighbors;++i){
        for(int j=0;j<(num_folds-1) *fold_size;++j){
            if(dist[i]==dist_copy[j]){
                neighbors[i]=train[j];
                //printf("����i=%d,j=%d\n",i,j);
            }
        }
    }
    double output_value=0.0;
    for(int i=0;i<num_neighbors;++i){
        output_value+=neighbors[i][col-1];
    }
    double prediction=output_value/(double)num_neighbors;
    //printf("����outputvalueΪ%f\n",output_value);
    free(dist);
    dist=NULL;
    free(dist_copy);
    dist_copy=NULL;
//    for (int i = 0; i < num_neighbors; ++i){
//        free(neighbors[i]);
//		neighbors[i] = NULL;
//	}
//	free(neighbors);
//	neighbors=NULL;
    return prediction;
}
