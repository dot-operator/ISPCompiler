// comment
const char my_char = 'c';

int add(int a, int b){
	return a + b;
}

int main(){
	char your_char = my_char;
	int i= 2 + 2;
	int j = add(2, 2);
	float my_float = 2.3;

	if(i != j){
		i = 4;
	}
    
	do {
		i++;
	} while(i == 5);
	return 0;
}
