; un commentaire
define i32 @mafonction(i32 %arg) {
; mafonction est une fonction (globale)
%x = mul i32 %arg, 2 ; multiplication entiere, retournant un i32
%y = add i32 %x , 32 ; addition entiere
%z = sub i32 %y , %x ; soustraction entiere
%a = sdiv i32 %z, 4 ; division entiere
ret i32 %a
}
define i32 @main() { ; main est une variable globale
%retval = call i32 @mafonction(i32 42)
ret i32 0
}
