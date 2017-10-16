int factorial(int x){
	if(x < 0){
		return 0;
	}
	if(x == 0){
		return 1;
	}

	if(x == 1){
		return 1;
	}

	return x * factorial(x - 1);
}

int f = factorial(5);
print("f = " + f + "\n");
