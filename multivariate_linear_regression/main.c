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

double rmse_metric(double** actual,double** predicted);
void normalize_dataset(double** data);
void evaluate_algorithm(double** data,int n_folds,double l_rate,int n_epoch);
void coefficients_sgd(double** train,double l_rate,int n_epoch);

double **data;
double **train;
double **test;
double **actual, **predicted;
double *coef, *scores;
int row,col,fold_size,num_folds;
double mean_rmse;


int main()
{
	char filename[] = "winequality-white.csv";
    char line[1024];
    row = get_row(filename);
    col = get_col(filename);
    data = (double **)malloc(row * sizeof(int *));
	for (int i = 0; i < row; ++i){
		data[i] = (double *)malloc(col * sizeof(double));
	}//动态申请二维数组
	get_two_dimension(line, data, filename);
	printf("数据行数 = %d\n", row);
	printf("数据列数 = %d\n", col);
//显示前5行数据
//	print_two_dimension(data,5 , col);
//打乱数据顺序
    random(data,row);
    printf("数据打乱后前五行\n");
    print_two_dimension(data,5 , col);
    //归一化数据
    normalize_dataset(data);
    printf("数据归一化后前五行\n");
    print_two_dimension(data,5,col);
    //多远回归拟合
    evaluate_algorithm(data,5,0.01,50);
    printf("\nmean_rmse=%3f\n",mean_rmse);
    for(int i=0;i<col;++i){
        printf("b%d=%f\n",i,coef[i]);
    }
    free(coef);
    coef=NULL;
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
   // for(int i=0;i<row;i++){
      //  free(data[i]);
       // data[i]=NULL;
  //  }
    free(data);
    data=NULL;
}

void get_two_dimension(char* line, double** data, char *filename)
{
	FILE* stream = fopen(filename, "r");
	int i = 0;
	while (fgets(line, 1024, stream))//逐行读取
    {
    	int j = 0;
    	char *tok;
        char* tmp = strdup(line);
        for (tok = strtok(line, ","); tok && *tok; j++, tok = strtok(NULL, ",\n")){
        	data[i][j] = atof(tok);//转换成浮点数
		}//字符串拆分操作
        i++;
        free(tmp);
    }
    fclose(stream);//文件打开后要进行关闭操作
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

//乱序数组
void random(double** data,int row)
{
    int index, tmp, i;
    //srand(time(NULL));
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

void normalize_dataset(double** data)
{
    double minmax[2][col];
    for(int j=0;j<col;++j){
        minmax[0][j]=data[0][j];
        minmax[1][j]=data[0][j];
    }
    for(int i=1;i<row;++i){
            for(int j=0;j<col;++j){
                if(minmax[1][j]<data[i][j]){
                    minmax[1][j]=data[i][j];
                }
                if(minmax[0][j]>data[i][j]){
                    minmax[0][j]=data[i][j];
                }
            }
    }
     for(int i=0;i<row;++i){
            for(int j=0;j<col;++j){
                data[i][j]=(data[i][j]-minmax[0][j])/(minmax[1][j]-minmax[0][j]);
            }
    }
}

void evaluate_algorithm(double** data,int n_folds,double l_rate,int n_epoch)
{
    fold_size=(int)(floor(row/n_folds));
    num_folds=n_folds;
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
    mean_rmse=0.0;
	for(int k=0;k<n_folds;++k){
        for(int i=0;i<fold_size;++i){
            test[i]=data[k*fold_size+i];
            actual[i][0]=test[i][col-1];
          //  test[i][col-1]=NAN;
        }
        for(int i=0;i<n_folds*fold_size;++i){
            if(i<k*fold_size){
                train[i]=data[i];
            }
            if(i>=(k+1)*fold_size){
                train[i-fold_size]=data[i];
            }
        }
        coefficients_sgd(train,l_rate,n_epoch);
        for(int i=0;i<fold_size;++i){
            double yhat=coef[0];
            for(int j=0;j<col-1;++j){
                yhat+=coef[j+1]*test[i][j];
            }
            predicted[i][0]=yhat;
        }
        scores[k]=rmse_metric(actual,predicted);
        printf("k=%d,score=%3f\n",k,scores[k]);
        mean_rmse+=scores[k];
	}

	mean_rmse=mean_rmse/(n_folds);

}

void coefficients_sgd(double** train,double l_rate,int n_epoch)
{
    coef = (double *)malloc(col * sizeof(double));
    for(int i=0;i<col;++i){
        coef[i]=0.0;
    }
    for(int i=0;i<n_epoch;++i){
        for(int j=0;j<(num_folds-1)*fold_size;++j){
            double yhat=coef[0];
            for(int k=0;k<col-1;++k){
                yhat+=coef[k+1]*train[j][k];
            }
            double error=yhat-train[j][col-1];
            coef[0]=coef[0]-l_rate*error;
            for(int k=0;k<col-1;++k){
                coef[k+1]=coef[k+1]-l_rate*error*train[j][k];
            }
        }
    }
    printf("learning_rate=%f,n_epoch=%d\n",l_rate,n_epoch);
}

