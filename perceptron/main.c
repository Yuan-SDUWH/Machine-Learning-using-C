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
double accuracy_metric(double** actual,double** predicted);
void evaluate_algorithm(double** data,int n_folds,double l_rate,int n_epoch);
void train_weights(double** train,double l_rate,int n_epoch);

double **data;
double **train;
double **test;
double **actual, **predicted;
double *weights, *scores;
char *strdata,*strdata_uniq;
int row,col,fold_size,num_folds;
double mean_accuracy;


int main()
{
	char filename[] = "sonar.all-data";
    char line[1024];
    row = get_row(filename);
    col = get_col(filename);
    data = (double **)malloc(row * sizeof(int *));
	for (int i = 0; i < row; ++i){
		data[i] = (double *)malloc(col * sizeof(double));
	}//动态申请二维数组
	strdata = (char *)malloc(row * sizeof(char));
	strdata_uniq = (char *)malloc(row/3 * sizeof(char));
	get_two_dimension(line, data, filename);
	printf("数据行数 = %d\n", row);
	printf("数据列数 = %d\n", col);
//显示第1行数据
//    printf("原始第一行数据为\n");
//	print_two_dimension(data,1 , col);

//把分类字符串转化为整数
    str_to_int(strdata);
//打乱数据顺序
    random(data,row);
//    printf("打乱后第一行数据为\n");
//    print_two_dimension(data,1 , col);
    printf("分类字符的类别对应index\n");
    for(int i=0;i<2;++i){
        printf("index%d=%c\n",i,strdata_uniq[i]);
            }
//训练模型并测试准确率
    evaluate_algorithm(data,4,0.01,500);
    printf("mean_accuracy=%3f%%\n",mean_accuracy);

    free(weights);
    free(strdata);
    free(strdata_uniq);
    strdata=NULL;
    strdata_uniq=NULL;
    weights=NULL;
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
  //  for(int i=0;i<row;i++){
   //     free(data[i]);
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
                if(j==(col-1)){
                    strdata[i]=*tok;//存储类型字符
                }
            data[i][j] = atof(tok);//转换成浮点数
		}//字符串拆分操作
        i++;
        free(tmp);
    }
    fclose(stream);//文件打开后要进行关闭操作
}
void str_to_int(char* strdata)
{
    int k=0;
    int j;
    for(int i=0;i<row;++i){
        for(j=i+1;j<row && strdata[i]!=strdata[j];++j);
        if(!(j-row))//没有重复元素
            strdata_uniq[k++]=strdata[i];
    }
    printf("分类总共有%d类\n",k);
    for(int i=0;i<row;++i){
            for(j=0;j<k;++j){
                if(strdata[i]==strdata_uniq[j]){
                    data[i][col-1]=j;
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

//乱序数组
void random(double** data,int row)
{
    int index, tmp, i;

  //  srand(time(NULL));
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

double accuracy_metric(double** actual,double** predicted)
{
    double correct=0;
    for(int i=0;i<fold_size;i++)
    {
        if(actual[i][0]==predicted[i][0]){
            correct+=1;
        }
    }
    return correct/(double)fold_size*100.0;
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
        train_weights(train,l_rate,n_epoch);
        for(int i=0;i<fold_size;++i){
            double activation=weights[0];
            double prediction;
            for(int j=0;j<col-1;++j){
                activation+=weights[j+1]*test[i][j];
            }
            if(activation>=0.0){
                prediction=1.0;
            }
            else{
                prediction=0.0;
            }
            predicted[i][0]=prediction;
        }
        printf("\n本次测试集长度为%d\n",fold_size);
        printf("随机抽查第40个测试数据%f\n",predicted[40][0]);
        printf("实际第40个数据%f\n",actual[40][0]);
        scores[k]=accuracy_metric(actual,predicted);
        printf("fold_k=%d,score=%3f\n",k,scores[k]);
        mean_accuracy+=scores[k];
	}

	mean_accuracy=mean_accuracy/(n_folds);

}

void train_weights(double** train,double l_rate, int n_epoch)
{
    weights = (double *)malloc(col * sizeof(double));
    for(int i=0;i<col;++i){
        weights[i]=0.0;
    }
    for(int i=0;i<n_epoch;++i){
        for(int j=0;j<(num_folds-1)*fold_size;++j){
            double activation=weights[0];
            double prediction;
            for(int k=0;k<col-1;++k){
                activation+=weights[k+1]*train[j][k];
            }
            if(activation>=0.0){
                prediction=1.0;
            }
            else{
                prediction=0.0;
            }
            double error=train[j][col-1]-prediction;
            weights[0]=weights[0]+l_rate*error;
            for(int k=0;k<col-1;++k){
                weights[k+1]=weights[k+1]+l_rate*error*train[j][k];
            }
        }
    }
//    printf("本次权重参数为\n");
//    for(int i=0;i<col;++i){
//        printf("%f   ",weights[i]);
//    }
   // printf("l_rate=%f,n_epoch=%d\n",l_rate,n_epoch);
}
