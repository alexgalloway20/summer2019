//Alex Galloway Last Modified 1/17/19
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[]){
	int print_matrix(int k, int *arr6);
	int k = atoi(argv[1]);
	int i = atoi(argv[2]);
	int id = atoi(argv[3]);
	int *arr6 = (int *) malloc(k * k * sizeof(int));
	memset(arr6, 0, k * k * sizeof(int));
	print_matrix(k, arr6);
	int a = k * k - 1;


	arr6[i] = id;
	print_matrix(k, arr6);

	while(i < a){
		if(arr6[i] == id && i + k < a){
			i = k + i;
			arr6[i] = id;
			print_matrix(k, arr6);
		}else{
			break;
		}
	}		
        

	while(i < a){
		i = i + 1;
		arr6[i] = id;
		print_matrix(k, arr6);
	}

	while(i >= 0){
		i--;
		arr6[i] = id;;
		print_matrix(k, arr6);
	}
}

int print_matrix(int k, int *arr6){
	int i;
	int j;
	for(i = 0; i < k; i++){
		for(j = 0; j < k; j++){
			printf("%3d", *(arr6 + (k * i + j)));
		}
		putchar('\n');
	}
	putchar('\n');
}
