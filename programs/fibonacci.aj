print("Enter the position of fibonacci number to find: ");
n = int(input());

def fibonacci(n) {
    if(n<=1) {
        return n;
    }
    else {
        return (fibonacci(n-1) + fibonacci(n-2));
    }
}

fibn = fibonacci(n);
print("The ", n, "th fibonacci number is: ", fibn, endl);
