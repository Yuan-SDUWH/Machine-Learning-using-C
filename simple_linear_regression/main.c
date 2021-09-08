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
void train_test_split(double** data,float split);
void rmse_metric(double** actual,double** predicted);
void cal_stat(double** train);
void simple_linear_regression(double** train,double** test);

double **data;
double **train;
double **test;
double **actual, **predicted;
int row,col,train_size,test_size;
double rmse,mean_x,mean_y,covariance,variance,b1,b0;


int main()
{
	char filename[] = "insurance.csv";
    char line[1024];
 //   double **data;
  //  int row, col;
    row = get_row(filename);
    col = get_col(filename);
    data = (double **)malloc(row * sizeof(int *));
	for (int i = 0; i < row; ++i){
		data[i] = (double *)malloc(col * sizeof(double));
	}//动态申请二维数组
	get_two_dimension(line, data, filename);
	printf("数据行数 = %d\n", row);
	printf("数据列数 = %d\n", col);
//打乱数据顺序
    random(data,row);
    printf("打乱后的前五行数据为\n");
    print_two_dimension(data,5, col);
    train_test_split(data,0.6);
    cal_stat(data);
    simple_linear_regression(train,test);
    printf("rmse为%f\nmean_x为%f\nmean_y为%f\nb1为%f\nb0为%f\n",rmse,mean_x,mean_y,b1,b0);
  //  printf("训练集");
  //  printf("%d",train_size);
   // print_two_dimension(train,train_size , col);
  //  print_two_dimension(test,test_size , col);




    for(int i=0;i<train_size;i++){
        free(train[i]);
        train[i]=NULL;
    }
    free(train);
    train=NULL;
    for(int i=0;i<test_size;i++){
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
  //      free(data[i]);
   //     data[i]=NULL;
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

void train_test_split(double** data,float split)
{
    train_size=round(row*split);
    printf("总数据量有%d行,train_size=%d\n",row,train_size);
    test_size=row-train_size;
    train = (double **)malloc(train_size * sizeof(int *));
    if(train==NULL){
        printf("分配失败");
    }
	for (int i = 0; i < train_size; ++i){
		train[i] = (double *)malloc(col * sizeof(double));
		train[i] = data[i];
	}
	test = (double **)malloc(test_size * sizeof(int *));
	for (int i = 0; i < test_size; ++i){
		test[i] = (double *)malloc(col * sizeof(double));
		test[i] = data[(int)(i+train_size)];
	}
}

void rmse_metric(double** actual,double** predicted)
{
    double sum_error=0.0;
    double prediction_error=0.0;
    for(int i=0;i<test_size;i++)
    {
        prediction_error=predicted[i][0]-actual[i][0];
        sum_error+= pow(prediction_error,2);
    }
    double mean_error=sum_error/test_size;
    rmse=sqrt(mean_error);
}

void cal_stat(double** train)
{
    double sumx=0.0;
    double sumy=0.0;
    for (int i = 0; i < row; i++)
    {
        sumx+=train[i][0];
        sumy+=train[i][1];
    }
    mean_x=sumx/row;
    mean_y=sumy/row;
    covariance=0.0;
    variance=0.0;
    for (int i = 0; i < row; i++)
    {
        covariance+=(train[i][0]-mean_x)*(train[i][1]-mean_y);
        variance+=pow((train[i][0]-mean_x),2);
    }
    b1=covariance/variance;
    b0=mean_y-b1*mean_x;
}

void simple_linear_regression(double** train,double** test)
{
    predicted = (double **)malloc(test_size * sizeof(int *));
	for (int i = 0; i < test_size; ++i){
		predicted[i] = (double *)malloc( sizeof(double));
		double yhat=b0+b1*test[i][0];
		predicted[i][0]=yhat;
	}
	actual = (double **)malloc(test_size * sizeof(int *));
	for (int i = 0; i < test_size; ++i){
		actual[i] = (double *)malloc( sizeof(double));
		actual[i][0]=test[i][1];
	}
	rmse_metric(actual,predicted);
}

