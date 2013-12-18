define i32 @calcule(i32 %x) {

%1 = add i32 1, 0

%2 = add i32 %1, %1

%3 = add i32 5, 0

%4 = add i32 2, 0
%5 = mul i32 %4, %3
%6 = sub i32 %5, %3
%7 = add i32 %2, %6
ret %7
}
