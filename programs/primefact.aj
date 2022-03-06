print("Enter number to find prime factorization of: ");
n = int(input());
print(endl, "The prime factorization of ", n, " is: ");
i = 2;
while(i*i<=n){
	while(n%i == 0){
		print(i, "*");
		n = n//i;
		if(i*i>n){
			break;
		}
	}
	i = i+1;
}
print(n, endl);
