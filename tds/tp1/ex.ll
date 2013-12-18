define i32 @calcule(i32 %x) {

%1 = add i32 1, 0

%2 = add i32 %1, %1

%3 = add i32 5, 0

%4 = add i32 2, 0
%5 = mul i32 %4, %3
%6 = sub i32 %5, %3
%7 = add i32 %2, %6
ret i32 %7
}

@str = constant [7 x i8 ] c"=> %d\0A\00"
declare i32 @printf ( i8 * , ...)

define i32 @main () {
%x = call i32 @calcule (i32 32)
call i32 (i8 * , ...)* @printf ( i8 * getelementptr ([7 x i8]* @str, i32 0, i32 0), i32 %x)
ret i32 0
}
